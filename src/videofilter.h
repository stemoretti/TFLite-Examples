#ifndef VIDEOFILTER_H
#define VIDEOFILTER_H

#include <QAbstractVideoFilter>
#include <QVideoFilterRunnable>
#include <QFuture>

class TFLiteBase;
template <typename T> class TFLite;

struct SimpleVideoFrame
{
    SimpleVideoFrame(QVideoFrame *frame) {
        frame->map(QAbstractVideoBuffer::ReadOnly);

        dataSize = frame->mappedBytes();
        data.reset(new uchar[dataSize]);
        memcpy(data.get(), frame->bits(), dataSize);
        size = frame->size();
        pixelFormat = frame->pixelFormat();

        frame->unmap();
    }

    std::shared_ptr<uchar> data;
    int dataSize;
    QSize size;
    QVideoFrame::PixelFormat pixelFormat;
};

class VideoFilter : public QAbstractVideoFilter
{
    friend class VideoFilterRunnable;

    Q_OBJECT

    Q_PROPERTY(int orientation MEMBER m_orientation NOTIFY orientationChanged)
    Q_PROPERTY(QRect captureRect MEMBER m_captureRect NOTIFY captureRectChanged)
    Q_PROPERTY(TFLiteBase *tflite READ tflite WRITE setTflite NOTIFY tfliteChanged)

public:
    explicit VideoFilter(QObject *parent = nullptr);
    ~VideoFilter();

    QVideoFilterRunnable *createFilterRunnable();

    TFLiteBase *tflite();
    void setTflite(TFLiteBase *tflite);

Q_SIGNALS:
    void orientationChanged(int orientation);
    void captureRectChanged(QRect captureRect);
    void tfliteChanged(const TFLiteBase *tflite);

private:
    QFuture<void> m_processThread;

    int m_orientation;
    QRect m_captureRect;
    TFLite<QImage> *m_tflite;
};

class VideoFilterRunnable : public QVideoFilterRunnable
{
public:
    explicit VideoFilterRunnable(VideoFilter *filter);

    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags);
    void processVideoFrame(SimpleVideoFrame &frame, int orientation);

private:
    VideoFilter *m_filter;
};

#endif // VIDEOFILTER_H
