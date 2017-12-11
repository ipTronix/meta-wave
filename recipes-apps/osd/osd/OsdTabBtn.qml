//OsdTabBtn.qml
import QtQuick 2.0

Item {
    id: btn

    property int btnWidth: 180
    property int btnHeight: 32
    property bool btnEnabled: true
    property bool btnSelected: false
    property bool btnHighlighted: false
    property string btnText: ""

    width:  btnWidth
    height: btnHeight

    signal btnClicked

    //--------------------------------
    // Corpo del tasto (sfondo scuro)
    //--------------------------------
    Rectangle {
        id: backg
        width: btn.width; height: btn.height
        color: btnHighlighted ? color_HighlightBackgd : color_Dark

        //Rettangolo di evidenziazione
        //copre la parte superiore del background lasciando il
        //bordo scuro inferiore
        Rectangle {
            id: backgHighl
            width: btn.width; height: btn.height-4
            color: btn.btnSelected ?
                       "transparent" :
                       (btnHighlighted ? color_HighlightBackgd : color_Background)
        }

        //Testo al centro
        Text {
            id: btnTxt
            anchors.centerIn: backg
            color: btnSelected ? "black" : color_HeavyDark
            font { family: "Myriad Pro"; pixelSize: 18 }
            text: btn.btnText
        }

        MouseArea {
            anchors.fill: parent
            //enabled: btnEnabled
            hoverEnabled: btnEnabled
            onClicked: btnClicked()
            onEntered: btnHighlighted = true
            onExited:  btnHighlighted = false
        }
    }
}
