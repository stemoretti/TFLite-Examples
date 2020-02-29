#ifndef IMAGECLASSIFICATION_H
#define IMAGECLASSIFICATION_H

#include "tflite.h"

#include <QImage>
#include <QVariantList>

class ImageClassification : public TFLite<QImage>
{
    Q_OBJECT

    Q_PROPERTY(QString labelsFile MEMBER m_labelsFile NOTIFY labelsFileChanged)
    Q_PROPERTY(QVariantList captions READ captions NOTIFY captionsChanged)
    Q_PROPERTY(float threshold MEMBER m_threshold NOTIFY thresholdChanged)

public:
    explicit ImageClassification(QObject *parent = nullptr);

    bool customInitStep() override;
    bool preProcessing(const QImage &input) override;
    void postProcessing() override;

    QVariantList captions() const;

Q_SIGNALS:
    void labelsFileChanged(const QString &labelsFile);
    void captionsChanged(const QVariantList &captions);
    void thresholdChanged(float threshold);

private:
    QStringList m_labels;
    int wanted_height, wanted_width, wanted_channels;

    QString m_labelsFile;
    QVariantList m_captions;
    float m_threshold;
};

#endif // IMAGECLASSIFICATION_H
