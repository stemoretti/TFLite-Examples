import QtQuick 2.12
import QtQuick.Controls 2.12
import QtMultimedia 5.12
import "../common"
import "../popups"

import Settings 1.0

import PoseEstimation 1.0
import VideoFilter 1.0

AppStackPage {
    id: root

    title: qsTr("Pose Estimation")

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
        onPoseModelChanged: {
            if (!videoFilter.active)
                videoFilter.active = true
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
        tflite: PoseEstimation {
            id: poseEstimation

            modelFile: Settings.poseModel
            acceleration: Settings.nnapi
            threads: Settings.threads
            contentSize: Qt.size(vout.width, vout.height)
            score: Settings.score
            onResults: {
                canvas.keypoints = keypoints
                canvas.scores = scores
                canvas.requestPaint()
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

    LabelTitle {
        anchors { horizontalCenter: parent.horizontalCenter; top: parent.top }
        padding: 10
        color: "white"
        style: Text.Outline
        styleColor: "black"
        visible: Settings.showTime && videoFilter.active && poseEstimation.inferenceTime > 0
        text: poseEstimation.inferenceTime + " ms"
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
        visible: poseEstimation.errorString.length > 0

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
                  poseEstimation.errorString + qsTr(
                      "<br><br>Please download a compatible pose estimation model from " +
                      "<a href='https://www.tensorflow.org/lite/models/pose_estimation/overview'>" +
                      "www.tensorflow.org/lite/models/pose_estimation/overview</a>, " +
                      "then load it in the settings page.")
            color: "white"
            textFormat: "RichText"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            onLinkActivated: Qt.openUrlExternally(link)
        }
    }
}
