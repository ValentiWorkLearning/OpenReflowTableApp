import QtQuick.Controls

Button
{
    id: backToHome
    width: 48
    height: 48
    hoverEnabled: true
    text: qsTr("<")
    ToolTip.visible: hovered
    ToolTip.text: qsTr("Back")
}
