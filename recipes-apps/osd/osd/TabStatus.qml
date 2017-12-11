/*****************************************************************************
*   Fie:		TabStatus.qml
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
import QtQuick 2.0
import "global.js" as Glob

Item {
    id: page
    width: 480; height: 320

    property int currT: 0

    Component.onCompleted: {
        console.log("-- TabStatus")
        console.log("data_model.monitorSerialN:", data_model.monitorSerialN)
        page.currT = data_model.getTemperature()
    }

    //Aggiorna la temperatura corrente
    Connections {
        target: sig_conn
        onUpdateTemperature: {
            page.currT = data_model.getTemperature()
        }
    }

    function formatTime(minutes) {
        var hours =    Math.floor(minutes / 60)
        var minUnits = Math.floor(minutes % 60)
        if (minUnits < 10) minUnits = "0" + minUnits
        return hours + "h:" + minUnits;
    }

    function resetCounters() {
        data_model.totalRunTime = 0
        //capture current temperature
        data_model.maxTemperature = page.currT
        data_model.minTemperature = page.currT
    }

    function btnResetAction() {
        resetCounters()
        resOsdTout()
        //invio comando alla seriale
        data_model.doResetCounters()
    }

    //---------------------------------------------------------------------------
    // Tastiera
    //---------------------------------------------------------------------------
    property int currElement: -1

    //--- Tasto MENU ---
    //seleziona a rotazione il controllo da editare
    Connections {
        target: main
        onKey_enter: {
            main.tabEditActive = true
            currElement = 0
        }
    }
    //--- Tasto PIU ---
    Connections {
        target: main
        onKey_plus: {
            btnResetAction()
            resOsdTout()
        }
    }

    //--- Tasto MENO ---
    Connections {
        target: main
        onKey_minus: {
            //nota: tasto non usato
            resOsdTout()
        }
    }

    //--- Tasto ESC ---
    //fine edit
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
        x: 130; y: 30
        spacing: 4

        //-----------------------------
        // Monitor serial
        //-----------------------------
        Item {
            width: 300; height: 20
            id: rw1
            //label
            Text {
                anchors { verticalCenter: rw1.verticalCenter; left: rw1.left }
                color: color_HeavyDark; font { family: "Myriad Pro"; pixelSize: 16 }
                text: qsTr("Monitor serial:")
            }
            //value
            Text {
                anchors {
                    verticalCenter: rw1.verticalCenter; left: rw1.left; leftMargin: 180
                }
                color: color_HeavyDark;
                font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 16 }
                text: data_model.monitorSerialN
            }
        }

        //-----------------------------
        // Software ID
        //-----------------------------
        Item {
            width: 300; height: 20
            id: rw2
            //label
            Text {
                anchors { verticalCenter: rw2.verticalCenter; left: rw2.left }
                color: color_HeavyDark; font { family: "Myriad Pro"; pixelSize: 16 }
                text: qsTr("Software ID:")
            }
            //value
            Text {
                anchors {
                    verticalCenter: rw2.verticalCenter; left: rw2.left; leftMargin: 180
                }
                color: color_HeavyDark;
                font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 16 }
                text: data_model.osd_softwareID //OSD_SW_REV //debug temporaneo
            }
        }

        //empty item - used for spacing
        Item { width: 300; height: 20 }

        //-----------------------------
        // Run time
        //-----------------------------
        Item {
            width: 300; height: 20
            id: rw3
            //label
            Text {
                anchors { verticalCenter: rw3.verticalCenter; left: rw3.left }
                color: color_HeavyDark; font { family: "Myriad Pro"; pixelSize: 16 }
                text: qsTr("Run time (BL ON time):")
            }
            //value
            Text {
                anchors {
                    verticalCenter: rw3.verticalCenter; left: rw3.left; leftMargin: 180
                }
                color: color_HeavyDark;
                font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 16 }
                text: formatTime(data_model.totalRunTime)
            }
        }

        //-----------------------------
        // Current temperature
        //-----------------------------
        Item {
            width: 300; height: 20
            id: rw4
            //label
            Text {
                anchors { verticalCenter: rw4.verticalCenter; left: rw4.left }
                color: color_HeavyDark; font { family: "Myriad Pro"; pixelSize: 16 }
                text: qsTr("Current temperature:")
            }
            //value
            Text {
                anchors {
                    verticalCenter: rw4.verticalCenter; left: rw4.left; leftMargin: 180
                }
                color: color_HeavyDark;
                font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 16 }
                text: page.currT + "°C"
            }
        }

        //-----------------------------
        // Max. temperature
        //-----------------------------
        Item {
            width: 300; height: 20
            id: rw5
            //label
            Text {
                anchors { verticalCenter: rw5.verticalCenter; left: rw5.left }
                color: color_HeavyDark; font { family: "Myriad Pro"; pixelSize: 16 }
                text: qsTr("Max. temperature:")
            }
            //value
            Text {
                anchors {
                    verticalCenter: rw5.verticalCenter; left: rw5.left; leftMargin: 180
                }
                color: color_HeavyDark;
                font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 16 }
                text: data_model.maxTemperature + "°C"
            }
        }

        //-----------------------------
        // Min. temperature
        //-----------------------------
        Item {
            width: 300; height: 20
            id: rw6
            //label
            Text {
                anchors { verticalCenter: rw6.verticalCenter; left: rw6.left }
                color: color_HeavyDark; font { family: "Myriad Pro"; pixelSize: 16 }
                text: qsTr("Min. temperature:")
            }
            //value
            Text {
                anchors {
                    verticalCenter: rw6.verticalCenter; left: rw6.left; leftMargin: 180
                }
                color: color_HeavyDark;
                font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 16 }
                text: data_model.minTemperature + "°C"
            }
        }

        //empty item - used for spacing
        Item { width: 300; height: 8 }

        //--------------------------
        // "Reset counters"
        //--------------------------
        CustomBtn {
            id: btnReset
            width: 230
            btnColor: color_Dark
            highlighted: currElement === 0
            btnTxt: qsTr("Reset counters")
            onBtnClicked: btnResetAction()
        }
    }
}










































