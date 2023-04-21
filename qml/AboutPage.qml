import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import BaseUI as UI

UI.AppStackPage {
    id: root

    title: qsTr("About")
    padding: 10

    leftButton: Action {
        icon.source: UI.Icons.arrow_back
        onTriggered: root.back()
    }

    Flickable {
        contentHeight: aboutPane.implicitHeight
        anchors.fill: parent

        Pane {
            id: aboutPane

            anchors.fill: parent

            ColumnLayout {
                width: parent.width

                UI.LabelTitle {
                    text: Qt.application.displayName
                    horizontalAlignment: Qt.AlignHCenter
                }

                UI.LabelBody {
                    text: "<a href='%1'>%1</a>".arg("https://github.com/stemoretti/tflite-examples")
                    linkColor: UI.Style.isDarkTheme ? "lightblue" : "blue"
                    horizontalAlignment: Qt.AlignHCenter
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }
        }
    }
}
