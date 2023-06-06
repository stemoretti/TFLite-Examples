import QtQuick

import BaseUI as UI

import TFLite

UI.App {
    id: root

    title: Qt.application.displayName
    width: 360
    height: 480

    initialPage: "qrc:/qml/ObjectDetectionPage.qml"

    Component.onCompleted: {
        UI.Style.theme = Qt.binding(function() { return Settings.darkTheme ? "Dark" : "Light" })
        UI.Style.primaryColor = Qt.binding(function() { return Settings.primaryColor })
        UI.Style.accentColor = Qt.binding(function() { return Settings.accentColor })
        UI.Style.isDarkTheme = Qt.binding(function() { return Settings.darkTheme })
    }
}
