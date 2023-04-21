#include "utils.h"

#include <QDebug>
#include <QFile>

#ifndef Q_OS_ANDROID
#include <QUrl>
#endif

#include <algorithm>

namespace Utils
{

QStringList readLabels(const QString &filename)
{
    QStringList labels;

    if (!filename.trimmed().isEmpty()) {
        QFile textFile;
#ifdef Q_OS_ANDROID
        textFile.setFileName(filename);
#else
        textFile.setFileName(QUrl(filename).toLocalFile());
#endif

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

int tensorElementsNumber(TfLiteTensor *tensor, std::vector<int> indices)
{
    uint dimsIndex = tensor->dims->size - 1;

    if (indices.size() > dimsIndex)
        return -1;

    int nelems = 0;
    int skip = 1;

    while (indices.size()) {
        nelems += skip * indices.back();
        indices.pop_back();
        skip *= tensor->dims->data[dimsIndex];
        dimsIndex--;
    }

    return nelems;
}

template<>
float *TensorData(TfLiteTensor *tensor, std::vector<int> indices)
{
    int nelems = tensorElementsNumber(tensor, indices);

    if (nelems < 0)
        return nullptr;

    if (tensor->type == kTfLiteFloat32)
        return tensor->data.f + nelems;

    qDebug() << "Should not reach here!";

    return nullptr;
}

template<>
uint8_t *TensorData(TfLiteTensor *tensor, std::vector<int> indices)
{
    int nelems = tensorElementsNumber(tensor, indices);

    if (nelems < 0)
        return nullptr;

    if (tensor->type == kTfLiteUInt8)
        return tensor->data.uint8 + nelems;

    qDebug() << "Should not reach here!";

    return nullptr;
}

}
