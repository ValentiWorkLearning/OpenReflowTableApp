import QtQuick
import QtQuick.Window
import QtQuick.Controls

import QtQuick.Layouts

import app.root_model
import app.presets_model
import app.reflow_controller


Popup
{
    id: regulatorSettingsPopup
    width: parent.width* 0.32;
    height: parent.height* 0.32;

    parent: Overlay.overlay

    x: parent.width / 2 - this.width / 2;
    y: parent.height / 2 - this.height / 2;

    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

    ColumnLayout
    {
        anchors.fill: parent;

        RowLayout
        {
            Label
            {
                text: qsTr("Hystresis")
            }
            Item
            {
                Layout.fillWidth:true;
            }
            TextField
            {
                id: hysteresis
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                placeholderText: qsTr("Hystresis")
                text:  AppModel.reflowController.regulatorParams.hysteresis
                font.weight: Font.DemiBold
            }
        }

        RowLayout
        {
            Label
            {
                text: qsTr("k")
            }
            Item
            {
                Layout.fillWidth:true;
            }
            TextField
            {
                id: k
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                placeholderText: qsTr("k")
                text:  AppModel.reflowController.regulatorParams.k
                font.weight: Font.DemiBold
            }
        }



        RowLayout{
            Layout.fillWidth: true;
            Layout.alignment: Qt.AlignHCenter;
            spacing: 28

            Button
            {
                id: addButton;
                text: qsTr("Apply")
                enabled: !k.empty && !hysteresis.empty && !AppModel.reflowController.systemState.isReflowRunning
                onClicked:
                {
                }
            }

            Button
            {
                id: cancelButton;
                text: "CLOSE"
                onClicked:
                {
                    regulatorSettingsPopup.close();
                }
            }
        }
    }
}
