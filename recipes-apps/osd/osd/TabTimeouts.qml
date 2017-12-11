/*****************************************************************************
*   Fie:		TabTimeouts.qml
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
import QtQuick 2.0
import "global.js" as Glob

Item {
    id: page
    width: 480; height: 320
    property int currOpenedCombo: 0
    property int splashToutIdx: data_model.splashLogoToutIdx
    property int nosToutIdx:    data_model.noSigToutIdx
    property int stslToutIdx:   data_model.stdbyToutIdx

    Component.onCompleted: console.log("-- TabTimeouts")

    //intercetta sia i cambiamenti da touch che da seriale
    onSplashToutIdxChanged: {
        combo1.currRow = splashToutIdx
        combo1.currVal = combo1.listItems[splashToutIdx]
        console.log("onSplashToutIdxChanged:", splashToutIdx,
                    "currVal:", combo1.currVal)
    }

    //intercetta sia i cambiamenti da touch che da seriale
    onNosToutIdxChanged: {
        combo2.currRow = nosToutIdx
        combo2.currVal = combo2.listItems[nosToutIdx]
    }

    //intercetta sia i cambiamenti da touch che da seriale
    onStslToutIdxChanged:  {
        combo3.currRow = stslToutIdx
        combo3.currVal = combo3.listItems[stslToutIdx]
    }

    //---------------------------------------------------------------------------
    // Tastiera
    //---------------------------------------------------------------------------
    property int currElement: -1

    //------ Tasto MENU ------
    //seleziona a rotazione il controllo da editare
    Connections {
        target: main
        onKey_enter: {
            main.tabEditActive = true
            if(++currElement >= 3)
                currElement = 0
        }
    }
    Connections {
        target: main

        //------ Tasto PIU ------
        onKey_plus: {
            if(currElement === 0) {
                if(data_model.splashLogoToutIdx < 9)
                    data_model.splashLogoToutIdx++
                data_model.updateSetting(Glob.osdLOGO_TIMEOUT, data_model.splashLogoToutIdx)
            }
            else if(currElement === 1) {
                if(data_model.noSigToutIdx < 5)
                    data_model.noSigToutIdx++
                data_model.updateSetting(Glob.osdNOSIG_TOUT, data_model.noSigToutIdx)
            }
            else {
                if(data_model.stdbyToutIdx < 5)
                    data_model.stdbyToutIdx++
                data_model.updateSetting(Glob.osdSTDBY_TOUT, data_model.stdbyToutIdx)
            }
            resOsdTout()
        }
    }

    //------ Tasto MENO ------
    Connections {
        target: main
        onKey_minus: {
            if(currElement === 0) {
                if(data_model.splashLogoToutIdx)
                    data_model.splashLogoToutIdx--
                data_model.updateSetting(Glob.osdLOGO_TIMEOUT, data_model.splashLogoToutIdx)
            }
            else if(currElement === 1) {
                if(data_model.noSigToutIdx)
                    data_model.noSigToutIdx--
                data_model.updateSetting(Glob.osdNOSIG_TOUT, data_model.noSigToutIdx)
            }
            else {
                if(data_model.stdbyToutIdx)
                    data_model.stdbyToutIdx--
                data_model.updateSetting(Glob.osdSTDBY_TOUT, data_model.stdbyToutIdx)
            }
            resOsdTout()
        }
    }

    //------ Tasto ESC fine edit ------
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

    Column {
        x: 90; y: 25; spacing: 1

        //------------------------------
        // Row 1: "Splash logo timeout"
        //------------------------------
        Item {
            id: itm1
            width: 390; height: 40
            Rectangle {
                id: backg1
                anchors.fill: parent
                visible: currElement === 0
                color: color_HighlightBackgd
            }
            //"Splash logo timeout" label
            Text {
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left; leftMargin: 5
                }
                color: color_HeavyDark
                font { family: "Myriad Pro"; weight: Font.Normal; pixelSize: 14 }
                text: qsTr("<b>Splash logo timeout</b> (sec)")
            }
            MouseArea {
                anchors.fill: backg1; hoverEnabled: main.tabEditActive === false
                onEntered: currElement = 0  // backg1.visible = true
                onExited:  currElement = -1  // backg1.visible = false
                onClicked: {
                    console.log("currOpenedCombo:", currOpenedCombo)
                    resOsdTout()
                }
            }
            OsdComboBox {
                id: combo1
//LOLLO
                listEnabled: true //data_model.logo_present
                enabled: currOpenedCombo === 0 || (currOpenedCombo !== 2 && currOpenedCombo !== 3)
                anchors {
                    verticalCenter: backg1.verticalCenter
                    left: backg1.left; leftMargin: 270
                }
                listItems: [1,2,3,4,5,10,30,60, qsTr("No logo"), qsTr("Persistent")]
                currRow: data_model.splashLogoToutIdx

                onListOpenEvent: {
                    //set to front
                    if(opened) {
                        itm1.z = 10;
                        currOpenedCombo = 1
                    }
                    //bring back to background
                    else {
                        itm1.z = 1;
                        currOpenedCombo = 0
                    }
                }
                onListItemChanged: {
                    data_model.splashLogoToutIdx = num
                    data_model.updateSetting(Glob.osdLOGO_TIMEOUT, num)
                }
            }
        }

        //----------------------------------
        // Row 2: "No signal to stand-by"
        //----------------------------------
        Item {
            id: itm2
            width: 390; height: 40
            Rectangle {
                id: backg2
                anchors.fill: parent
                visible: currElement === 1
                color: color_HighlightBackgd
            }
            //"No signal to stand-by" label
            Text {
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left; leftMargin: 5
                }
                color: color_HeavyDark
                font { family: "Myriad Pro"; weight: Font.Normal; pixelSize: 14 }
                text: qsTr("<b>No signal to stand-by</b> (sec)")
            }
            MouseArea {
                anchors.fill: backg2; hoverEnabled: main.tabEditActive === false
                onEntered: { if(combo1.listOpened == false ) currElement = 1 /*backg2.visible = true*/ }
                onExited: currElement = -1 //backg2.visible = false
                onClicked: {
                    console.log("currOpenedCombo:", currOpenedCombo)
                    resOsdTout()
                }
            }

            OsdComboBox {
                id: combo2
                enabled: currOpenedCombo === 0 || (currOpenedCombo !== 1 && currOpenedCombo !== 3)
                anchors {
                    verticalCenter: backg2.verticalCenter
                    left: backg2.left; leftMargin: 270
                }
                listItems: [3, 5, 10, 30, 60, qsTr("Stay on")]

                currRow: data_model.noSigToutIdx

                onListOpenEvent: {
                    if(opened) {
                        itm2.z = 10;
                        currOpenedCombo = 2
                    }
                    else {
                        itm2.z = 1;
                        currOpenedCombo = 0
                    }
                }
                onListItemChanged: {
                    //indice elemento della combo
                    data_model.noSigToutIdx = num

                    //valore in secondi (-1 = "Stay on")
                    var parVal = num < 5 ? listItems[num] : -1
                    console.log("noSigTout:", parVal)

                    //invio comando alla seriale
                    data_model.updateSetting(Glob.osdNOSIG_TOUT, data_model.noSigToutIdx)
                }
            }
        }

        //----------------------------------
        // Row 3: "Stand-by to sleep mode"
        //----------------------------------
        Item {
            id: itm3
            width: 390; height: 40
            Rectangle {
                id: backg3
                anchors.fill: parent
                visible: currElement === 2
                color: color_HighlightBackgd
            }
            //"Stand-by to sleep mode" label
            Text {
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left; leftMargin: 5
                }
                color: color_HeavyDark
                font { family: "Myriad Pro"; weight: Font.Normal; pixelSize: 14 }
                text: qsTr("<b>Stand-by to sleep mode</b> (min)")
            }
            MouseArea {
                anchors.fill: backg3; hoverEnabled: main.tabEditActive === false
                onEntered: { if(combo1.listOpened == false) currElement = 2  /*backg3.visible = true*/ }
                onExited: currElement = -1 //backg3.visible = false
                onClicked: {
                    console.log("currOpenedCombo:", currOpenedCombo)
                    resOsdTout()
                }
            }
            OsdComboBox {
                id: combo3
                enabled: currOpenedCombo === 0 || (currOpenedCombo !== 1 && currOpenedCombo !== 2)
                anchors {
                    verticalCenter: backg3.verticalCenter
                    left: backg3.left; leftMargin: 270
                }
                listItems: [3, 5, 10, 30, 60, qsTr("Do not sleep")]

                currRow: data_model.stdbyToutIdx

                onListOpenEvent: {
                    if(opened) {
                        itm3.z = 10;
                        currOpenedCombo = 3
                    }
                    else {
                        itm3.z = 1;
                        currOpenedCombo = 0
                    }
                }
                onListItemChanged: {
                    //indice elemento della combo
                    data_model.stdbyToutIdx = num

                    //valore in secondi (-1 = "Do not sleep")
                    var parVal = num < 5 ? listItems[num] : -1
                    console.log("stdbyTout:", parVal)

                    //invio comando alla seriale
                    data_model.updateSetting(Glob.osdSTDBY_TOUT, data_model.stdbyToutIdx)
                }
            }
        }
    }
}









































