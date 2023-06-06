import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

import BaseUI as UI

import TFLite

UI.AppStackPage {
    id: root

    property alias active: videoFilter.active
    property alias tflite: videoFilter.tflite
    property alias errorString: messageLabel.text
    property alias videoOutput: vout

    leftButton: Action {
        icon.source: UI.Icons.arrow_back
        onTriggered: root.back()
    }

    rightButtons: [
        Action {
            icon.source: UI.Icons.more_vert
            onTriggered: optionsMenu.open()
        }
    ]

    Connections {
        target: Application
        function onStateChanged() {
            if (Application.state == Qt.ApplicationSuspended) {
                videoFilter.active = false
                camera.stop()
                videoFilter.clearSink()
            } else if (Application.state == Qt.ApplicatoinInactive) {
                videoFilter.active = false
            }
        }
    }

    MediaDevices {
        id: devices
    }

    Camera {
        id: camera

        active: false
        cameraDevice: devices.defaultVideoInput
    }

    CaptureSession {
        id: capture

        videoOutput: vout
        camera: camera
    }

    // Sometimes the camera fails to load properly.
    // To get around this problem start the camera after a delay.
    // https://bugreports.qt.io/browse/QTBUG-98559
    Timer {
        interval: 400
        running: true
        onTriggered: {
            camera.start()
            videoFilter.active = true
        }
    }

    VideoFilter {
        id: videoFilter

        orientation: vout.orientation
        captureRect: vout.sourceRect
        videoSink: vout.videoSink
    }

    VideoOutput {
        id: vout

        anchors.fill: parent
        fillMode: VideoOutput.PreserveAspectCrop
    }

    UI.LabelTitle {
        anchors { horizontalCenter: parent.horizontalCenter; top: parent.top }
        padding: 10
        color: "white"
        style: Text.Outline
        styleColor: "black"
        visible: Settings.showTime && videoFilter.active && root.tflite.inferenceTime > 0
        text: root.tflite.inferenceTime + " ms"
    }

    Menu {
        id: optionsMenu

        modal: true
        dim: false
        closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
        x: parent.width - width - 6
        y: -appToolBar.height + 6
        transformOrigin: Menu.TopRight

        onAboutToHide: currentIndex = -1 // reset highlighting

        MenuItem {
            text: videoFilter.active ? qsTr("Pause inference") : qsTr("Resume inference")
            onTriggered: {
                if (!videoFilter.active) {
                    camera.start()
                }
                videoFilter.active = !videoFilter.active
            }
        }
        MenuItem {
            text: qsTr("Select camera")
            onTriggered: camerasPopup.open()
        }
        MenuItem {
            text: qsTr("Select camera resolution")
            onTriggered: resolutionsPopup.open()
        }
    }

    UI.OptionsDialog {
        id: camerasPopup

        title: qsTr("Available cameras")
        model: devices.videoInputs
        delegate: RadioButton {
            checked: modelData === camera.cameraDevice
            text: modelData.description
            onClicked: {
                camera.stop()
                camera.cameraDevice = modelData
                camera.start()
                camerasPopup.close()
            }
        }
    }

    UI.OptionsDialog {
        id: resolutionsPopup

        property var pixelFormats: [
            "Invalid",
            "ARGB8888",
            "ARGB8888_Premultiplied",
            "XRGB8888",
            "BGRA8888",
            "BGRA8888_Premultiplied",
            "BGRX8888",
            "ABGR8888",
            "XBGR8888",
            "RGBA8888",
            "RGBX8888",
            "AYUV",
            "AYUV_Premultiplied",
            "YUV420P",
            "YUV422P",
            "YV12",
            "UYVY",
            "YUYV",
            "NV12",
            "NV21",
            "IMC1",
            "IMC2",
            "IMC3",
            "IMC4",
            "Y8",
            "Y16",
            "P010",
            "P016",
            "SamplerExternalOES",
            "Jpeg",
            "SamplerRect",
            "YUV420P10",
        ]

        title: qsTr("Available resolutions")
        model: camera.cameraDevice.videoFormats
        delegate: RadioButton {
            checked: modelData === camera.cameraFormat
            text: "%1x%2 (%3)"
                    .arg(modelData.resolution.width)
                    .arg(modelData.resolution.height)
                    .arg(resolutionsPopup.pixelFormats[modelData.pixelFormat])
            onClicked: {
                camera.stop()
                camera.cameraFormat = modelData
                camera.start()
                resolutionsPopup.close()
            }
        }
    }

    Rectangle {
        visible: root.tflite.errorString.length > 0

        anchors.centerIn: parent

        implicitWidth: root.width * (root.width < root.height ? 0.9 : 0.6)
        implicitHeight: Math.min(messageLabel.implicitHeight, root.height * 0.9)

        color: "darkred"
        opacity: 0.7
        border.color: "red"
        border.width: 2
        radius: 10

        UI.LabelSubheading {
            id: messageLabel

            anchors.fill: parent
            topPadding: 20
            bottomPadding: 20
            leftPadding: 8
            rightPadding: 8
            color: "white"
            textFormat: "RichText"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            onLinkActivated: Qt.openUrlExternally(link)
        }
    }
}
