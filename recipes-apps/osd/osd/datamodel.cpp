/*****************************************************************************
*   Fie:		datamodel.cpp
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#include <fcntl.h>
#include <unistd.h>
#include "datamodel.h"
#include "global.h"

#define manufacturer_ID_default		"NIN"
#define monitor_type_default		""

DataModel::DataModel(QObject *parent) :
    QObject(parent)
{
    PRINT_FUNC_NAME

    loadNetwParams();

    //se impostato il DHCP, all'avvio lancio il client
    //opzione -b= Background if lease is not immediately obtained
    //cmq ottenuto il lease va lo stesso automaticamente in background
    //Ottenuto il lease, il client chiama lo script /etc/udhcpc.d/50default
    //che internamente intercetta i parametri (passati ad esso come variabili)
    //e li passa all'OSD tramite un file temporaneo
    //Il demone poi si occupa automaticamente di rinnovare il lease.
    if(m_dhcp_on) {
        qDebug("-- Avvio il client DHCP --");
        system("udhcpc -b &");
    }
    else {
        qDebug("Configurazione di rete: statica");
    }

    m_logo_present = false;
    m_doOpenOSD = false;
    sendOSD_READY = false;
    m_inputs1Disable = false;
    m_MonitorSerialN = "---";
    m_SoftwareID = "---";
    m_TotalRunTime = 0;
    m_MaxTemperature = 0;
    m_MinTemperature = 0;
    m_fm = 0;
    m_sp = 0;
    m_monSerialAdr = 0;
    m_remoteSerialAdr = 0;
    m_j24config = 0;
    m_lockOnSerial = false;
    m_lockPIN = 0;
    m_mdimMaster = false;
    m_keyRelayOn = false;
    m_eth_enabled = false;
	m_dimming_pot_ena = false;
	m_debug_console = false;
	m_saab_config = false;

    //nota: probabilmente non serve mettere a 1 perché all'avvio viene
    //impostato l'ingresso attivo dal micro tramite SOURCE_SET
    m_CurrentIN = 1;        //default DP1
    m_CurrentDDC_IN = 1;    //default DP1

    m_osd_dimming_table.clear();
    Curr_TemperatureC = 0;	

    //defaults all'avvio in attesa che LPC invii tutti i SET
    loadDefaultSettings();

    //quando sull'OSD si commuta lo stato della funzione di relay dei tasti
    //invio al micro il relativo comando
    connect(this, SIGNAL(keyRelayOnChanged(bool)), this, SLOT(doKeyRelay(bool)));
}

//--------------------------------------------------------------
//If no edid.txt is found on the SD card, all digital inputs
//shall be loaded with the following EDID data:
//--------------------------------------------------------------
const int DataModel::DEFAULT_EDID[] = {
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x39, 0xE9, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x01, 0x1B, 0x01, 0x04, 0x80, 0x29, 0x1A, 0x78, 0x82, 0xE5, 0xB5, 0xA3, 0x55, 0x49, 0x99, 0x27,
    0x13, 0x50, 0x54, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
    0x45, 0x00, 0x80, 0x68, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x57,
    0x41, 0x56, 0x45, 0x20, 0x45, 0x56, 0x4F, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE5,
};

const struct DataModel::s_dimm DataModel::DEFAULT_DIMMING_TABLE[101] = {
    {2000,    0, 255,   0,   0},
    {2000,   20, 252,   3,   1},
    {2000,   40, 250,   5,   2},
    {2000,   60, 247,   8,   3},
    {2000,   80, 245,  10,   4},
    {2000,  100, 242,  13,   5},
    {2000,  120, 240,  15,   6},
    {2000,  140, 237,  18,   7},
    {2000,  160, 235,  20,   8},
    {2000,  180, 232,  23,   9},
    {2000,  200, 229,  26,  10},
    {2000,  220, 227,  28,  11},
    {2000,  240, 224,  31,  12},
    {2000,  260, 222,  33,  13},
    {2000,  280, 219,  36,  14},
    {2000,  300, 217,  38,  15},
    {2000,  320, 214,  41,  16},
    {2000,  340, 212,  43,  17},
    {2000,  360, 209,  46,  18},
    {2000,  380, 207,  48,  19},
    {2000,  400, 204,  51,  20},
    {2000,  420, 201,  54,  21},
    {2000,  440, 199,  56,  22},
    {2000,  460, 196,  59,  23},
    {2000,  480, 194,  61,  24},
    {2000,  500, 191,  64,  25},
    {2000,  520, 189,  66,  26},
    {2000,  540, 186,  69,  27},
    {2000,  560, 184,  71,  28},
    {2000,  580, 181,  74,  29},
    {2000,  600, 178,  77,  30},
    {2000,  620, 176,  79,  31},
    {2000,  640, 173,  82,  32},
    {2000,  660, 171,  84,  33},
    {2000,  680, 168,  87,  34},
    {2000,  700, 166,  89,  35},
    {2000,  720, 163,  92,  36},
    {2000,  740, 161,  94,  37},
    {2000,  760, 158,  97,  38},
    {2000,  780, 156,  99,  39},
    {2000,  800, 153, 102,  40},
    {2000,  820, 150, 105,  41},
    {2000,  840, 148, 107,  42},
    {2000,  860, 145, 110,  43},
    {2000,  880, 143, 112,  44},
    {2000,  900, 140, 115,  45},
    {2000,  920, 138, 117,  46},
    {2000,  940, 135, 120,  47},
    {2000,  960, 133, 122,  48},
    {2000,  980, 130, 125,  49},
    {2000, 1000, 127, 128,  50},
    {2000, 1020, 125, 130,  51},
    {2000, 1040, 122, 133,  52},
    {2000, 1060, 120, 135,  53},
    {2000, 1080, 117, 138,  54},
    {2000, 1100, 115, 140,  55},
    {2000, 1120, 112, 143,  56},
    {2000, 1140, 110, 145,  57},
    {2000, 1160, 107, 148,  58},
    {2000, 1180, 105, 150,  59},
    {2000, 1200, 102, 153,  60},
    {2000, 1220,  99, 156,  61},
    {2000, 1240,  97, 158,  62},
    {2000, 1260,  94, 161,  63},
    {2000, 1280,  92, 163,  64},
    {2000, 1300,  89, 166,  65},
    {2000, 1320,  87, 168,  66},
    {2000, 1340,  84, 171,  67},
    {2000, 1360,  82, 173,  68},
    {2000, 1380,  79, 176,  69},
    {2000, 1400,  76, 179,  70},
    {2000, 1420,  74, 181,  71},
    {2000, 1440,  71, 184,  72},
    {2000, 1460,  69, 186,  73},
    {2000, 1480,  66, 189,  74},
    {2000, 1500,  64, 191,  75},
    {2000, 1520,  61, 194,  76},
    {2000, 1540,  59, 196,  77},
    {2000, 1560,  56, 199,  78},
    {2000, 1580,  54, 201,  79},
    {2000, 1600,  51, 204,  80},
    {2000, 1620,  48, 207,  81},
    {2000, 1640,  46, 209,  82},
    {2000, 1660,  43, 212,  83},
    {2000, 1680,  41, 214,  84},
    {2000, 1700,  38, 217,  85},
    {2000, 1720,  36, 219,  86},
    {2000, 1740,  33, 222,  87},
    {2000, 1760,  31, 224,  88},
    {2000, 1780,  28, 227,  89},
    {2000, 1800,  25, 230,  90},
    {2000, 1820,  23, 232,  91},
    {2000, 1840,  20, 235,  92},
    {2000, 1860,  18, 237,  93},
    {2000, 1880,  15, 240,  94},
    {2000, 1900,  13, 242,  95},
    {2000, 1920,  10, 245,  96},
    {2000, 1940,   8, 247,  97},
    {2000, 1960,   5, 250,  98},
    {2000, 1980,   3, 252,  99},
    {2000, 2000,   0, 255, 100},
};

const struct DataModel::s_timings DataModel::DEFAULT_PANEL_T_TABLE[15] = {
    {1920, 1080, 88, 44, 148, 4, 5, 0, 36},
    {1920, 1080, 88, 44, 148, 4, 5, 0, 36},
    {1920, 1080, 88, 44, 148, 4, 5, 0, 36},
    {1920, 1080, 88, 44, 148, 4, 5, 0, 36},
    {1920, 1080, 88, 44, 148, 4, 5, 0, 36},
    {1920, 1080, 88, 44, 148, 4, 5, 0, 36},
    {1920, 1080, 88, 44, 148, 4, 5, 0, 36},
    {1920, 1080, 88, 44, 148, 4, 5, 0, 36},
    {1920, 1080, 88, 44, 148, 4, 5, 0, 36},
    {1920, 1080, 88, 44, 148, 4, 5, 0, 36},
    {1920, 1080, 88, 44, 148, 4, 5, 0, 36},
    {1920, 1080, 88, 44, 148, 4, 5, 0, 36},
    {1920, 1080, 88, 44, 148, 4, 5, 0, 36},
    {1920, 1080, 88, 44, 148, 4, 5, 0, 36},
    {1920, 1080, 88, 44, 148, 4, 5, 0, 36}
};

//per impostazione icona riga TabInput
const QString DataModel::IN_TYPES[6]= {
    "inp_DVI", "inp_DP", "inp_DVI", "inp_DP", "inp_DVI", "inp_DVI"
};

void DataModel::RegisterSp(SerialProtocol *serialprot)
{
    m_sp = serialprot;
}

//------------------------------------------------------
void DataModel::CRC_AddByte(unsigned char data)
{
    tabs_crc16 ^= data << 8;                //XOR hi-byte of CRC w/dat
    for(int x=8; x; --x)					//Then, for 8 bit shifts...
        if(tabs_crc16 & 0x8000)             //If the uppermost bit of CRC is a 1
            tabs_crc16 = tabs_crc16 << 1 ^ 0x1021;	//if set, shift & XOR w/$1021 (CRC16-CCITT)  XOR the previous remainder with the divisor.
        else
            tabs_crc16 <<= 1;               //Else, just shift left once. Shift the next bit of the message into the remainder.
}

//------------------------------------------------------
unsigned short DataModel::ComputeTablesCRC()
{
    PRINT_FUNC_NAME

    int i;
    tabs_crc16 = 0;

    //EDID tables
    for(i=0; i<128; i++) CRC_AddByte(EDID_table_DVID1.at(i));
    for(i=0; i<128; i++) CRC_AddByte(EDID_table_DP1.at(i));
    for(i=0; i<128; i++) CRC_AddByte(EDID_table_DVID2.at(i));
    for(i=0; i<128; i++) CRC_AddByte(EDID_table_DP2.at(i));
    qDebug() << "EDID crc16:" << tabs_crc16;

    //Dimming table
    for(i=0; i<101; i++)
    {
        CRC_AddByte(LO(DimmingTable[i].wPeriod));
        CRC_AddByte(HI(DimmingTable[i].wPeriod));
        CRC_AddByte(LO(DimmingTable[i].wONTime));
        CRC_AddByte(HI(DimmingTable[i].wONTime));
        CRC_AddByte(DimmingTable[i].cDAC);
        CRC_AddByte(DimmingTable[i].cLED);
        CRC_AddByte(DimmingTable[i].cOSD);
    }
    qDebug() << "Dimming crc16:" << tabs_crc16;

    //Gamma table
    for(i=0; i<256; i++) {
        CRC_AddByte(GammaTable[i].redVal);
        CRC_AddByte(GammaTable[i].greenVal);
        CRC_AddByte(GammaTable[i].blueVal);
    }
    qDebug() << "Gamma crc16:" << tabs_crc16;

    //dati monitorinfo.txt
    for(i=0; i<4; i++)
        CRC_AddByte(array_MonitorSerial.at(i));
    qDebug() << "MonitorSerial crc16:" << tabs_crc16;

    for(i=0; i<4; i++)
        CRC_AddByte(array_SoftwareID.at(i));
    qDebug() << "SoftwareID crc16:" << tabs_crc16;

    for(i=0; i<16; i++)
        CRC_AddByte(array_manufacturer_ID.at(i));
    qDebug() << "Manufacturer ID crc16:" << tabs_crc16;

    for(i=0; i<16; i++)
        CRC_AddByte(array_monitor_type.at(i));
    qDebug() << "Monitor type ID crc16:" << tabs_crc16;

    //Panel Timings table
    for(i=0; i<15; i++) {
        CRC_AddByte(LO(PanelTimingsTable[i].Width));
        CRC_AddByte(HI(PanelTimingsTable[i].Width));
        CRC_AddByte(LO(PanelTimingsTable[i].Height));
        CRC_AddByte(HI(PanelTimingsTable[i].Height));
        CRC_AddByte(LO(PanelTimingsTable[i].H_front_porch));
        CRC_AddByte(HI(PanelTimingsTable[i].H_front_porch));
        CRC_AddByte(LO(PanelTimingsTable[i].H_sync));
        CRC_AddByte(HI(PanelTimingsTable[i].H_sync));
        CRC_AddByte(LO(PanelTimingsTable[i].H_back_porch));
        CRC_AddByte(HI(PanelTimingsTable[i].H_back_porch));
        CRC_AddByte(LO(PanelTimingsTable[i].V_front_porch));
        CRC_AddByte(HI(PanelTimingsTable[i].V_front_porch));
        CRC_AddByte(PanelTimingsTable[i].V_sync);
        CRC_AddByte(PanelTimingsTable[i].clock_idx);
        CRC_AddByte(LO(PanelTimingsTable[i].V_back_porch));
        CRC_AddByte(HI(PanelTimingsTable[i].V_back_porch));
    }

    QString hexval = QString("0x%1").arg(tabs_crc16, 4, 16, QLatin1Char('0'));
    qDebug() << "Final crc16:" << tabs_crc16 << "HEX:"<< hexval;

    return tabs_crc16;
}

//---------------------------------------------------------
void DataModel::loadDefaultSettings()
{
    PRINT_FUNC_NAME

    int i;
    this->setProperty("dimLevel",       100);   //Backlight level: 100%
    this->setProperty("contrastLevel",  50);    //[TBD]
    this->setProperty("buzzerVol",      50);
    this->setProperty("scalingMode",    1);     //Fill-all
    this->setProperty("inputMode",      1);
    this->setProperty("ddc_ci_Mode",    1);     //Auto (DDC/CI follow active input).
    this->setProperty("splashLogoToutIdx", 4);  //[TBD] 5 sec
    this->setProperty("noSigToutIdx",   2);
    this->setProperty("stdbyToutIdx",   3);

    m_inputIndex.clear();
    m_inputTypes.clear();
    m_ddcCiStates.clear();
    m_inputStates.clear();
    m_anaEDIDStates.clear();

    m_inputIndex << 0 << 1 << 2 << 3 << 4 << 5;
    m_inputTypes << "inp_DVI" << "inp_DP" << "inp_DVI" << "inp_DP" << "inp_DVI" << "inp_DVI";
    m_ddcCiStates  << 0 << 1 << 0 << 0 << 0 << 0;
    m_inputStates  << 0 << 1 << 0 << 0 << 0 << 0;

    //Use analog EDID: none set - future use
    m_anaEDIDStates << 0 << 0 << 0 << 0 << 0 << 0; //Use analog EDID: none set

    //load default EDID (use in case no valid files are found)
    for(i=0; i<128; i++) {
        int val = DataModel::DEFAULT_EDID[i];
        EDID_table_DVID1 << val;
        EDID_table_DP1   << val;
        EDID_table_DVID2 << val;
        EDID_table_DP2   << val;
    }

    memcpy(DimmingTable, DEFAULT_DIMMING_TABLE, 101 * sizeof(struct s_dimm));

    //per cifra slider OSD
    for(int i=0; i<101; i++)
        m_osd_dimming_table << DEFAULT_DIMMING_TABLE[i].cOSD;

//    qDebug() << "\nDIMMING TABLE\n";
//    for(int i=0; i<101; i++) {
//        qDebug() << DimmingTable[i].wPeriod
//                 << DimmingTable[i].wONTime
//                 << DimmingTable[i].cDAC
//                 << DimmingTable[i].cLED
//                 << DimmingTable[i].cOSD;
//    }

    //valori di default (gamma lineare)
    for(int i=0; i<256; i++) {
        GammaTable[i].redVal = i;
        GammaTable[i].greenVal = i;
        GammaTable[i].blueVal = i;
    }

    //timings pannello di default
    memcpy(PanelTimingsTable, DEFAULT_PANEL_T_TABLE, 15 * sizeof(struct s_timings));
	
    array_MonitorSerial = QByteArray(4, '\0');
    array_SoftwareID =    QByteArray(4, '\0');
    array_manufacturer_ID =  QByteArray(manufacturer_ID_default, 16);
    array_monitor_type =  QByteArray(monitor_type_default, 16);

	int len = array_manufacturer_ID.size();
	//padding con \0
	if(len < 16) {
		array_manufacturer_ID.resize(16);
		for(int i=len; i<16; i++)
			array_manufacturer_ID[i] = 0;
	}
	
	len = array_monitor_type.size();
	//padding con \0
	if(len < 16) {
		array_monitor_type.resize(16);
		for(int i=len; i<16; i++)
			array_monitor_type[i] = 0;
	}
}

//--------------------------------------------------------------
//Ricostruisce le varie matrici per il rendering del Tab Inputs
//Chiamato se arriva da seriale il comando SOURCE_SET
//--------------------------------------------------------------
void DataModel::set_InputTable(int p0, int p1, int p2, int p3, int p4, int p5)
{
    PRINT_FUNC_NAME

    //preset
    m_inputIndex.clear();
    m_inputTypes.clear();
    m_inputStates.clear();
    m_ddcCiStates.clear();
    //m_anaEDIDStates.clear(); --> future use

    //Priorità  0=max .. <num inputs-1> = min
    m_inputIndex << p0 << p1 << p2 << p3 << p4 << p5;

    //Icons (DVI / DP)
    m_inputTypes << IN_TYPES[p0] << IN_TYPES[p1] << IN_TYPES[p2]
                 << IN_TYPES[p3] << IN_TYPES[p4] << IN_TYPES[p5];

    //Current Input:
    //per posizionare il pallino in corrispondenza dell'ingresso attivo
    //posizione relativa a come sono ordinate le righe in base alla priority
    //nota: sempre 6 entries anche se ho 4 ingressi
    //mette un 1 se l'Id dell'ingresso attivo corrisponde a quello nella riga
    for(int i=0; i<6; i++)
        m_inputStates << (m_inputIndex.at(i).toInt() == m_CurrentIN ? 1:0);

    //Current DDC/CI Input:
    //per posizionare il pallino in corrispondenza dell'ingresso DDC/CI selezionato
    //nota: sempre 6 entries anche se ho 4 ingressi
    for(int i=0; i<6; i++)
        m_ddcCiStates << (m_inputIndex.at(i).toInt() == m_CurrentDDC_IN ? 1:0);

    //m_anaEDIDStates --> future use

    qDebug() << "inputTypes -->"  << m_inputTypes;
    qDebug() << "inputIndex -->"  << m_inputIndex;
    qDebug() << "inputStates -->" << m_inputStates;
    qDebug() << "ddcCiStates -->" << m_ddcCiStates;
}

//---------------------------------------------------------
//send parameter to LPC via serial link
void DataModel::updateSetting(int parID, int par_val)
{
    qDebug() << COL_YELLOW
             << "DataModel::updateSetting parID:" << parID << par_val
             << COL_NORMAL;

    if(m_sp)
        m_sp->Cmd_PARAM_SET((enum e_params)parID, par_val);
}

void DataModel::updateSourceSettings()
{
    PRINT_FUNC_NAME
    qDebug() << "m_CurrentIN:" << m_CurrentIN
                << "m_CurrentDDC_IN:" << m_CurrentDDC_IN
                ;
    if(m_sp)
        m_sp->Cmd_SOURCE_SET(m_InputMode, m_CurrentIN, m_DDC_CI_Mode,
                             m_CurrentDDC_IN, m_inputIndex, m_inputs1Disable);
}

void DataModel::doSettingsSave()
{
    PRINT_FUNC_NAME
    if(m_sp)
        m_sp->Cmd_COMMAND(cmd_SETTINGS_SAVE);
}

void DataModel::doSettingsRecall()
{
    PRINT_FUNC_NAME
    if(m_sp)
        m_sp->Cmd_COMMAND(cmd_SETTINGS_RECALL);
}

void DataModel::doFactoryReset()
{
    PRINT_FUNC_NAME
    if(m_sp)
        m_sp->Cmd_COMMAND(cmd_FACTORY_RESET);
}

void DataModel::doResetCounters()
{
    PRINT_FUNC_NAME
    if(m_sp)
        m_sp->Cmd_COMMAND(cmd_RESET_COUNTERS);
}

void DataModel::doSendKey(int key, int dest_adr)
{
    PRINT_FUNC_NAME
    qDebug("key: %d", key);
    if(m_sp)
        m_sp->Cmd_SEND_KEY(key, dest_adr);
}

void DataModel::doBrightUP()
{
    PRINT_FUNC_NAME
    if(m_sp)
        m_sp->Cmd_COMMAND(cmd_BRT_UP);
}

void DataModel::doBrightDOWN()
{
    PRINT_FUNC_NAME
    if(m_sp)
        m_sp->Cmd_COMMAND(cmd_BRT_DWN);
}

void DataModel::doBrightBroadUP()
{
    PRINT_FUNC_NAME
    if(m_sp)
        m_sp->Cmd_COMMAND(cmd_BRT_BROAD_UP);
}

void DataModel::doBrightBroadDOWN()
{
    PRINT_FUNC_NAME
    if(m_sp)
        m_sp->Cmd_COMMAND(cmd_BRT_BROAD_DWN);
}

void DataModel::doSourceChange()
    {
    PRINT_FUNC_NAME
    if(m_sp)
        m_sp->Cmd_COMMAND(cmd_SRC_CHAG);
}

//Controlla se c'è connessione di rete su eth0
bool DataModel::isNetworkUp()
{
    QFile file("/sys/class/net/eth0/operstate");
    if(file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);
        QString line = in.readLine();
        return (line.compare("up") == 0);
    }
    //in caso di errore non faccio apparire l'icona di warning
    else
        return true;
}

//invalida il flag al cambio di tab
void DataModel::invalidateWaitSet()
{
    if(m_sp)
        m_sp->setPacketWaited = false;
}

void DataModel::doOSDstatus(bool open)
{
    if(m_sp)
        m_sp->Cmd_OSD_STATUS_SET(open);

    if(open)
        QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
    else
        QGuiApplication::setOverrideCursor(Qt::BlankCursor);
}

/***************************************************************************
*   Routine:    setDefaultNetwParams()
*   Decrizione:
*   Note:
****************************************************************************/
void DataModel::setDefaultNetwParams()
{
    PRINT_FUNC_NAME

    this->setProperty("dhcp_on", false);
    this->setProperty("scom_srv", false);

    this->setProperty("ipAddr0", 192);
    this->setProperty("ipAddr1", 168);
    this->setProperty("ipAddr2", 1);
    this->setProperty("ipAddr3", 2);

    this->setProperty("netMask0", 255);
    this->setProperty("netMask1", 255);
    this->setProperty("netMask2", 255);
    this->setProperty("netMask3", 0);

    this->setProperty("defGW0", 192);
    this->setProperty("defGW1", 168);
    this->setProperty("defGW2", 1);
    this->setProperty("defGW3", 1);

    this->setProperty("dns0", 8);
    this->setProperty("dns1", 8);
    this->setProperty("dns2", 8);
    this->setProperty("dns3", 8);

    this->setProperty("scomSrvIP0", 192);
    this->setProperty("scomSrvIP1", 168);
    this->setProperty("scomSrvIP2", 1);
    this->setProperty("scomSrvIP3", 10);
}

