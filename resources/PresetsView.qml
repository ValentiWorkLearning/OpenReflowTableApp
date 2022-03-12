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

                BackButton
                {
                    onClicked: requestBackToHome();
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
            delegate: RowLayout {
                width: ListView.view.width
                implicitHeight: 48
                Label{
                    text: preset_name
                    elide: Text.ElideRight;
                }
                Button
                {
                    id: setupPresetButton
                    width: 48
                    height: 48
                    text: qsTr("EDIT")

                    hoverEnabled: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Edit preset")

                }
            }
        }
    }
}
