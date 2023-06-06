import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtMultimedia

import BaseUI as UI

import TFLite as TFL

UI.AppStackPage {
    id: root

    property alias active: videoFilter.active
    property alias tflite: videoFilter.tflite
    property alias errorString: messageLabel.text
    property alias videoOutput: vout

    function _timeFormat(time) {
        var seconds = parseInt(time / 1000) % 60
        var minutes = parseInt(time / 60000) % 60
        var hours = parseInt(time / (1000 * 60 * 60))

        if (seconds < 10)
            seconds = "0" + seconds
        if (minutes < 10)
            minutes = "0" + minutes

        return (hours > 0 ? hours + ":" : "") + minutes + ":" + seconds
    }

    leftButton: Action {
        icon.source: UI.Icons.arrow_back
        onTriggered: root.back()
    }

    rightButtons: [
        Action {
            icon.source: UI.Icons.more_vert
            onTriggered: optionsMenu.open()
        }
    ]

    footer: Pane {
        id: mainPane

        ColumnLayout {
            width: parent.width

            RowLayout {
                UI.LabelBody {
                    id: playTime

                    text: _timeFormat(mediaPlayer.position)

                    Binding {
                        target: playTime
                        property: "text"
                        value: _timeFormat(slider.value)
                        when: slider.pressed
                    }
                }

                UI.LabelBody {
                    text: _timeFormat(mediaPlayer.duration)
                    horizontalAlignment: Text.AlignRight
                }
            }

            Slider {
                id: slider

                enabled: mediaPlayer.seekable
                from: 0
                to: mediaPlayer.duration

                Layout.fillWidth: true

                onPressedChanged: {
                    if (mediaPlayer.seekable && !pressed && hovered) {
                        playTime.text = _timeFormat(mediaPlayer.position)
                        mediaPlayer.position = value
                    }
                }

                Binding {
                    target: slider
                    property: "value"
                    value: mediaPlayer.position
                    when: !slider.pressed
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter

                ToolButton {
                    icon.source: mediaPlayer.playbackState == MediaPlayer.PlayingState ? UI.Icons.pause : UI.Icons.play_arrow

                    onClicked: {
                        if (mediaPlayer.playbackState == MediaPlayer.PlayingState)
                            mediaPlayer.pause()
                        else
                            mediaPlayer.play()
                    }
                }
            }
        }
    }

    Connections {
        target: Application
        function onStateChanged() {
            if (Application.state == Qt.ApplicationSuspended) {
                videoFilter.active = false
                videoFilter.clearSink()
            } else if (Application.state == Qt.ApplicatoinInactive) {
                videoFilter.active = false
            }
        }
    }

    MediaPlayer {
        id: mediaPlayer

        videoOutput: vout
    }

    TFL.VideoFilter {
        id: videoFilter

        orientation: vout.orientation
        captureRect: vout.sourceRect
        videoSink: vout.videoSink
    }

    VideoOutput {
        id: vout

        anchors.fill: parent
        fillMode: VideoOutput.PreserveAspectFit
    }

    UI.LabelTitle {
        anchors { horizontalCenter: parent.horizontalCenter; top: parent.top }
        padding: 10
        color: "white"
        style: Text.Outline
        styleColor: "black"
        visible: TFL.Settings.showTime && videoFilter.active && root.tflite.inferenceTime > 0
        text: root.tflite.inferenceTime + " ms"
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
            text: "Open video file"
            onTriggered: fileDialog.open()
        }
        MenuItem {
            text: "Rotation: " + vout.orientation
            onTriggered: vout.orientation = (vout.orientation + 90) % 360
        }
    }

    Rectangle {
        visible: root.tflite.errorString.length > 0

        anchors.centerIn: parent

        implicitWidth: root.width * (root.width < root.height ? 0.9 : 0.6)
        implicitHeight: Math.min(messageLabel.implicitHeight, root.height * 0.9)

        color: "darkred"
        opacity: 0.7
        border.color: "red"
        border.width: 2
        radius: 10

        UI.LabelSubheading {
            id: messageLabel

            anchors.fill: parent
            topPadding: 20
            bottomPadding: 20
            leftPadding: 8
            rightPadding: 8
            color: "white"
            textFormat: "RichText"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            onLinkActivated: Qt.openUrlExternally(link)
        }
    }

    FileDialog {
        id: fileDialog

        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        onAccepted: {
            videoFilter.active = false
            mediaPlayer.stop()
            mediaPlayer.source = selectedFile
            mediaPlayer.play()
            mediaPlayer.pause()
            mediaPlayer.position = 0
            vout.orientation = 0
        }
    }
}
