#ifndef IMAGECLASSIFICATION_H
#define IMAGECLASSIFICATION_H

#include <QImage>
#include <QVariantList>

#include <QtQml/qqmlregistration.h>

#include "tflite.h"

class ImageClassification : public TFLite<QImage>
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString labelsFile MEMBER m_labelsFile NOTIFY labelsFileChanged)
    Q_PROPERTY(float threshold MEMBER m_threshold NOTIFY thresholdChanged)

public:
    explicit ImageClassification(QObject *parent = nullptr);

    bool customInitStep() override;
    bool preProcessing(const QImage &input) override;
    void postProcessing() override;

Q_SIGNALS:
    void results(const QVariantList &captions);

    void labelsFileChanged(const QString &labelsFile);
    void thresholdChanged(float threshold);

private:
    QStringList m_labels;
    int wanted_height, wanted_width, wanted_channels;

    QString m_labelsFile;
    float m_threshold;
};

#endif // IMAGECLASSIFICATION_H
