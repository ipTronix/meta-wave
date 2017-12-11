/*****************************************************************************
*   Fie:		PopupPIN.qml
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
import QtQuick 2.0
import "global.js" as Glob

Rectangle {
    id: popup

    width: 350; height: 150
    color: "white"
    border { color: "gray"; width: 1 }

    property bool isEdit: true //sto editando oppure inserendo

    //workaround, gli array non funzionano
    property int pin0: 0   //unità
    property int pin1: 0
    property int pin2: 0
    property int pin3: 0   //migliaia

    function splitDigits(val)
    {
        pin0 = (val %10).toFixed(0)  //unità
        val -= pin0 //tolgo l'unità altrimenti la divisione usa l'arrotondamento
        val /= 10   //perché un var non è un int ma in questo caso float
        pin1 = (val %10).toFixed(0)
        val -= pin1
        val /= 10
        pin2 = (val %10).toFixed(0)
        val -= pin2
        val /= 10
        pin3 = (val %10).toFixed(0)   //migliaia

        console.log("splitDigits:", pin0, pin1, pin2, pin3)
    }

    function mergeDigits()
    {
        return pin0 + pin1*10 + pin2*100 + pin3*1000
    }

    Component.onCompleted: {
        console.log("--PopupPIN")
    }

    //all'apertura del popup appare 0-0-0-0 che l'utente dovrà editare
    onVisibleChanged: {
        if(visible) {
            console.log("-> PopupPIN onVisibleChanged")
            main.tabEditActive = true
            currElement = 0
            splitDigits(0)
        }
    }

    //---------------------------------------------------------------------------
    // Tastiera
    //---------------------------------------------------------------------------
    property int currElement: -1

    //Tasto MENU seleziona a rotazione il controllo da editare
    Connections {
        target: main

        //---- Tasto MENU -----
        onKey_enter: {
            if(currElement < 4)
                currElement++
            else
                currElement = 0
        }
    }

    //---- Tasto PIU -----

    Connections {
        target: main
        onKey_plus: {
            if(currElement === 0) {
                if(pin3 < 9) pin3++
            }
            else if(currElement === 1) {
                if(pin2 < 9) pin2++
            }
            else if(currElement === 2) {
                if(pin1 < 9) pin1++
            }
            else if(currElement === 3) {
                if(pin0 < 9) pin0++
            }
            resOsdTout()

            //tasto "OK": confermo (o controllo) le cifre inserite
            if(currElement === 4) {
                confirmPIN()
            }
        }
    }

    //---- Tasto MENO -----
    Connections {
        target: main
        onKey_minus: {
            if(currElement === 0) {
                if(pin3) pin3--
            }
            else if(currElement === 1) {
                if(pin2) pin2--
            }
            else if(currElement === 2) {
                if(pin1) pin1--
            }
            else if(currElement === 3) {
                if(pin0) pin0--
            }
            resOsdTout()
        }
    }

    //---- Tasto ESC -----
    Connections {
        target: main
        onKey_esc: {
            if(isEdit)
                popup.visible = false
            //se sono in fase di inserimento PIN, con ESC chiudo direttamente l'OSD
            else if(popup.visible)
                hideOSD()
        }
    }

    //-------------------------
    // Dummy area
    //-------------------------
    //evita di propagare il mouse hover alla finestra sotto
    MouseArea {
        width: 480; height: 320
        anchors.centerIn: parent
        hoverEnabled: main.tabEditActive === false
    }

    //------------------------------
    // Titolo "Enter PIN"
    //------------------------------
    Text {
        anchors {
            top:  parent.top; topMargin: 8
            left: parent.left; leftMargin: 8
        }
        color: color_HeavyDark
        font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 16 }
        text: isEdit ? qsTr("Edit PIN") : qsTr("Enter PIN")
    }

    Text {
        id: pinErrTxt
        visible: false
        anchors {
            top: parent.top; topMargin: 8
            horizontalCenter: parent.horizontalCenter
        }
        color: "red"
        font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 16 }
        text: qsTr("Invalid PIN")

        onVisibleChanged: if(visible) offWarnTimer.start()

        //off messaggio pin errato
        Timer {
            id: offWarnTimer
            interval: 1200
            onTriggered: pinErrTxt.visible = false
        }
    }

    //------------------------------
    // 4 Spin boxes inserimento PIN
    //------------------------------
    Row {
        id: boxes
        anchors { centerIn: parent; verticalCenterOffset: -20 }
        spacing: 5

        //nota: non uso un repeater per i soliti problemi ai bindings QML
        OsdSpinBox {
            id: box0
            highlighted: currElement === 0
            currVal: pin3
            onSpinUp:   if(pin3 < 9) pin3++
            onSpinDown: if(pin3)     pin3--
        }
        OsdSpinBox {
            id: box1
            highlighted: currElement === 1
            currVal: pin2
            onSpinUp:   if(pin2 < 9) pin2++
            onSpinDown: if(pin2)     pin2--
        }
        OsdSpinBox {
            id: box2
            highlighted: currElement === 2
            currVal: pin1
            onSpinUp:   if(pin1 < 9) pin1++
            onSpinDown: if(pin1)     pin1--
        }
        OsdSpinBox {
            id: box3
            highlighted: currElement === 3
            currVal: pin0
            onSpinUp:   if(pin0 < 9) pin0++
            onSpinDown: if(pin0)     pin0--
        }
    }

    //------------------------------
    // Tasto "OK" in basso
    //------------------------------
    function confirmPIN()
    {
        var inserita = mergeDigits()
        console.log("inserita:", inserita)
        resOsdTout()

        //sto editando il PIN
        if(isEdit) {
            data_model.lockPIN = inserita
            data_model.updateSetting(Glob.osdLOCK_PIN, inserita)
            popup.visible = false
        }
        //sto sbloccando
        else {
            if(inserita === data_model.lockPIN) {
                data_model.serLockActive = false
                popup.visible = false
            }
            else {
                pinErrTxt.visible = true
                console.log("PIN errato")
            }
        }
    }

    CustomBtn {
        id: btnOK
        width: 75; height: 34

        anchors { centerIn: parent; verticalCenterOffset: 40 }
        btnColor: color_Dark
        highlighted: currElement === 4
        btnTxt: qsTr("OK")

        onBtnClicked: confirmPIN()
    }
}

















































