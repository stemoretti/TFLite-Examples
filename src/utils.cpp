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

}