/***************************************************************************
*   Routine:	saveNetwParams()
*   Decrizione:
*   Note:
****************************************************************************/
bool DataModel::saveNetwParams()
{
    PRINT_FUNC_NAME

    QFile file(NET_PARAM_FILE);
    if(file.open(QIODevice::WriteOnly))
    {
        QDataStream dataStream(&file);

        dataStream << m_dhcp_on;
        dataStream << m_scom_srv;

        dataStream << m_ipAddr0;
        dataStream << m_ipAddr1;
        dataStream << m_ipAddr2;
        dataStream << m_ipAddr3;

        dataStream << m_netMask0;
        dataStream << m_netMask1;
        dataStream << m_netMask2;
        dataStream << m_netMask3;

        dataStream << m_defGW0;
        dataStream << m_defGW1;
        dataStream << m_defGW2;
        dataStream << m_defGW3;

        dataStream << m_dns0;
        dataStream << m_dns1;
        dataStream << m_dns2;
        dataStream << m_dns3;

        dataStream << m_scomSrvIP0;
        dataStream << m_scomSrvIP1;
        dataStream << m_scomSrvIP2;
        dataStream << m_scomSrvIP3;

        file.close();
        system("sync");
        return true;
    }
    qDebug("Errore salvataggio file parametri net");
    return false;
}

