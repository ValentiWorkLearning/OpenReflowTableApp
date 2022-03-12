import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow  {
    width: 640
    height: 480
    visible: true
    title: qsTr("Open Oven Project")

    FontLoader
    {
        id:mainAppFont;
        source:"qrc:/fonts/Roboto-Black.ttf";
    }

    StackView {
        id: ovenStackView
        initialItem: mainView
        anchors.fill: parent
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
