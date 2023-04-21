import QtQuick

import TFLite

VideoFilterBasePage {
    id: root

    title: qsTr("Pose Estimation")

    tflite: PoseEstimation {
        id: poseEstimation

        modelFile: Settings.poseModel
        acceleration: Settings.nnapi
        threads: Settings.threads
        contentSize: Qt.size(videoOutput.width, videoOutput.height)
        score: Settings.score

        onResults: function(keypoints, scores) {
            canvas.keypoints = keypoints
            canvas.scores = scores
            canvas.requestPaint()
        }
    }

    errorString: qsTr("Error loading model file:")
        + "<br><br>%1<br><br>".arg(poseEstimation.errorString)
        + qsTr("Download a compatible pose estimation model from the address below, then load it in the settings page.")
        + "<br><br><a href='%1'>%1</a>"
          .arg("https://www.tensorflow.org/lite/models/trained")

    Canvas {
        id: canvas

        property var keypoints: []
        property var scores: []
        property var pairs: [
            [PoseEstimation.LeftWrist, PoseEstimation.LeftElbow],
            [PoseEstimation.LeftElbow, PoseEstimation.LeftShoulder],
            [PoseEstimation.LeftShoulder, PoseEstimation.RightShoulder],
            [PoseEstimation.RightShoulder, PoseEstimation.RightElbow],
            [PoseEstimation.RightElbow, PoseEstimation.RightWrist],
            [PoseEstimation.LeftShoulder, PoseEstimation.LeftHip],
            [PoseEstimation.RightShoulder, PoseEstimation.RightHip],
            [PoseEstimation.LeftHip, PoseEstimation.RightHip],
            [PoseEstimation.LeftHip, PoseEstimation.LeftKnee],
            [PoseEstimation.LeftKnee, PoseEstimation.LeftAnkle],
            [PoseEstimation.RightHip, PoseEstimation.RightKnee],
            [PoseEstimation.RightKnee, PoseEstimation.RightAnkle]
        ]

        function drawPoint(ctx, point) {
            ctx.ellipse(point.x - 5, point.y - 5, 10, 10)
        }

        function drawLine(ctx, from, to) {
            ctx.beginPath()
            ctx.moveTo(from.x, from.y)
            ctx.lineTo(to.x, to.y)
            ctx.stroke()
        }

        visible: root.active
        anchors.fill: parent

        onPaint: {
            var ctx = getContext('2d')
            ctx.reset()
            ctx.save()
            ctx.lineWidth = 2
            ctx.strokeStyle = "red"
            for (var i = 0; i < pairs.length; i++) {
                var bodypart = pairs[i][0]
                var bodyjoint = pairs[i][1]
                if (scores[bodypart] > poseEstimation.score &&
                    scores[bodyjoint] > poseEstimation.score) {
                    drawLine(ctx, keypoints[bodypart], keypoints[bodyjoint])
                }
            }
            ctx.fillStyle = "blue"
            for (var j = 0; j < keypoints.length; j++) {
                if (scores[j] > poseEstimation.score)
                    drawPoint(ctx, keypoints[j])
            }
            ctx.fill()
            ctx.restore()
        }
    }
}
