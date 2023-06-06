import QtQuick

import BaseUI as UI

import TFLite

VideoPage {
    id: root

    title: qsTr("Object Detection")

    tflite: ObjectDetection {
        id: objectDetection

        modelFile: Settings.objectsModel
        labelsFile: Settings.objectsLabels
        acceleration: Settings.nnapi
        threads: Settings.threads
        contentSize: Qt.size(videoOutput.contentRect.width, videoOutput.contentRect.height)
        confidence: Settings.confidence
    }

    errorString: qsTr("Error loading model file:")
        + "<br><br>%1<br><br>".arg(objectDetection.errorString)
        + qsTr("Download a compatible object detection model from the address below, then load it in the settings page.")
        + "<br><br><a href='%1'>%1</a>"
          .arg("https://www.tensorflow.org/lite/models/trained")

    ObjectDetectionView {
        anchors.fill: parent

        active: root.active
        model: objectDetection.boxes
        contentRect: root.videoOutput.contentRect
    }
}
