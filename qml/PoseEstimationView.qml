import QtQuick

import TFLite

Item {
    id: root

    required property real active
    required property rect contentRect
    required property real score

    function repaint(keypoints, scores) {
        canvas.keypoints = keypoints
        canvas.scores = scores
        canvas.requestPaint()
    }

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
            ctx.ellipse(root.contentRect.x + point.x - 5, root.contentRect.y + point.y - 5, 10, 10)
        }

        function drawLine(ctx, from, to) {
            ctx.beginPath()
            ctx.moveTo(root.contentRect.x + from.x, root.contentRect.y + from.y)
            ctx.lineTo(root.contentRect.x + to.x, root.contentRect.y + to.y)
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
                if (scores[bodypart] > root.score &&
                    scores[bodyjoint] > root.score) {
                    drawLine(ctx, keypoints[bodypart], keypoints[bodyjoint])
                }
            }
            ctx.fillStyle = "blue"
            for (var j = 0; j < keypoints.length; j++) {
                if (scores[j] > root.score)
                    drawPoint(ctx, keypoints[j])
            }
            ctx.fill()
            ctx.restore()
        }
    }
}
