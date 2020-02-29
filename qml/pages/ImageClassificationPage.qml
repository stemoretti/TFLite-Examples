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

    Component.onCompleted: {
        loadCamera.start()
        checkModel.start()
    }

    Timer {
        id: loadCamera

        repeat: false
        interval: 300
        onTriggered: {
            vout.source = camera
            camera.start()
        }
    }

    Timer {
        id: checkModel

        repeat: false
        interval: 2000
        onTriggered: {
            if (!imageClassification.isInitialized())
                modelPopup.open()
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
        anchors { left: parent.left; right: parent.right; margins: 10 }
        y: parent.height - contentHeight
        height: contentHeight
        interactive: false
        visible: videoFilter.active
        model: imageClassification.captions
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

    PopupInfo {
        id: modelPopup

        property string message:
            qsTr("<br>Please download an image classification model from " +
                 "<a href='https://www.tensorflow.org/lite/models/image_classification/overview'>" +
                 "www.tensorflow.org/lite/models/image_classification/overview</a>, " +
                 "then load it in the settings page.")

        text: (Settings.classifierModel.length > 0 ?
                   qsTr("Invalid model file.") :
                   qsTr("Model filename empty.")) + message
    }
}