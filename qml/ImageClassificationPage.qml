import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import BaseUI as UI

import TFLite as TFL

MainPage {
    id: root

    title: qsTr("Image Classification")

    ColumnLayout {
        anchors { left: parent.left; right: parent.right }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter

            ToolButton {
                icon.source: UI.Icons.camera
                text: "Camera"
                onClicked: stack.push(Qt.resolvedUrl("ImageClassificationCameraPage.qml"))
            }
            ToolButton {
                icon.source: UI.Icons.video_library
                text: "Video"
                onClicked: stack.push(Qt.resolvedUrl("ImageClassificationVideoPage.qml"))
            }
        }

        UI.SettingsItem {
            title: qsTr("Image classification model")
            subtitle: _formatFilename(TFL.Settings.classifierModel)
            subtitlePlaceholder: qsTr("No file selected")
            onClicked: selectModelFile(title, "classifierModel")
        }

        UI.SettingsItem {
            title: qsTr("Image classification labels")
            subtitle: _formatFilename(TFL.Settings.classifierLabels)
            subtitlePlaceholder: qsTr("No file selected")
            onClicked: selectModelFile(title, "classifierLabels")
        }

        UI.SettingsItem {
            title: qsTr("Classification threshold")
            subtitle: (TFL.Settings.threshold * 100).toFixed(1) + " %"
            onClicked: thresholdPopup.open()
        }
    }

    CustomDialog {
        id: thresholdPopup

        onAccepted: TFL.Settings.threshold = thresholdSpin.value / 1000

        SpinBox {
            id: thresholdSpin

            anchors.centerIn: parent
            from: 1
            value: Math.round(TFL.Settings.threshold * 1000)
            to: 1000
            stepSize: 1

            validator: IntValidator {
                bottom: thresholdSpin.from
                top: thresholdSpin.to
            }

            textFromValue: function(value, locale) {
                return value / 10 + " %"
            }

            valueFromText: function(text, locale) {
                return Number.fromLocaleString(locale, text) * 10
            }
        }
    }
}
