import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import app.root_model
import app.presets_model


ApplicationWindow  {
    id: mainWindow
    width: 640
    height: 480
    visible: true
    title: qsTr("Open Oven Project")
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    MouseArea {
        anchors.fill: parent
        propagateComposedEvents: true

        onPressed:(mouse)=>{ pos = Qt.point(mouse.x, mouse.y);}
        onClicked: (mouse)=>{mouse.accepted = false};

        onReleased: (mouse)=> {mouse.accepted = false};
        onDoubleClicked: (mouse)=>{mouse.accepted = false};
        onPressAndHold: (mouse)=>{mouse.accepted = false};

        onPositionChanged:(mouse)=>{
            var diff = Qt.point(mouse.x - pos.x, mouse.y - pos.y)
            ApplicationWindow.window.x += diff.x
            ApplicationWindow.window.y += diff.y
            mouse.accepted = false;
        }
        property point pos
    }

    FontLoader
    {
        id:mainAppFont;
        source:"qrc:/fonts/Roboto-Black.ttf";
    }

    Button
    {
        id:refreshButton
        anchors.top: parent.top;
        anchors.right: closeAppButton.left
        anchors.topMargin: 8
        anchors.rightMargin: 8
        icon.source: "qrc:/icons/refresh.svg"
        onClicked:
        {
            AppModel.refresh();
        }
    }
    Button
    {
        id: closeAppButton
        anchors.top: parent.top;
        anchors.right: parent.right
        anchors.topMargin: 8
        anchors.rightMargin: 8

        width: 48
        height: 48

        text: qsTr("X")
        onClicked:
        {
            Qt.quit()
        }
    }

    ColumnLayout
    {
        anchors.fill:parent

        StackView {
            id: ovenStackView
            initialItem: mainView
            Layout.fillWidth:true
            Layout.fillHeight:true
        }

        ConnectionHandler
        {
            id: connectionHandler
            Layout.fillWidth:true
        }
    }
    

    Component
    {
        id: presetsView
        PresetsView
        {
            onRequestBackToHome: ovenStackView.pop(null)
        }
    }
    Component
    {
        id:reflowView
        ReflowView
        {
            onRequestBackToHome: ovenStackView.pop(null)
        }

    }
    SelectionView
    {
        id: mainView

        onPresetsSetupClicked:{
            ovenStackView.push(presetsView)
        }
        onReflowProcessClicked:{
            ovenStackView.push(reflowView);
        }
    }
}
