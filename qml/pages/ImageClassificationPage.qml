import QtQuick 2.12
import QtQuick.Controls 2.12
import QtMultimedia 5.12
import "../common"
import "../popups"

import Settings 1.0

import ImageClassification 1.0
import VideoFilter 1.0

AppStackPage {
    id: root

    title: qsTr("Image Classification")

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

    // If the video output source is set in the component the camera will fail
    // to load properly.
    // To get around this problem the video output source is set after a delay.
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

    Connections {
        target: Settings
        onClassifierModelChanged: {
            if (!videoFilter.active)
                videoFilter.active = true
        }
    }

    VideoFilter {
        id: videoFilter

        orientation: vout.orientation
        tflite: ImageClassification {
            id: imageClassification

            modelFile: Settings.classifierModel
            labelsFile: Settings.classifierLabels
            acceleration: Settings.nnapi
            threads: Settings.threads
            threshold: Settings.threshold
            onResults: {
                captionsList.model = captions
            }
        }
    }

    VideoOutput {
        id: vout

        anchors.fill: parent
        autoOrientation: true
        fillMode: VideoOutput.PreserveAspectCrop
        filters: [videoFilter]
    }

    ListView {
        id: captionsList

        anchors { left: parent.left; right: parent.right; margins: 10 }
        y: parent.height - contentHeight
        height: contentHeight
        interactive: false
        visible: videoFilter.active
        delegate: LabelTitle {
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
            style: Text.Outline
            styleColor: "black"
            opacity: {
                return [1, 0.7, 0.4, 0.4, 0.4][index]
            }
            text: modelData.caption + " " + (modelData.confidence * 100).toFixed(1) + "%"
        }
    }

    LabelTitle {
        anchors { horizontalCenter: parent.horizontalCenter; top: parent.top }
        padding: 10
        color: "white"
        style: Text.Outline
        styleColor: "black"
        visible: Settings.showTime && videoFilter.active && imageClassification.inferenceTime > 0
        text: imageClassification.inferenceTime + " ms"
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
        visible: imageClassification.errorString.length > 0

        x: (parent.width - width) / 2
        y: (parent.height - height) / 2 - appToolBar.height / 2
        implicitWidth: appWindow.width * (isPortrait ? 0.9 : 0.6)
        implicitHeight: Math.min(messageLabel.implicitHeight, appWindow.height * 0.9)

        color: "darkred"
        opacity: 0.7
        border.color: "red"
        border.width: 2
        radius: 10

        LabelSubheading {
            id: messageLabel

            anchors.fill: parent
            topPadding: 20
            bottomPadding: 20
            leftPadding: 8
            rightPadding: 8
            text: qsTr("Error loading model file:<br><br>") +
                  imageClassification.errorString + qsTr(
                      "<br><br>Please download a compatible image classification model from " +
                      "<a href='https://www.tensorflow.org/lite/models/image_classification/overview'>" +
                      "www.tensorflow.org/lite/models/image_classification/overview</a>, " +
                      "then load it in the settings page.")
            color: "white"
            textFormat: "RichText"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            onLinkActivated: Qt.openUrlExternally(link)
        }
    }
}
