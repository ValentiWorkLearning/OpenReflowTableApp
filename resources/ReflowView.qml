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

    function addRealtimePlotData(systemState)
    {
        let realtimeTemperature = AppModel.reflowController.systemState.currentTemperature;
        let timepoint = AppModel.reflowController.systemState.systemTime;
        realtimeDataSeries.append(timepoint,realtimeTemperature);
    }

    function processSelectedPresetIdApplying(systemState)
    {
        let itemsCount = AppModel.presetsModel.itemsCount();

        for(let it = 0; it !== AppModel.presetsModel.itemsCount(); ++it)
        {
            let isPresetFound = AppModel.reflowController.systemState.activePresetId ===
                AppModel.presetsModel.at(it).presetId;
            if(isPresetFound){
                presetsCombobox.currentIndex = it;
            }
        }
    }



    Loader
    {
        id:regulatorSettingsLoader;
        onLoaded:
            item.open();
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
            addRealtimePlotData(systemState);

            let canApplyPresetIdFromState =
                presetsCombobox.currentIndex ===-1
                && AppModel.reflowController.systemState.activePresetId !== "";

            if(!canApplyPresetIdFromState)
                return;

            processSelectedPresetIdApplying(systemState);

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

            ColumnLayout{
                BackButton
                {
                    onClicked: requestBackToHome();
                }
                ComboBox
                {
                    id: presetsCombobox
                    implicitContentWidthPolicy: ComboBox.WidestText
                    hoverEnabled: true
                    enabled: !AppModel.reflowController.systemState.isReflowRunning;
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Available Presets")

                    textRole: "presetName"
                    model: AppModel.presetsModel
                    onCurrentIndexChanged:
                    {
                        refreshPresetChartView(AppModel.presetsModel.at(presetsCombobox.currentIndex).presetId);
                    }
                }
            }
            ColumnLayout{
                Button
                {
                    id: startReflowButton
                    text: qsTr("START")
                    hoverEnabled: true
                    enabled: presetsCombobox.currentIndex !== -1 && !AppModel.reflowController.systemState.isReflowRunning;

                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Start reflow process")
                    Material.background: Material.Green

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
                    enabled: AppModel.reflowController.systemState.isReflowRunning;
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
            }
            ColumnLayout{
                Label
                {
                    id:currentTemperatureLabel
                    text: AppModel.reflowController.systemState.currentTemperature
                    font.pixelSize: 42
                }
                Button
                {
                    id: regulatorSettings
                    text: qsTr("REGULATOR")
                    hoverEnabled: true

                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Change regulator settings")

                    onClicked:
                    {
                        regulatorSettingsLoader.setSource(
                                    "qrc:/RegulatorSettingsPopup.qml"
                                    )
                    }
                }
            }

        }

        ChartView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            antialiasing: true
            theme: ChartView.ChartThemeDark
            animationOptions:ChartView.AllAnimations

            ValueAxis
            {
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
