import QtQuick
import QtQuick.Window
import QtQuick.Controls

import QtQuick.Layouts
import app.edit_popup.mode

Popup
{
    id: editPresetPopupRoot
    width: parent.width* 0.3;
    height: parent.height* 0.6;

    parent: Overlay.overlay

    x: parent.width / 2 - this.width / 2;
    y: parent.height / 2 - this.height / 2;

    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

    property alias initialPresetName: presetName.text;

    property int popupMode: EditPopupMode.ADD_NEW;
    property int editItemIndex: 0;

    signal addingPresetCompleted(string itemName )
    signal editPresetCompleted(int itemId, string itemName)

    ColumnLayout
    {
        anchors.fill: parent;
        TextField
        {
            id: presetName
            Layout.alignment: Qt.AlignHCenter
            width: parent.width;
            placeholderText: "Preset name"
            font.weight: Font.DemiBold
        }

        RowLayout{
            Layout.fillWidth: true;
            Layout.alignment: Qt.AlignHCenter;
            spacing: 28

            Button
            {
                id: addButton;
                text: qsTr("Apply")
                enabled: presetName.text!=="";
                onClicked:
                {
                    if(isInputFieldsValid()){
                        if(popupMode === EditPopupMode.ADD_NEW)
                            addingPresetCompleted(presetName.text);
                        else
                            addingPresetCompleted(addCheckListItemPopupRoot.editItemIndex, presetName.text);
                    }
                }
            }

            Button
            {
                id: cancelButton;
                text: "Cancel"
                onClicked:
                {
                    editPresetPopupRoot.close();
                }
            }
        }
    }

    function isInputFieldsValid()
    {
        return presetName.text !== "";
    }
}
