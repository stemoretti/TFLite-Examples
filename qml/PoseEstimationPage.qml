import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import BaseUI as UI

import TFLite as TFL

MainPage {
    id: root

    title: qsTr("Pose Estimation")

    ColumnLayout {
        anchors { left: parent.left; right: parent.right }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter

            ToolButton {
                icon.source: UI.Icons.camera
                text: "Camera"
                onClicked: stack.push(Qt.resolvedUrl("PoseEstimationCameraPage.qml"))
            }
            ToolButton {
                icon.source: UI.Icons.video_library
                text: "Video"
                onClicked: stack.push(Qt.resolvedUrl("PoseEstimationVideoPage.qml"))
            }
        }

        UI.SettingsItem {
            title: qsTr("Pose estimation model")
            subtitle: _formatFilename(TFL.Settings.poseModel)
            subtitlePlaceholder: qsTr("No file selected")
            onClicked: selectModelFile(title, "poseModel")
        }

        UI.SettingsItem {
            title: qsTr("Minimum score")
            subtitle: TFL.Settings.score.toFixed(1)
            onClicked: scorePopup.open()
        }
    }

    CustomDialog {
        id: scorePopup

        onAccepted: TFL.Settings.score = scoreSpin.value / 10

        SpinBox {
            id: scoreSpin

            anchors.centerIn: parent
            from: 1
            value: Math.round(TFL.Settings.score * 10)
            to: 10

            validator: IntValidator {
                bottom: scoreSpin.from
                top: scoreSpin.to
            }

            textFromValue: function(value, locale) {
                return value / 10
            }

            valueFromText: function(text, locale) {
                return Number.fromLocaleString(locale, text) * 10
            }
        }
    }
}
