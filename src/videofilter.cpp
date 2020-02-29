#include "videofilter.h"

#include <QDebug>
#include <QtConcurrent>

#include <algorithm>

#include "utils.h"
#include "tflite.h"

static void yuv_to_rgb(uchar y, uchar u, uchar v, uchar *r, uchar *g, uchar *b)
{
    int rTmp = y + (1.370705 * (v - 128));
    int gTmp = y - (0.698001 * (v - 128)) - (0.337633 * (u - 128));
    int bTmp = y + (1.732446 * (u - 128));

    *r = std::clamp(rTmp, 0, 255);
    *g = std::clamp(gTmp, 0, 255);
    *b = std::clamp(bTmp, 0, 255);
}

static QImage yuv_nv21_to_rgb(const uchar *yuv, int width, int height) {
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

            r = std::clamp(r, 0, 255);
            g = std::clamp(g, 0, 255);
            b = std::clamp(b, 0, 255);

            *rgb++ = r;
            *rgb++ = g;
            *rgb++ = b;
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
    uchar *rgbInit = image.bits();

    for (int y = 1; y <= height; ++y) {
        // Quick fix for iOS devices. Will be handled better in the future
#ifdef Q_OS_IOS
        uchar *rgb = rgbInit + (y - 1) * width * 3;
#else
        uchar *rgb = rgbInit + (height - y) * width * 3;
#endif
        for (int x = 0; x < width; ++x) {
            // QTBUG-69968: endianess dependent format
            uint32_t pixel = *(uint32_t*)data;
            uchar r = 0xff & (pixel >> ((stride - 1 - red) * 8));
            uchar g = 0xff & (pixel >> ((stride - 1 - green) * 8));
            uchar b = 0xff & (pixel >> ((stride - 1 - blue) * 8));
            if (isPremultiplied) {
                uchar a = 0xff & (pixel >> ((3 - alpha) * 8));
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
    : QAbstractVideoFilter(parent)
    , m_orientation(0)
    , m_tflite(nullptr)
{
}

VideoFilter::~VideoFilter()
{
    if (!m_processThread.isFinished())
        m_processThread.waitForFinished();
}

QVideoFilterRunnable *VideoFilter::createFilterRunnable()
{
    return new VideoFilterRunnable(this);
}

TFLiteBase *VideoFilter::tflite()
{
    return m_tflite;
}

void VideoFilter::setTflite(TFLiteBase *tflite)
{
    if (m_tflite == tflite)
        return;

    m_tflite = dynamic_cast<TFLite<QImage>*>(tflite);
    emit tfliteChanged(m_tflite);
}

VideoFilterRunnable::VideoFilterRunnable(VideoFilter *filter)
    : m_filter(filter)
{
}

QVideoFrame VideoFilterRunnable::run(QVideoFrame *input,
                                      const QVideoSurfaceFormat &surfaceFormat,
                                      RunFlags flags)
{
    Q_UNUSED(flags)
    Q_UNUSED(surfaceFormat)

    if (!input && !input->isValid())
        return QVideoFrame();

    if (m_filter->m_processThread.isRunning() || !m_filter->m_tflite)
        return *input;

    m_filter->m_processThread
            = QtConcurrent::run(this,
                                &VideoFilterRunnable::processVideoFrame,
                                SimpleVideoFrame(input),
                                -m_filter->m_orientation);

    return *input;
}

void VideoFilterRunnable::processVideoFrame(SimpleVideoFrame &frame, int orientation)
{
    if (frame.dataSize < 1) {
        qDebug() << "VideoFilterRunnable: Buffer is empty";
        return;
    }

    int width = frame.size.width();
    int height = frame.size.height();
    const uchar *data = frame.data.get();

    uchar *rgb;
    int wh;
    int w_2;
    int wh_54;

    QImage image;

    switch (frame.pixelFormat) {
    case QVideoFrame::Format_RGB32:
    case QVideoFrame::Format_ARGB32:
        image = argb_data_to_image(data, width, height, 0, 1, 2, 3);
        break;
    case QVideoFrame::Format_ARGB32_Premultiplied:
        image = argb_data_to_image(data, width, height, 0, 1, 2, 3, true);
        break;
    case QVideoFrame::Format_BGRA32:
        image = argb_data_to_image(data, width, height, 3, 2, 1, 0);
        break;
    case QVideoFrame::Format_BGRA32_Premultiplied:
        image = argb_data_to_image(data, width, height, 3, 2, 1, 0, true);
        break;
    case QVideoFrame::Format_BGR32:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
    case QVideoFrame::Format_ABGR32:
#endif
        image = argb_data_to_image(data, width, height, 0, 3, 2, 1);
        break;
    case QVideoFrame::Format_BGR24:
        image = argb_data_to_image(data, width, height, -1, 2, 1, 0);
        break;
    case QVideoFrame::Format_BGR555:
        /// This is a forced "conversion", colors end up swapped.
        image = QImage(data, width, height, QImage::Format_RGB555);
        break;
    case QVideoFrame::Format_BGR565:
        /// This is a forced "conversion", colors end up swapped.
        image = QImage(data, width, height, QImage::Format_RGB16);
        break;
    case QVideoFrame::Format_YUV420P:
        image = QImage(width, height, QImage::Format_RGB888);
        rgb = image.bits();
        wh = width * height;
        w_2 = width / 2;
        wh_54 = wh * 5 / 4;

        for (int y = 0; y < height; ++y) {
            int Y_offset = y * width;
            int y_2 = y / 2;
            int U_offset = y_2 * w_2 + wh;
            int V_offset = y_2 * w_2 + wh_54;
            for (int x = 0; x < width; ++x) {
                int x_2 = x / 2;
                uchar Y = data[Y_offset + x];
                uchar U = data[U_offset + x_2];
                uchar V = data[V_offset + x_2];
                yuv_to_rgb(Y, U, V, rgb, rgb + 1, rgb + 2);
                rgb += 3;
            }
        }
        break;
    case QVideoFrame::Format_NV12:
        /// nv12 format, encountered on macOS
        image = yuv_nv21_to_rgb(data, width, height);
        break;
        /// TODO: Handle (create QImages from) YUV formats.
    default:
        QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat);
        image = QImage(data, width, height, imageFormat);
        break;
    }

    if (image.isNull()) {
        qDebug() << "VideoFilterRunnable error: Cannot create image file to process.";
        qDebug() << "Maybe it was a format conversion problem? ";
        qDebug() << "VideoFrame format: " << frame.pixelFormat;
        qDebug() << "Image corresponding format: " << QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat);
        return;
    }

    if (m_filter->m_captureRect.isValid())
        image = image.copy(m_filter->m_captureRect);

    if (orientation)
        image = Utils::rotateImage(image, orientation);

    m_filter->m_tflite->run(image);
}
