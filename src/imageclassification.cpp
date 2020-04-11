#include "imageclassification.h"

#include <QDebug>
#include <QJsonObject>

#include "utils.h"

#include "get_top_n.h"

using namespace tflite;

ImageClassification::ImageClassification(QObject *parent)
    : TFLite<QImage>(parent)
    , m_threshold(0.001)
{
    connect(this, &ImageClassification::labelsFileChanged, this, &TFLite::queueInit);
}

bool ImageClassification::customInitStep()
{
    // Get input dimension from the input tensor metadata
    // Assuming one input only
    int input = m_interpreter->inputs()[0];
    TfLiteIntArray *dims = m_interpreter->tensor(input)->dims;

    wanted_height = dims->data[1];
    wanted_width = dims->data[2];
    wanted_channels = dims->data[3];

    m_labels = Utils::readLabels(m_labelsFile);
    if (!m_labels.empty())
        qDebug() << "There are" << m_labels.count() << "labels.";
    else
        qDebug() << "There are NO labels";

    return true;
}

bool ImageClassification::preProcessing(const QImage &input)
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

void ImageClassification::postProcessing()
{
    std::vector<std::pair<float, int>> top_results;

    if (m_interpreter->outputs().empty())
        return;

    int output = m_interpreter->outputs()[0];
    TfLiteIntArray *output_dims = m_interpreter->tensor(output)->dims;

    int output_size = output_dims->data[output_dims->size - 1];
    size_t num_results = 5;

    switch (m_interpreter->tensor(output)->type) {
    case kTfLiteFloat32:
        get_top_n<float>(m_interpreter->typed_output_tensor<float>(0),
                         output_size, num_results, m_threshold, &top_results, true);
        break;
    case kTfLiteUInt8:
        get_top_n<uint8_t>(m_interpreter->typed_output_tensor<uint8_t>(0),
                           output_size, num_results, m_threshold, &top_results, false);
        break;
    default:
        qWarning() << "Cannot handle output type"
                   << m_interpreter->tensor(output)->type << "yet";
        return;
    }

    if (top_results.empty())
        return;

    m_captions.clear();
    for (const auto &result : top_results) {
        QJsonObject jobj;
        jobj.insert("caption",
                    (result.second >= 0 && result.second < m_labels.size()) ?
                        m_labels[result.second] : "");
        jobj.insert("confidence", result.first);
        m_captions.append(jobj);
    }
    emit captionsChanged(m_captions);
}

QVariantList ImageClassification::captions() const
{
    return m_captions;
}
