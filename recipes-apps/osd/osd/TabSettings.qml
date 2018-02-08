/*****************************************************************************
*   Fie:		TabSettings.qml
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
import QtQuick 2.0
import "global.js" as Glob

Item {
    id: page
    width: 480; height: 320
    Component.onCompleted: console.log("-- TabSettings")

    property bool dimPot: data_model.dimming_pot_ena //se true disabilita il controllo (lo rende grigio)

    //---------------------------------------------------------------------------
    // Tastiera
    //---------------------------------------------------------------------------
    property int currElement: -1

    //----- Tasto MENU -----
    //seleziona a rotazione il controllo da editare
    Connections {
        target: main
        onKey_enter: {
            main.tabEditActive = true
            if(popPIN.visible === false) {
                if(currElement < 6)
                    currElement++
                else
                    currElement = 0

                if(dimPot && currElement === 0)
                    currElement = 1;

                //console.log("--> TabSettings currElement", currElement)
            }
        }
    }

    //----- Tasto PIU -----
    Connections {
        target: main
        onKey_plus: {
            switch(currElement) {
            case 0:
                if(data_model.dimLevel < 100) {
                    data_model.dimLevel++
                    data_model.updateSetting(Glob.osdBRIGHTNESS, data_model.dimLevel)
                }
                break;
				
			case 1:
				data_model.dimming_pot_ena = true
				data_model.updateSetting(Glob.osdPOT_ENA, true)
				break;

            case 2:
                if(data_model.contrastLevel < 100) {
                    data_model.contrastLevel++
                    data_model.updateSetting(Glob.osdCONTRAST, data_model.contrastLevel)
                }
                break;

            case 3:
                if(data_model.buzzerVol < 100) {
                    data_model.buzzerVol++
                    data_model.updateSetting(Glob.osdBUZZER, data_model.buzzerVol)
                }
                break;

            case 4:
                if(data_model.scalingMode < 2) {
                    data_model.scalingMode++
					data_model.updateSetting(Glob.osdSCALING_MODE, data_model.scalingMode)
                }
                break;

            case 5:
                if(data_model.inputMode < 1) {
                    data_model.inputMode++
					data_model.updateSourceSettings()
                }
                break;

            case 6:
                if(data_model.ddc_ci_Mode < 2) {
                    data_model.ddc_ci_Mode++
					data_model.updateSourceSettings()
                }
                break;
            }
            resOsdTout()
        }
    }

    //----- Tasto MENO -----
    Connections {
        target: main
        onKey_minus: {
            switch(currElement) {
            case 0:
                if(data_model.dimLevel) {
                    data_model.dimLevel--
                    data_model.updateSetting(Glob.osdBRIGHTNESS, data_model.dimLevel)
                }
                break;

			case 1:
				data_model.dimming_pot_ena = false
				data_model.updateSetting(Glob.osdPOT_ENA, false)
				break;
				
            case 2:
                if(data_model.contrastLevel) {
                    data_model.contrastLevel--
                    data_model.updateSetting(Glob.osdCONTRAST, data_model.contrastLevel)
                }
                break;

            case 3:
                if(data_model.buzzerVol) {
                    data_model.buzzerVol--
                    data_model.updateSetting(Glob.osdBUZZER, data_model.buzzerVol)
                }
                break;

            case 4:
                if(data_model.scalingMode) {
                    data_model.scalingMode--
					data_model.updateSetting(Glob.osdSCALING_MODE, data_model.scalingMode)
                }
                break;

            case 5:
                if(data_model.inputMode) {
                    data_model.inputMode--
					data_model.updateSourceSettings()
                }
                break;

            case 6:
                if(data_model.ddc_ci_Mode) {
                    data_model.ddc_ci_Mode--
					data_model.updateSourceSettings()
                }
                break;

            }
            resOsdTout()
        }
    }

    //----- Tasto ESC -----
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
        x: 90; y: 14; spacing: 1

        //--------------- Row 1: "Dimming" ----------------
        Item {
            width: 390; height: 40

            Rectangle {
                id: backg1
                anchors.fill: parent
                visible: currElement === 0
                color: color_HighlightBackgd
            }
            //"Dimming" label
            Text {
                opacity: dimPot ? 0.5 : 1.0
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left; leftMargin: 5
                }
                color: color_HeavyDark
                font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 14 }
                text: qsTr("Dimming")
            }
            MouseArea {
                anchors.fill: backg1
                hoverEnabled: main.tabEditActive === false
                onEntered: { if(!dimPot) currElement = 0 } // backg1.visible = true
                onExited:  { if(!dimPot)currElement = -1 } // backg1.visible = false
                onClicked: resOsdTout()
            }
            OsdSlider {
                id: slider1
                sliderEnabled: !dimPot
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left; leftMargin: 100
                }
                //valore nominale 0..100% per posizione slider
                //nota: la cifra numerica è invece ricavata dalla colonna "osd"
                //del file dimming.txt (oppure default valori lineari 0..100)
                percenVal: data_model.dimLevel
                percenDigit: data_model.osd_dimming_table[data_model.dimLevel]

                //Component.onCompleted: percenVal = data_model.dimLevel

                onSliderValChanged:  {
                    if(!dimPot) {
                    data_model.dimLevel = percVal
                    data_model.updateSetting(Glob.osdBRIGHTNESS, percVal)
                    resOsdTout()
					}
				}
			}			
        }
		
        //--------------- Row 2: "Potentiometer enable" ----------------
		Item {
			width: 390; height: 40 
            Rectangle {
                id: backg_pot
                anchors.fill: parent
                visible:  currElement === 1
                color: color_HighlightBackgd
            }
            //"Potentiometer enable" label
            Text {
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left; leftMargin: 5
                }
                color: color_HeavyDark
                font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 14 }
                text: qsTr("Potentiometer enable")
            }
            MouseArea {
                anchors.fill: backg_pot
                hoverEnabled: main.tabEditActive === false
                onEntered: currElement = 1  //backg2.visible = true
                onExited:  currElement = -1  //backg2.visible = false
                onClicked: resOsdTout()
            }
			OsdCheckBox {
				id: pot_chkbox
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left; leftMargin: 160
                }
				value: data_model.dimming_pot_ena
				onBoxClicked: {
					data_model.dimming_pot_ena = !data_model.dimming_pot_ena
					data_model.updateSetting(Glob.osdPOT_ENA, data_model.dimming_pot_ena)
					resOsdTout()
				}
			}
			
		}

        //--------------- Row 3: "Contrast" ----------------
        Item {
            width: 390; height: 40
            Rectangle {
                id: backg2
                anchors.fill: parent
                visible:  currElement === 2
                color: color_HighlightBackgd
            }
            //"Contrast" label
            Text {
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left; leftMargin: 5
                }
                color: color_HeavyDark
                font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 14 }
                text: qsTr("Contrast")
            }
            MouseArea {
                anchors.fill: backg2
                hoverEnabled: main.tabEditActive === false
                onEntered: currElement = 2  //backg2.visible = true
                onExited:  currElement = -1  //backg2.visible = false
                onClicked: resOsdTout()
            }
            OsdSlider {
                id: slider2
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left; leftMargin: 100
                }
                percenVal: data_model.contrastLevel
                onSliderValChanged: {
                    data_model.contrastLevel = percVal
                    data_model.updateSetting(Glob.osdCONTRAST, percVal)
                    resOsdTout()
                }
            }
        }

        //--------------- Row 4: "Buzzer vol." ----------------
        Item {
            width: 390; height: 40
            Rectangle {
                id: backg3
                anchors.fill: parent
                visible:  currElement === 3
                color: color_HighlightBackgd
            }
            //"Buzzer vol." label
            Text {
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left; leftMargin: 5
                }
                color: color_HeavyDark
                font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 14 }
                text: qsTr("Buzzer vol.")
            }
            MouseArea {
                anchors.fill: backg3
                hoverEnabled: main.tabEditActive === false
                onEntered: currElement = 3  //backg3.visible = true
                onExited:  currElement = -1  //backg3.visible = false
                onClicked: resOsdTout()
            }
            OsdSlider {
                id: slider3
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left; leftMargin: 100
                }                
                percenVal: data_model.buzzerVol
                onSliderValChanged: {
                    data_model.buzzerVol = percVal
                    data_model.updateSetting(Glob.osdBUZZER, percVal)
                    resOsdTout()
                }
            }
        }

        //------- Spacer -------
        //Item { width: 390; height: 12 }

        //--------------- Row 5: "Scaling" ----------------
        Item {
            width: 390; height: 40

            //separation line
            //note: y: -1 avoids overlap with highlight area
            Rectangle { y: -1; width: 390; height: 1; color: color_HeavyDark }

            //highlight background
            Rectangle {
                id: backg4
                anchors.fill: parent
                visible: currElement === 4
                color: color_HighlightBackgd
            }
            //"Scaling" label
            Text {
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left; leftMargin: 5
                }
                color: color_HeavyDark
                font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 14 }
                text: qsTr("Scaling")
            }
            MouseArea {
                anchors.fill: backg4
                hoverEnabled: main.tabEditActive === false
                onEntered: currElement = 4  // backg4.visible = true
                onExited:  currElement = -1 // backg4.visible = false
                onClicked: resOsdTout()
            }
            //debug provvisorio: creare un Item per dimensionamento e posizionamento
            //parametrizzati non con coordinate fisse
            //inoltre la mouse area deve coprire anche la label del radio button
            OsdRadioButtons {
//                anchors {
//                    verticalCenter: parent.verticalCenter
//                    //left: parent.left; leftMargin: 90
//                }
                xPos: 100; yPos: 10 //debug provvisorio
                radioLabels: ["One-to-one", "Fit-all", "Fill-aspect"]
                currItem: data_model.scalingMode
                onCurr_ItemChanged: {
                    data_model.scalingMode = val
                    data_model.updateSetting(Glob.osdSCALING_MODE, val)
                }
            }
        }

        //--------------- Row 6: "Input mode" ----------------
        Item {
            width: 390; height: 40

            //separation line
            Rectangle { y: -1; width: 390; height: 1; color: color_HeavyDark }

            Rectangle {
                id: backg5
                anchors.fill: parent
                visible: currElement === 5
                color: color_HighlightBackgd
            }
            //"Input mode" label
            Text {
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left; leftMargin: 5
                }
                color: color_HeavyDark
                font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 14 }
                text: qsTr("Input mode")
            }
            MouseArea {
                anchors.fill: backg5
                hoverEnabled: main.tabEditActive === false
                onEntered: currElement = 5  // backg5.visible = true
                onExited:  currElement = -1 // backg5.visible = false
                onClicked: resOsdTout()
            }
            OsdRadioButtons {
                xPos: 100; yPos: 10; model: 2
                radioLabels: ["Manual", "Auto"]
                currItem: data_model.inputMode
                onCurr_ItemChanged: {
                    data_model.inputMode = val
                    console.log("Input mode val:", val)
                    //invio comando alla seriale
                    //parametro unico che aggiorna tutti i settaggi degli inputs
                    data_model.updateSourceSettings()
                }
            }
        }

        //--------------- Row 7: "DDC/CI" mode ----------------
        Item {
            width: 390; height: 40

            //separation line
            Rectangle { y: -1; width: 390; height: 1; color: color_HeavyDark }

            Rectangle {
                id: backg6
                anchors.fill: parent
                visible: currElement === 6
                color: color_HighlightBackgd
            }
            //"DDC/CI" label
            Text {
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left; leftMargin: 5
                }
                color: color_HeavyDark
                font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 14 }
                text: qsTr("DDC/CI")
            }
            MouseArea {
                anchors.fill: backg6
                hoverEnabled: main.tabEditActive === false
                onEntered: currElement = 6 //backg6.visible = true
                onExited:  currElement = -1//backg6.visible = false
                onClicked: resOsdTout()
            }
            OsdRadioButtons {
                xPos: 100; yPos: 10
                radioLabels: ["Manual", "Auto", "Off"]
                currItem: data_model.ddc_ci_Mode
                onCurr_ItemChanged: {
                    data_model.ddc_ci_Mode = val
                    //invio comando alla seriale
                    //parametro unico che aggiorna tutti i settaggi degli inputs
                    data_model.updateSourceSettings()
                }
            }
        }
        //Item {
        //    width: 390; height: 40
        //
        //    //separation line
        //    Rectangle { y: -1; width: 390; height: 1; color: color_HeavyDark }
        //}
    }

    //------------------------------------
    // Popup richiesta PIN
    //------------------------------------
    Connections {
        target: main
        onShow_popup_pin: popPIN.visible = true
    }

    //Nota: posizionata qui per essere in primo piano
    //perché TabSettings è il primo tab che appare
    PopupPIN {
        id: popPIN
        visible: false
        anchors.centerIn: parent
        isEdit: false
    }
}


/* EOF */















































