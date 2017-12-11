/*****************************************************************************
*   Fie:		TabLoadSave.qml
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
import QtQuick 2.0

Item {
    id: page
    width: 480; height: 320
    property int popupMsgID: 0

    Component.onCompleted: console.log("-- TabLoadSave")

    //---------------------------------------------------------------------------
    // Tastiera
    //---------------------------------------------------------------------------
    property int currElement: -1

    //Tasto MENU seleziona a rotazione il controllo da editare
    Connections {
        target: main
        onKey_enter: {
            main.tabEditActive = true
            if(currElement < 2)
                currElement++
            else
                currElement = 0
        }
    }
    Connections {
        target: main
        onKey_plus: {
            switch(currElement) {
                case 0: btn1Action(); break;
                case 1: btn2Action(); break;
                case 2: btn3Action(); break;
            }
            resOsdTout()
        }
    }

    Connections {
        target: main
        onKey_minus: {
            //nota: non usato
            resOsdTout()
        }
    }

    //Tasto ESC fine edit
    Connections {
        target: main
        onKey_esc: {
            currElement = -1
            main.tabEditActive = false
        }
    }

    //---------------------------------------------------------------------------
    // Controlli
    //---------------------------------------------------------------------------

    //---------------------
    // "Save settings"
    //---------------------
    function btn1Action() {
        //nota: visualizzato subito perché non abbiamo un feedback da
        //LPC relativo al salvataggio
        popup.visible = true
        popupMsgID = 0
        resOsdTout()
        data_model.doSettingsSave()
    }

    CustomBtn {
        id: testBtn
        x: 130; y: 40
        btnColor: color_Dark
        highlighted: currElement === 0
        btnTxt: qsTr("Save settings")
        onBtnClicked: btn1Action()
    }

    //--------------------------
    // "Recall saved settings"
    //--------------------------
    function btn2Action() {
        popup.visible = false
        popupMsgID = 1
        resOsdTout()
        data_model.doSettingsRecall()
    }

    CustomBtn {
        id: testBtn2
        x: 130; y: 110
        btnColor: color_Dark
        highlighted: currElement === 1
        btnTxt: qsTr("Recall saved settings")
        onBtnClicked: btn2Action()
    }

    //--------------------------
    // "Load factory settings"
    //--------------------------
    function btn3Action() {
        popup.visible = false
        popupMsgID = 2
        resOsdTout()
        data_model.doFactoryReset()
    }

    CustomBtn {
        id: testBtn3
        x: 130; y: 180
        btnColor: color_Dark
        highlighted: currElement === 2
        btnTxt: qsTr("Load factory settings")
        onBtnClicked: btn3Action()
    }

    //intercetta il segnale di arrivo di almeno un SET valido
    //per visualizzare il popup di conferma es."Settings saved"
    Connections {
        target: sig_conn
        onSettingsCmdSent: {
            //console.log("\n___onSettingsCmdSent___")
            popup.visible = true
        }
    }

    //---------------------
    // Popup
    //---------------------
    Rectangle {
        id: popup
        width: 250; height: 80; anchors.centerIn: parent
        color: "white"; border {color: "black"; width: 1 }
        visible: false
        onVisibleChanged: plotUpdateTimer.restart()

        property var popupMsg: [qsTr("Settings saved"),
                                qsTr("Settings loaded"),
                                qsTr("Settings restored")]
        Text {
            id: popupTxt
            anchors.centerIn: parent
            color: "black" //color_Dark
            font { family: "Myriad Pro"; pixelSize: 20 }
            text: popup.popupMsg[popupMsgID]
        }

        //close popup
        Timer {
            id: plotUpdateTimer
            interval: 1000
            onTriggered: popup.visible = false
        }
    }
}
































