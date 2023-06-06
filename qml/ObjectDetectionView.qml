import QtQuick

import BaseUI as UI

Item {
    id: root

    required property rect contentRect
    required property bool active
    property alias model: boxes.model

    Repeater {
        id: boxes

        Rectangle {
            visible: root.active
            x: Math.max(root.contentRect.x + model.rect.x, 0)
            y: Math.max(root.contentRect.y + model.rect.y, 0)
            width: root.contentRect.x + model.rect.x < 0
                ? root.contentRect.x + model.rect.width
                : (x + model.rect.width < root.width ? model.rect.width : root.width - x)
            height: root.contentRect.y + model.rect.y < 0
                ? root.contentRect.y + model.rect.height
                : (y + model.rect.height < root.height ? model.rect.height : root.height - y)
            color: "transparent"
            border.color: model.color
            border.width: 2

            UI.LabelTitle {
                color: "white"
                text: "%1 %2%".arg(model.name).arg(Math.round(model.confidence * 100))

                background: Rectangle {
                    anchors.fill: parent
                    color: model.color
                }

                Component.onCompleted: {
                    if (parent.y + parent.height + implicitHeight > root.height)
                        anchors.bottom = parent.bottom
                    else
                        anchors.top = parent.bottom

                    if (parent.x + implicitWidth > root.width)
                        anchors.right = parent.right
                    else
                        anchors.left = parent.left
                }
            }
        }
    }
}
