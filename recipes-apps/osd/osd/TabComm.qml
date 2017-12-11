/*****************************************************************************
*   Fie:		TabComm.qml
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
import QtQuick 2.0
import "global.js" as Glob

Item {
    id: page
    width: 480; height: 320
    property bool pageLoaded: false
    property bool isTabSerial: true

    property var ipTest: data_model.ipAddr
    property bool ethEnabled: data_model.eth_enabled
    property bool serBroadcAdr: false
    property bool dhcpLeaseWaiting: false
    property int currAdr

    Component.onCompleted: {
        console.log("-- TabComm")
        pageLoaded = true

        //quando apro il tab se il DHCP è già attivo
        //aggiorna le celle con gli IP ottenuti (in background)
        //oppure mostra i trattini se il lease non è ancora ottenuto
        if(data_model.dhcp_on)
        {
            if(data_model.isDHCP_leased()) {
                console.log("udhcpc ha gia' ottenuto il lease")
                data_model.getDHCPresults()
            }
            else {
                console.log("udhcpc sta attendendo il lease")
                dhcpLeaseWaiting = true
                leasePollTimer.start()
            }
        }
    }

    //-----------------------------------
    function setSerBroadcast(val)
    {
        console.log("setSerBroadcast_")
        if(val) {
            currAdr = data_model.remoteSerialAdr
            data_model.remoteSerialAdr = 255
            spb.boxEnabled = false
        }
        else {
            data_model.remoteSerialAdr = currAdr
            spb.boxEnabled = true
        }
    }

    //---------------------------------------------------------------------------
    // Tastiera
    //---------------------------------------------------------------------------
    property int currElement: -1

    Connections {
        target: data_model

        onKeyRelayOnChanged: {
            if(data_model.keyRelayOn === false) {
                resOsdTout()
                currElement = -1
            }
        }
    }

    //-------------
    // Tasto MENU
    //-------------
    //Seleziona a rotazione il controllo da editare
    Connections {
        target: main

        onKey_enter: {
            if(popPIN.visible) return
            main.tabEditActive = true

            //se Ethernet disabilitata il tab Networking è nascosto
            var lastElement = ethEnabled ? 7 : 6

            if(currElement < (isTabSerial ? lastElement : 22))
            {
                currElement++

                if(!isTabSerial)
                {
                    //se ho DHCP impostato salta evidenziazione delle caselle IP
                    if(chkDHCP.value && currElement === 2)
                        currElement = 18

                    //se questo monitor è il server SCOM salta evidenz. caselle SCOM server IP
                    if(chkSCOMsrv.value && currElement === 18)
                        currElement = 22
                }
            }
            else {
                currElement = 0
        }
            console.log("currElement:",currElement)
        }
    }

    //------------
    // Tasto PIU
    //------------
    Connections {
        target: main

        onKey_plus: {

            if(popPIN.visible) return

            //Tab seriale
            if(isTabSerial) {
                switch(currElement)
                {
                case 0: if(data_model.monSerialAdr < 255) data_model.monSerialAdr++; break;
                case 1: if(data_model.j24config === 0) data_model.j24config = 1; break;
                    case 2:
                        data_model.lockOnSerial = true
                        data_model.updateSetting(Glob.osdLOCK_ON, true)
                        popPIN.visible = true
                        //evito che alla chiusura del popup con il tasto + su OK il tasto
                        //tasto + determini nuovanente il click su questo elemento: tolgo la selezione
                        currElement = -1
                        console.log("Tasto PIU -> popPIN.visible = true:")
                        break;

                case 3: data_model.mdimMaster = true; break;
                case 4: if(data_model.remoteSerialAdr < 255) data_model.remoteSerialAdr++; break;
                    case 5: serBroadcAdr = true; setSerBroadcast(true); break;
                case 6: keyRelayToggle(); break;
                    case 7: if(ethEnabled) isTabSerial = false; break; //passo al Tab Networking
                }
            }
            //Tab networking
            else {
                doSaveNetworkPar = true

                switch(currElement)
                {
                    case 0: data_model.dhcp_on = true; break;
                    case 1: data_model.scom_srv = true; break;

                    case 2:
                        if(data_model.ipAddr0 < 255) data_model.ipAddr0++; break;
                    case 3:
                        if(data_model.ipAddr1 < 255) data_model.ipAddr1++; break;
                    case 4:
                        if(data_model.ipAddr2 < 255) data_model.ipAddr2++; break;
                    case 5:
                        if(data_model.ipAddr3 < 255) data_model.ipAddr3++; break;

                    case 6:
                        if(data_model.netMask0 < 255) data_model.netMask0++; break;
                    case 7:
                        if(data_model.netMask1 < 255) data_model.netMask1++; break;
                    case 8:
                        if(data_model.netMask2 < 255) data_model.netMask2++; break;
                    case 9:
                        if(data_model.netMask3 < 255) data_model.netMask3++; break;

                    case 10:
                        if(data_model.defGW0 < 255) data_model.defGW0++; break;
                    case 11:
                        if(data_model.defGW1 < 255) data_model.defGW1++; break;
                    case 12:
                        if(data_model.defGW2 < 255) data_model.defGW2++; break;
                    case 13:
                        if(data_model.defGW3 < 255) data_model.defGW3++; break;

                    case 14:
                        if(data_model.dns0 < 255) data_model.dns0++; break;
                    case 15:
                        if(data_model.dns1 < 255) data_model.dns1++; break;
                    case 16:
                        if(data_model.dns2 < 255) data_model.dns2++; break;
                    case 17:
                        if(data_model.dns3 < 255) data_model.dns3++; break;

                    case 18:
                        if(data_model.scomSrvIP0 < 255) data_model.scomSrvIP0++; break;
                    case 19:
                        if(data_model.scomSrvIP1 < 255) data_model.scomSrvIP1++; break;
                    case 20:
                        if(data_model.scomSrvIP2 < 255) data_model.scomSrvIP2++; break;
                    case 21:
                        if(data_model.scomSrvIP3 < 255) data_model.scomSrvIP3++; break;

                    case 22: isTabSerial = true; break; //passo al Tab Networking
                }
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

            if(popPIN.visible) return

            //Tab seriale
            if(isTabSerial) {
                switch(currElement)
                {
                case 0: if(data_model.monSerialAdr) data_model.monSerialAdr--; break;
                case 1: if(data_model.j24config) data_model.j24config--; break;
                    case 2:
                        data_model.lockOnSerial = false
                        data_model.updateSetting(Glob.osdLOCK_ON, false)
                        break;

                case 3: data_model.mdimMaster = false; break;
                case 4: if(data_model.remoteSerialAdr) data_model.remoteSerialAdr--; break;
                    case 5: serBroadcAdr = false; setSerBroadcast(false); break;
                case 6: keyRelayToggle(); break;
            }
            }
            //Tab networking
            else {
                doSaveNetworkPar = true

                switch(currElement)
                {
                    case 0: data_model.dhcp_on = false; break;
                    case 1: data_model.scom_srv = false; break;

                    case 2:
                        if(data_model.ipAddr0) data_model.ipAddr0--; break;
                    case 3:
                        if(data_model.ipAddr1) data_model.ipAddr1--; break;
                    case 4:
                        if(data_model.ipAddr2) data_model.ipAddr2--; break;
                    case 5:
                        if(data_model.ipAddr3) data_model.ipAddr3--; break;

                    case 6:
                        if(data_model.netMask0) data_model.netMask0--; break;
                    case 7:data_model.dhcp_on
                        if(data_model.netMask1) data_model.netMask1--; break;
                    case 8:
                        if(data_model.netMask2) data_model.netMask2--; break;
                    case 9:
                        if(data_model.netMask3) data_model.netMask3--; break;

                    case 10:
                        if(data_model.defGW0) data_model.defGW0--; break;
                    case 11:
                        if(data_model.defGW1) data_model.defGW1--; break;
                    case 12:
                        if(data_model.defGW2) data_model.defGW2--; break;
                    case 13:
                        if(data_model.defGW3) data_model.defGW3--; break;

                    case 14:
                        if(data_model.dns0) data_model.dns0--; break;
                    case 15:
                        if(data_model.dns1) data_model.dns1--; break;
                    case 16:
                        if(data_model.dns2) data_model.dns2--; break;
                    case 17:
                        if(data_model.dns3) data_model.dns3--; break;

                    case 18:
                        if(data_model.scomSrvIP0) data_model.scomSrvIP0--; break;
                    case 19:
                        if(data_model.scomSrvIP1) data_model.scomSrvIP1--; break;
                    case 20:
                        if(data_model.scomSrvIP2) data_model.scomSrvIP2--; break;
                    case 21:
                        if(data_model.scomSrvIP3) data_model.scomSrvIP3--; break;

                    case 22: isTabSerial = true; break; //passo al Tab Networking
                }
            }
            resOsdTout()
        }
    }

    //---------------------
    // Tasto ESC fine edit
    //---------------------
    Connections {
        target: main
        onKey_esc: {
            //nota: se faccio ESC dal popup PIN automaticamente provoco questa routine
            //non posso fare if(popPIN.visible) return
            //perché con ESC visible diventa ovviamente false
            //quindi mi perdo l'evidenziazionie gialla ma non ha importanza in quanto
            //ho completato l'edit del PIN che era l'operazione principale
            currElement = -1
            main.tabEditActive = false

            doSaveNetworkPar = false
            data_model.saveNetwParams()
        }
    }

    onCurrElementChanged: {
        //console.log("-> onCurrElementChanged:", currElement)
    }

    //--------------------------------------------
    // "Relay keypresses to monitor at address"
    //--------------------------------------------
    function keyRelayToggle()
    {
        data_model.keyRelayOn = !data_model.keyRelayOn
    }

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // Controlli
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------

    //------------------------------------
    // TABS in alto selezione sottopagina
    //------------------------------------

    //Testo "Serial settings" (se Ethernet disabilitata)
    Text {
        visible: !ethEnabled
        x: 90; y: 24
        color: color_HeavyDark
        font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 18 }
        text: "Serial settings"
    }

    //"Serial settings"
    OsdTabBtn {
        id: btnTabSerial
        visible: ethEnabled
        x: 90; y: 8
        btnSelected: isTabSerial
        btnHighlighted: !isTabSerial && currElement === 22
        btnText: "Serial settings"
        onBtnClicked: {
            isTabSerial = true
            resOsdTout()
        }
    }

    //"Network settings"
    OsdTabBtn {
        id: btnTabNet
        visible: ethEnabled
        x: 276; y: 8
        btnSelected: isTabSerial === false
        btnHighlighted: isTabSerial && currElement === 7
        btnText: "Network settings"
        onBtnClicked:  {
            isTabSerial = false
            resOsdTout()
        }
    }

    //------------------------------------------------------------------
    //------------------------------------------------------------------
    // TAB Serial settings
    //------------------------------------------------------------------
    //------------------------------------------------------------------
    Item {
        id: serParams
        visible: isTabSerial
        anchors.fill: parent
        onVisibleChanged: currElement = -1

        //posizione e forma del rettangolo giallo di evidenziazione
        property var highl_xpos:  [85,   85,  85,  85, 0, 402, 0]
        property var highl_ypos:  [56,   95, 135, 177, 0, 234, 0]
        property var highl_width: [315, 315, 255, 255, 0,  55, 0]

    //Rettangolo evidenziazione (forma e posizione dinamici)
    Rectangle {
        id: evid
            visible: currElement >= 0 && currElement < 7
            x:      visible ? serParams.highl_xpos[currElement]  : 0
            y:      visible ? serParams.highl_ypos[currElement]  : 0
            width:  visible ? serParams.highl_width[currElement] : 0
        height: 36;
        color: color_HighlightBackgd
    }

    //---------------------------------
    // Serial address of this monitor
    //---------------------------------
    Text {
            x: 90; y: 70-5
        color: color_HeavyDark
        font { family: "Myriad Pro"; weight: Font.Normal; pixelSize: 14 }
        text: "Serial address of this monitor"
    }

    OsdSpinBox {
            x: 314; y: 58
        digitBoxWidth: 52
        currVal: data_model.monSerialAdr
        onSpinUp: {
            if(data_model.monSerialAdr < 255)
                data_model.monSerialAdr++
            data_model.updateSetting(Glob.osdMON_SER_ADR, data_model.monSerialAdr)
        }
        onSpinDown: {
            if(data_model.monSerialAdr)
                data_model.monSerialAdr--
            data_model.updateSetting(Glob.osdMON_SER_ADR, data_model.monSerialAdr)
        }
    }

    //---------------------
    // J24 configuration
    //---------------------
    Text {
            x: 90; y: 110-5
        color: color_HeavyDark;
        font { family: "Myriad Pro"; weight: Font.Normal; pixelSize: 14 }
        text: "J24 configuration"
    }

    //Serve per intercettare il cambiamento di valore e modificare il valore visualizzato
    //nella casella della drop box
    property int j24cfg: data_model.j24config

    //intercetta sia i cambiamenti da touch che da seriale
    onJ24cfgChanged:  {
        combo1.currRow = data_model.j24config
        combo1.currVal = combo1.listItems[j24cfg]
    }

    OsdComboBox {
        id: combo1
            x: 354; y: 100+26/2
        cellWidth: 80
        //listEnabled: data_model.logo_present
        //enabled: currOpenedCombo === 0 || (currOpenedCombo !== 2 && currOpenedCombo !== 3)

        listItems: ["RS485", "RS422"]
        currRow: data_model.j24config
        onListOpenEvent: {
            //set to front
            if(opened) {
                combo1.z = 10;
                //currOpenedCombo = 1
            }
            //bring back to background
            else {
                combo1.z = 1;
                //currOpenedCombo = 0
            }
        }
        onListItemChanged: {
            data_model.j24config = num
            data_model.updateSetting(Glob.osdJ24_CFG, num)
            console.log("j24config:", data_model.j24config)
        }
    }

    //---------------------
    // Lock on serial comm
    //---------------------
    Text {
            x: 90; y: 150-5
        color: color_HeavyDark
        font { family: "Myriad Pro"; weight: Font.Normal; pixelSize: 14 }
        text: "Lock on serial comm"
    }

    OsdCheckBox {
            x: 314; y: 150-5-2
        value: data_model.lockOnSerial

            onBoxClicked: {
                data_model.lockOnSerial = !data_model.lockOnSerial
                data_model.updateSetting(Glob.osdLOCK_ON, data_model.lockOnSerial)

                //appare il tastierino edit PIN
                if(data_model.lockOnSerial) {
                popPIN.visible = true
            }
        }
    }

    //--------------------------------------------
    // "This monitor is MultiDim master"
    //--------------------------------------------
    Text {
            x: 90; y: 190-5
        color: color_HeavyDark
        font { family: "Myriad Pro"; weight: Font.Normal; pixelSize: 14 }
        text: "This monitor is MultiDim master"
    }

    OsdCheckBox {
            x: 314; y: 190-5
        value: data_model.mdimMaster

            onBoxClicked: {
            //filtro il changed che scatta al caricamento della pagina
            if(pageLoaded) {
                    data_model.mdimMaster = !data_model.mdimMaster
                data_model.updateSetting(Glob.osdMULTI_DIM, value)
            }
        }
    }

    CustomBtn {
        id: btn
            x: 90; y: 232; width: 200
        btnColor: color_Dark
        btnTxtColor: data_model.keyRelayOn ? "#FFAAAA" : color_BtnTxtNormal
        txtPixsize: 16
        highlighted: currElement === 6
        btnTxt: data_model.keyRelayOn ?
                    qsTr("Stop relaying keypresses\n[or press ESC for 5 sec]") :
                    qsTr("Relay keypresses to monitor:")

        onBtnClicked: keyRelayToggle()
    }

    //--------------------
    // Indirizzo 0..254
    //--------------------
    OsdSpinBox {
        id: spb
            x: 330-15; y: 232+8
        digitBoxWidth: 42
        currVal: data_model.remoteSerialAdr
        highlighted: currElement === 4
        onSpinUp:   if(data_model.remoteSerialAdr < 255) data_model.remoteSerialAdr++
        onSpinDown: if(data_model.remoteSerialAdr)       data_model.remoteSerialAdr--
    }

    //-----------------------------------
    // Check forza indirizzo Broadcast
    //-----------------------------------
    Item {
        id: itm
            x: 395; y: 228
        width: 65; height: 42

        Text {
            anchors { centerIn: itm; verticalCenterOffset: -8 }
            color: color_HeavyDark
            font { family: "Myriad Pro"; weight: Font.Normal; pixelSize: 12 }
            text: qsTr("Broadcast")
        }

        OsdCheckBox {
            id: chkBroadc
                value: serBroadcAdr
            anchors { centerIn: itm; verticalCenterOffset: 10 }

                onBoxClicked: {
                    serBroadcAdr = !serBroadcAdr
                    setSerBroadcast(serBroadcAdr)
            }
        }
    }

    //--------------------------------------------------
    // Pop-up impostazione PIN a 4 cifre
    //--------------------------------------------------
    PopupPIN {
        id: popPIN
        z: 10
        anchors.centerIn: parent
        visible: false
        isEdit: true
    }
}

    //------------------------------------------------------------------
    //------------------------------------------------------------------
    // TAB Network settings
    //------------------------------------------------------------------
    //------------------------------------------------------------------
    Item {
        id: netParams
        visible: isTabSerial === false
        anchors.fill: parent
        onVisibleChanged: currElement = -1

        //posizione e forma del rettangolo giallo di evidenziazione
        property var highl_xpos:  [142, 311-60]
        property var highl_ypos:  [46,  46 ]
        property var highl_width: [90,  124+86]

        //Rettangolo evidenziazione (forma e posizione dinamici)
        Rectangle {
            id: evid2
            visible: currElement === 0 || currElement === 1
            x:      visible ? netParams.highl_xpos[currElement]  : 0
            y:      visible ? netParams.highl_ypos[currElement]  : 0
            width:  visible ? netParams.highl_width[currElement] : 0
            height: 36;
            color: color_HighlightBackgd
        }

        //-----------------------------------------
        // Icona assenza connessione di rete
        //-----------------------------------------
        Image {
            id: icoNoNetwork
            visible: false
            anchors {
                verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 8
            }
            source: _IMG_PATH + "ico_no_netw.png"

            //controllo connettività all'apertura ed ogni 2 secondi
            Timer {
                interval: 2000; triggeredOnStart: true; repeat: true; running: true
                onTriggered: {
                    icoNoNetwork.visible = !data_model.isNetworkUp()
                }
            }
        }

        //---------------------
        // DHCP
        //---------------------
        Text {
            x: 150; y: 56
            color: color_HeavyDark
            font { family: "Myriad Pro"; weight: Font.Normal; pixelSize: 14 }
            text: "DHCP"
        }

        OsdCheckBox {
            id: chkDHCP
            x: 198; y: 55
            value: data_model.dhcp_on

            onBoxClicked: {
                doSaveNetworkPar = true
                //tolgo evidenziazione se era proprio sulle caselle che devo disabilitare
                currElement = -1

                if(data_model.dhcp_on === false) {
                    console.log("Avvio DHCP")
                    data_model.dhcp_on = true
                    dhcpLeaseWaiting = true
                    data_model.startDHCPclient()
                    leasePollTimer.start()
                }
                else {
                    console.log("Stop DHCP")
                    data_model.dhcp_on = false
                    dhcpLeaseWaiting = false
                    leasePollTimer.stop()
                    data_model.stopDHCPclient()
                    //nota: lascio l'IP e le impostazioni correnti
                }
            }
        }

        //controllo lease ottenuto ogni 2 secondi
        Timer {
            id: leasePollTimer
            interval: 2000; triggeredOnStart: true; repeat: true
            onTriggered: {
                if(data_model.isDHCP_leased()) {
                    running = false //stoppo il polling
                    dhcpLeaseWaiting = false
                    console.log("DHCP lease ottenuto")
                    //recupero e visualizzo i nuovi IP
                    data_model.getDHCPresults()
                }
            }
        }

        //---------------------
        // SCOM server
        //---------------------
        Text {
            x: 318-60; y: 56
            color: color_HeavyDark
            font { family: "Myriad Pro"; weight: Font.Normal; pixelSize: 14 }
            text: "This monitor is SCOM server"
        }

        OsdCheckBox {
            id: chkSCOMsrv
            x: 431; y: 55
            value: data_model.scom_srv

            onBoxClicked: {
                data_model.scom_srv = !data_model.scom_srv
                doSaveNetworkPar = true
                //tolgo evidenziazione se era proprio sulle caselle che devo disabilitare
                currElement = -1
            }
        }

        //---------------------
        // IP address vari
        //---------------------
        Column {
            x: 90; y: 86
            spacing: 4

            //---------------------
            //"IP address"
            //---------------------
            OsdIpEditor {
                id: ipEdit0
                ipEnabled: !data_model.dhcp_on
                highlIdx: currElement - 2
                ipEditTxt: "IP address"

                //nota: -1 fa visualizzare i trattini "--" nella cella
                ip0: dhcpLeaseWaiting ? - 1: data_model.ipAddr0
                onIp0Up:  if(data_model.ipAddr0 < 255) data_model.ipAddr0++
                onIp0Dwn: if(data_model.ipAddr0)       data_model.ipAddr0--

                ip1: dhcpLeaseWaiting ? - 1: data_model.ipAddr1
                onIp1Up:  if(data_model.ipAddr1 < 255) data_model.ipAddr1++
                onIp1Dwn: if(data_model.ipAddr1)       data_model.ipAddr1--

                ip2: dhcpLeaseWaiting ? - 1: data_model.ipAddr2
                onIp2Up:  if(data_model.ipAddr2 < 255) data_model.ipAddr2++
                onIp2Dwn: if(data_model.ipAddr2)       data_model.ipAddr2--

                ip3: dhcpLeaseWaiting ? - 1: data_model.ipAddr3
                onIp3Up:  if(data_model.ipAddr3 < 255) data_model.ipAddr3++
                onIp3Dwn: if(data_model.ipAddr3)       data_model.ipAddr3--
            }

            //---------------------
            //"Subnet mask"
            //---------------------
            OsdIpEditor {
                id: ipEdit1
                ipEnabled: !data_model.dhcp_on
                highlIdx: currElement - 6
                ipEditTxt: "Subnet mask"

                ip0: dhcpLeaseWaiting ? - 1: data_model.netMask0
                onIp0Up:  if(data_model.netMask0 < 255) data_model.netMask0++
                onIp0Dwn: if(data_model.netMask0)       data_model.netMask0--

                ip1: dhcpLeaseWaiting ? - 1: data_model.netMask1
                onIp1Up:  if(data_model.netMask1 < 255) data_model.netMask1++
                onIp1Dwn: if(data_model.netMask1)       data_model.netMask1--

                ip2: dhcpLeaseWaiting ? - 1: data_model.netMask2
                onIp2Up:  if(data_model.netMask2 < 255) data_model.netMask2++
                onIp2Dwn: if(data_model.netMask2)       data_model.netMask2--

                ip3: dhcpLeaseWaiting ? - 1: data_model.netMask3
                onIp3Up:  if(data_model.netMask3 < 255) data_model.netMask3++
                onIp3Dwn: if(data_model.netMask3)       data_model.netMask3--
            }

            //---------------------
            //"Default gateway"
            //---------------------
            OsdIpEditor {
                id: ipEdit2
                ipEnabled: !data_model.dhcp_on
                highlIdx: currElement - 10
                ipEditTxt: "Default gateway"

                ip0: dhcpLeaseWaiting ? - 1: data_model.defGW0
                onIp0Up:  if(data_model.defGW0 < 255) data_model.defGW0++
                onIp0Dwn: if(data_model.defGW0)       data_model.defGW0--

                ip1: dhcpLeaseWaiting ? - 1: data_model.defGW1
                onIp1Up:  if(data_model.defGW1 < 255) data_model.defGW1++
                onIp1Dwn: if(data_model.defGW1)       data_model.defGW1--

                ip2: dhcpLeaseWaiting ? - 1: data_model.defGW2
                onIp2Up:  if(data_model.defGW2 < 255) data_model.defGW2++
                onIp2Dwn: if(data_model.defGW2)       data_model.defGW2--

                ip3: dhcpLeaseWaiting ? - 1: data_model.defGW3
                onIp3Up:  if(data_model.defGW3 < 255) data_model.defGW3++
                onIp3Dwn: if(data_model.defGW3)       data_model.defGW3--
            }

            //---------------------
            //"DNS"
            //---------------------
            OsdIpEditor {
                id: ipEdit3
                ipEnabled: !data_model.dhcp_on
                highlIdx: currElement - 14
                ipEditTxt: "DNS"

                ip0: dhcpLeaseWaiting ? - 1: data_model.dns0
                onIp0Up:  if(data_model.dns0 < 255) data_model.dns0++
                onIp0Dwn: if(data_model.dns0)       data_model.dns0--

                ip1: dhcpLeaseWaiting ? - 1: data_model.dns1
                onIp1Up:  if(data_model.dns1 < 255) data_model.dns1++
                onIp1Dwn: if(data_model.dns1)       data_model.dns1--

                ip2: dhcpLeaseWaiting ? - 1: data_model.dns2
                onIp2Up:  if(data_model.dns2 < 255) data_model.dns2++
                onIp2Dwn: if(data_model.dns2)       data_model.dns2--

                ip3: dhcpLeaseWaiting ? - 1: data_model.dns3
                onIp3Up:  if(data_model.dns3 < 255) data_model.dns3++
                onIp3Dwn: if(data_model.dns3)       data_model.dns3--
            }

            //---------------------
            //"SCOM server IP"
            //---------------------
            OsdIpEditor {
                id: ipEdit4
                ipEnabled: !data_model.scom_srv
                highlIdx: currElement - 18
                ipEditTxt: "SCOM server IP"

                ip0: data_model.scomSrvIP0
                onIp0Up:  if(data_model.scomSrvIP0 < 255) data_model.scomSrvIP0++
                onIp0Dwn: if(data_model.scomSrvIP0)       data_model.scomSrvIP0--

                ip1: data_model.scomSrvIP1
                onIp1Up:  if(data_model.scomSrvIP1 < 255) data_model.scomSrvIP1++
                onIp1Dwn: if(data_model.scomSrvIP1)       data_model.scomSrvIP1--

                ip2: data_model.scomSrvIP2
                onIp2Up:  if(data_model.scomSrvIP2 < 255) data_model.scomSrvIP2++
                onIp2Dwn: if(data_model.scomSrvIP2)       data_model.scomSrvIP2--

                ip3: data_model.scomSrvIP3
                onIp3Up:  if(data_model.scomSrvIP3 < 255) data_model.scomSrvIP3++
                onIp3Dwn: if(data_model.scomSrvIP3)       data_model.scomSrvIP3--
            }
        }
    }
}


























