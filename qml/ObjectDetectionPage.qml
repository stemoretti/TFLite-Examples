import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import BaseUI as UI

import TFLite as TFL

MainPage {
    id: root

    title: qsTr("Object Detection")

    ColumnLayout {
        anchors { left: parent.left; right: parent.right }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter

            ToolButton {
                icon.source: UI.Icons.camera
                text: "Camera"
                onClicked: stack.push(Qt.resolvedUrl("ObjectDetectionCameraPage.qml"))
            }
            ToolButton {
                icon.source: UI.Icons.video_library
                text: "Video"
                onClicked: stack.push(Qt.resolvedUrl("ObjectDetectionVideoPage.qml"))
            }
        }

        UI.SettingsItem {
            title: qsTr("Objects detection model")
            subtitle: _formatFilename(TFL.Settings.objectsModel)
            subtitlePlaceholder: qsTr("No file selected")
            onClicked: selectModelFile(title, "objectsModel")
        }

        UI.SettingsItem {
            title: qsTr("Objects detection labels")
            subtitle: _formatFilename(TFL.Settings.objectsLabels)
            subtitlePlaceholder: qsTr("No file selected")
            onClicked: selectModelFile(title, "objectsLabels")
        }

        UI.SettingsItem {
            title: qsTr("Minimum confidence")
            subtitle: Math.round(TFL.Settings.confidence * 100) + " %"
            onClicked: confidencePopup.open()
        }
    }

    CustomDialog {
        id: confidencePopup

        onAccepted: TFL.Settings.confidence = confidenceSpin.value / 100

        SpinBox {
            id: confidenceSpin

            anchors.centerIn: parent
            from: 1
            value: Math.round(TFL.Settings.confidence * 100)
            to: 100
            stepSize: 1

            validator: IntValidator {
                bottom: confidenceSpin.from
                top: confidenceSpin.to
            }

            textFromValue: function(value, locale) {
                return value + " %"
            }

            valueFromText: function(text, locale) {
                return Number.fromLocaleString(locale, text)
            }
        }
    }
}
