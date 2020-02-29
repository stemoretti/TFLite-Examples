import QtQuick 2.12
import QtQuick.Controls 2.12

Page {
    id: root

    property bool canNavigateBack: false
    property alias appToolBar: appToolBar
    property alias leftButton: appToolBar.leftButton
    property alias rightButtons: appToolBar.rightButtons

    function pop(item, operation) {
        if (StackView.view.currentItem !== root)
            return false

        return StackView.view.pop(item, operation)
    }

    function back() {
        return pop()
    }

    function push(item, properties, operation) {
        return StackView.view.push(item, properties, operation)
    }

    Keys.onBackPressed: {
        if (StackView.view.depth > 1) {
            event.accepted = true
            back()
        } else {
            Qt.quit()
        }
    }

    Action {
        id: backAction
        icon.source: "image://icon/arrow_back"
        onTriggered: root.back()
    }

    AppToolBar {
        id: appToolBar
        title: root.title
        leftButton: canNavigateBack ? backAction : null
    }
}
