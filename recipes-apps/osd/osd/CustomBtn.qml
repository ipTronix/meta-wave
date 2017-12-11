/*****************************************************************************
*   Fie:		CustomBtn.qml
*   Project:	OSD North Invent
*   Decription: rectangular shaped pushbutton
*               chages colour when mouse over
******************************************************************************/
import QtQuick 2.0

Rectangle {
    id: btn
    width: 200; height: 45
    color: (highlighted || mouseHovered) ? color_HighlightBackgd : btnColor

    property color btnColor
    property color btnTxtColor: color_BtnTxtNormal
    property string btnTxt
    property bool highlighted: false
    property bool mouseHovered: false
    property int txtPixsize: 20
    signal btnClicked

    Text {
        id: txt
        anchors.centerIn: btn
        color: (highlighted || mouseHovered) ? "black" : btnTxtColor
        font { family: "Myriad Pro"; pixelSize: txtPixsize }
        text: btnTxt
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: main.tabEditActive === false
        onClicked: btnClicked()
        onEntered: mouseHovered = true
        onExited:  mouseHovered = false
    }
}
