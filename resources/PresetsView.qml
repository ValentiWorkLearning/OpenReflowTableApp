import QtQuick
import QtQuick.Controls

Item {
    ListModel {
        id: testModel
        ListElement {
            preset_name: "Solder preset 1"
        }
        ListElement {
            preset_name: "Solder preset 2"
        }
        ListElement {
            preset_name: "Solder preset 3"
        }
    }

    ListView {
        anchors.fill: parent

        model: testModel
        delegate: Label {
            text: preset_name
        }
    }
}
