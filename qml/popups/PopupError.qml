// ekke (Ekkehard Gentz) @ekkescorner
import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0
import "../common"

Popup {
    id: popup
    closePolicy: Popup.CloseOnPressOutside
    bottomMargin: isLandscape? 24 : 80
    implicitWidth: isLandscape? parent.width * 0.50 : parent.width * 0.80

    x: (appWindow.width - implicitWidth) / 2
    y: (appWindow.height - height)
    background: Rectangle {
        color: Material.color(Material.Red, isDarkTheme? Material.Shade500 : Material.Shade800)
        radius: 24
        opacity: toastOpacity
    }
    Timer {
        id: errorTimer
        interval: 3000
        repeat: false
        onTriggered: {
            popup.close()
        }
    } // toastTimer
    ColumnLayout {
        width: parent.width - 32
        Layout.fillWidth: true
        Layout.leftMargin: 16
        Layout.rightMargin: 16
        RowLayout {
            Layout.fillWidth: true
            spacing: 16
            Image {
                id: alarmIcon
                smooth: true
                source: "image://icon/error"
                sourceSize.width: 36
                sourceSize.height: 36
                ColorOverlay {
                    anchors.fill: alarmIcon
                    source: alarmIcon
                    color: "white"
                }
            }
            Label {
                id: errorLabel
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                rightPadding: 24
                font.pixelSize: 16
                color: "white"
                // TODO Why is WordWrap ignored ?
                wrapMode: Label.WordWrap
            } // errorLabel
        }
    }

    onAboutToShow: {
        errorTimer.start()
    }
    onAboutToHide: {
        errorTimer.stop()
    }
    function start(errorText) {
        errorLabel.text = errorText
        if(!errorTimer.running) {
            open()
        } else {
            errorTimer.restart()
        }
    } // function start
} // popup errorPopup
