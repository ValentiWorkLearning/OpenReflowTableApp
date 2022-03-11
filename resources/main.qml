import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts

ApplicationWindow  {
    width: Screen.desktopAvailableWidth / 2
    height: Screen.desktopAvailableHeight / 2
    visible: true
    title: qsTr("Open Oven Project")

    FontLoader
    {
        id:mainAppFont;
        source:"qrc:/fonts/Roboto-Black.ttf";
    }

    ChartView {
        title: "Line"
        anchors.fill: parent
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
