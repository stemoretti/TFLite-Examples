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
        icon.source: UI.Icons.menu
        onTriggered: navDrawer.open()
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
        captureRect: vout.contentRect
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
            onTriggered: videoFilter.active = !videoFilter.active
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

        title: qsTr("Available resolutions")
        model: camera.cameraDevice.videoFormats
        delegate: RadioButton {
            checked: modelData === camera.cameraFormat
            text: "%1x%2".arg(modelData.resolution.width).arg(modelData.resolution.height)
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

    Drawer {
        id: navDrawer

        interactive: stack?.depth === 1
        width: Math.min(240, Math.min(parent.width, parent.height) / 3 * 2 )
        height: parent.height

        onAboutToShow: menuColumn.enabled = true

        Flickable {
            anchors.fill: parent
            contentHeight: menuColumn.implicitHeight
            boundsBehavior: Flickable.StopAtBounds

            ColumnLayout {
                id: menuColumn

                anchors { left: parent.left; right: parent.right }
                spacing: 0

                Label {
                    text: Qt.application.displayName
                    color: Material.foreground
                    font.pixelSize: UI.Style.fontSizeHeadline
                    padding: (root.appToolBar.implicitHeight - contentHeight) / 2
                    leftPadding: 20
                    Layout.fillWidth: true
                }

                UI.HorizontalListDivider {}

                Repeater {
                    id: actionsList

                    model: [
                        {
                            icon: UI.Icons.account_box,
                            text: QT_TR_NOOP("Object Detection"),
                            page: "ObjectDetectionPage.qml"
                        },
                        {
                            icon: UI.Icons.local_florist,
                            text: QT_TR_NOOP("Image Classification"),
                            page: "ImageClassificationPage.qml"
                        },
                        {
                            icon: UI.Icons.directions_walk,
                            text: QT_TR_NOOP("Pose Estimation"),
                            page: "PoseEstimationPage.qml"
                        }
                    ]

                    delegate: ItemDelegate {
                        icon.source: modelData.icon
                        text: qsTr(modelData.text)

                        Layout.fillWidth: true

                        onClicked: {
                            videoFilter.active = false
                            camera.stop()
                            // Disable, or a double click will push the page twice.
                            menuColumn.enabled = false
                            navDrawer.close()
                            stack.replace(Qt.resolvedUrl(modelData.page))
                        }
                    }
                }

                UI.HorizontalListDivider { }

                Repeater {
                    id: pageList

                    model: [
                        {
                            icon: UI.Icons.settings,
                            text: QT_TR_NOOP("Settings"),
                            page: "SettingsPage.qml"
                        },
                        {
                            icon: UI.Icons.info_outline,
                            text: QT_TR_NOOP("About"),
                            page: "AboutPage.qml"
                        }
                    ]

                    delegate: ItemDelegate {
                        icon.source: modelData.icon
                        text: qsTr(modelData.text)

                        Layout.fillWidth: true

                        onClicked: {
                            // Disable, or a double click will push the page twice.
                            menuColumn.enabled = false
                            navDrawer.close()
                            stack.push(Qt.resolvedUrl(modelData.page))
                        }
                    }
                }
            }
        }
    }
}
