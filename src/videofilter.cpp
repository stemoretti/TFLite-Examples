#include "videofilter.h"

#include <QDebug>
#include <QtConcurrent>

#include <algorithm>

#include "utils.h"
#include "tflite.h"

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

    m_tflite = dynamic_cast<TFLite<QImage> *>(tflite);
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

    m_frame.copyData(input);

    m_filter->m_processThread
            = QtConcurrent::run(this,
                                &VideoFilterRunnable::processVideoFrame,
                                -m_filter->m_orientation,
                                m_filter->m_captureRect);

    return *input;
}

void VideoFilterRunnable::processVideoFrame(int orientation, QRect captureRect)
{
    if (m_frame.dataSize < 1) {
        qDebug() << "VideoFilterRunnable: Buffer is empty";
        return;
    }

    int width = m_frame.size.width();
    int height = m_frame.size.height();
    const uchar *data = m_frame.data.get();

    QImage image;

    switch (m_frame.pixelFormat) {
    case QVideoFrame::Format_RGB32:
    case QVideoFrame::Format_ARGB32:
        image = Utils::argb_data_to_image(data, width, height, 0, 1, 2, 3);
        break;
    case QVideoFrame::Format_ARGB32_Premultiplied:
        image = Utils::argb_data_to_image(data, width, height, 0, 1, 2, 3, true);
        break;
    case QVideoFrame::Format_BGRA32:
        image = Utils::argb_data_to_image(data, width, height, 3, 2, 1, 0);
        break;
    case QVideoFrame::Format_BGRA32_Premultiplied:
        image = Utils::argb_data_to_image(data, width, height, 3, 2, 1, 0, true);
        break;
    case QVideoFrame::Format_BGR32:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
    case QVideoFrame::Format_ABGR32:
#endif
        image = Utils::argb_data_to_image(data, width, height, 0, 3, 2, 1);
        break;
    case QVideoFrame::Format_BGR24:
        image = Utils::argb_data_to_image(data, width, height, -1, 2, 1, 0);
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
        image = Utils::yuv_420p_to_rgb(data, width, height);
        break;
    case QVideoFrame::Format_NV12:
        /// nv12 format, encountered on macOS
        image = Utils::yuv_nv21_to_rgb(data, width, height);
        break;
        /// TODO: Handle (create QImages from) YUV formats.
    default:
        QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(m_frame.pixelFormat);
        image = QImage(data, width, height, imageFormat);
        break;
    }

    if (image.isNull()) {
        qDebug() << "VideoFilterRunnable error: Cannot create image file to process.";
        qDebug() << "Maybe it was a format conversion problem? ";
        qDebug() << "VideoFrame format: " << m_frame.pixelFormat;
        qDebug() << "Image corresponding format: " << QVideoFrame::imageFormatFromPixelFormat(m_frame.pixelFormat);
        return;
    }

    if (captureRect.isValid())
        image = image.copy(captureRect);

    if (orientation)
        image = Utils::rotateImage(image, orientation);

    if (!m_filter->m_tflite->run(image))
        m_filter->setActive(false);
}
