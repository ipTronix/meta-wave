/*****************************************************************************
*   Fie:		Slider.qml
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
import QtQuick 2.0

MouseArea {
    id: slider
    width: 165; height: 40

    //posizione cursore
    property int percenVal: 0

    //cifra numerica (default = posizione cursore)
    property int percenDigit: percenVal

    property int sliderWidth: slider.width - 10
    signal sliderValChanged(int percVal)
    propagateComposedEvents: true

    Component.onCompleted: setSlider(percenVal)

    function setSlider(percVal)
    {
        //Range cursore: 0..100
        if(percVal > 100) percVal = 100;

        var xpos = slider.sliderWidth * percVal / 100
        cursor.x = xpos - cursor.width/2
        bar_sx.width = xpos;
    }

    onPercenValChanged: {
        setSlider(percenVal)
    }

    //segmento di sfondo
    Rectangle {
        id: bar_sfondo
        anchors { left: slider.left; verticalCenter: parent.verticalCenter }
        width: slider.sliderWidth //340;
        height: 8
        border { color: "black"; width: 1 }
        color: color_InternBackgd
    }

    //segmento di sx
    Rectangle {
        id: bar_sx
        anchors { left: slider.left; verticalCenter: parent.verticalCenter }
        width: 0; height: 8
        color: color_HeavyDark
    }

    //cursor
    Rectangle {
        id: cursor
        anchors.verticalCenter: parent.verticalCenter
        width: 10; height: 20
        color: "white"
        border { color: "black"; width: 1 }
    }

    //percentage %left
    Text {
        id: percTxt
        anchors.verticalCenter: parent.verticalCenter
        anchors { left: slider.right; leftMargin: 5 }
        //anchors { centerIn: slider; verticalCenterOffset: -18 }
        color: "black"
        font { family: "Myriad Pro"; pixelSize: 16 }
        text: slider.percenDigit + "%"
    }

    onMouseXChanged: {
        var buf = mouseX;

        //clamps
        if(buf > slider.sliderWidth)
            buf = slider.sliderWidth;
        else if(buf < 0)
            buf = 0

        cursor.x = buf - 10/2 //10/2 e' la larghezzza del cursore
        bar_sx.width = buf;
        //trigger event
        slider.sliderValChanged(buf * 100 / slider.sliderWidth)
    }
}



















































