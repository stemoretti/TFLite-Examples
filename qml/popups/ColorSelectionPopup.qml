import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import "../common"

import Settings 1.0

BaseModalPopup {
    property bool selectAccentColor: false
    property color currentColor: primaryColor
    property int currentIndex: 0
    property alias model: colorModel

    implicitWidth: appWindow.width * 0.9
    contentHeight: colorsList.contentHeight

    ListView {
        id: colorsList
        anchors.fill: parent
        clip: true
        delegate: ItemDelegate {
            width: parent.width
            implicitHeight: 40
            Row {
                spacing: 0
                topPadding: 8
                leftPadding: 10
                Rectangle {
                    visible: selectAccentColor
                    anchors.verticalCenter: parent.verticalCenter
                    implicitHeight: 32
                    implicitWidth: 48
                    color: primaryColor
                }
                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    implicitHeight: 32
                    implicitWidth: 32
                    color: Material.color(model.bg)
                }
                LabelBody {
                    leftPadding: 10
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.title
                }
            }
            onClicked: {
                if (selectAccentColor)
                    Settings.accentColor = Material.color(model.bg)
                else
                    Settings.primaryColor = Material.color(model.bg)
                currentIndex = index
                close()
            }
        }

        model: ListModel {
            id: colorModel
            ListElement { title: qsTr("Material Red"); bg: Material.Red }
            ListElement { title: qsTr("Material Pink"); bg: Material.Pink }
            ListElement { title: qsTr("Material Purple"); bg: Material.Purple }
            ListElement { title: qsTr("Material DeepPurple"); bg: Material.DeepPurple }
            ListElement { title: qsTr("Material Indigo"); bg: Material.Indigo }
            ListElement { title: qsTr("Material Blue"); bg: Material.Blue }
            ListElement { title: qsTr("Material LightBlue"); bg: Material.LightBlue }
            ListElement { title: qsTr("Material Cyan"); bg: Material.Cyan }
            ListElement { title: qsTr("Material Teal"); bg: Material.Teal }
            ListElement { title: qsTr("Material Green"); bg: Material.Green }
            ListElement { title: qsTr("Material LightGreen"); bg: Material.LightGreen }
            ListElement { title: qsTr("Material Lime"); bg: Material.Lime }
            ListElement { title: qsTr("Material Yellow"); bg: Material.Yellow }
            ListElement { title: qsTr("Material Amber"); bg: Material.Amber }
            ListElement { title: qsTr("Material Orange"); bg: Material.Orange }
            ListElement { title: qsTr("Material DeepOrange"); bg: Material.DeepOrange }
            ListElement { title: qsTr("Material Brown"); bg: Material.Brown }
            ListElement { title: qsTr("Material Grey"); bg: Material.Grey }
            ListElement { title: qsTr("Material BlueGrey"); bg: Material.BlueGrey }
        }
    }
    Component.onCompleted: {
        for (var i = 0; i < colorModel.count; ++i) {
            var tmp = colorModel.get(i)
            if (Material.color(tmp.bg) === currentColor) {
                currentIndex = i
                return
            }
        }
    }
}
