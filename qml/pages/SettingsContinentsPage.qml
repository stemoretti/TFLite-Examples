import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import "../common"
import "../languages.js" as JS

AppStackPage {
    title: qsTr("Continents")
    padding: 0

    ListView {
        anchors.fill: parent
        model: JS.regions.map(function (o) { return o.name })

        delegate: ItemDelegate {
            width: parent.width
//            height: 50
            contentItem: LabelSubheading {
                text: modelData
            }
            onClicked: {
                replace(Qt.resolvedUrl("SettingsCountriesPage.qml"),
                        { "continent": index })
            }
        }
    }
}
