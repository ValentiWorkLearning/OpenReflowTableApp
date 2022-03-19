import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import app.edit_popup.mode

import app.root_model
import app.presets_model

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
                function onAddingPresetCompleted(presetName)
                {
                    console.log(`On adding new preset completed:${presetName}`)
                    AppModel.presetsModel.addReflowPreset(presetName);
                    addPresetPopupLoader.item.close();
                }
            }
        }

        ListView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            model: AppModel.presetsModel
            delegate: RowLayout {
                width: ListView.view.width
                implicitHeight: 48
                Label{
                    text: presetName
                    font.pointSize: 21
                    elide: Text.ElideRight;
                    Layout.alignment: Qt.AlignVCenter
                }
                Item
                {
                    Layout.fillWidth:true;
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
