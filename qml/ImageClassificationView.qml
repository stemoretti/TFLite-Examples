import QtQuick

import BaseUI as UI

Item {
    id: root

    required property bool active
    property alias model: captionsList.model

    ListView {
        id: captionsList

        anchors { left: parent.left; right: parent.right; margins: 10 }
        y: parent.height - contentHeight
        height: contentHeight
        interactive: false
        visible: root.active

        delegate: UI.LabelTitle {
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
            style: Text.Outline
            styleColor: "black"
            opacity: { return [1, 0.7, 0.4, 0.4, 0.4][index] }
            text: "%1 %2%".arg(modelData.caption).arg((modelData.confidence * 100).toFixed(1))
        }
    }
}
