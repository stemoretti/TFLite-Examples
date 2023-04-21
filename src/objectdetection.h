#ifndef OBJECTDETECTION_H
#define OBJECTDETECTION_H

#include <QImage>

#include <QtQml/qqmlregistration.h>

#include "boxesmodel.h"
#include "tflite.h"

class ObjectDetection : public TFLite<QImage>
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString labelsFile MEMBER m_labelsFile NOTIFY labelsFileChanged)
    Q_PROPERTY(BoxesModel *boxes READ boxes CONSTANT)
    Q_PROPERTY(QSize contentSize MEMBER m_contentSize NOTIFY contentSizeChanged)
    Q_PROPERTY(float confidence MEMBER m_confidence NOTIFY confidenceChanged)

public:
    explicit ObjectDetection(QObject *parent = nullptr);

    bool customInitStep() override;
    bool preProcessing(const QImage &input) override;
    void postProcessing() override;

    BoxesModel *boxes() const;

Q_SIGNALS:
    void results(const QList<QRectF> &boxes,
                 const QStringList &captions,
                 const QList<float> &confidences);

    void labelsFileChanged(const QString &labelsFile);
    void contentSizeChanged(QSize contentSize);
    void confidenceChanged(float confidence);

private:
    std::vector<TfLiteTensor *> m_outputs;
    QStringList m_labels;
    int wanted_height, wanted_width, wanted_channels;

    QString m_labelsFile;
    BoxesModel *m_boxes;
    QSize m_contentSize;
    float m_confidence;
};

#endif // OBJECTDETECTION_H
