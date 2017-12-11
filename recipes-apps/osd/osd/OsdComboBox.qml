/*****************************************************************************
*   Fie:		OsdComboBox.qml
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
import QtQuick 2.0

Item {
    id: comboBox

    property bool listEnabled: true
    property var listItems: []
    property int numListRows: listItems.length
    property int cellWidth: 140
    property int cellHeight: 26
    property int dropDwnCellHeight: 22
    property int currRow: 1
    property var currVal
    property bool listOpened: false
    signal listOpenEvent(bool opened)
    signal listItemChanged(int num)

    Component.onCompleted: {
        currVal = listItems[currRow]
        console.log("comboBox onCompleted  currVal:", currVal,
                    "currRow", currRow,
                    "listItems",listItems[currRow]
                    )
    }

    //-----------------------------------
    // Text box with arrow on the right
    //-----------------------------------
    Rectangle {
        id: dropDownCell
        width: comboBox.cellWidth; height: comboBox.cellHeight
        anchors.centerIn: parent
        color: color_InternBackgd
        border { color: "black"; width: 1 }

        //------------------
        // Valore corrente
        //------------------
        Text {
            id: dropDownTxt
            anchors {
                verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 8
            }
            color: "black"
            font { family: "Myriad Pro"; pixelSize: 16 }
            //note: keeps fixed while the user scrolls the drop-down list
            //updates only when a new value has been selected
            text: listEnabled ? comboBox.currVal : "- - -"
        }

        //Freccia in basso
        Image {
            visible: listEnabled
            anchors {
                verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: 5
            }
            source: _IMG_PATH + "ico_arrow_dwn.png"
        }
        MouseArea {
            anchors.fill: parent
            enabled: listEnabled
            onClicked: {
                resOsdTout()
                //toggle open/close
                comboBox.listOpened = !comboBox.listOpened
                comboBox.listOpenEvent(comboBox.listOpened)
            }
        }
    }

    //-----------------------------------
    // Drop down list
    //-----------------------------------
    Rectangle {
        id: dropDown2
        width:   comboBox.cellWidth
        height:  comboBox.numListRows * comboBox.dropDwnCellHeight
        visible: comboBox.listOpened && listEnabled
        anchors { horizontalCenter: parent.horizontalCenter; top: dropDownCell.bottom }
        color: color_InternBackgd
        border { color: "black"; width: 1 } //nota: il bordo viene coperto dal repeater

        Repeater {
            model: comboBox.numListRows
            Rectangle {
                x: 0; y: comboBox.dropDwnCellHeight * index
                width: comboBox.cellWidth; height: comboBox.dropDwnCellHeight
                color: index === comboBox.currRow? "black" : color_InternBackgd
                Text {
                    id: itemTxt
                    anchors.centerIn: parent
                    color: index === comboBox.currRow? color_InternBackgd: "black"
                    font { family: "Myriad Pro"; pixelSize: 16 }
                    text: comboBox.listItems[index]
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: main.tabEditActive === false
                    onEntered: comboBox.currRow = index

                    onClicked: {
                        comboBox.currRow = index
                        console.log("cliccato dropDown item:", index, "val:",
                                    comboBox.listItems[index])
                        comboBox.currVal = comboBox.listItems[index]
                        listItemChanged(index)
                        comboBox.listOpened = false
                        comboBox.listOpenEvent(false)  //enable other combos
                    }
                }
            }
            Component.onCompleted: console.log("numListRows:", comboBox.numListRows)
        }
    }
}















































