import QtQuick 2.12
import QtQuick.Controls 2.12
import QtMultimedia 5.12
import "../common"
import "../popups"

import Settings 1.0

import ObjectDetection 1.0
import VideoFilter 1.0

AppStackPage {
    id: root

    title: qsTr("Object Detection")

    leftButton: Action {
        icon.source: "image://icon/menu"
        onTriggered: navDrawer.open()
    }

    rightButtons: [
        Action {
            icon.source: "image://icon/more_vert"
            onTriggered: optionsMenu.open()
        }
    ]

    Component.onCompleted: {
        loadCamera.start()
    }

    Timer {
        id: loadCamera

        interval: 300
        onTriggered: {
            vout.source = camera
            camera.start()
        }
    }

    VideoFilter {
        id: videoFilter

        orientation: vout.orientation
        captureRect: {
            vout.contentRect
            vout.sourceRect
            return vout.mapRectToSource(Qt.rect(0, 0, root.width, root.height))
        }
        tflite: ObjectDetection {
            id: objectDetection

            modelFile: Settings.objectsModel
            labelsFile: Settings.objectsLabels
            acceleration: Settings.nnapi
            threads: Settings.threads
            contentSize: Qt.size(vout.width, vout.height)
            confidence: Settings.confidence
        }
    }

    VideoOutput {
        id: vout

        anchors.fill: parent
        autoOrientation: true
        fillMode: VideoOutput.PreserveAspectCrop
        filters: [videoFilter]
    }

    Repeater {
        model: objectDetection.boxes

        Rectangle {
            visible: videoFilter.active
            x: model.rect.x
            y: model.rect.y
            width: model.rect.width
            height: model.rect.height
            color: "transparent"
            border.color: model.color
            border.width: 2

            LabelTitle {
                color: "white"
                text: model.name + " " + Math.round(model.confidence * 100) + "%"
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

    LabelTitle {
        anchors { horizontalCenter: parent.horizontalCenter; top: parent.top }
        padding: 10
        color: "white"
        style: Text.Outline
        styleColor: "black"
        visible: Settings.showTime && videoFilter.active && objectDetection.inferenceTime > 0
        text: objectDetection.inferenceTime + " ms"
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
    }

    ListPopup {
        id: camerasPopup

        model: QtMultimedia.availableCameras
        delegateFunction: function (type) { return type.displayName }
        onClicked: {
            camera.stop()
            camera.deviceId = data.deviceId
            camera.start()
            close()
        }
    }

    Rectangle {
        id: modelMessage

        visible: objectDetection.errorString.length > 0

        x: (parent.width - width) / 2
        y: (parent.height - height) / 2 - appToolBar.height / 2
        implicitWidth: appWindow.width * (isPortrait ? 0.9 : 0.6)
        implicitHeight: Math.min(messageLabel.implicitHeight, appWindow.height * 0.9)

        color: "red"
        opacity: 0.7
        border.color: "darkred"
        border.width: 2
        radius: 10

        LabelSubheading {
            id: messageLabel

            property string message:
                qsTr("<br>Please download an object detection model from " +
                     "<a href='https://www.tensorflow.org/lite/models/object_detection/overview'>" +
                     "www.tensorflow.org/lite/models/object_detection/overview</a>, " +
                     "then load it in the settings page.")

            anchors.fill: parent
            topPadding: 20
            bottomPadding: 20
            leftPadding: 8
            rightPadding: 8
            text: objectDetection.errorString + "<br>" +
                  (Settings.objectsModel.length > 0 ?
                       qsTr("Invalid model file.") :
                       qsTr("Model filename empty.")) +
                  message
            color: "white"
            opacity: 1.0
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            linkColor: isDarkTheme ? "lightblue" : "blue"
            onLinkActivated: Qt.openUrlExternally(link)
        }
    }
}
