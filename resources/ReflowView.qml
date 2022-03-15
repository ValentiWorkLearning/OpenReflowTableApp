import QtQuick
import QtCharts
import QtQuick.Layouts
import QtQuick.Controls

import app.root_model
import app.presets_model
import app.reflow_controller

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

            ComboBox
            {
                id: presetsCombobox
                implicitContentWidthPolicy: ComboBox.WidestText
                hoverEnabled: true
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Available Presets")
                textRole: "presetName"
                model: AppModel.presetsModel

            }


            Row{
                spacing: 8
                Button
                {
                    id: startReflowButton
                    text: qsTr("START")
                    hoverEnabled: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Start reflow process")
                    Material.background: Material.Green
                    enabled: presetsCombobox.currentIndex !== -1;
                    onClicked:
                    {
                        AppModel.reflowController.selectPreset(AppModel.presetsModel.at(presetsCombobox.currentIndex).presetId);
                        AppModel.reflowController.startReflow();
                    }
                }
                Button
                {
                    id: stopReflowButton
                    text: qsTr("STOP")

                    hoverEnabled: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Stop reflow process")

                    Material.background: Material.Red
                    onClicked:
                    {
                        AppModel.reflowController.stopReflow();
                    }
                }
            }

        }

        ChartView {
            title: "Line"
            Layout.fillWidth: true
            Layout.fillHeight: true
            antialiasing: true
            theme: ChartView.ChartThemeDark
            LineSeries {
                name: "LineSeries"
                XYPoint { x: 0; y: 0 }
                XYPoint { x: 1.1; y: 2.1 }
                XYPoint { x: 1.9; y: 3.3 }
                XYPoint { x: 2.1; y: 2.1 }
                XYPoint { x: 2.9; y: 4.9 }
                XYPoint { x: 3.4; y: 3.0 }
                XYPoint { x: 4.1; y: 3.3 }
            }
        }

    }
}