/***************************************************************************
*   Routine:    startDHCPclient()
*   Decrizione:
*   Note:
****************************************************************************/
void DataModel::startDHCPclient()
{
    PRINT_FUNC_NAME

    system("rm -f " DHCP_LEASE_OK_FILE);
    system("udhcpc -b &");
}

/***************************************************************************
*   Routine:    isDHCP_leased()
*   Decrizione:
*   Note:
****************************************************************************/
bool DataModel::isDHCP_leased()
{
    return QFile::exists(DHCP_LEASE_OK_FILE);
}

/***************************************************************************
*   Routine:    stopDHCPclient()
*   Decrizione:
*   Note:
****************************************************************************/
void DataModel::stopDHCPclient()
{
    PRINT_FUNC_NAME

    system("killall udhcpc");
    system("rm -f " DHCP_LEASE_OK_FILE);
}

/***************************************************************************
*   Routine:    loadNetwParams()
*   Decrizione:
*   Note:
****************************************************************************/
void DataModel::loadNetwParams()
{
    PRINT_FUNC_NAME

    //provo ad aprire il file
    QFile file(NET_PARAM_FILE);
    if(file.open(QIODevice::ReadOnly))
    {
        QDataStream dataStream(&file);

        dataStream >> m_dhcp_on;
        dataStream >> m_scom_srv;
        dataStream >> m_ipAddr0;
        dataStream >> m_ipAddr1;
        dataStream >> m_ipAddr2;
        dataStream >> m_ipAddr3;

        dataStream >> m_netMask0;
        dataStream >> m_netMask1;
        dataStream >> m_netMask2;
        dataStream >> m_netMask3;

        dataStream >> m_defGW0;
        dataStream >> m_defGW1;
        dataStream >> m_defGW2;
        dataStream >> m_defGW3;

        dataStream >> m_dns0;
        dataStream >> m_dns1;
        dataStream >> m_dns2;
        dataStream >> m_dns3;

        dataStream >> m_scomSrvIP0;
        dataStream >> m_scomSrvIP1;
        dataStream >> m_scomSrvIP2;
        dataStream >> m_scomSrvIP3;

        file.close();
    }
    //se file parametri non esiste o errore caricamento,
    //ricreo il file con i parametri di default
    else {
        qDebug("Creo file parametri net");
        setDefaultNetwParams();
        saveNetwParams();
    }
}

