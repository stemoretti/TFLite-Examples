#include "videofilter.h"

#include <QDebug>
#include <QtConcurrent>

#include "tflite.h"

static QImage yuv_420p_to_rgb(const uchar *yuv, int width, int height)
{
    QImage image = QImage(width, height, QImage::Format_RGB888);
    uchar *rgb = image.bits();
    int wh = width * height;
    int w_2 = width / 2;
    int wh_54 = wh * 5 / 4;

    for (int y = 0; y < height; y++) {
        int Y_offset = y * width;
        int y_2 = y / 2;
        int U_offset = y_2 * w_2 + wh;
        int V_offset = y_2 * w_2 + wh_54;
        for (int x = 0; x < width; x++) {
            int x_2 = x / 2;
            uchar Y = yuv[Y_offset + x];
            uchar U = yuv[U_offset + x_2];
            uchar V = yuv[V_offset + x_2];

            int r = Y + (1.370705 * (V - 128));
            int g = Y - (0.698001 * (V - 128)) - (0.337633 * (U - 128));
            int b = Y + (1.732446 * (U - 128));

            *rgb++ = std::clamp(r, 0, 255);
            *rgb++ = std::clamp(g, 0, 255);
            *rgb++ = std::clamp(b, 0, 255);
        }
    }

    return image;
}

static QImage yuv_nv21_to_rgb(const uchar *yuv, int width, int height)
{
    QImage image = QImage(width, height, QImage::Format_RGB888);
    uchar *rgb = image.bits();
    int frameSize = width * height;

    int ii = 0;
    int ij = 0;
    int di = 1;
    int dj = 1;

    for (int i = 0, ci = ii; i < height; ++i, ci += di) {
        for (int j = 0, cj = ij; j < width; ++j, cj += dj) {
            int y = (0xff & ((int) yuv[ci * width + cj]));
            int v = (0xff & ((int) yuv[frameSize + (ci >> 1) * width + (cj & ~1) + 0]));
            int u = (0xff & ((int) yuv[frameSize + (ci >> 1) * width + (cj & ~1) + 1]));
            y = y < 16 ? 16 : y;

            int r = (int) (1.164f * (y - 16) + 1.596f * (v - 128));
            int g = (int) (1.164f * (y - 16) - 0.813f * (v - 128) - 0.391f * (u - 128));
            int b = (int) (1.164f * (y - 16) + 2.018f * (u - 128));

            *rgb++ = std::clamp(r, 0, 255);
            *rgb++ = std::clamp(g, 0, 255);
            *rgb++ = std::clamp(b, 0, 255);
        }
    }

    return image;
}

static QImage argb_data_to_image(const uchar *data, int width, int height,
                                 int alpha, int red, int green, int blue,
                                 bool isPremultiplied = false)
{
    int stride = (alpha < 0 ? 3 : 4);

    QImage image(width, height, QImage::Format_RGB888);

    for (int y = 0; y < height; y++) {
        uchar *rgb = image.scanLine(y);
        for (int x = 0; x < width; x++) {
            uchar r = data[red];
            uchar g = data[green];
            uchar b = data[blue];
            if (isPremultiplied) {
                uchar a = data[alpha];
                r = uchar((uint(r) * 255) / a);
                g = uchar((uint(g) * 255) / a);
                b = uchar((uint(b) * 255) / a);
            }
            *rgb++ = r;
            *rgb++ = g;
            *rgb++ = b;
            data += stride;
        }
    }

    return image;
}

VideoFilter::VideoFilter(QObject *parent)
    : QObject(parent)
    , m_running(false)
    , m_active(false)
    , m_orientation(0)
    , m_tflite(nullptr)
    , m_videoSink(nullptr)
{
    connect(&m_watcher, &QFutureWatcher<bool>::finished, this, &VideoFilter::processingFinished);
}

VideoFilter::~VideoFilter()
{
    if (!m_future.isFinished())
        m_future.waitForFinished();
}

bool VideoFilter::active()
{
    return m_active;
}

void VideoFilter::setActive(bool active)
{
    if (m_active == active)
        return;

    m_active = active;
    Q_EMIT activeChanged(active);
}

QObject *VideoFilter::tflite()
{
    return m_tflite;
}

void VideoFilter::setTflite(QObject *tflite)
{
    if (m_tflite == tflite)
        return;

    m_tflite = dynamic_cast<TFLite<QImage> *>(tflite);
}

