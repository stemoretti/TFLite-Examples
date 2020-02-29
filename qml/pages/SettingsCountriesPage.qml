import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import "../common"
import "../languages.js" as JS

import Settings 1.0

AppStackPage {
    property int continent: 0

    function back() {
        return StackView.view.replace(Qt.resolvedUrl("SettingsContinentsPage.qml"),
                                      StackView.PopTransition)
    }

    title: qsTr("Countries")
    padding: 0

    ListView {
        anchors.fill: parent
        model: JS.regions[continent].countries.map(function (o) { return o.code })

        delegate: ItemDelegate {
            width: parent.width
            contentItem: ColumnLayout {
                spacing: 0
                LabelSubheading {
                    text: JS.getCountryFromCode(modelData)
                }
                LabelBody {
                    text: JS.getCountryFromCode(modelData, "native")
                    opacity: 0.6
                }
            }
            onClicked: {
                Settings.country = modelData
                pop()
            }
        }

        ScrollIndicator.vertical: ScrollIndicator { }
    }
}
