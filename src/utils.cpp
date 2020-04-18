#include "utils.h"

#include <QFile>
#include <QPoint>
#include <QMatrix>

#include <algorithm>

namespace Utils
{

QImage rotateImage(QImage img, double rotation)
{
    QPoint center = img.rect().center();
    QMatrix matrix;
    matrix.translate(center.x(), center.y());
    matrix.rotate(rotation);

    return img.transformed(matrix);
}

QStringList readLabels(const QString &filename)
{
    QStringList labels;

    if (!filename.trimmed().isEmpty()) {
        QFile textFile(filename);

        if (textFile.exists() && textFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString text = textFile.readAll();
            labels = text.split('\n');
            std::for_each(labels.begin(), labels.end(), [](QString &label) {
                label = label.trimmed();
                if (!label.isEmpty())
                    label[0] = label[0].toUpper();
            });
            textFile.close();
        }
    }

    return labels;
}

void yuv_to_rgb(uchar y, uchar u, uchar v, uchar *r, uchar *g, uchar *b)
{
    int rTmp = y + (1.370705 * (v - 128));
    int gTmp = y - (0.698001 * (v - 128)) - (0.337633 * (u - 128));
    int bTmp = y + (1.732446 * (u - 128));

    *r = std::clamp(rTmp, 0, 255);
    *g = std::clamp(gTmp, 0, 255);
    *b = std::clamp(bTmp, 0, 255);
}

QImage yuv_420p_to_rgb(const uchar *yuv, int width, int height)
{
    uchar *rgb;
    int wh;
    int w_2;
    int wh_54;

    QImage image = QImage(width, height, QImage::Format_RGB888);
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
            uchar Y = yuv[Y_offset + x];
            uchar U = yuv[U_offset + x_2];
            uchar V = yuv[V_offset + x_2];
            yuv_to_rgb(Y, U, V, rgb, rgb + 1, rgb + 2);
            rgb += 3;
        }
    }

    return image;
}

QImage yuv_nv21_to_rgb(const uchar *yuv, int width, int height)
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

QImage argb_data_to_image(const uchar *data, int width, int height,
                          int alpha, int red, int green, int blue,
                          bool isPremultiplied)
{
    int stride = (alpha < 0 ? 3 : 4);

    QImage image(width, height, QImage::Format_RGB888);

    for (int y = 1; y <= height; ++y) {
        // Quick fix for iOS devices. Will be handled better in the future
#ifdef Q_OS_IOS
        uchar *rgb = image.scanLine(y - 1);
#else
        uchar *rgb = image.scanLine(height - y);
#endif
        for (int x = 0; x < width; ++x) {
            // QTBUG-69968: endianess dependent format
            uint32_t pixel = *reinterpret_cast<const uint32_t *>(data);
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

template<>
float *TensorData(TfLiteTensor *tensor, std::vector<int> indices)
{
    uint dimsIndex = tensor->dims->size - 1;
    if (indices.size() > dimsIndex)
        return nullptr;

    int nelems = 0;
    int skip = 1;

    while (indices.size()) {
        nelems += skip * indices.back();
        indices.pop_back();
        skip *= tensor->dims->data[dimsIndex];
        dimsIndex--;
    }

    if (tensor->type == kTfLiteFloat32)
        return tensor->data.f + nelems;

    qDebug() << "Should not reach here!";

    return nullptr;
}

template<>
uint8_t *TensorData(TfLiteTensor *tensor, std::vector<int> indices)
{
    uint dimsIndex = tensor->dims->size - 1;
    if (indices.size() > dimsIndex)
        return nullptr;

    int nelems = 0;
    int skip = 1;

    while (indices.size()) {
        nelems += skip * indices.back();
        indices.pop_back();
        skip *= tensor->dims->data[dimsIndex];
        dimsIndex--;
    }

    if (tensor->type == kTfLiteUInt8)
        return tensor->data.uint8 + nelems;

    qDebug() << "Should not reach here!";

    return nullptr;
}

}
