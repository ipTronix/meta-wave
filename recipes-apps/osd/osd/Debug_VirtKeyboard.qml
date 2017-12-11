//Debug_VirtKeyboard
import QtQuick 2.0

Rectangle {
    width: 240
    height: 50
    border.color: "blue"
    border.width: 2

    //Tastierino per simulazione tastiera fisica
    Row {
        anchors.centerIn: parent

        Rectangle {
            width: 60; height: 40
            border.color: "blue"
            border.width: 2

            Text {
                anchors.centerIn: parent
                text: "ESC"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log("ESC")
                    handle_Key_Escape()
                }

            }
        }

        Rectangle {
            width: 60; height: 40
            border.color: "blue"
            border.width: 2

            Text {
                anchors.centerIn: parent
                color: "red"
                text: "Enter"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log("Enter")
                    handle_Key_Menu()
                }
            }
        }

        Rectangle {
            width: 60; height: 40
            border.color: "blue"
            border.width: 2

            Text {
                anchors.centerIn: parent
                text: "Up"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log("Up")
                    handle_Key_Plus()
                }
            }
        }

        Rectangle {
            width: 60; height: 40
            border.color: "blue"
            border.width: 2

            Text {
                anchors.centerIn: parent
                text: "Down"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log("Down")
                    handle_Key_Minus()
                }
            }
        }
    }
}
