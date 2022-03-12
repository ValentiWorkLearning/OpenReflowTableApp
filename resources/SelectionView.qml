import QtQuick
import QtQuick.Controls

Item {

    signal presetsSetupClicked;
    signal reflowProcessClicked;

    Column
    {
        anchors.centerIn: parent
        Button
        {
            id:presetsButton
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("PRESETS SETUP")

            onClicked: presetsSetupClicked();
        }

        Button
        {
            id:reflowProcessSetup
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("REFLOW PROCESS")

            onClicked: reflowProcessClicked();
        }
    }
}