/***************************************************************************
*   Routine:    getDHCPresults()
*   Decrizione: legge i vari IP ottenuti dal client DHCP
*   Note:       sono scritti in un file temporaneo
****************************************************************************/
bool DataModel::getDHCPresults()
{
    PRINT_FUNC_NAME

    //leggo i parametri ricevuti dal server DHCP
    QFile f(DHCP_LEASE_OK_FILE);

    if(f.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&f);
        //qDebug() << "file opened  size:" << f.size();
        //mi aspetto 4 linee nel file (IP, Subnet mask, default GW, DNS server)
        QString line1 = in.readLine();
        QString line2 = in.readLine();
        QString line3 = in.readLine();
        QString line4 = in.readLine();
        f.close();

        if(line1 == "" || line2 == "" || line3 == "" || line4 == "") {
            qDebug("errore1 formato file dhcp_resp");
            return false;
        }
        else
        {
            QStringList ip;

            //********************************************************
            // Host IP
            //********************************************************
            ip = line1.split(".", QString::KeepEmptyParts, Qt::CaseInsensitive);
            //controllo che ci siano almeno 4 campi
            if(ip.length() == 4) {
                qDebug() << "--> DHCP Host IP:" << ip.at(0) << ip.at(1) << ip.at(2) << ip.at(3);
                this->setProperty("ipAddr0", ip.at(0).toInt());
                this->setProperty("ipAddr1", ip.at(1).toInt());
                this->setProperty("ipAddr2", ip.at(2).toInt());
                this->setProperty("ipAddr3", ip.at(3).toInt());
            }
            else {
                qDebug("errore2 formato file dhcp_resp");
                return false;
            }

            //********************************************************
            // Subnet mask
            //********************************************************
            ip = line2.split(".", QString::KeepEmptyParts, Qt::CaseInsensitive);
            //controllo che ci siano almeno 4 campi
            if(ip.length() == 4) {
                qDebug() << "--> Subnet mask:" << ip.at(0) << ip.at(1) << ip.at(2) << ip.at(3);
                this->setProperty("netMask0", ip.at(0).toInt());
                this->setProperty("netMask1", ip.at(1).toInt());
                this->setProperty("netMask2", ip.at(2).toInt());
                this->setProperty("netMask3", ip.at(3).toInt());
            }
            else {
                qDebug("errore3 formato file dhcp_resp");
                return false;
            }
            //********************************************************
            // Default GW
            //********************************************************
            ip = line3.split(".", QString::KeepEmptyParts, Qt::CaseInsensitive);
            //controllo che ci siano almeno 4 campi
            if(ip.length() == 4) {
                qDebug() << "--> Default GW :" << ip.at(0) << ip.at(1) << ip.at(2) << ip.at(3);
                this->setProperty("defGW0", ip.at(0).toInt());
                this->setProperty("defGW1", ip.at(1).toInt());
                this->setProperty("defGW2", ip.at(2).toInt());
                this->setProperty("defGW3", ip.at(3).toInt());
            }
            else {
                qDebug("errore4 formato file dhcp_resp");
                return false;
            }
            //********************************************************
            // DNS server
            //********************************************************
            ip = line3.split(".", QString::KeepEmptyParts, Qt::CaseInsensitive);
            //controllo che ci siano almeno 4 campi
            if(ip.length() == 4) {
                qDebug() << "--> DNS server :" << ip.at(0) << ip.at(1) << ip.at(2) << ip.at(3);
                this->setProperty("dns0", ip.at(0).toInt());
                this->setProperty("dns1", ip.at(1).toInt());
                this->setProperty("dns2", ip.at(2).toInt());
                this->setProperty("dns3", ip.at(3).toInt());
            }
            else {
                qDebug("errore5 formato file dhcp_resp");
                return false;
            }
        }
        return true;
    }
    qDebug("dhcp_resp file open error");
    return false;
}

/* EOF */



































