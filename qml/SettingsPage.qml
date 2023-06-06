import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs

import BaseUI as UI

import TFLite as TFL

UI.AppStackPage {
    id: root

    title: qsTr("Settings")
    padding: 0

    leftButton: Action {
        icon.source: UI.Icons.arrow_back
        onTriggered: root.back()
    }

    function getLanguageFromCode(code)
    {
        var languages = [
            { "code": "en", "name": "English", "nativeName": "English" },
            { "code": "it", "name": "Italian", "nativeName": "Italiano" },
        ]
        var codes = languages.map(o => o.code)
        var i = codes.indexOf(code)

        if (i < 0)
            return ""

        var name = languages[i].name
        var nativeName = languages[i].nativeName

        if (name !== nativeName) {
            name = nativeName + " (" + name + ")"
        }

        return name
    }

    component CustomDialog: Dialog {
        parent: Overlay.overlay

        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        modal: true
        dim: true

        closePolicy: Popup.CloseOnEscape

        footer: DialogButtonBox {
            alignment: Qt.AlignHCenter

            UI.ButtonFlat {
                text: qsTr("Cancel")
                textColor: UI.Style.accentColor
                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            }
            UI.ButtonContained {
                text: qsTr("Ok")
                textColor: UI.Style.textOnPrimary
                buttonColor: UI.Style.primaryColor
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            }
        }
    }

    Flickable {
        contentHeight: settingsPane.implicitHeight
        anchors.fill: parent

        Pane {
            id: settingsPane

            anchors.fill: parent
            padding: 0

            ColumnLayout {
                width: parent.width
                spacing: 0

                UI.SettingsSectionTitle { text: qsTr("Theme and colors") }

                UI.SettingsCheckItem {
                    title: qsTr("Dark Theme")
                    checkState: TFL.Settings.darkTheme ? Qt.Checked : Qt.Unchecked
                    onClicked: TFL.Settings.darkTheme = !TFL.Settings.darkTheme
                    Layout.fillWidth: true
                }

                UI.SettingsItem {
                    title: qsTr("Primary color")
                    subtitle: colorDialog.getColorName(TFL.Settings.primaryColor)
                    onClicked: {
                        colorDialog.selectAccentColor = false
                        colorDialog.open()
                    }
                }

                UI.SettingsItem {
                    title: qsTr("Accent color")
                    subtitle: colorDialog.getColorName(TFL.Settings.accentColor)
                    onClicked: {
                        colorDialog.selectAccentColor = true
                        colorDialog.open()
                    }
                }

                UI.SettingsSectionTitle { text: qsTr("Localization") }

                UI.SettingsItem {
                    title: qsTr("Language")
                    subtitle: root.getLanguageFromCode(TFL.Settings.language)
                    onClicked: languagePopup.open()
                }

                UI.SettingsSectionTitle { text: qsTr("Display") }

                UI.SettingsCheckItem {
                    title: qsTr("Show inference time")
                    checkState: TFL.Settings.showTime ? Qt.Checked : Qt.Unchecked
                    onClicked: TFL.Settings.showTime = !TFL.Settings.showTime
                    Layout.fillWidth: true
                }

                UI.SettingsSectionTitle { text: qsTr("Hardware capabilities") }

                UI.SettingsCheckItem {
                    title: qsTr("NNAPI acceleration")
                    subtitle: qsTr("On Android devices running " +
                                   "8.1 or higher " +
                                   "hardware acceleration is available")
                    checkState: TFL.Settings.nnapi ? Qt.Checked : Qt.Unchecked
                    onClicked: TFL.Settings.nnapi = !TFL.Settings.nnapi
                    Layout.fillWidth: true
                }

                UI.SettingsItem {
                    title: qsTr("Number of threads")
                    subtitle: TFL.Settings.threads
                    onClicked: threadsPopup.open()
                }
            }
        }
    }

    UI.OptionsDialog {
        id: languagePopup

        title: qsTr("Language")
        model: TFL.System.translations()
        delegate: RadioButton {
            checked: modelData === TFL.Settings.language
            text: root.getLanguageFromCode(modelData)
            onClicked: { languagePopup.close(); TFL.Settings.language = modelData }
        }
    }

    UI.OptionsDialog {
        id: colorDialog

        property bool selectAccentColor: false

        function getColorName(color) {
            var filtered = colorDialog.model.filter((c) => {
                return Material.color(c.bg) === color
            })
            return filtered.length ? filtered[0].name : ""
        }

        title: selectAccentColor ? qsTr("Choose accent color") : qsTr("Choose primary color")
        model: [
            { name: "Material Red", bg: Material.Red },
            { name: "Material Pink", bg: Material.Pink },
            { name: "Material Purple", bg: Material.Purple },
            { name: "Material DeepPurple", bg: Material.DeepPurple },
            { name: "Material Indigo", bg: Material.Indigo },
            { name: "Material Blue", bg: Material.Blue },
            { name: "Material LightBlue", bg: Material.LightBlue },
            { name: "Material Cyan", bg: Material.Cyan },
            { name: "Material Teal", bg: Material.Teal },
            { name: "Material Green", bg: Material.Green },
            { name: "Material LightGreen", bg: Material.LightGreen },
            { name: "Material Lime", bg: Material.Lime },
            { name: "Material Yellow", bg: Material.Yellow },
            { name: "Material Amber", bg: Material.Amber },
            { name: "Material Orange", bg: Material.Orange },
            { name: "Material DeepOrange", bg: Material.DeepOrange },
            { name: "Material Brown", bg: Material.Brown },
            { name: "Material Grey", bg: Material.Grey },
            { name: "Material BlueGrey", bg: Material.BlueGrey }
        ]
        delegate: RowLayout {
            spacing: 0

            Rectangle {
                visible: colorDialog.selectAccentColor
                color: UI.Style.primaryColor
                Layout.margins: 0
                Layout.leftMargin: 10
                Layout.minimumWidth: 48
                Layout.minimumHeight: 32
            }

            Rectangle {
                color: Material.color(modelData.bg)
                Layout.margins: 0
                Layout.leftMargin: colorDialog.selectAccentColor ? 0 : 10
                Layout.minimumWidth: 32
                Layout.minimumHeight: 32
            }

            RadioButton {
                checked: {
                    if (colorDialog.selectAccentColor)
                        Material.color(modelData.bg) === UI.Style.accentColor
                    else
                        Material.color(modelData.bg) === UI.Style.primaryColor
                }
                text: modelData.name
                Layout.leftMargin: 4
                onClicked: {
                    colorDialog.close()
                    if (colorDialog.selectAccentColor)
                        TFL.Settings.accentColor = Material.color(modelData.bg)
                    else
                        TFL.Settings.primaryColor = Material.color(modelData.bg)
                }
            }
        }
    }

    CustomDialog {
        id: threadsPopup

        onAccepted: TFL.Settings.threads = threadsSpin.value

        SpinBox {
            id: threadsSpin

            anchors.centerIn: parent
            from: 1
            value: TFL.Settings.threads
            to: 8
            stepSize: 1

            validator: IntValidator {
                bottom: threadsSpin.from
                top: threadsSpin.to
            }
        }
    }
}
