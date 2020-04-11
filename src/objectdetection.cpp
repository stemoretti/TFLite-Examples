#include "objectdetection.h"

#include <QDebug>

#include "utils.h"

using namespace tflite;

ObjectDetection::ObjectDetection(QObject *parent)
    : TFLite<QImage>(parent)
    , m_boxes(new BoxesModel(this))
    , m_confidence(0.6)
{
    connect(this, &ObjectDetection::labelsFileChanged, this, &TFLite::queueInit);
    connect(this, &ObjectDetection::results, m_boxes, &BoxesModel::reset, Qt::QueuedConnection);
}

bool ObjectDetection::customInitStep()
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

    m_labels = Utils::readLabels(m_labelsFile);
    if (!m_labels.empty())
        qDebug() << "There are" << m_labels.count() << "labels.";
    else
        qDebug() << "There are NO labels";

    return true;
}

bool ObjectDetection::preProcessing(const QImage &input)
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

void ObjectDetection::postProcessing()
{
    QStringList captions;
    QList<float> confidences;
    QList<QRectF> boxes;

    int    num_detections    = *Utils::TensorData<float>(m_outputs[3], 0);
    float *detection_classes =  Utils::TensorData<float>(m_outputs[1], 0);
    float *detection_scores  =  Utils::TensorData<float>(m_outputs[2], 0);
    float *detection_boxes   =  Utils::TensorData<float>(m_outputs[0], 0);

    for (int i = 0; i < num_detections; ++i) {
        int cls = detection_classes[i] + 1;

        if (cls == 0)
            continue;

        float score = detection_scores[i];

        if (score < m_confidence)
            break;

        QString label = (cls >= 0 && cls < m_labels.size()) ? m_labels[cls] : "";

        float top = std::max(detection_boxes[4 * i], 0.0f);
        float left = std::max(detection_boxes[4 * i + 1], 0.0f);
        float height = std::min(detection_boxes[4 * i + 2], 1.0f) - top;
        float width = std::min(detection_boxes[4 * i + 3], 1.0f) - left;

        QRectF box(left * m_contentSize.width(),
                   top * m_contentSize.height(),
                   width * m_contentSize.width(),
                   height * m_contentSize.height());

        captions.append(label);
        confidences.append(score);
        boxes.append(box);
    }

    emit results(boxes, captions, confidences);
}

BoxesModel *ObjectDetection::boxes() const
{
    return m_boxes;
}
