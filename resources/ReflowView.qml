import QtQuick
import QtCharts
import QtQuick.Layouts
import QtQuick.Controls

import app.root_model
import app.presets_model
import app.reflow_controller

Item {
    signal requestBackToHome;


    function refreshPresetChartView(presetId)
    {
        AppModel.presetsModel.sheduleGetPresetStages(presetId)
    }

    function cleanupRealtimeChart()
    {
        realtimeDataSeries.removePoints(0,realtimeDataSeries.count)
    }
    Connections
    {
        target:  AppModel.presetsModel
        function onPresetStagesReady(presetStages)
        {
            selectedPresetSeries.removePoints(0,selectedPresetSeries.count)
            selectedPresetSeries.append(0,0);
            for(let presetStage of presetStages){
                selectedPresetSeries.append(presetStage.duration,presetStage.temperature);
            }
        }
    }

    Connections
    {
        target: AppModel.reflowController
        function onSystemStateChanged(systemState)
        {
            let realtimeTemperature = AppModel.reflowController.systemState.currentTemperature;
            let timepoint = AppModel.reflowController.systemState.systemTime;
            realtimeDataSeries.append(timepoint,realtimeTemperature);
        }
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

            ComboBox
            {
                id: presetsCombobox
                implicitContentWidthPolicy: ComboBox.WidestText
                hoverEnabled: true
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Available Presets")
                textRole: "presetName"
                model: AppModel.presetsModel
                onCurrentIndexChanged:
                {
                    refreshPresetChartView(AppModel.presetsModel.at(presetsCombobox.currentIndex).presetId);
                }
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
                        cleanupRealtimeChart();
                    }
                }
                Label
                {
                    id:currentTemperatureLabel
                    text: AppModel.reflowController.systemState.currentTemperature
                     font.pixelSize: 48
                }
            }

        }

        ChartView {
            title: "Line"
            Layout.fillWidth: true
            Layout.fillHeight: true
            antialiasing: true

            ValueAxis {
                        id: axisYConstrain
                        min: 0
                        max: 350
                    }

            ValueAxis
            {
                id: axisXConstrain
                min: 0
                max: 600
            }

            theme: ChartView.ChartThemeDark

            LineSeries {
                id: selectedPresetSeries
                axisY:  axisYConstrain
                axisX: axisXConstrain
                name: AppModel.presetsModel.at(presetsCombobox.currentIndex).presetName
            }
            LineSeries {
                id: realtimeDataSeries
                axisY:  axisYConstrain
                axisX: axisXConstrain
                name:"Realtime plot"
            }
        }

    }
}
