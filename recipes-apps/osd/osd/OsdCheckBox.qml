/*****************************************************************************
*   Fie:		OsdCheckBox.qml
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
import QtQuick 2.0

//external box
Rectangle {
    id: radio
    property bool value
    signal boxClicked

    width: 20; height: 20
    border { color: "black"; width: 1 }

    //Component.onCompleted: console.log("OsdCheckBox value", value)

    //internal box
    Rectangle {
        id: radioDot
        visible: value === true
        anchors.centerIn: parent
        width: 10; height: 10
        color: "black"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            boxClicked()
            resOsdTout()
        }
    }
}


