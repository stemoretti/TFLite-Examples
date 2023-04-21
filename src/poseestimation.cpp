#include "poseestimation.h"

#include <cmath>

#include "utils.h"

using namespace tflite;

PoseEstimation::PoseEstimation(QObject *parent)
    : TFLite<QImage>(parent)
    , m_score(0.5)
{
}

bool PoseEstimation::customInitStep()
{
    // Get input dimension from the input tensor metadata
    // Assuming one input only
    int input = m_interpreter->inputs()[0];
    TfLiteIntArray *dims = m_interpreter->tensor(input)->dims;

    wanted_height = dims->data[1];
    wanted_width = dims->data[2];
    wanted_channels = dims->data[3];

    if (m_interpreter->outputs().size() != 4) {
        setErrorString("Graph needs to have exactly 4 outputs");
        return false;
    }

    m_outputs.clear();
    for (int i = 0; i < 4; ++i)
        m_outputs.push_back(m_interpreter->tensor(m_interpreter->outputs()[i]));

    return true;
}

bool PoseEstimation::preProcessing(const QImage &input)
{
    QImage image(input);

    std::vector<int> inputs = m_interpreter->inputs();

    for (unsigned int i = 0; i < m_interpreter->inputs().size(); ++i) {
        int input = inputs[i];

        switch (m_interpreter->tensor(input)->type) {
        case kTfLiteFloat32:
            Utils::resize<float>(m_interpreter->typed_tensor<float>(input), image,
                                 wanted_height, wanted_width, wanted_channels, true);
            break;
        case kTfLiteUInt8:
            Utils::resize<uint8_t>(m_interpreter->typed_tensor<uint8_t>(input), image,
                                   wanted_height, wanted_width, wanted_channels, false);
            break;
        default:
            setErrorString("Cannot handle input type " +
                           QString::number(m_interpreter->tensor(input)->type) + " yet");
            return false;
        }
    }

    return true;
}

void PoseEstimation::postProcessing()
{
    QVariantList keypoints;
    QVariantList scores;

    TfLiteTensor *heatmaps = m_outputs[0];
    TfLiteTensor *offsets = m_outputs[1];
#if 0
    float *displacements_fwd = Utils::TensorData<float>(m_outputs[2], 0);
    float *displacements_bwd = Utils::TensorData<float>(m_outputs[3], 0);
#endif

    if (m_outputs[0]->dims->size < 4) {
        setErrorString("Unsupported model heatmaps number");
        return;
    }

    int height = m_outputs[0]->dims->data[1];
    int width = m_outputs[0]->dims->data[2];
    int num_key = m_outputs[0]->dims->data[3];

    for (int i = 0; i < num_key; i++) {
        float max_val = 0.0f;
        int maxrow = 0;
        int maxcol = 0;
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                float *point = Utils::TensorData<float>(heatmaps, { row, col, i });
                if (point && (*point > max_val)) {
                    max_val = *point;
                    maxrow = row;
                    maxcol = col;
                }
            }
        }

        float *offsetX = Utils::TensorData<float>(offsets, { maxrow, maxcol, num_key + i });
        float *offsetY = Utils::TensorData<float>(offsets, { maxrow, maxcol, i });
        if (!offsetX || !offsetY) {
            setErrorString("Undefined pointer to offset");
            return;
        }

        QPoint point;
        point.setX((maxcol / float(width - 1) + *offsetX / wanted_width) * m_contentSize.width());
        point.setY((maxrow / float(height - 1) + *offsetY / wanted_height) * m_contentSize.height());

        keypoints.append(point);
        scores.append(1.0f / (1.0f + std::exp(-max_val)));
    }

    Q_EMIT results(keypoints, scores);
}
