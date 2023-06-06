import QtQuick

import BaseUI as UI

import TFLite

CameraPage {
    id: root

    title: qsTr("Image Classification")

    tflite: ImageClassification {
        id: imageClassification

        modelFile: Settings.classifierModel
        labelsFile: Settings.classifierLabels
        acceleration: Settings.nnapi
        threads: Settings.threads
        threshold: Settings.threshold

        onResults: function(captions) { captionsList.model = captions }
    }

    errorString: qsTr("Error loading model file:")
        + "<br><br>%1<br><br>".arg(imageClassification.errorString)
        + qsTr("Download a compatible image classification model from the address below, then load it in the settings page.")
        + "<br><br><a href='%1'>%1</a>"
          .arg("https://www.tensorflow.org/lite/models/trained")

    ImageClassificationView {
        id: captionsList
        anchors.fill: parent
        active: root.active
    }
}
