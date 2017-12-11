/*****************************************************************************
*   Fie:		OsdRadioButtons.qml
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
import QtQuick 2.0

Repeater {
    id: radioGroup
    model: 3
    property var radioLabels: []
    property int currItem: 0
    property int xPos
    property int yPos
    signal curr_ItemChanged(int val)

    //external dot
    Rectangle {
        id: radio
        //anchors.centerIn: parent
        x: radioGroup.xPos + index * 100; y: radioGroup.yPos
        width: 20; height: 20; radius: 10
        border { color: "gray"; width: 1 }
        property string radioTxt: ""

        //internal dot
        Rectangle {
            id: radioDot
            visible: radioGroup.currItem === index
            anchors.centerIn: parent
            width: 10; height: 10; radius: 5
            color: "black"
        }
        //label
        Text {
            id: radioLabel
            anchors {
                verticalCenter: radio.verticalCenter; left: radio.right; leftMargin: 6
            }
            color: "black"
            font { family: "Myriad Pro"; pixelSize: 12 }
            text: radioGroup.radioLabels[index]
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                curr_ItemChanged(index)
                resOsdTout()
            }
        }
    }
}
