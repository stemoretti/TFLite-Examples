import QtQuick

import BaseUI as UI

import TFLite

VideoFilterBasePage {
    id: root

    title: qsTr("Object Detection")

    tflite: ObjectDetection {
        id: objectDetection

        modelFile: Settings.objectsModel
        labelsFile: Settings.objectsLabels
        acceleration: Settings.nnapi
        threads: Settings.threads
        contentSize: Qt.size(videoOutput.width, videoOutput.height)
        confidence: Settings.confidence
    }

    errorString: qsTr("Error loading model file:")
        + "<br><br>%1<br><br>".arg(objectDetection.errorString)
        + qsTr("Download a compatible object detection model from the address below, then load it in the settings page.")
        + "<br><br><a href='%1'>%1</a>"
          .arg("https://www.tensorflow.org/lite/models/trained")

    Repeater {
        model: objectDetection.boxes

        Rectangle {
            visible: root.active
            x: model.rect.x
            y: model.rect.y
            width: model.rect.width
            height: model.rect.height
            color: "transparent"
            border.color: model.color
            border.width: 2

            UI.LabelTitle {
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
}
