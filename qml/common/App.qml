import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Window 2.12
import "../popups"

import Settings 1.0

ApplicationWindow {
    property bool isPortrait: Screen.primaryOrientation === Qt.PortraitOrientation
                              || Screen.primaryOrientation === Qt.InvertedPortraitOrientation
    property bool isLandscape: !isPortrait

    property bool isDarkTheme: Settings.darkTheme

    // ui constants
    property color primaryColor: Material.primary
    property color primaryDarkColor: Qt.darker(Material.primary)
    property color textOnPrimaryDark: textOnPrimary
    property color cardAndDialogBackground: Material.background
    property color accentColor: Material.accent
    property color textOnPrimary: isDarkColor(Material.primary) ? "#FFFFFF" : "#000000"
    property color textOnAccent: isDarkColor(Material.accent) ? "#FFFFFF" : "#000000"

    property color dividerColor: isDarkTheme ? "#FFFFFF" : "#000000"
    property real primaryTextOpacity: isDarkTheme ? 1.0 : 0.87
    property real secondaryTextOpacity: isDarkTheme ? 0.7 : 0.54
    property real dividerOpacity: isDarkTheme ? 0.12 : 0.12
    property color flatButtonTextColor: isDarkTheme ? "#FFFFFF" : "#424242"
    property color popupTextColor: isDarkTheme ? "#FFFFFF" : "#424242"
    property color toastColor: isDarkTheme ? "Darkgrey" : "#323232"
    property real toastOpacity: isDarkTheme ? 0.9 : 0.75

//    property color dropShadow: isDarkTheme ? "#E4E4E4" : Material.dropShadowColor
    property color dropShadow: Material.dropShadowColor

    // font sizes - defaults from Google Material Design Guide
    property int fontSizeDisplay4: 112
    property int fontSizeDisplay3: 56
    property int fontSizeDisplay2: 45
    property int fontSizeDisplay1: 34
    property int fontSizeHeadline: 24
    property int fontSizeTitle: 20
    property int fontSizeSubheading: 16
    property int fontSizeBodyAndButton: 14 // is Default
    property int fontSizeCaption: 12
    // fonts are grouped into primary and secondary with different Opacity
    // to make it easier to get the right property,
    // here's the opacity per size:
    property real opacityDisplay4: secondaryTextOpacity
    property real opacityDisplay3: secondaryTextOpacity
    property real opacityDisplay2: secondaryTextOpacity
    property real opacityDisplay1: secondaryTextOpacity
    property real opacityHeadline: primaryTextOpacity
    property real opacityTitle: primaryTextOpacity
    property real opacitySubheading: primaryTextOpacity
    // body can be both: primary or secondary text
    property real opacityBodyAndButton: primaryTextOpacity
    property real opacityBodySecondary: secondaryTextOpacity
    property real opacityCaption: secondaryTextOpacity
    property var currentLocale: Qt.locale(Settings.country)

    function isDarkColor(color) {
        var a = 1.0 - (0.299 * color.r + 0.587 * color.g + 0.114 * color.b)
        return color.a > 0.0 && a >= 0.3
    }

    function dateTimeString(datetime, format) {
        if (datetime === undefined)
            return ""
        if (format === undefined)
            format = Locale.ShortFormat
        return datetime.toLocaleString(currentLocale, format)
    }

    function dateString(date, format) {
        if (date === undefined)
            return ""
        if (format === undefined)
            format = Locale.ShortFormat
        return date.toLocaleDateString(currentLocale, format)
    }

    function timeString(time, format) {
        if (time === undefined)
            return ""
        if (format === undefined)
            format = Locale.ShortFormat
        return time.toLocaleTimeString(currentLocale, format)
    }

    function showToast(info) {
        popupToast.start(info)
    }
    function showError(info) {
        popupError.start(info)
    }

    visible: true
    title: qsTr("App")
    locale: Qt.locale("en_US")

    Material.primary: Settings.primaryColor
    Material.accent: Settings.accentColor
    Material.theme: Settings.darkTheme ? Material.Dark : Material.Light

    // APP WINDOW POPUPS
    PopupToast {
        id: popupToast
    }
    PopupError {
        id: popupError
    }
    // end APP WINDOW POPUPS
}
