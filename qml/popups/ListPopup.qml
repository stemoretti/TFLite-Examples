import QtQuick 2.12
import QtQuick.Controls 2.5

BaseModalPopup {
    id: root

    property alias model: list.model
    property alias currentIndex: list.currentIndex
    property var delegateFunction: function(data) { return data }
    property alias delegate: list.delegate

    signal clicked(var data, int index)

    implicitWidth: appWindow.width * 0.9
    contentHeight: list.contentHeight

    ListView {
        id: list
        anchors.fill: parent
        clip: true
        delegate: ItemDelegate {
            id: listDelegate
            width: parent.width
            implicitHeight: 40
            text: delegateFunction(modelData)
            onClicked: root.clicked(modelData, index)
        }
        highlightMoveDuration: 0
        onCurrentIndexChanged: {
            list.positionViewAtIndex(currentIndex, ListView.Center)
        }
    }
}
