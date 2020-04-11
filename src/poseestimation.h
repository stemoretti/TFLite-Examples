#ifndef POSEESTIMATION_H
#define POSEESTIMATION_H

#include "tflite.h"

#include <QImage>

class PoseEstimation : public TFLite<QImage>
{
    Q_OBJECT

    Q_PROPERTY(QSize contentSize MEMBER m_contentSize NOTIFY contentSizeChanged)
    Q_PROPERTY(float score MEMBER m_score NOTIFY scoreChanged)

public:
    enum BodyPart {
        Nose,
        LeftEye,
        RightEye,
        LeftEar,
        RightEar,
        LeftShoulder,
        RightShoulder,
        LeftElbow,
        RightElbow,
        LeftWrist,
        RightWrist,
        LeftHip,
        RightHip,
        LeftKnee,
        RightKnee,
        LeftAnkle,
        RightAnkle
    };
    Q_ENUMS(BodyPart);

    explicit PoseEstimation(QObject *parent = nullptr);

    bool customInitStep() override;
    bool preProcessing(const QImage &input) override;
    void postProcessing() override;

Q_SIGNALS:
    void results(const QVariantList &keypoints, const QVariantList &scores);
    void contentSizeChanged(QSize contentSize);
    void scoreChanged(float score);

private:
    std::vector<TfLiteTensor *> m_outputs;
    int wanted_height, wanted_width, wanted_channels;

    QSize m_contentSize;
    float m_score;
};

#endif // POSEESTIMATION_H
