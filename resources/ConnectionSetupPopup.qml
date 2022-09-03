import QtQuick
import QtQuick.Window
import QtQuick.Controls

import QtQuick.Layouts
import app.edit_popup.mode

Popup
{
    id: connectionSetupPopup
    width: parent.width* 0.7;
    height: parent.height* 0.5;

    parent: Overlay.overlay

    x: parent.width / 2 - this.width / 2;
    y: parent.height / 2 - this.height / 2;

    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

    property alias deviceAddress: addressInput.text;

    signal connectionSettingsEditCompleted(string deviceAddress)

    ColumnLayout
    {
        anchors.fill: parent;
        TextField
        {
            id: addressInput
            Layout.fillWidth:true

            Layout.alignment: Qt.AlignHCenter
            width: parent.width;
            placeholderText: "Device address"
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
                enabled: addressInput.text!=="";
                onClicked:
                {
                    if(!isInputFieldsValid())
                        return;
                    
                    connectionSetupPopup.connectionSettingsEditCompleted(addressInput.text);

                }
            }

            Button
            {
                id: cancelButton;
                text: "Cancel"
                onClicked:
                {
                    connectionSetupPopup.close();
                }
            }
        }
    }

    function isInputFieldsValid()
    {
        return addressInput.text !== "";
    }
}
