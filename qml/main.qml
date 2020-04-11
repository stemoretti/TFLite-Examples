import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtMultimedia 5.12
import "common"

import Settings 1.0

App {
    id: appWindow

    title: "TFLite examples"
    header: pageStack.currentItem.appToolBar
    width: 360
    height: 480

    StackView {
        id: pageStack

        anchors.fill: parent
        initialItem: Qt.resolvedUrl("pages/ObjectDetectionPage.qml")

        onCurrentItemChanged: {
            if (currentItem) {
                currentItem.canNavigateBack = depth > 1
                currentItem.forceActiveFocus()
            }
        }
    }

    Camera {
        id: camera

        cameraState: Camera.LoadedState
        viewfinder.resolution: Settings.resolution
    }

    Drawer {
        id: navDrawer

        interactive: pageStack.depth === 1
        width: Math.min(240,  Math.min(appWindow.width, appWindow.height) / 3 * 2 )
        height: appWindow.height

        onAboutToShow: menuColumn.enabled = true

        Flickable {
            anchors.fill: parent
            contentHeight: menuColumn.implicitHeight
            boundsBehavior: Flickable.StopAtBounds

            ColumnLayout {
                id: menuColumn

                anchors { left: parent.left; right: parent.right }
                spacing: 0

                Rectangle {
                    id: topItem
                    height: 140
                    color: primaryColor
                    Layout.fillWidth: true
                    Text {
                        text: appWindow.title
                        color: textOnPrimary
                        font.pixelSize: fontSizeHeadline
                        wrapMode: Text.WordWrap
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                            margins: 25
                        }
                    }
                }

                Repeater {
                    id: actionsList

                    model: ListModel {
                        ListElement {
                            iconUrl: "image://icon/account_box"
                            text: qsTr("Object Detection")
                            page: "pages/ObjectDetectionPage.qml"
                        }
                        ListElement {
                            iconUrl: "image://icon/local_florist"
                            text: qsTr("Image Classification")
                            page: "pages/ImageClassificationPage.qml"
                        }
                        ListElement {
                            iconUrl: "image://icon/directions_walk"
                            text: qsTr("Pose Estimation")
                            page: "pages/PoseEstimationPage.qml"
                        }
                    }

                    delegate: ItemDelegate {
                        icon.source: model.iconUrl
                        text: model.text
                        Layout.fillWidth: true
                        onClicked: {
                            // Disable, or a double click will push the page twice.
                            menuColumn.enabled = false
                            navDrawer.close()
                            camera.stop()
                            pageStack.replace(Qt.resolvedUrl(model.page))
                        }
                    }
                }

                Repeater {
                    id: pageList

                    model: ListModel {
                        ListElement {
                            iconUrl: "image://icon/settings"
                            text: qsTr("Settings")
                            page: "pages/SettingsPage.qml"
                        }
                        ListElement {
                            iconUrl: "image://icon/info"
                            text: qsTr("About")
                            page: "pages/AboutPage.qml"
                        }
                    }

                    delegate: ItemDelegate {
                        icon.source: iconUrl
                        text: model.text
                        Layout.fillWidth: true
                        onClicked: {
                            // Disable, or a double click will push the page twice.
                            menuColumn.enabled = false
                            navDrawer.close()
                            pageStack.push(Qt.resolvedUrl(model.page))
                        }
                    }
                }
            }
        }
    }
}
