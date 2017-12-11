/*****************************************************************************
*   Fie:		OsdNumBox.qml
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
import QtQuick 2.0

Item {
    id: numBox

    width:  80
    height: 26

    property var currVal
    property bool boxEnabled: true
    property bool typePIN: false
    property bool highlighted: false

    signal boxClicked

    Component.onCompleted: {
        //console.log("OsdNumBox onCompleted  currVal:", currVal)
    }

    //------------------------------
    // Text box cifra numerica
    //------------------------------
    Rectangle {
        id: box
        width: numBox.width; height: numBox.height
        color: highlighted ? color_HighlightBackgd : color_InternBackgd
        border {color: "black"; width: 1 }

        Text {
            id: numTxt
            anchors.centerIn: parent
            color: boxEnabled ? "black" : "gray"
            font { family: "Myriad Pro"; pixelSize: 16 }
            text: typePIN ? "****" : numBox.currVal
        }

        MouseArea {
            anchors.fill: parent
            enabled: boxEnabled
            hoverEnabled: main.tabEditActive === false
            onClicked: {
                boxClicked()
                resOsdTout()
            }
            onEntered: highlighted = true
            onExited:  highlighted = false
        }
    }
}
















