QObject *VideoFilter::videoSink()
{
    return m_videoSink;
}

void VideoFilter::setVideoSink(QObject *videoSink)
{
    if (m_videoSink == videoSink)
        return;

    m_videoSink = qobject_cast<QVideoSink *>(videoSink);
    connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &VideoFilter::newVideoFrame);
}

void VideoFilter::newVideoFrame(const QVideoFrame &frame)
{
    if (m_running || !m_active || !m_tflite)
        return;

    QVideoFrame input(frame);
    if (!input.isValid() || !input.map(QVideoFrame::ReadOnly)) {
        qWarning() << "input frame not valid or not readable";
        return;
    }
    m_frame.copyData(&input);
    input.unmap();

    QRect rect(-m_captureRect.x(),
               -m_captureRect.y(),
                m_captureRect.x() * 2 + m_videoSink->videoSize().width(),
                m_captureRect.y() * 2 + m_videoSink->videoSize().height());

    m_future = QtConcurrent::run(&VideoFilter::processVideoFrame,
                                 this,
                                 m_orientation,
                                 frame.surfaceFormat().isMirrored(),
                                 rect);
    m_watcher.setFuture(m_future);
    m_running = true;
}

bool VideoFilter::processVideoFrame(int orientation, bool mirrored, QRect captureRect) const
{
    if (m_frame.dataSize < 1) {
        qDebug() << "VideoFilter::processVideoFrame: Buffer is empty";
        return false;
    }

    int width = m_frame.size.width();
    int height = m_frame.size.height();
    const unsigned char *data = m_frame.data.get();

    QImage image;

    switch (m_frame.pixelFormat) {
    case QVideoFrameFormat::Format_ARGB8888:
    case QVideoFrameFormat::Format_XRGB8888:
        image = argb_data_to_image(data, width, height, 0, 1, 2, 3);
        break;
    case QVideoFrameFormat::Format_ARGB8888_Premultiplied:
        image = argb_data_to_image(data, width, height, 0, 1, 2, 3, true);
        break;
    case QVideoFrameFormat::Format_RGBA8888:
    case QVideoFrameFormat::Format_RGBX8888:
        image = argb_data_to_image(data, width, height, 3, 0, 1, 2);
        break;
    case QVideoFrameFormat::Format_BGRA8888:
    case QVideoFrameFormat::Format_BGRX8888:
        image = argb_data_to_image(data, width, height, 3, 2, 1, 0);
        break;
    case QVideoFrameFormat::Format_BGRA8888_Premultiplied:
        image = argb_data_to_image(data, width, height, 3, 2, 1, 0, true);
        break;
    case QVideoFrameFormat::Format_ABGR8888:
    case QVideoFrameFormat::Format_XBGR8888:
        image = argb_data_to_image(data, width, height, 0, 3, 2, 1);
        break;
    case QVideoFrameFormat::Format_YUV420P:
        image = yuv_420p_to_rgb(data, width, height);
        break;
    case QVideoFrameFormat::Format_NV12:
        /// nv12 format, encountered on macOS
        image = yuv_nv21_to_rgb(data, width, height);
        break;
        /// TODO: Handle (create QImages from) YUV formats.
    default:
        QImage::Format imageFormat = QVideoFrameFormat::imageFormatFromPixelFormat(m_frame.pixelFormat);
        image = QImage(data, width, height, imageFormat);
        break;
    }

    if (image.isNull()) {
        qDebug() << "VideoFilter::processVideoFrame error: Cannot create image file to process.";
        qDebug() << "Maybe it was a format conversion problem? ";
        qDebug() << "VideoFrame format: " << m_frame.pixelFormat;
        qDebug() << "Image corresponding format: " << QVideoFrameFormat::imageFormatFromPixelFormat(m_frame.pixelFormat);
        return false;
    }

    if (captureRect.isValid())
        image = image.copy(captureRect);

    if (mirrored)
        image.mirror(false, true);

    if (orientation) {
        QPoint center = image.rect().center();
        QTransform matrix;
        matrix.translate(center.x(), center.y());
        matrix.rotate(-orientation);
        image = image.transformed(matrix);
    }

    return m_tflite->run(image);
}

void VideoFilter::processingFinished()
{
    if (!m_watcher.result())
        setActive(false);
    m_running = false;
}
