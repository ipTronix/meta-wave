/*****************************************************************************
*   Fie:		OsdSpinBox.qml
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
import QtQuick 2.0

Item {
    id: numBox

    height: 32
    property int digitBoxWidth: 30
    property int arrowWidth: 18
    width: digitBoxWidth + arrowWidth

    property int currVal: 0
    property bool boxEnabled: true
    property bool highlighted: false
    property bool mouseHovered: false
    property bool doIncrement: true

    signal spinBoxClicked
    signal spinUp
    signal spinDown

    Component.onCompleted: {
        //console.log("OsdSpinBox onCompleted  currVal:", currVal)
    }

    onCurrValChanged: resOsdTout()

    //workaround problema di binding
    onBoxEnabledChanged: {
        if(boxEnabled) {
            frecciaSu.opacity = 1.0
            frecciaGiu.opacity = 1.0
        }
        else {
            frecciaSu.opacity = 0.5
            frecciaGiu.opacity = 0.5
        }
    }

    //------------------------------
    // Text box cifra numerica
    //------------------------------
    Rectangle {
        id: box
        width: numBox.width; height: numBox.height
        color: (highlighted || mouseHovered) ? color_HighlightBackgd : color_InternBackgd
        border { color: "black"; width: 1 }

        //Area della cifra a sx
        Rectangle {
            width: digitBoxWidth
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
            }
            color: "transparent"
            Text {
                id: numTxt
                anchors.centerIn: parent
                color: boxEnabled ? "black" : "gray"
                font { family: "Myriad Pro"; pixelSize: 20 }
                //numero negativo visualizzo i trattini: serve per le caselle
                //IP se DHCP non ha (ancora) ottenuto un lease
                text: numBox.currVal >= 0 ? numBox.currVal : "--"
            }
        }

        //Area mouse che copre l'intero controllo (per l'hover)
        MouseArea {
            anchors.fill: parent
            enabled: boxEnabled
            hoverEnabled: main.tabEditActive === false
            onClicked: {
                spinBoxClicked()
                resOsdTout()
            }
            onEntered: mouseHovered = true
            onExited:  mouseHovered = false
        }

        //Ripetizione veloce
        Timer {
            id: repeatTimer
            interval: 50; running: false; repeat: true
            onTriggered: {
                if(doIncrement)
                    spinUp()
                else
                    spinDown()
            }
        }

        //----------------------------
        // Ico arrow up
        //----------------------------
        MouseArea {
            id: icoup
            width: arrowWidth; height: 16
            enabled: boxEnabled
            anchors {
                verticalCenter: parent.verticalCenter
                verticalCenterOffset: -8
                right: parent.right
            }
//Rectangle {
//    color: "#e26969"
//    anchors.fill: parent
//}
            onClicked: spinUp()

            onPressAndHold: {
                doIncrement = true
                repeatTimer.start()
            }
            onPressed: {
                frecciaSu.opacity = 0.5
            }
            onReleased: {
                frecciaSu.opacity = 1.0
                repeatTimer.stop()
            }
        }
        Image {
            id: frecciaSu
            anchors.centerIn: icoup
            //opacity: boxEnabled ? 1.0: 0.5  //ha problemi, vedi workaround
            source: _IMG_PATH + "ico_arrow_up.png" //15+9
        }

        //----------------------------
        // Ico arrow down
        //----------------------------
        MouseArea {
            id: icdown
            width: arrowWidth; height: 16
            enabled: boxEnabled
            anchors {
                verticalCenter: parent.verticalCenter
                verticalCenterOffset: 8
                right: parent.right
            }
//Rectangle {
//    color: "#7de268"
//    anchors.fill: parent
//}
            onClicked: spinDown()

            onPressAndHold: {
                doIncrement = false
                repeatTimer.start()
            }
            onPressed: {
                frecciaGiu.opacity = 0.5
            }
            onReleased: {
                frecciaGiu.opacity = 1.0
                repeatTimer.stop()
            }
        }

        Image {
            id: frecciaGiu
            anchors.centerIn: icdown
            //opacity: boxEnabled ? 1.0: 0.5  //ha problemi, vedi workaround
            source: _IMG_PATH + "ico_arrow_dwn.png"
        }
    }
}
















































