import QtQuick
import QtCharts
import QtQuick.Layouts
import QtQuick.Controls

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

            Column{
                Label
                {
                    text: qsTr("Back")
                }
                Button
                {
                    id: backToHome
                    width: 48
                    height: 48
                    text: qsTr("<")

                    onClicked: requestBackToHome();
                }
            }
            Item
            {
                Layout.fillWidth: true
            }
            Column{
                id: presetsLayout
                Layout.alignment: Qt.AlignRight
                Label
                {
                    text: qsTr("Available Presets")
                }
                ComboBox
                {
                    id: presetsCombobox
                    implicitContentWidthPolicy: ComboBox.WidestText
                    model: ["Preset Name 1", "Preset Name 2", "Preset Name 3"]
                }
            }

            Column{
                Layout.alignment: Qt.AlignRight
                Label
                {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Reflow control")
                }
                Row{
                    spacing: 8
                    Button
                    {
                        id: startReflowButton
                        text: qsTr("START")
                        Material.background: Material.Green
                    }
                    Button
                    {
                        id: stopReflowButton
                        text: qsTr("STOP")
                        Material.background: Material.Red
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
