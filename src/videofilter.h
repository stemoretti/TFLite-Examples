#ifndef VIDEOFILTER_H
#define VIDEOFILTER_H

#include <QFuture>
#include <QFutureWatcher>
#include <QVideoFrame>
#include <QVideoSink>

#include <QtQml/qqmlregistration.h>

#include <memory>

#include "tflite.h"

struct SimpleVideoFrame
{
    SimpleVideoFrame() : dataSize(0) {}
    Q_DISABLE_COPY(SimpleVideoFrame)

    void copyData(QVideoFrame *frame) {
        int sz = 0;
        for (int i = 0; i < frame->planeCount(); i++)
            sz += frame->mappedBytes(i);

        if (sz != dataSize) {
            dataSize = sz;
            data = std::make_unique<unsigned char[]>(dataSize);
        }

        for (int i = 0, shift = 0; i < frame->planeCount(); i++) {
            int bytes = frame->mappedBytes(i);
            memcpy(data.get() + shift, frame->bits(i), bytes);
            shift += bytes;
        }
        size = frame->size();
        pixelFormat = frame->pixelFormat();
    }

    std::unique_ptr<unsigned char[]> data;
    int dataSize;
    QSize size;
    QVideoFrameFormat::PixelFormat pixelFormat;
};

class VideoFilter : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(int orientation MEMBER m_orientation NOTIFY orientationChanged)
    Q_PROPERTY(QRect captureRect MEMBER m_captureRect NOTIFY captureRectChanged)
    Q_PROPERTY(QObject *tflite READ tflite WRITE setTflite)
    Q_PROPERTY(QObject *videoSink READ videoSink WRITE setVideoSink)

public:
    explicit VideoFilter(QObject *parent = nullptr);
    ~VideoFilter();

    bool active();
    void setActive(bool active);

    QObject *tflite();
    void setTflite(QObject *tflite);

    QObject *videoSink();
    void setVideoSink(QObject *videoSink);

Q_SIGNALS:
    void activeChanged(bool active);
    void orientationChanged(int orientation);
    void captureRectChanged(QRect captureRect);

private:
    void newVideoFrame(const QVideoFrame &frame);
    bool processVideoFrame(int orientation, bool mirrored, QRect captureRect) const;
    void processingFinished();

    QFuture<bool> m_future;
    QFutureWatcher<bool> m_watcher;
    bool m_running;

    SimpleVideoFrame m_frame;

    bool m_active;
    int m_orientation;
    QRect m_captureRect;
    TFLite<QImage> *m_tflite;
    QVideoSink *m_videoSink;
};

#endif // VIDEOFILTER_H
