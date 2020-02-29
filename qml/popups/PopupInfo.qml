import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import "../common"

BaseModalPopup {
    id: root

    property alias text: popupLabel.text

    ColumnLayout {
        anchors.right: parent.right
        anchors.left: parent.left
        spacing: 10

        LabelSubheading {
            id: popupLabel

            Layout.fillWidth: true
            topPadding: 20
            leftPadding: 8
            rightPadding: 8
            text: ""
            color: popupTextColor
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            linkColor: isDarkTheme ? "lightblue" : "blue"
            onLinkActivated: Qt.openUrlExternally(link)
        }
        ButtonFlat {
            id: okButton

            text: "OK"
            textColor: Material.accent
            onClicked: {
                root.close()
            }
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
