/*****************************************************************************
*   Fie:		InputRowDelegate.qml
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
import QtQuick 2.0
import "global.js" as Glob

//----------------------------------------
// Row model
//----------------------------------------
Item {
    id: itm
    width: 320; height: rowHeight
    state: "st_normal"
    property bool   isCurrInput: false
    property string input_Name: "DVI"
    property string input_Type: "inp_DVI" // "inp_DP"
    property bool   ddcCi_State:     false
    property bool   anaEDID_State:   false
    property bool   analogEDIDcheck: false
    property bool   arrowUpEnabled:  true
    property bool   arrowDwnEnabled: true

    property int    prioNumber: 0  //priorità effettiva 1..6
    property int    prioDigit:  0  //cifra priorità visualizzata 1..6 a sx della riga
    //sono state differenziate per gestire il caso di ingressi DVI-D1 e DP1
    //disabilitati. Le priorità restano 1..6 ed è possibile scambiare di posto 3 e 4
    //che verranno poi convertiti in 0..1 per l'invio sulla seriale.

    property bool manMode: data_model.inputMode === Glob.inModeMAN

    //indica se questa riga è in corso di modifica da tastiera
    property bool isEditing: false
    //elemento della riga in corso di modifica da tastiera
    property int editItem: 0

    signal switchRows(int from, int to)
    signal curr_IN_clicked()
    signal ddc_CI_pos_clicked()

    function setInpIcon()
    {
         if(input_Type === "inp_DVI")
             return _IMG_PATH + "ico_dvi.png"
         else if(input_Type === "inp_DP")
             return _IMG_PATH + "ico_dp.png"
         else
             return ""
    }

    //------------
    // Tasto PIU
    //------------
    Connections {
        target: main
        onKey_plus: {
            if(isEditing === false) return
            switch(editItem) {
                case 0: if(manMode) curr_IN_clicked(); break;
                case 1: ddc_CI_pos_clicked(); break;
                case 2: doArrowUp(); break;
            }
            resOsdTout()
        }
    }

    //------------
    // Tasto MENO
    //------------
    Connections {
        target: main
        onKey_minus: {
            if(isEditing === false) return
            if(editItem === 2) doArrowDown()
            resOsdTout()
        }
    }

    //----------------------------
    // Priority number
    //----------------------------
    Text {
        id: tabTxt
        anchors.verticalCenter: parent.verticalCenter
        color: "black"; font { family: "Myriad Pro"; pixelSize: 14 }
        //non visualizzo il numero a fianco ai primi due ingressi DVI-D1 e DP1
        //se sono disabilitati
        //text: (prioNumber != 0) ? prioNumber : ""
        text: (prioDigit != 0) ? prioDigit : ""
    }

    //----------------------------
    // Body
    //----------------------------
    Rectangle {
        id: backg1
        anchors {
            verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 18
        }
        width: 320; height: rowHeight

        MouseArea {
            anchors.fill: parent
            //evito  interferenza con la tastiera
            hoverEnabled: main.tabEditActive === false
            onEntered: itm.state = "st_highlighted"
            onExited:  itm.state = "st_normal"
            //onClicked: console.log("cliccato TabInputs")
        }
    }

    //-----------------------------------------------
    // Evidenziazione gialla (per edit da tastiera)
    //-----------------------------------------------
    Rectangle {
        id: highl
        property var pos: [128, 183, 288]
        property var rectWidth: [32, 32, 50]
        visible: main.tabEditActive && isEditing
        width: rectWidth[editItem]; height: 30
        x: pos[editItem]
        anchors.verticalCenter: parent.verticalCenter
        color: color_HighlightBackgd
    }

    //----------------------------
    // Input connector icon
    //----------------------------
    Image {
        id: inIcon
        width: rowHeight; height: rowHeight
        anchors {
            verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 5+18
        }
        source: setInpIcon()
    }

    //----------------------------
    // Input name
    //----------------------------
    Text {
        id: txtInpName
        anchors {
            verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 45+18
        }
        color: "black"
        font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 16 }
        text: input_Name
    }

    //------------------------------------------
    // Current input radio button placeholder
    //------------------------------------------
    Rectangle {
        id: btn
        visible: itm.state !== "st_disabled"
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left; leftMargin: 135
        }
        width: 18; height: 18; radius: 9  //round shape
        color: isCurrInput? (manMode ? "red" : "gray") : "white"
        border { color: "gray"; width: 1 }
        //LOLLO nota: eventualmente fare area più grande per agevolare l'intercettazione
        //del click
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if(manMode) curr_IN_clicked()
            }
        }
    }

    //-------------------------------------------
    // DDC/CI State radio button placeholder
    //-------------------------------------------
    Rectangle {
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left; leftMargin: 190
        }
        visible: (data_model.ddc_ci_Mode !== Glob.ddciOFF) &&
                 (itm.state !== "st_disabled")

        width: 18; height: 18; radius: 9  //round shape
        color: {
            var mode = data_model.ddc_ci_Mode

            //auto (follow active input)
            if(mode === Glob.ddciAUTO)
                return isCurrInput? "gray" : "white"
            //off
            else if(mode === Glob.ddciOFF)
                return "white"
            //manual
            else if(mode === Glob.ddciMAN)
                return ddcCi_State? "red" : "white"
            else
                return ""
        }
        border { color: "gray"; width: 1 }
        MouseArea {
            anchors.fill: parent
            onClicked: ddc_CI_pos_clicked()
        }
    }

    //-------------------------------------
    // Analog EDID check box placeholder
    //-------------------------------------
    Rectangle {
        visible: analogEDIDcheck && (itm.state !== "st_disabled")
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left; leftMargin: 240
        }
        width: 18; height: 18
        color: anaEDID_State? "red" : "white"
        border { color: "gray"; width: 1 }
    }

    //----------------------------
    // Ico arrow up
    //----------------------------
    function doArrowUp()
    {
        //not if already on first row
        if(prioNumber > 1)
            switchRows(prioNumber-1, prioNumber-2) //adj from range 1..6 to 0..5
    }

    MouseArea {
        width: 28; height: 28
        //Hide up arrow on first row
        visible: (prioNumber !== 1) &&
                 (itm.state !== "st_disabled") &&
                  arrowUpEnabled
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left; leftMargin: 285
        }
//        Rectangle {
//            color: "#e26969"
//            anchors.fill: parent
//        }
        Image {
            anchors.centerIn: parent
            source: _IMG_PATH + "ico_arrow_up.png"
        }
        onClicked: {
            doArrowUp()
        }
    }

    //----------------------------
    // Ico arrow down
    //----------------------------
    function doArrowDown()
    {
        //not if already on last row
        //(questo check non serve se DVI-D1 e DP1 disabilitati)
        if(prioNumber < N_INPUTS)
            switchRows(prioNumber-1, prioNumber) //adj from range 1..6 to 0..5
    }

    MouseArea {
        width: 28; height: 28
        //Hide down arrow on last row
        visible: {
            var numRows = N_INPUTS
            if(data_model.inputs1Disable)
                numRows = 2
            //sono sull'ultima riga
            return (prioNumber !== numRows) &&
                   (itm.state !== "st_disabled") &&
                   arrowDwnEnabled
        }
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left; leftMargin: 314
        }
//        Rectangle {
//            color: "#7de268"
//            anchors.fill: parent
//        }
        Image {
            anchors.centerIn: parent
            source: _IMG_PATH + "ico_arrow_dwn.png"
        }
        onClicked: doArrowDown()
    }

    //-----------------------------------------------
    states: [
        State {
            name: "st_normal"
            PropertyChanges {
                target: backg1; color: "black"; opacity: 0.1
            }
        },
        State {
            name: "st_highlighted"
            PropertyChanges {
                target: backg1; color: color_HighlightBackgd; opacity: 1.0
            }
        },
        //per primi due ingressi DVI-D1 e DP1 disabilitati (se non montati su PCB)
        State {
            name: "st_disabled"
            PropertyChanges {
                target: itm; visible: false //opacity: 0.4
            }
        }
    ]
}



























