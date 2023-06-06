import QtQuick
import QtQuick.Controls

import BaseUI as UI

Dialog {
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
