#include "poseestimation.h"

#include <QDebug>

#include "utils.h"

using namespace tflite;

PoseEstimation::PoseEstimation(QObject *parent)
    : TFLite<QImage>(parent)
    , m_score(0.6)
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
        qWarning() << errorString();
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
            qWarning() << errorString();
            return false;
        }
    }

    return true;
}

void PoseEstimation::postProcessing()
{
    QVariantList keypoints;
    QVariantList scores;

    float *heatmaps = Utils::TensorData<float>(m_outputs[0], 0);
    float *offsets = Utils::TensorData<float>(m_outputs[1], 0);
#if 0
    float *displacements_fwd = Utils::TensorData<float>(m_outputs[2], 0);
    float *displacements_bwd = Utils::TensorData<float>(m_outputs[3], 0);
#endif

    if (m_outputs[0]->dims->size < 4) {
        qWarning() << "Unsupported model";
        return;
    }

    int height = m_outputs[0]->dims->data[1];
    int width = m_outputs[0]->dims->data[2];
    int num_key = m_outputs[0]->dims->data[3];

    for (int keypoint = 0; keypoint < num_key; keypoint++) {
        float max_val = *heatmaps;
        int maxrow = 0;
        int maxcol = 0;
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                int point = heatmaps[row * (width * num_key) + col * num_key + keypoint];
                if (point > max_val) {
                    max_val = point;
                    maxrow = row;
                    maxcol = col;
                }
            }
        }
        keypoints.append(QPoint(maxcol, maxrow));
        scores.append(max_val);
    }

    for (int i = 0; i < num_key; i++) {
        int x = keypoints[i].toPoint().x();
        int y = keypoints[i].toPoint().y();
        float offsetX = offsets[y * (width * num_key * 2) + x * (num_key * 2) + num_key + i];
        float offsetY = offsets[y * (width * num_key * 2) + x * (num_key * 2) + i];
        QPoint point = keypoints[i].toPoint();
        point.setX((x / float(width - 1) + offsetX / wanted_width) * m_contentSize.width());
        point.setY((y / float(height - 1) + offsetY / wanted_height) * m_contentSize.height());
        keypoints[i] = point;
    }

    emit results(keypoints, scores);
}
