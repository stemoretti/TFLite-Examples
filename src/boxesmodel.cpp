#include "boxesmodel.h"

#include <QColor>

BoxesModel::BoxesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int BoxesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_boxes.size();
}

QVariant BoxesModel::data(const QModelIndex &index, int role) const
{
    static const QList<QColor> defColors = {
        "#F6A625", "#99CA53", "#2097D2", "#B5563D", "#7264D6"
    };

    if (!index.isValid())
        return QVariant();

    switch (role) {
    case Rect:
        return m_boxes[index.row()].rect;
    case Name:
        return m_boxes[index.row()].name;
    case Color:
        return defColors[m_boxes[index.row()].color % defColors.size()];
    case Confidence:
        return m_boxes[index.row()].confidence;
    }

    return QVariant();
}

QHash<int, QByteArray> BoxesModel::roleNames() const
{
    static QHash<int, QByteArray> roles = {
        { Rect, "rect" },
        { Name, "name" },
        { Color, "color" },
        { Confidence, "confidence" }
    };
    return roles;
}

void BoxesModel::reset(const QList<QRectF> &boxes,
                       const QStringList &labels,
                       const QList<float> &confidences)
{
    beginResetModel();

    m_boxes.clear();

    if (boxes.size() > 0) {
        QStringList uniqueLabels = labels;
        uniqueLabels.removeDuplicates();

        for (int i = 0; i < boxes.size(); ++i) {
            Box box;
            box.rect = boxes[i];
            box.name = labels[i];
            box.confidence = confidences[i];
            box.color = uniqueLabels.indexOf(box.name);
            m_boxes.append(box);
        }
    }

    endResetModel();
}
