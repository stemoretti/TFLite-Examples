import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import "../common"
import "../popups"
import "../languages.js" as JS

import Settings 1.0
import System 1.0

AppStackPage {
    id: root

    title: qsTr("Settings")
    padding: 0

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

                SettingsItem {
                    title: qsTr("Dark theme")
                    check.visible: true
                    check.checked: Settings.darkTheme
                    check.onClicked: Settings.darkTheme = !Settings.darkTheme
                    onClicked: check.clicked()
                }

                SettingsItem {
                    title: qsTr("Primary color")
                    subtitle: primaryColorPopup.model.get(primaryColorPopup.currentIndex).title
                    onClicked: primaryColorPopup.open()
                }

                SettingsItem {
                    title: qsTr("Accent color")
                    subtitle: accentColorPopup.model.get(accentColorPopup.currentIndex).title
                    onClicked: accentColorPopup.open()
                }

                HorizontalListDivider { }

                SettingsItem {
                    title: qsTr("Language")
                    subtitle: JS.getLanguageFromCode(Settings.language)
                    onClicked: languagePopup.open()
                }

                SettingsItem {
                    property string name: JS.getCountryFromCode(Settings.country)
                    property string nativeName: JS.getCountryFromCode(Settings.country, "native")

                    title: qsTr("Country")
                    subtitle: nativeName + ((name !== nativeName) ? " (" + name + ")" : "")
                    onClicked: push(Qt.resolvedUrl("SettingsContinentsPage.qml"))
                }

                HorizontalListDivider { }

                SettingsItem {
                    title: qsTr("Resolution")
                    subtitle: camera.viewfinder.resolution.width + "x" + camera.viewfinder.resolution.height
                    onClicked: resolutionsPopup.open()
                }

                HorizontalListDivider { }

                SettingsItem {
                    title: qsTr("Show inference time")
                    check.visible: true
                    check.checked: Settings.showTime
                    check.onClicked: Settings.showTime = !Settings.showTime
                    onClicked: check.clicked()
                }

                SettingsItem {
                    title: qsTr("NNAPI acceleration")
                    subtitle: qsTr("On Android devices running " +
                                   "8.1 or higher " +
                                   "hardware acceleration is available")
                    check.visible: true
                    check.checked: Settings.nnapi
                    check.onClicked: Settings.nnapi = !Settings.nnapi
                    onClicked: check.clicked()
                }

                SettingsItem {
                    title: qsTr("Number of threads")
                    subtitle: Settings.threads
                    onClicked: threadsPopup.open()
                }

                HorizontalListDivider { }

                SettingsItem {
                    title: qsTr("Objects detection model")
                    subtitle: Settings.objectsModel.replace(/.*\//, "")
                    placeholderText: qsTr("No file selected")
                    onClicked: push(fileChooserComponent, { "option": "objectsModel" })
                }

                SettingsItem {
                    title: qsTr("Objects detection labels")
                    subtitle: Settings.objectsLabels.replace(/.*\//, "")
                    placeholderText: qsTr("No file selected")
                    onClicked: push(fileChooserComponent, { "option": "objectsLabels" })
                }

                SettingsItem {
                    title: qsTr("Minimum confidence")
                    subtitle: Math.round(Settings.confidence * 100) + " %"
                    onClicked: confidencePopup.open()
                }

                HorizontalListDivider { }

                SettingsItem {
                    title: qsTr("Image classification model")
                    subtitle: Settings.classifierModel.replace(/.*\//, "")
                    placeholderText: qsTr("No file selected")
                    onClicked: push(fileChooserComponent, { "option": "classifierModel" })
                }

                SettingsItem {
                    title: qsTr("Image classification labels")
                    subtitle: Settings.classifierLabels.replace(/.*\//, "")
                    placeholderText: qsTr("No file selected")
                    onClicked: push(fileChooserComponent, { "option": "classifierLabels" })
                }

                SettingsItem {
                    title: qsTr("Classification threshold")
                    subtitle: (Settings.threshold * 100).toFixed(1) + " %"
                    onClicked: thresholdPopup.open()
                }

                HorizontalListDivider { }

                SettingsItem {
                    title: qsTr("Pose estimation model")
                    subtitle: Settings.poseModel.replace(/.*\//, "")
                    placeholderText: qsTr("No file selected")
                    onClicked: push(fileChooserComponent, { "option": "poseModel" })
                }

                SettingsItem {
                    title: qsTr("Minimum score")
                    subtitle: Settings.score.toFixed(1)
                    onClicked: scorePopup.open()
                }
            }
        }
    }

    Component {
        id: fileChooserComponent

        FileChooserPage {
            property string option
            onFileSelected: {
                Settings[option] = name
            }
        }
    }

    ColorSelectionPopup {
        id: primaryColorPopup
    }

    ColorSelectionPopup {
        id: accentColorPopup

        selectAccentColor: true
        currentColor: accentColor
    }

    ListPopup {
        id: languagePopup

        model: System.translations()
        delegateFunction: JS.getLanguageFromCode

        onClicked: {
            Settings.language = data
            currentIndex = index
            close()
        }

        Component.onCompleted: currentIndex = model.indexOf(Settings.language)
    }

    ListPopup {
        id: resolutionsPopup

        model: camera.supportedViewfinderResolutions()
        delegateFunction: function (size) { return size.width + "x" + size.height }

        onClicked: {
            camera.stop()
            Settings.resolution = data.width + "x" + data.height
            camera.start()
            close()
        }
    }

    BaseModalPopup {
        id: confidencePopup

        onClosed: Settings.confidence = confidenceSpin.value / 100

        SpinBox {
            id: confidenceSpin

            anchors.centerIn: parent
            from: 1
            value: Math.round(Settings.confidence * 100)
            to: 100
            stepSize: 1

            validator: IntValidator {
                bottom: confidenceSpin.from
                top: confidenceSpin.to
            }

            textFromValue: function(value, locale) {
                return value + " %"
            }

            valueFromText: function(text, locale) {
                return Number.fromLocaleString(locale, text)
            }
        }
    }

    BaseModalPopup {
        id: threadsPopup

        onClosed: Settings.threads = threadsSpin.value

        SpinBox {
            id: threadsSpin

            anchors.centerIn: parent
            from: 1
            value: Settings.threads
            to: 4
            stepSize: 1

            validator: IntValidator {
                bottom: threadsSpin.from
                top: threadsSpin.to
            }
        }
    }

    BaseModalPopup {
        id: thresholdPopup

        onClosed: Settings.threshold = thresholdSpin.value / 1000

        SpinBox {
            id: thresholdSpin

            anchors.centerIn: parent
            from: 1
            value: Math.round(Settings.threshold * 1000)
            to: 1000
            stepSize: 1

            validator: IntValidator {
                bottom: thresholdSpin.from
                top: thresholdSpin.to
            }

            textFromValue: function(value, locale) {
                return value / 10 + " %"
            }

            valueFromText: function(text, locale) {
                return Number.fromLocaleString(locale, text) * 10
            }
        }
    }

    BaseModalPopup {
        id: scorePopup

        onClosed: Settings.score = scoreSpin.value / 10

        SpinBox {
            id: scoreSpin

            anchors.centerIn: parent
            from: 1
            value: Math.round(Settings.score * 10)
            to: 10

            validator: IntValidator {
                bottom: scoreSpin.from
                top: scoreSpin.to
            }

            textFromValue: function(value, locale) {
                return value / 10
            }

            valueFromText: function(text, locale) {
                return Number.fromLocaleString(locale, text) * 10
            }
        }
    }
}
