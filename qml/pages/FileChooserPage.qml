import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import Qt.labs.folderlistmodel 2.12
import "../common"

import System 1.0

AppStackPage {
    id: root

    property string folder: System.genericPath()
    property alias filters: storage.nameFilters

    signal fileSelected(string name)

    function canMoveUp() {
        return storage.folder.toString() !== System.genericPath()
    }

    function isFolder(name) {
	return storage.isFolder(storage.indexOf(storage.folder + "/" + name))
    }

    title: canMoveUp() ? storage.folder.toString().replace(/.*\//, "") : "Main"

    rightButtons: [
        Action {
            icon.source: "image://icon/arrow_upward"
            enabled: canMoveUp()
            onTriggered: root.folder = storage.parentFolder
        }
    ]

    FolderListModel {
	id: storage

        showDirsFirst: true
        showHidden: true
        folder: root.folder
    }

    ListView {
	anchors.fill: parent
        model: storage
        delegate: ColumnLayout {
            width: parent.width
            spacing: 0

            ItemDelegate {
                height: 64
                text: fileName
                font.pixelSize: 20
                icon.source: isFolder(fileName) ?
                                 "image://icon/folder" :
                                 "image://icon/insert_drive_file"
                Layout.fillWidth: true

                onClicked: {
                    if (!fileIsDir) {
                        fileSelected(filePath)
                        back()
                    } else {
                        root.folder = fileURL
                    }
                }
            }

            HorizontalListDivider { }
        }

	ScrollIndicator.vertical: ScrollIndicator { }
    }
}
