#ifndef BOXESMODEL_H
#define BOXESMODEL_H

#include <QAbstractListModel>

#include <QRectF>
#include <QList>

struct Box {
    QRectF rect;
    QString name;
    int color;
    double confidence;
};

class BoxesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum {
        Rect = Qt::UserRole,
        Name,
        Color,
        Confidence
    };

    explicit BoxesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void reset(const QList<QRectF> &boxes = QList<QRectF>(),
               const QStringList &labels = QStringList(),
               const QList<double> &confidences = QList<double>());

private:
    QList<Box> m_boxes;
};

#endif // BOXESMODEL_H
