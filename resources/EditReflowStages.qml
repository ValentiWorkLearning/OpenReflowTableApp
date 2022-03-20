import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtCharts
import QtQuick.Layouts

import app.root_model
import app.presets_model
import app.reflow_controller

Popup
{
    id: editReflowStages

    property int editPresetId;
    property int presetIndex;

    width: parent.width* 0.9;
    height: parent.height* 0.8;

    parent: Overlay.overlay

    x: parent.width / 2 - this.width / 2;
    y: parent.height / 2 - this.height / 2;

    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

    function refreshEditChartView(presetId)
    {
        AppModel.presetsModel.sheduleGetPresetStages(presetId)
    }

    function appendNewCharpoint()
    {
        let pointsCount = stagesSeries.count;
        let canUseLastPoint = pointsCount >= 2;

        let isEmpty = stagesSeries.count === 0;

        if(isEmpty)
        {
            const medianTimepoint = 150;
            const medianTempreature = 150;

            stagesSeries.append(0, 0);
            stagesSeries.append(medianTimepoint,medianTempreature);

            scatterSeries.append(0,0);
            scatterSeries.append(medianTimepoint,medianTempreature);
            return;
        }

        if(!canUseLastPoint)
            return;

        const timepointOffset = 50;

        let lastPoint = stagesSeries.at(pointsCount - 1);
        let newStageX = lastPoint.x + timepointOffset;
        let newStageY = lastPoint.y;

        stagesSeries.append(newStageX, newStageY);
        scatterSeries.append(newStageX, newStageY);
    }

    Connections
    {
        target:  AppModel.presetsModel
        function onPresetStagesReady(presetStages)
        {
            const isStagesEmpty = presetStages.length === 0;
            if(isStagesEmpty)
                return;

            stagesSeries.append(0,0);
            scatterSeries.append(0,0);

            for(let presetStage of presetStages){
                stagesSeries.append(presetStage.duration,presetStage.temperature);
                scatterSeries.append(presetStage.duration,presetStage.temperature);
            }
        }
    }

    ColumnLayout
    {
        anchors.fill: parent
        RowLayout
        {
            Label
            {
                id:timepointLabel
                text: qsTr("Timepoint:")
            }
            Label
            {
                id:timepointValue
                Layout.preferredWidth: 80
                font.pixelSize: 42
                text:editChartview.selectedPoint? Math.round(editChartview.selectedPoint.x):""
            }

            Label
            {
                id:temperatureLabel
                text:qsTr("Temperature:")
            }
            Label
            {
                id:temperatureValue
                Layout.preferredWidth: 80
                font.pixelSize: 42
                text: editChartview.selectedPoint?Math.round(editChartview.selectedPoint.y) : ""
            }
            CheckBox
            {
                id:enableScatterChart
                text: qsTr("Scatter")
                checked: true;
            }
            Button
            {
                id:addNewChartpoint
                text:qsTr("Add point")
                hoverEnabled: true

                ToolTip.visible: hovered
                ToolTip.text: qsTr("Add new reflow stage point")

                onClicked:
                {
                    appendNewCharpoint();
                }
            }
        }
        //https://stackoverflow.com/questions/48789849/draging-a-point-on-qtcharts-in-qml
        ChartView {
            id: editChartview

            Layout.fillHeight: true;
            Layout.fillWidth: true;
            antialiasing: true
            theme: ChartView.ChartThemeDark

            property real toleranceX: 15
            property real toleranceY: 15

            property var selectedPoint: undefined
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
                id: stagesSeries
                axisY:  axisYConstrain
                axisX: axisXConstrain
                pointsVisible: true
                name: AppModel.presetsModel.at(editReflowStages.presetIndex).presetName
            }
            ScatterSeries
            {
                id:scatterSeries
                axisY: axisYConstrain
                axisX: axisXConstrain
                pointsVisible: true
                visible: enableScatterChart.checked
                markerSize: 15
            }
            MouseArea {
                anchors.fill: parent
                onPressed:(mouse)=>
                          {
                              let cp = editChartview.mapToValue(Qt.point(mouse.x,mouse.y));
                              for(let i = 1;i < stagesSeries.count; ++i)
                              {
                                  let p = stagesSeries.at(i);
                                  let canSelect =
                                  Math.abs(cp.x - p.x) <= editChartview.toleranceX
                                  && Math.abs(cp.y - p.y) <= editChartview.toleranceY;

                                  if(canSelect)
                                  {
                                      editChartview.selectedPoint = p;
                                      break;
                                  }
                              }
                          }

                onPositionChanged:(mouse)=> {
                                      if(editChartview.selectedPoint != undefined) {
                                          let p = Qt.point(mouse.x, mouse.y);
                                          let cp = editChartview.mapToValue(p);
                                          let canReplace =
                                          cp.x >= axisXConstrain.min
                                          && cp.x <= axisXConstrain.max
                                          && cp.y >= axisYConstrain.min
                                          && cp.y <= axisYConstrain.max;

                                          if(canReplace) {
                                              stagesSeries.replace(editChartview.selectedPoint.x, editChartview.selectedPoint.y, cp.x, cp.y);
                                              scatterSeries.replace(editChartview.selectedPoint.x, editChartview.selectedPoint.y, cp.x, cp.y);
                                              editChartview.selectedPoint = cp;
                                          }
                                      }
                                  }

                onReleased: {
                    editChartview.selectedPoint = undefined;
                }
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
                onClicked:
                {
                    let stagesArray = new Array();

                    for(let i = 0; i<stagesSeries.count; ++i)
                    {
                        let stageItem = {"temperature":Math.round(stagesSeries.at(i).y), "duration":Math.round(stagesSeries.at(i).x)};
                        stagesArray.push(stageItem);
                    }
                    let refreshObject = {"stages":stagesArray};
                    AppModel.presetsModel.updatePresetStages(editPresetId,JSON.stringify(refreshObject));
                    editReflowStages.close();
                }
            }

            Button
            {
                id: cancelButton;
                text: "Cancel"
                onClicked:
                {
                    editReflowStages.close();
                }
            }
        }
    }
    Component.onCompleted:{
        refreshEditChartView(editReflowStages.editPresetId);
    }
}
