import QtQuick

import TFLite

CameraPage {
    id: root

    title: qsTr("Pose Estimation")

    tflite: PoseEstimation {
        id: poseEstimation

        modelFile: Settings.poseModel
        acceleration: Settings.nnapi
        threads: Settings.threads
        contentSize: Qt.size(videoOutput.contentRect.width, videoOutput.contentRect.height)
        score: Settings.score

        onResults: (keypoints, scores) => { poseEstimationView.repaint(keypoints, scores) }
    }

    errorString: qsTr("Error loading model file:")
        + "<br><br>%1<br><br>".arg(poseEstimation.errorString)
        + qsTr("Download a compatible pose estimation model from the address below, then load it in the settings page.")
        + "<br><br><a href='%1'>%1</a>"
          .arg("https://www.tensorflow.org/lite/models/trained")

    PoseEstimationView {
        id: poseEstimationView
        anchors.fill: parent
        active: root.active
        contentRect: videoOutput.contentRect
        score: poseEstimation.score
    }
}
