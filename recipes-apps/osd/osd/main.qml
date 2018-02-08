/*****************************************************************************
*   Fie:		main.qml
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
import QtQuick 2.2
import QtQuick.Controls 1.1
import "global.js" as Glob

ApplicationWindow {
    id: main
    visible: true
    width: 1024; height: 768
    title: qsTr("OSD")

    property bool debug_mode: false
    //property bool debug_mode: true

    property bool escPressMeasure: false
    property bool doSaveNetworkPar: false

    property int screen_width //non usati
    property int screen_height

    property color color_chroma_key:        "purple" //RGB = 128,0,128
    property color color_Background:        "#b2dcd9" //green/blue
    property color color_Dark:              "#2e7b90" //dark blue
    property color color_HeavyDark:         "#022d3b" //dark dark blue
    property color color_TxtNormal:         "#2e7b90" //
    property color color_BtnTxtNormal:      "white"
    property color color_BtnTxtInv:         color_Dark
    property color color_InternBackgd:      "#f1fafe"
    property color color_HighlightBackgd:   "#ffeeaa"
    property color color_Disabled:          "#cccccc"

    property var tabItems: ["tab_settings",
                            "tab_inputs",
                            "tab_timeouts",
                            "tab_load_save",
                            "tab_status",
                            "tab_comm"]

    //LOLLO per test Windows
   // property string _IMG_PATH: "/img/" //from resource file
    property string _IMG_PATH: DEB_WIN ? IMG_PATH_WIN : "/img/"

    property string currOsdTab: ""
    property bool osdVisible: false

    //per navigazione da tastiera
    property bool tabEditActive: false
    property int currTab: 0
	
	property bool dimmingPot: data_model.dimming_pot_ena

    signal key_enter
    signal key_plus
    signal key_minus
    signal key_esc

    signal show_popup_pin

    Component.onCompleted: {
        console.log("-- main")
        console.log("--> IMG_PATH_WIN", _IMG_PATH)

        currOsdTab = "tab_settings"
        windowBackground.color = color_chroma_key
        bkgMouseArea.hideCursor(); //testare

        //se nel messaggio INIT_END ho il flag segnale video non presente
        //faccio apparire OSD all'avvio
        if(data_model.doOpenOSD)
            showOSD()

        if(data_model.inputs1Disable)
            console.log("inputs1Disable settato")
        else
            console.log("inputs1Disable resettato")
    }

    //-----------------------
    // Fa apparire l'OSD
    //-----------------------
    function showOSD()
    {
        if(osdVisible === false) {
            osdVisible = true
            currOsdTab = tabItems[0] //tab settings
            currTab = 0
            //show mouse cursor
            bkgMouseArea.cursorShape = Qt.ArrowCursor
            bkgMouseArea.waitRelease = 2
            closeOsdTout.start()
            data_model.doOSDstatus(true)

            if(data_model.lockOnSerial && data_model.serLockActive)
                show_popup_pin()
        }
    }

    //-----------------------
    // Chiusura OSD
    //-----------------------
    Connections {
        target: data_model
        //chiusura forzata da comando SER_LOCK
        //nota: è in conflitto con la funzione keypress relay
        onCloseOSD: hideOSD()
    }

    function hideOSD()
    {
        //non chiudo se keypress relay in corso
        if(osdVisible && data_model.keyRelayOn === false) {
            osdVisible = false
            bkgMouseArea.hideCursor()
            data_model.doOSDstatus(false)

            //salvataggio pending delle impostazioni di rete
            if(doSaveNetworkPar) {
                doSaveNetworkPar = false
                data_model.saveNetwParams()
            }
        }
    }

    //-----------------------
    // Chroma key Background
    //-----------------------
    Rectangle {
        id: windowBackground
        anchors.fill: parent
        color: "transparent" //color_chroma_key
    }

//-----------------------------------------------------
// tasto di debug per simulare tasto destro mouse
//-----------------------------------------------------
Rectangle {
    id: test
    visible: debug_mode
    width: 100; height: 100
    color: "blue"
    MouseArea {
        anchors.fill: parent
        onClicked: showOSD()
    }
}

    //----------------------
    // OSD Timeout
    //----------------------
    function resOsdTout() {
        closeOsdTout.restart()
        //console.log("closeOsdTout.restart")
    }

    Timer {
        id: closeOsdTout
        interval: OSD_CLOSE_TOUT * 1000 //da sec a ms
        onTriggered: {
            console.log("Timeout: close OSD")
            hideOSD()
        }
    }

    //-------------------------------------------------------------------
    //Launch OSD on Right Button, close on click outside OSD window
    MouseArea {
        id: bkgMouseArea
        visible: !debug_mode
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        cursorShape: Qt.BlankCursor
        //counter to filter out the first onClicked after the right button onPressed
        property int waitRelease: 0

        function hideCursor() {
            cursorShape = Qt.BlankCursor
        }

        onPressed: {
            //nota: pressedButtons funziona solo con onPressed
            if(pressedButtons & Qt.RightButton) {
                console.log("RightButton")
                showOSD()
            }
        }
        //note: here we intercept all clicks outside the OSD area,
        //see "dummyMouseArea"
        onClicked: {
            //console.log("bkgMouseArea onClicked")
            if(osdVisible) {
                if(waitRelease) {
                    //filter out the first onClicked
                    if(--waitRelease === 0) {
                        hideOSD()
                    }
                }
            }
        }
    }

            //----------------------
            // Tasto ESC
            //----------------------
    function handle_Key_Escape()
            {
        //se OSD chiuso ESC forza il cambio sorgente
        if(osdVisible === false)
            data_model.doSourceChange() //sourceChgRepeatTimer.restart()

                //gestisco solo la prima pressione, filtro la ripetizione automatica
                if(escPressMeasure === false)
                {
                    escPressMeasure = true
                    escLongPressTimer.start()

            //Rilancio tasto ESC a monitor remoto (nota: qui OSD è forzato aperto)
                    if(data_model.keyRelayOn)
                        data_model.doSendKey(251, data_model.remoteSerialAdr) //0xFB
                    else
                    {
                        console.log("Qt.Key_Escape  tabEditActive:", tabEditActive)
                        if(tabEditActive) {
                            key_esc()
                            resOsdTout()
                        }
                        else
                            hideOSD()
                    }
                }
            }

    //Intervallo 5 secondi per controllo pressione lunga tasto ESC
    //per terminare la modalità relay keypress
    Timer {
        id: escLongPressTimer
        interval: 5 * 1000 //5 sec
        onTriggered: {
            console.log("escLongPress timeout")
            data_model.keyRelayOn = false
        }
    }

            //----------------------
            // Tasto MENU
            //----------------------
    function handle_Key_Menu()
            {
                //Rilancio tasti a monitor remoto
                if(data_model.keyRelayOn)
                    data_model.doSendKey(247, data_model.remoteSerialAdr) //0xF7
                else
                {
                    if(osdVisible === false)
                        showOSD()
                    else {
                        key_enter()
                        resOsdTout()
                    }
                }
            }

            //----------------------
            // Tasto PIU
            //----------------------
    function handle_Key_Plus()
            {
                console.log("Qt.Key_Plus")
                //se attivo il relay rilancio tasti al monitor remoto
                //in questo caso la navigazione da tastiera è bloccata
   //si esce da questa condizione con tasto ESC lungo o con il mouse
                if(data_model.keyRelayOn) {
                    data_model.doSendKey(252, data_model.remoteSerialAdr) //0xFC
                }
                //se OSD è chiuso gestisco il brightess (locale e/o remoto)
                else if(osdVisible === false)
                {
                    console.log("osdVisible false")
                    //se il monitor è il MultiDim master invio tasto + in broadcast
					if (!dimmingPot)
					{
						if(data_model.mdimMaster)
							data_model.doBrightBroadUP()
						else
							data_model.doBrightUP()
					}
                }
                //OSD aperto
                else {
                    console.log("OSD aperto")
                    //se sono dentro un tab gli rilancio il tasto +
                    if(tabEditActive) {
                        key_plus()
                        return
                    }
                    //navigo i tab
                    console.log("navigo i tab")
                    if(currTab < Glob.NUM_OSD_TABS - 1)
                        currTab++
                    else
                        currTab = 0
                    currOsdTab = tabItems[currTab]

                    resOsdTout()
                }
            }

            //----------------------
            // Tasto MENO
            //----------------------
    function handle_Key_Minus()
            {
                console.log("Qt.Key_Minus")
                //se attivo il relay rilancio tasti al monitor remoto
                //in questo caso la navigazione da tastiera è bloccata
   //si esce da questa condizione con tasto ESC lungo o con il mouse
                if(data_model.keyRelayOn) {
                    data_model.doSendKey(253, data_model.remoteSerialAdr) //0xFD
                }
                //se OSD è chiuso gestisco il brightess (locale e/o remoto)
                else if(osdVisible === false)
                {
                    console.log("osdVisible false")
					if (!dimmingPot)
					{
						//se il monitor è il MultiDim master invio tasto + in broadcast
						if(data_model.mdimMaster)
							data_model.doBrightBroadDOWN()
						else
							data_model.doBrightDOWN()
					}
                }
                //OSD aperto
                else {
                    console.log("OSD aperto")
                    //se sono dentro un tab gli rilancio il tasto +
                    if(tabEditActive) {
                        key_minus()
                        return
                    }
                    //navigo i tab
                    console.log("navigo i tab")
                    if(currTab)
                        currTab--
                    else
                        currTab = Glob.NUM_OSD_TABS - 1

                    currOsdTab = tabItems[currTab]

                    resOsdTout()
                }
            }

    //---------------------
    // OSD Background
    //---------------------
    Rectangle {
        width: 481; height: 340
        anchors.centerIn: parent
        color: color_Background
        visible: osdVisible
        focus: true

        Keys.onReleased: {
            if(event.key === Qt.Key_Escape) {
                console.log("ESC released");
                escPressMeasure = false
                escLongPressTimer.stop()
            }
        }

        //-------------------------------------------
        // Navigazione tramite tastiera
        //-------------------------------------------
        Keys.onPressed: {
//            console.log('pressed:', event.key, "count:", event.count);

            if(event.key === Qt.Key_Plus)
                handle_Key_Plus()
            else if(event.key === Qt.Key_Minus)
                handle_Key_Minus()
            //else if(event.key === Qt.Key_Return)
            //LOLLO per test Windows
            else if(event.key === Qt.Key_Enter)
                handle_Key_Menu()
            else if(event.key === Qt.Key_Escape)
                handle_Key_Escape()

            //prevents the key event from being propagated to the item's parent.
            event.accepted = true;
        }

        //dummy area that intercepts clicks inside the OSD window
        //preventing the OSD from disappearing (only clicks on outside area do that)
        MouseArea {
            id: dummyMouseArea
            anchors.fill: parent
            onClicked: console.log("OSD Background onClicked")
            //test lollo
            //cursorShape: Qt.BlankCursor
        }

        //-------------------------------------------------------------
        // Carica il TAB corrente
        //-------------------------------------------------------------
        Loader {
            id: tabsLoader
            anchors.fill: parent
            z: 3; //sopra la schermata corrente

            source: ""
            state: currOsdTab

            onStateChanged: {
                //se ero in attesa di pacchetti validi per lanciare il pupup
                //in Tab load&save e cambio tab, resetto il flag
                if(state !== "tab_load_save")
                    data_model.invalidateWaitSet()
            }

            states: [
                State {
                    name: "tab_settings"
                    PropertyChanges { target: tabsLoader; source: "TabSettings.qml"; }
                },
                State {
                    name: "tab_inputs"
                    PropertyChanges { target: tabsLoader; source: "TabInputs.qml"; }
                },
                State {
                    name: "tab_timeouts"
                    PropertyChanges { target: tabsLoader; source: "TabTimeouts.qml"; }
                },
                State {
                    name: "tab_load_save"
                    PropertyChanges { target: tabsLoader; source: "TabLoadSave.qml"; }
                },
                State {
                    name: "tab_status"
                    PropertyChanges { target: tabsLoader; source: "TabStatus.qml"; }
                },
                State {
                    name: "tab_comm"
                    PropertyChanges { target: tabsLoader; source: "TabComm.qml"; }
                }
            ]
        }

        //---------------------
        // Logo
        //---------------------
        Image {
            id: imgDP
            x: 4; y: 12; source: _IMG_PATH + "logo.png"
            //[debug feature]
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log("cliccato logo")
                }
            }
        }

		//------------------------------------
		// Versione SW OSD [PROVVISORIO]
		//------------------------------------
		//Text {
		//	id: swVer
		//	x: 4; y: 65;
		//	color: "red"
		//	font { family: "Myriad Pro"; pixelSize: 12 }
		//	text: "OSD  v"+ OSD_SW_REV_MAJ + "." + OSD_SW_REV_MIN
		//}

		//Text {
		//	id: swLPCVer
		//	x: 4; y: 81;
		//	color: "black"
		//	font { family: "Myriad Pro"; pixelSize: 12 }
		//	text: "LPC  v" + "1" + "." + "0"
		//}

		//Text {
		//	id: swFPGAVer
		//	x: 4; y: 97;
		//	color: "black"
		//	font { family: "Myriad Pro"; pixelSize: 12 }
		//	text: "FPGA v" + "2" + "." + "7"
		//}
		
        //-----------------------------------------------------
        // Footer bar
        //-----------------------------------------------------
        Rectangle {
            id: footer
            x: 0; y: 280+20; width: 481; height: 320-280
            color: color_Dark
            property int tabWidth: width / 6
            property var tabLabels: [qsTr("Settings"), qsTr("Inputs"), qsTr("Timeouts"),
                                     qsTr("Load & save"), qsTr("Status"), qsTr("Comm")]

            Repeater {
                id: tabGroupb
                model: 6

                //----------------------------
                //Tab item (page not selected)
                Rectangle {
                    x: index * footer.tabWidth; anchors.verticalCenter: parent.verticalCenter
                    width: footer.tabWidth; height: 320-280
                    color: "transparent"

                    //Separator
                    Rectangle {
                        visible: index < 5 && (index !== currTab - 1) //skip last one (near window's border)
                        x: 80/*95*/; anchors.verticalCenter: parent.verticalCenter
                        width: 1; height: 25
                        color: color_Background
                    }
                    MouseArea {
                        anchors.fill: parent
                        enabled: data_model.keyRelayOn === false
                        onClicked: {
                            currTab = index
                            currOsdTab = tabItems[index]
                            resOsdTout()
                        }
                    }

                    //----------------------------
                    //Tab big (page opened)
                    Rectangle {
                        visible: index === currTab
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: 4
                        width: 81/*97*/; height: 50
                        color: color_Background
                    }
                    Text {
                        id: tabTxt
                        anchors.centerIn: parent
                        anchors.verticalCenterOffset: index === currTab ? 8 : 0
                        color: index === currTab ? color_HeavyDark : "white"
                        font { family: "Myriad Pro"; pixelSize: 14 }
                        text: footer.tabLabels[index]
                    }
                }
            }
        }
    }

    //per simulare la tastiera se non c'è il driver (es. uno dei monitor Lorenzo)
    //NOTA: non rileva l'evento di rilascio, quindi il tasto ESC non funziona
    //completamente
//    Debug_VirtKeyboard {
//        x: 370; y: 570
//    }
}

/* EOF */





































