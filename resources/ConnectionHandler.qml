import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import app.root_model
import app.presets_model
import app.reflow_controller

Rectangle
{

	implicitWidth: 30;
	implicitHeight:connectionStatusButton.implicitHeight*1.2;

	opacity: 0.9
	Material.elevation: 6
	color: AppModel.reflowController.connectionStatus? Material.color(Material.Green):Material.color(Material.Red);


	Loader
    {
        id:connectionSettingsPopupLoader;
        onLoaded:
            item.open();
    }


	Label
	{
		anchors.centerIn:parent
		font.pixelSize: 18
		text: AppModel.reflowController.connectionStatus? qsTr("Device is connected"):qsTr("Can't connect to the device") 
	}

	Button
	{
		id:connectionStatusButton
		anchors.verticalCenter: parent.verticalCenter;
		anchors.right: parent.right
		anchors.rightMargin:8

		visible: !AppModel.reflowController.connectionStatus;
		text: qsTr("Connection setup")

		onClicked:
		{
			connectionSettingsPopupLoader.setSource(
				"qrc:/ConnectionSetupPopup.qml",
				{
				    deviceAddress:AppModel.reflowController.deviceAddress
				}
			)
		}
	}

	 Connections
     {
         target: connectionSettingsPopupLoader.item;
         function onConnectionSettingsEditCompleted(newDeviceAddress)
         {
             AppModel.reflowController.deviceAddress=newDeviceAddress
             connectionSettingsPopupLoader.item.close();
         }
     }
}