/*****************************************************************************
*   Fie:		TabInputs.qml
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
import QtQuick 2.0
import "global.js" as Glob

Item {
    id: page
    width: 480; height: 320
    property int rowHeight: 30

    property int currEditRow:  0
    property int currEditItem: 0

    Component.onCompleted: {
        updateRows()
        console.log("-- TabInputs -- Num INPUTS:", N_INPUTS)
    }

    //---------------------------------------------------------------------------
    // Tastiera
    //---------------------------------------------------------------------------

    //---- Tasto MENU ----
    //seleziona a rotazione il controllo da editare
    Connections {
        target: main
        onKey_enter: {
            if(main.tabEditActive === false)
                main.tabEditActive = true
            else {
                if(currEditItem < 2) {
                    currEditItem++
                }
                else {
                    currEditItem = 0
                    if(++currEditRow >= N_INPUTS)
                        currEditRow = 0
                }
            }
            resOsdTout()
        }
    }

    //---- Tasto PLUS ----
    Connections {
        target: main
        onKey_plus: {
            resOsdTout()
        }
    }

    //---- Tasto MENO ----
    Connections {
        target: main
        onKey_minus: {
            resOsdTout()
        }
    }

    //---- Tasto ESC ----
    //fine edit
    Connections {
        target: main
        onKey_esc: {
            main.tabEditActive = false
        }
    }

    //---------------------------------------------------------------------------
    // Controlli
    //---------------------------------------------------------------------------

    //Arrivato nuovo pacchetto SOURCE_SET, aggiorno il rendering delle righe
    //le matrici sono già state aggiornate da DataModel::set_InputTable()
    Connections {
        target: sig_conn
        onNewSourceData: {
            //console.log("__onNewSourceData__")
            updateRows();
        }
    }

    //------------------------------------------
    // Render the page and send matrices to LPC
    function update()
    {
        updateRows()

        //invio comando alla seriale
        data_model.updateSourceSettings()
    }

    //-----------------------------------------
    //set active input's relative position in the rows list in the page
    function set_Current_IN(relativePos)
    {
        resOsdTout()
        console.log("set_Current_IN  relativePos:", relativePos)

        //sicurezza
        if(relativePos === -1) return

        //Switch allowed in manual mode only or from serial command
        if(data_model.inputMode === Glob.inModeMAN)
        {
            //preset all off
            for(var i=0; i<N_INPUTS; i++)
                data_model.setinput_States_at(i, 0)

            //only this one is active
            data_model.setinput_States_at(relativePos, 1);
            //convert into real IN index
            data_model.currentIN = data_model.input_Index[relativePos]

            update()
        }
    }

    //-----------------------------------------
    function set_Current_DDC_IN(relativePos)
    {
        resOsdTout()
        console.log("set_Current_DDC_IN  relativePos:", relativePos)

        //Switch allowed in manual mode only
        if(data_model.ddc_ci_Mode === Glob.ddciMAN)
        {
            //preset all off
            for(var i=0; i<N_INPUTS; i++)
                data_model.setddcCi_States_at(i, 0)

            //only this one is active
            data_model.setddcCi_States_at(relativePos, 1)
            data_model.currentDDC_IN = data_model.input_Index[relativePos]  //index to be sent to LPC

            update()
        }
    }

    //-----------------------------------------
    function updateRows()
    {
        console.log("-> updateRows  row.count:", row.count)

        for(var i=0; i<row.count; i++)
        {
                    //console.log("-> i"+i, data_model.input_Types[i],
                    //getNameFromIdx(data_model.input_Index[i]), "\t",
                    //data_model.input_Index[i],
                    //"\tINstate:", data_model.input_States[i],
                    //"ddc:", data_model.ddcCi_States[i])

            row.itemAt(i).input_Type = data_model.input_Types[i]                //icon
            row.itemAt(i).input_Name = getNameFromIdx(data_model.input_Index[i]) //string
            row.itemAt(i).isCurrInput = data_model.input_States[i] === 1         //active in
            row.itemAt(i).ddcCi_State = data_model.ddcCi_States[i] === 1         //active ddc in
        }

//        //only for analog Inputs (future feature)
//        row.itemAt(4).anaEDID_State = data_model.anaEDID_States[4] //temporary
//        row.itemAt(5).anaEDID_State = data_model.anaEDID_States[5]
    }

    //-----------------------------------------
    function doSwitchRows(from, to)
    {
        resOsdTout()
        console.log("-> doSwitchRows: ", from, to)

        var buff = data_model.input_Index[to]
        data_model.setinput_Index_at(to, data_model.input_Index[from])
        data_model.setinput_Index_at(from,buff)

        var idx = data_model.input_Index
        data_model.set_InputTable(idx[0], idx[1], idx[2], idx[3], idx[4], idx[5])
        update()
    }

    //-----------------------------------------
    function getNameFromIdx(idx)
    {
        switch(idx)
        {
            case 0: return "DVI-D1";
            case 1: return "DP 1";
            case 2: return "DVI-D2";
            case 3: return "DP 2";
            case 4: return "DVI-A1"; //future use
            case 5: return "DVI-A2"; //future use
            default: return "---";
        }
    }

    //---------------------------------
    // Rows headers
    //---------------------------------
    //"Priority" label
    Text {
        x: 95; y: 24
        color: "black"; font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 12 }
        text: "Priority"
    }
    //"Current" label
    Text {
        x: 235; y: 24
        color: "black"; font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 12 }
        text: "Current"
    }
    //"DDC/CI" label
    Text {
        x: 285; y: 24
        visible: data_model.ddc_ci_Mode !== Glob.ddciOFF
        color: "black"; font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 12 }
        text: "DDC/CI"
    }

//future use
//    //"Analog EDID" label
//    Text {
//        x: 340; y: 24
//        color: "black"; font { family: "Myriad Pro"; weight: Font.Bold; pixelSize: 12 }
//        text: "Analog EDID"
//    }

    //---------------------------------
    // Video inputs rows
    //---------------------------------
    //nota: se DVI-D1 e DP1 sono disabilitati, le relative righe sono non visibili e
    //automaticamente il repeater sposta le righe 3 e 4 al loro posto
    Column {
        x: 110; y: 45; spacing: 3

        Repeater {
            id: row
            model: N_INPUTS

            InputRowDelegate {
                //priorità effettiva (1..6). Se DVI-D1 e DP1 sono disabilitati
                //saranno scambiabili tra loro solo le righe centrali (prio 3-4)
                prioNumber: index + 1
                isEditing: currEditRow === index
                editItem: currEditItem
                //cifra priorità a sx della riga
                //se ingressi DVI-D1 e DP1 disabilitati nascondo i numeri priorità
                //1 e 2 delle prime due righe, i numeri 3 e 4 li visualizzo come 1 e 2
                //nota: attenzione che index ha range 0..5
                prioDigit: {
                    if(data_model.inputs1Disable)
                        return index > 1 ? index - 1 : 0 //2-3 -> 1-2
                    else
                        return index + 1 //range 1..6
                }

                //se ingressi DVI-D1 e DP1 disabilitati: righe in grigietto
                state: {
                    if(data_model.inputs1Disable)
                        return index > 1 ? "st_normal" : "st_disabled"
                    else
                        return "st_normal"
                }

                //se ingressi DVI-D1 e DP1 disabilitati nasconde alcune frecce
                arrowUpEnabled:  !(data_model.inputs1Disable && index === 2)
                arrowDwnEnabled: !(data_model.inputs1Disable && index === 3)

                onCurr_IN_clicked:      set_Current_IN(index)
                onDdc_CI_pos_clicked:   set_Current_DDC_IN(index)
                onSwitchRows:           doSwitchRows(from, to)
            }
        }
    }
}
























