import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Item {
    id: root

    property alias title: titleLabel.text
    property string subtitle: subtitleLabel.text
    property string placeholderText: ""
    property alias check: settingSwitch

    signal clicked()

    implicitHeight: row.height + 10
    Layout.fillWidth: true

    ItemDelegate {
        anchors.fill: parent
        onClicked: root.clicked()

        RowLayout {
            id: row

            width: parent.width
            anchors.verticalCenter: parent.verticalCenter

            ColumnLayout {
                spacing: 2

                LabelSubheading {
                    id: titleLabel

                    leftPadding: 20
                }

                Label {
                    id: subtitleLabel

                    leftPadding: 20
                    opacity: 0.6
                    wrapMode: Text.WordWrap
                    text: subtitle.length > 0 ? subtitle : placeholderText
                    visible: text.length > 0 || placeholderText.length > 0
                    Layout.fillWidth: true
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Switch {
                id: settingSwitch

                rightPadding: 20
                visible: false
            }
        }
    }
}
