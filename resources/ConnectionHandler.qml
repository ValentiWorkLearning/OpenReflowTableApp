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
	implicitHeight:30;

	opacity: 0.9
	Material.elevation: 6
	color: AppModel.reflowController.hasConnection()? Material.color(Material.Green):Material.color(Material.Red);
}