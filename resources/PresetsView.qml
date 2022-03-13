import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import app.edit_popup.mode 1.0

Item {

    signal requestBackToHome;


    Loader
    {
        id:addPresetPopupLoader;
        onLoaded:
            item.open();
    }


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

            Button
            {
                id:createNewPreset
                hoverEnabled: true
                text: qsTr("+")
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Add new preset")
                onClicked:
                {
                    addPresetPopupLoader.setSource(
                                "qrc:/EditPresetPopup.qml",
                                {
                                    popupMode:EditPopupMode.ADD_NEW
                                }
                                )
                }
            }

            Connections
            {
                target: addPresetPopupLoader.item;
                function addingPresetCompleted(presetName)
                {
                    console.log(`On adding new preset completed:${suiteName}`)
                    addPresetPopupLoader.item.close();
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
