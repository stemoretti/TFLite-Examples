import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import BaseUI as UI

import TFLite as TFL

UI.AppStackPage {
    id: root

    function selectModelFile(modeltitle, modelfile) {
        if (TFL.Settings[modelfile].length > 0)
            fileDialog.currentFolder = TFL.Settings[modelfile]

        fileDialog.fileSetting = modelfile
        fileDialog.open()
    }

    function _formatFilename(path) {
        return path ? decodeURIComponent(path).replace(/.*\//, "") : ""
    }

    leftButton: Action {
        icon.source: UI.Icons.menu
        onTriggered: navDrawer.open()
    }

    Drawer {
        id: navDrawer

        interactive: stack?.depth === 1
        width: Math.min(240, Math.min(parent.width, parent.height) / 3 * 2 )
        height: parent.height

        onAboutToShow: menuColumn.enabled = true
        topPadding: 0
        bottomPadding: 0

        Flickable {
            anchors.fill: parent
            contentHeight: menuColumn.implicitHeight
            boundsBehavior: Flickable.StopAtBounds

            ColumnLayout {
                id: menuColumn

                anchors { left: parent.left; right: parent.right }
                spacing: 0

                Label {
                    text: Qt.application.displayName
                    color: Material.foreground
                    font.pixelSize: UI.Style.fontSizeHeadline
                    padding: (root.appToolBar.implicitHeight - contentHeight) / 2
                    leftPadding: 20
                    Layout.fillWidth: true
                }

                UI.HorizontalListDivider {}

                Repeater {
                    id: actionsList

                    model: [
                        {
                            icon: UI.Icons.account_box,
                            text: QT_TR_NOOP("Object Detection"),
                            page: "ObjectDetectionPage.qml"
                        },
                        {
                            icon: UI.Icons.local_florist,
                            text: QT_TR_NOOP("Image Classification"),
                            page: "ImageClassificationPage.qml"
                        },
                        {
                            icon: UI.Icons.directions_walk,
                            text: QT_TR_NOOP("Pose Estimation"),
                            page: "PoseEstimationPage.qml"
                        }
                    ]

                    delegate: ItemDelegate {
                        icon.source: modelData.icon
                        text: qsTr(modelData.text)

                        Layout.fillWidth: true

                        onClicked: {
                            // Disable, or a double click will push the page twice.
                            menuColumn.enabled = false
                            navDrawer.close()
                            stack.replace(Qt.resolvedUrl(modelData.page))
                        }
                    }
                }

                UI.HorizontalListDivider { }

                Repeater {
                    id: pageList

                    model: [
                        {
                            icon: UI.Icons.settings,
                            text: QT_TR_NOOP("Settings"),
                            page: "SettingsPage.qml"
                        },
                        {
                            icon: UI.Icons.info_outline,
                            text: QT_TR_NOOP("About"),
                            page: "AboutPage.qml"
                        }
                    ]

                    delegate: ItemDelegate {
                        icon.source: modelData.icon
                        text: qsTr(modelData.text)

                        Layout.fillWidth: true

                        onClicked: {
                            // Disable, or a double click will push the page twice.
                            menuColumn.enabled = false
                            navDrawer.close()
                            stack.push(Qt.resolvedUrl(modelData.page))
                        }
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileDialog

        property string fileSetting

        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        onAccepted: TFL.Settings[fileSetting] = selectedFile
    }
}
