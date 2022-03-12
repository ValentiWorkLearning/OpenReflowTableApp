import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {

    signal requestBackToHome;

    ColumnLayout
    {
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 8

        RowLayout
        {
            id: reflowRibbonLayout
            Layout.fillWidth: true

            Column{
                Label
                {
                    text: qsTr("Back")
                }
                Button
                {
                    id: backToHome
                    width: 48
                    height: 48
                    text: qsTr("<")

                    onClicked: requestBackToHome();
                }
            }
        }
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
            Layout.fillHeight: true
            Layout.fillWidth: true

            model: testModel
            delegate: Label {
                text: preset_name
            }
        }
    }
}
