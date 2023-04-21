import QtQuick

import BaseUI as UI

import TFLite

VideoFilterBasePage {
    id: root

    title: qsTr("Image Classification")

    tflite: ImageClassification {
        id: imageClassification

        modelFile: Settings.classifierModel
        labelsFile: Settings.classifierLabels
        acceleration: Settings.nnapi
        threads: Settings.threads
        threshold: Settings.threshold

        onResults: function(captions) {
            captionsList.model = captions
        }
    }

    errorString: qsTr("Error loading model file:")
        + "<br><br>%1<br><br>".arg(imageClassification.errorString)
        + qsTr("Download a compatible image classification model from the address below, then load it in the settings page.")
        + "<br><br><a href='%1'>%1</a>"
          .arg("https://www.tensorflow.org/lite/models/trained")

    ListView {
        id: captionsList

        anchors { left: parent.left; right: parent.right; margins: 10 }
        y: parent.height - contentHeight
        height: contentHeight
        interactive: false
        visible: root.active

        delegate: UI.LabelTitle {
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
            style: Text.Outline
            styleColor: "black"
            opacity: { return [1, 0.7, 0.4, 0.4, 0.4][index] }
            text: modelData.caption + " " + (modelData.confidence * 100).toFixed(1) + "%"
        }
    }
}
