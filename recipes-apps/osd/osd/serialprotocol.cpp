/*****************************************************************************
*   Fie:		serialprotocol.cpp
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#include "global.h"
#ifndef DEBUG_WINDOWS
#include <termios.h>
#include <sys/poll.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <QEventLoop>
#include "serialprotocol.h"

#define LO_BYTE(x) (unsigned char)(x % 256)
#define HI_BYTE(x) (unsigned char)(x / 256)

//da chiarire
int MAX_PAR_VAL[] = {
    MAX_OSD_BRIGHTNESS,	//osdBRIGHTNESS
    MAX_OSD_CONTRAST,	//osdCONTRAST
    MAX_OSD_BUZZER,		//osdBUZZER
    0,					//osdPOWER_MODE
    0,					//osdSCALING_MODE
    0,					//osdNOSIG_TOUT
    0,					//osdSTDBY_TOUT
    0,					//osdTEMPERATURE
    0,					//osdMIN_TEMP
    0,					//osdMAX_TEMP
    0,					//osdLOGO_TIMEOUT
    0,					//osdMON_SER_ADR
    0,					//osdJ24_CFG
    0,					//osdLOCK_ON_SER
    0 					//osdMULTI_DIM
};

//-----------------------------------------------------------------------
//trailer offset (posizione del primo byte 0x33 del trailer)
int TAB_OFFS[] = {
    8,  //cmdPARAM_SET
    13, //cmdSOURCE_SET
    7,  //cmdRUNTIME_SET
    0,  //cmdCOMMAND        non usato
    0,  //cmdOSD_READY      non usato
    3,  //cmdSEND_TABLES
    12, //cmdINIT_END
    3,  //cmdSEND_TEMPER
    3,  //cmdSER_LOCK
    5,  //cmdSEND_KEY   non usato (inviato da imx6 a LPC)
};

//-----------------------------------------------------------------------
SerialProtocol::SerialProtocol(QObject *parent) :
    QObject(parent)
{
    PRINT_FUNC_NAME
    setPacketWaited = false;
    initOSDGraphics = false;
    serial_fd = -1;
    m_dm = 0;
    m_tm = 0;
    m_ql = 0;
}

//-----------------------------------------------------------------------
bool SerialProtocol::openSerialPort()
{
#ifndef DEBUG_WINDOWS

    #ifdef DEBUG_IMX_LORENZO
    //connettore esterno RS232 del terminale di test
    serial_fd = open ("/dev/ttymxc1", O_RDWR | O_NOCTTY);
    #else
    printf("Apro /dev/ttyACM0\n"); fflush(stdout);
    serial_fd = open ("/dev/ttyACM0", O_RDWR | O_NOCTTY);
    #endif
    if(serial_fd < 0) {
        printf("Error %d opening serial: %s\n", errno, strerror(errno));
        fflush(stdout);
        return false;
    }
#endif
    return true;
}

//-----------------------------------------------------------------------
bool SerialProtocol::setSerialParams()
{
#ifndef DEBUG_WINDOWS
    int parity = 0;
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (serial_fd, &tty) != 0) {
        printf("error %d from tcgetattr", errno);
        return false;
    }

    cfsetospeed (&tty, B115200);
    cfsetispeed (&tty, B115200);

    //cfmakeraw(tty);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break as \000 chars
    tty.c_iflag &= ~IGNBRK;         		// disable break processing
    tty.c_lflag = 0;                		// no signaling chars, no echo, no canonical processing
    tty.c_oflag = 0;                		// no remapping, no delays
    tty.c_cc[VMIN] = 0;                     // read doesn't block
    tty.c_cc[VTIME] = 5;            		// 0.5 seconds read timeout
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                    | INLCR | IGNCR | ICRNL | IXON);
    tty.c_cflag |= (CLOCAL | CREAD);		// ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);		// shut off parity
    tty.c_cflag &= ~INLCR;                  // do not convert 0x0d to 0x0a
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr (serial_fd, TCSANOW, &tty) != 0) {
        //return("error %d from tcsetattr", errno);
        return false;
    }
#endif
    return true;
}

//---------------------------------------------------------------
int SerialProtocol::Cmd_PARAM_SET(enum e_params parID, int par_val)
{
	int retVal;
	
    if((int)parID >= (int)osdINVALID) return -1; //clamp

	ser_mutex.lock();
	
    int maxVal = MAX_PAR_VAL[(int)parID];

    txbuff[0] = 0xAA;               //head
    txbuff[1] = 0x55;
    txbuff[2] = cmdPARAM_SET;       //comando
    txbuff[3] = parID;              //parametro
    txbuff[4] = LO_BYTE(maxVal);    //valore massimo //LOLLO mettere formula Fabrizio
    txbuff[5] = HI_BYTE(maxVal);
    txbuff[6] = LO_BYTE(par_val);   //valore parametro
    txbuff[7] = HI_BYTE(par_val);
    txbuff[8] = 0x33;               //tail
    txbuff[9] = 0xCC;

    retVal = Send_bytes(10);
	
	ser_mutex.unlock();
	
	return retVal;
}

//-------------------------------------------------------------------------
//scrittura sulla seriale concorrente con le altre
//perche' questa e' chiamata dal timer del main thread
//vedi TempMonitor::UpdateTemperature()
int SerialProtocol::Cmd_TEMPERATURE_SET(int temp)
{
	ser_mutex.lock();
	
    txbuff[0] = 0xAA;               //head
    txbuff[1] = 0x55;
    txbuff[2] = cmdPARAM_SET;       //comando
    txbuff[3] = osdTEMPERATURE;     //parametro
    txbuff[4] = 0;                  //valore massimo
    txbuff[5] = 0;
    txbuff[6] = LO_BYTE(temp);      //valore parametro
    txbuff[7] = HI_BYTE(temp);
    txbuff[8] = 0x33;               //tail
    txbuff[9] = 0xCC;

    if(serial_fd == -1) {
        qDebug("serial_fd errato, salto 2");
		ser_mutex.unlock();
        return 0;
    }

    int rc = write(serial_fd, txbuff, 10);
	
	ser_mutex.unlock();
	
    if(rc < 0) {
        printf("write error: %s\n", strerror(errno));
        return rc;
    }
    return 0;
}

//-------------------------------------------------------------------------
int SerialProtocol::Cmd_SOURCE_SET(int inMode,  int currIn,
                                   int ddcMode, int currDDC,
                                   QVariantList inIndex, bool inDisabled)
{
	int retVal;
	
    if(inIndex.length() < 6) return -1; //sanity check
	
	ser_mutex.lock();

    txbuff[0] = 0xAA;           //head
    txbuff[1] = 0x55;
    txbuff[2] = cmdSOURCE_SET;   //comando
    txbuff[3] =  (unsigned char)inMode;
    txbuff[4] =  (unsigned char)currIn;
    txbuff[5] =  (unsigned char)ddcMode;
    txbuff[6] =  (unsigned char)currDDC;
    txbuff[7] =  (unsigned char)inIndex.at(0).toInt();  //DVI-D1
    txbuff[8] =  (unsigned char)inIndex.at(1).toInt();  //DP1
    txbuff[9] =  (unsigned char)inIndex.at(2).toInt();  //DVI-D2
    txbuff[10] = (unsigned char)inIndex.at(3).toInt();  //DP2
    txbuff[11] = 0xFF; //(unsigned char)inIndex.at(4).toInt(); //VGA1
    txbuff[12] = 0xFF; //(unsigned char)inIndex.at(5).toInt(); //VGA2
    txbuff[13] = 0x33;         //tail
    txbuff[14] = 0xCC;

    //se DVI-D1 e DP1 sono disabilitati, sono marcati con 0xff
    if(inDisabled) {
        txbuff[7] = 0xFF; //conservano priorità 0 e 1 ma sono mascherate da 0xFF
        txbuff[8] = 0xFF;
        txbuff[9]  -= 2; //priorità 2-3 traslate a 0-1
        txbuff[10] -= 2;
        txbuff[11] = 0xFF;
        txbuff[12] = 0xFF;
    }

    retVal = Send_bytes(15);
	
	ser_mutex.unlock();
	
	return retVal;
}

//-----------------------------------------------------
int SerialProtocol::Cmd_COMMAND(int cmdID)
{
	int retVal;
	
    PRINT_FUNC_NAME

	ser_mutex.lock();
	
    txbuff[0] = 0xAA;   //head
    txbuff[1] = 0x55;
    txbuff[2] = cmdCOMMAND;
    txbuff[3] = cmdID;  //comando
    txbuff[4] = 0x33;   //tail
    txbuff[5] = 0xCC;

    retVal = Send_bytes(6);
	
	ser_mutex.unlock();
	
	return retVal;
}

#define ETHERNET_FLAG		0x01
#define UNUSED_FLAG			0x02
#define DEBUG_CONSOLE_FLAG	0x04
#define SAAB_CFG_FLAG		0x08
#define SAAB_SCALE_FLAG		0x10

//-------------------------------------------------------------------------
int SerialProtocol::Cmd_OSD_READY()
{
	int retVal;
	
    PRINT_FUNC_NAME

	ser_mutex.lock();
	
    txbuff[0] = 0xAA;               //head
    txbuff[1] = 0x55;
    txbuff[2] = cmdOSD_READY;
    txbuff[3] = m_dm->property("logo_present").toInt();
	txbuff[4] = 0;
    txbuff[4] |= (m_dm->property("eth_enabled").toBool())? ETHERNET_FLAG : 0;
    //txbuff[4] |= (m_dm->property("dimming_pot_ena").toBool())? UNUSED_FLAG : 0;
    txbuff[4] |= (m_dm->property("debug_console").toBool())? DEBUG_CONSOLE_FLAG : 0;
    txbuff[4] |= (m_dm->property("saab_config").toBool())? SAAB_CFG_FLAG : 0;
    txbuff[4] |= (m_dm->property("saab_pot_scale").toBool())? SAAB_SCALE_FLAG : 0;
    txbuff[5] = LO(m_dm->tabs_crc16);
    txbuff[6] = HI(m_dm->tabs_crc16);
    txbuff[7] = 0x33;               //tail
    txbuff[8] = 0xCC;

    retVal = Send_bytes(9);
	
	ser_mutex.unlock();
	
	return retVal;
}

//----------------------------------------------------------------------------
int SerialProtocol::Send_chunk(int cmdID, int chunkNum, unsigned char *chunk)
{
	//Do not lock the mutex here
	
    txbuff[0] = 0xAA;               //head
    txbuff[1] = 0x55;
    txbuff[2] = cmdID;              //command
    txbuff[3] = chunkNum;
    memcpy(&txbuff[4], chunk, 32);  //4..35 => 32 bytes payload
    txbuff[36] = 0x33;              //tail
    txbuff[37] = 0xCC;

    return Send_bytes(38);
}

//-----------------------------------------------------
int SerialProtocol::Cmd_EDID_SET()
{
    PRINT_FUNC_NAME

    unsigned char table[128];
    int i;
    unsigned char chunkNum;

	ser_mutex.lock();
	
    chunkNum = 0;
    for(i=0; i<128; i++) table[i] = m_dm->EDID_table_DVID1.at(i);
    Send_chunk(cmdEEDID_SET_IN0, chunkNum++, table);
    Send_chunk(cmdEEDID_SET_IN0, chunkNum++, &table[32]);
    Send_chunk(cmdEEDID_SET_IN0, chunkNum++, &table[64]);
    Send_chunk(cmdEEDID_SET_IN0, chunkNum,   &table[96]);

    chunkNum = 0;
    for(i=0; i<128; i++) table[i] = m_dm->EDID_table_DP1.at(i);
    Send_chunk(cmdEEDID_SET_IN1, chunkNum++, table);
    Send_chunk(cmdEEDID_SET_IN1, chunkNum++, &table[32]);
    Send_chunk(cmdEEDID_SET_IN1, chunkNum++, &table[64]);
    Send_chunk(cmdEEDID_SET_IN1, chunkNum,   &table[96]);

    chunkNum = 0;
    for(i=0; i<128; i++) table[i] = m_dm->EDID_table_DVID2.at(i);
    Send_chunk(cmdEEDID_SET_IN2, chunkNum++, table);
    Send_chunk(cmdEEDID_SET_IN2, chunkNum++, &table[32]);
    Send_chunk(cmdEEDID_SET_IN2, chunkNum++, &table[64]);
    Send_chunk(cmdEEDID_SET_IN2, chunkNum,   &table[96]);

    chunkNum = 0;
    for(i=0; i<128; i++) table[i] = m_dm->EDID_table_DP2.at(i);
    Send_chunk(cmdEEDID_SET_IN3, chunkNum++, table);
    Send_chunk(cmdEEDID_SET_IN3, chunkNum++, &table[32]);
    Send_chunk(cmdEEDID_SET_IN3, chunkNum++, &table[64]);
    Send_chunk(cmdEEDID_SET_IN3, chunkNum,   &table[96]);

	ser_mutex.unlock();
	
    return 0;
}

//-----------------------------------------------------
int SerialProtocol::Cmd_DIMMING_SET()
{
    PRINT_FUNC_NAME

    unsigned char chunk[32];
    unsigned char *pchunk = chunk;
    unsigned char chunkNum = 0;
	
	ser_mutex.lock();
	
    //prime 100 righe, ogni 4 invio sulla seriale un chunk
    for(int rowNum=0; rowNum<100; rowNum++)
    {
        *pchunk++ = LO(m_dm->DimmingTable[rowNum].wPeriod);
        *pchunk++ = HI(m_dm->DimmingTable[rowNum].wPeriod);
        *pchunk++ = LO(m_dm->DimmingTable[rowNum].wONTime);
        *pchunk++ = HI(m_dm->DimmingTable[rowNum].wONTime);
        *pchunk++ = m_dm->DimmingTable[rowNum].cDAC;
        *pchunk++ = m_dm->DimmingTable[rowNum].cLED;
        *pchunk++ = m_dm->DimmingTable[rowNum].cOSD;
        *pchunk++ = 0;    //padding

        if(((rowNum + 1) % 4) == 0) {
            Send_chunk(cmdDIMMING_SET, chunkNum++, chunk);
            pchunk = chunk; //ricomincio a riempire il buffer del chunk
        }
    }

    //ultimo chunk con padding
    memset(chunk, 0, 32);
    chunk[0] = LO(m_dm->DimmingTable[100].wPeriod);
    chunk[1] = HI(m_dm->DimmingTable[100].wPeriod);
    chunk[2] = LO(m_dm->DimmingTable[100].wONTime);
    chunk[3] = HI(m_dm->DimmingTable[100].wONTime);
    chunk[4] = m_dm->DimmingTable[100].cDAC;
    chunk[5] = m_dm->DimmingTable[100].cLED;
    chunk[6] = m_dm->DimmingTable[100].cOSD;
    Send_chunk(cmdDIMMING_SET, chunkNum, chunk);

	ser_mutex.unlock();
	
    return 0;
}

//-----------------------------------------------------
int SerialProtocol::Cmd_GAMMA_SET()
{
    PRINT_FUNC_NAME

    unsigned char chunk[32];
    unsigned char *pchunk = chunk;
    unsigned char chunkNum = 0;

	ser_mutex.lock();
	
    //ogni chunk ha 8 righe della tabella
    for(int rowNum=0; rowNum<256; rowNum++)
    {
        *pchunk++ = m_dm->GammaTable[rowNum].redVal;
        *pchunk++ = m_dm->GammaTable[rowNum].greenVal;
        *pchunk++ = m_dm->GammaTable[rowNum].blueVal;
        *pchunk++ = 0;    //padding

        if(((rowNum + 1) % 8) == 0) {
            Send_chunk(cmdGAMMA_SET, chunkNum++, chunk);
            pchunk = chunk; //ricomincio a riempire il buffer del chunk
        }
    }
	
	ser_mutex.unlock();
	
    return 0;
}

//-----------------------------------------------------
int SerialProtocol::Cmd_INFO_SET()
{
	int retVal;
	
    PRINT_FUNC_NAME

	ser_mutex.lock();
	
    txbuff[0] = 0xAA;           //head
    txbuff[1] = 0x55;
    txbuff[2] = cmdINFO_SET;    //command
    txbuff[3] = 0;              //nota: CHUNK 0

    qDebug("MONITOR_SERIAL: %d %d %d %d"
           ,m_dm->array_MonitorSerial.at(0)
           ,m_dm->array_MonitorSerial.at(1)
           ,m_dm->array_MonitorSerial.at(2)
           ,m_dm->array_MonitorSerial.at(3));
    txbuff[4] = m_dm->array_MonitorSerial.at(0);
    txbuff[5] = m_dm->array_MonitorSerial.at(1);
    txbuff[6] = m_dm->array_MonitorSerial.at(2);
    txbuff[7] = m_dm->array_MonitorSerial.at(3);

    txbuff[8] =  m_dm->array_SoftwareID.at(0);
    txbuff[9] =  m_dm->array_SoftwareID.at(1);
    txbuff[10] = m_dm->array_SoftwareID.at(2);
    txbuff[11] = m_dm->array_SoftwareID.at(3);
	
	for (int i=12; i<36; i++)
	{
		txbuff[i] = 0;
	}

    txbuff[36] = 0x33;                  //tail
    txbuff[37] = 0xCC;

    retVal = Send_bytes(38);
    if(retVal < 0) {
        ser_mutex.unlock();
        return retVal;
    }

    //Invio MANUFACTURER_ID e MONITOR_TYPE
	txbuff[3] = 1;              //CHUNK 1
    for(int i=0; i<16; i++) {
        txbuff[4 +i] = m_dm->array_manufacturer_ID.at(i);
        txbuff[20+i] = m_dm->array_monitor_type.at(i);
    }
    txbuff[36] = 0x33;              //tail
    txbuff[37] = 0xCC;

    retVal = Send_bytes(38);
	
	ser_mutex.unlock();
	
	return retVal;
}

//-----------------------------------------------------
int SerialProtocol::Cmd_PANEL_TIMINGS_SET()
{
    PRINT_FUNC_NAME

    unsigned char chunk[32];
    unsigned char *pchunk = chunk;
    unsigned char chunkNum = 0;

	ser_mutex.lock();
	
    //ogni chunk ha 8 righe della tabella
    for(int rowNum=0; rowNum<14; rowNum++)
    {
        *pchunk++ = LO(m_dm->PanelTimingsTable[rowNum].Width);
        *pchunk++ = HI(m_dm->PanelTimingsTable[rowNum].Width);
        *pchunk++ = LO(m_dm->PanelTimingsTable[rowNum].Height);
        *pchunk++ = HI(m_dm->PanelTimingsTable[rowNum].Height);
        *pchunk++ = LO(m_dm->PanelTimingsTable[rowNum].H_front_porch);
        *pchunk++ = HI(m_dm->PanelTimingsTable[rowNum].H_front_porch);
        *pchunk++ = LO(m_dm->PanelTimingsTable[rowNum].H_sync);
        *pchunk++ = HI(m_dm->PanelTimingsTable[rowNum].H_sync);
        *pchunk++ = LO(m_dm->PanelTimingsTable[rowNum].H_back_porch);
        *pchunk++ = HI(m_dm->PanelTimingsTable[rowNum].H_back_porch);
        *pchunk++ = LO(m_dm->PanelTimingsTable[rowNum].V_front_porch);
        *pchunk++ = HI(m_dm->PanelTimingsTable[rowNum].V_front_porch);
        *pchunk++ =    m_dm->PanelTimingsTable[rowNum].V_sync;
        *pchunk++ =    m_dm->PanelTimingsTable[rowNum].clock_idx;
        *pchunk++ = LO(m_dm->PanelTimingsTable[rowNum].V_back_porch);
        *pchunk++ = HI(m_dm->PanelTimingsTable[rowNum].V_back_porch);

        if(((rowNum + 1) % 2) == 0) {
            Send_chunk(cmdPANEL_TIMINGS, chunkNum++, chunk);
            pchunk = chunk; //ricomincio a riempire il buffer del chunk
        }
    }
    //ultimo chunk con padding
    memset(chunk, 0, 32);
    chunk[0] =  LO(m_dm->PanelTimingsTable[14].Width);
    chunk[1] =  HI(m_dm->PanelTimingsTable[14].Width);
    chunk[2] =  LO(m_dm->PanelTimingsTable[14].Height);
    chunk[3] =  HI(m_dm->PanelTimingsTable[14].Height);
    chunk[4] =  LO(m_dm->PanelTimingsTable[14].H_front_porch);
    chunk[5] =  HI(m_dm->PanelTimingsTable[14].H_front_porch);
    chunk[6] =  LO(m_dm->PanelTimingsTable[14].H_sync);
    chunk[7] =  HI(m_dm->PanelTimingsTable[14].H_sync);
    chunk[8] =  LO(m_dm->PanelTimingsTable[14].H_back_porch);
    chunk[9] =  HI(m_dm->PanelTimingsTable[14].H_back_porch);
    chunk[10] = LO(m_dm->PanelTimingsTable[14].V_front_porch);
    chunk[11] = HI(m_dm->PanelTimingsTable[14].V_front_porch);
    chunk[12] =    m_dm->PanelTimingsTable[14].V_sync;
    chunk[13] =    m_dm->PanelTimingsTable[14].clock_idx;
    chunk[14] = LO(m_dm->PanelTimingsTable[14].V_back_porch);
    chunk[15] = HI(m_dm->PanelTimingsTable[14].V_back_porch);

    Send_chunk(cmdPANEL_TIMINGS, chunkNum, chunk);
	
	ser_mutex.unlock();

    return 0;
}

//-----------------------------------------------------
int SerialProtocol::Cmd_TEMPER_SET()
{
	int retVal;
	
    PRINT_FUNC_NAME

	ser_mutex.lock();
	
    txbuff[0] = 0xAA;               //head
    txbuff[1] = 0x55;
    txbuff[2] = cmdTEMPER_SET;
    txbuff[3] = m_dm->Curr_TemperatureC;
    txbuff[4] = 0x33;               //tail
    txbuff[5] = 0xCC;

    retVal = Send_bytes(6);
	
	ser_mutex.unlock();
	
	return retVal;
}

//-----------------------------------------------------------
int SerialProtocol::Cmd_SEND_KEY(int keyCode, int dest_adr)
{
	int retVal;
	
    PRINT_FUNC_NAME

	ser_mutex.lock();
	
    txbuff[0] = 0xAA;     //head
    txbuff[1] = 0x55;
    txbuff[2] = cmdSEND_KEY;
    txbuff[3] = keyCode;
    txbuff[4] = dest_adr;
    txbuff[5] = 0x33;     //tail
    txbuff[6] = 0xCC;

    retVal = Send_bytes(7);
	
	ser_mutex.unlock();
	
	return retVal;
}

//-----------------------------------------------------
int SerialProtocol::Cmd_OSD_STATUS_SET(bool open)
{
	int retVal;
	
    PRINT_FUNC_NAME

	ser_mutex.lock();
	
    txbuff[0] = 0xAA;               //head
    txbuff[1] = 0x55;
    txbuff[2] = cmdOSD_STATUS_SET;
    txbuff[3] = open ? 1:0;
    txbuff[4] = 0x33;               //tail
    txbuff[5] = 0xCC;

    retVal = Send_bytes(6);
	
	ser_mutex.unlock();
	
	return retVal;
}

//-----------------------------------------------------
int SerialProtocol::Send_bytes(int len)
{
    #define DUMP_SERIAL
    #ifdef DUMP_SERIAL
    for(int i=0; i<len; i++)
        printf("txbuff[%d]: 0x%02X\n", i, txbuff[i]);
    fflush(stdout);

    #endif

    emit Postpone_Temperature_packet();

    if(serial_fd == -1) {
        qDebug("serial_fd errato, salto");
        return 0;
    }

    int rc = write(serial_fd, txbuff, len);
    if(rc < 0) {
        printf("write error: %s\n", strerror(errno));
        fflush(stdout);
    }
    return rc;
}

//-----------------------------------------------------
// Parsing comandi in arrivo
//-----------------------------------------------------
int SerialProtocol::parse_in_serial_frame()
{
    qDebug("comando: %d", frame_buff[0]);

    switch(frame_buff[0])
    {
        //----------------------------------------------
        case cmdPARAM_SET: {
            qDebug("-cmdPARAM_SET");
            fflush(stderr);

            int parID =  frame_buff[1];
            int vMax = frame_buff[3] * 256 + frame_buff[2];
            int val =  frame_buff[5] * 256 + frame_buff[4];

            //formula per adattare il range degli slider OSD al range del bus DDC-CI
            //si applica solo ai primi 3 parametri (per gli altri: VMAX=0):
            //Val_osd = (Max_osd*Val_busDDCCI + (Max_busDDCCI/2))/Max_busDDCCI
            if(vMax && parID <= osdBUZZER) {
                int scaled_val = val * MAX_PAR_VAL[parID] + vMax / 2;
                scaled_val /= vMax;
                val = scaled_val;
                qDebug("parID: %d val: %d vMax: %d scaled_val: %d",
                       parID, val, vMax, scaled_val);
            }

            switch(parID)
            {
                case osdBRIGHTNESS:
                    m_dm->setProperty("dimLevel", val);
                    break;

                case osdCONTRAST:
                    m_dm->setProperty("contrastLevel", val);
                    break;

                case osdBUZZER:
                    m_dm->setProperty("buzzerVol", val);
                    break;

                case osdPOWER_MODE:
                    //al momento non gestito
                    break;

                case osdSCALING_MODE:
                    m_dm->setProperty("scalingMode", val);
                    break;

                case osdLOGO_TIMEOUT:
                    qDebug("set osdLOGO_TIMEOUT=%d", val);
                    m_dm->setProperty("splashLogoToutIdx", val);
                    m_dm->SplashLogoToutIdxChanged(val);
                    break;

                case osdNOSIG_TOUT:
                    m_dm->setProperty("noSigToutIdx", val);
                    m_dm->NoSigToutIdxChanged(val);
                    break;

                case osdSTDBY_TOUT:
                    m_dm->setProperty("stdbyToutIdx", val);
                    m_dm->StdbyToutIdxChanged(val);
                    break;

                 case osdMIN_TEMP:
                    m_dm->setProperty("minTemperature", val);
                    break;

                case osdMAX_TEMP:
                    m_dm->setProperty("maxTemperature", val);
                    break;

                case osdMON_SER_ADR:
                    m_dm->setProperty("monSerialAdr", val);
                    break;

                case osdJ24_CFG:
                    m_dm->setProperty("j24config", val);
                    break;

                case osdLOCK_ON_SER:
                    m_dm->setProperty("lockOnSerial", val);
                    break;

                case osdMULTI_DIM:
                    m_dm->setProperty("mdimMaster", val);
                    break;

                case osdLOCK_PIN:
                    m_dm->setProperty("lockPIN", val);
                    break;
					
				case osdPOT_ENA_SER:
					m_dm->setProperty("dimming_pot_ena", val);
					break;

                 default:
                    //invalida il flag se parametro non valido
                    setPacketWaited = false;
                    break;
            }
            //fa apparire il popup in tab Load&save
            if(setPacketWaited && parID == osdBRIGHTNESS) {
                setPacketWaited = false; //one shot
                emit m_ql->settingsCmdSent();
            }
            break;
        }

        //---------------------------------------------
        case cmdSOURCE_SET:
        {
            qDebug("-set osdSOURCE");
            fflush(stderr);

            /*
            Byte0: Input mode
            Byte1: Current Input
            Byte2: DDC/CI mode
            Byte3: DDC/CI Input
            Byte4: priorità DVI-D1 (0xFF se disabilitato)
            Byte5: priorità DP1    (0xFF se disabilitato)
            Byte6: priorità DVI-D2 (0..1 se DVI-D1 e DP1 disabilitati)
            Byte7: priorità DP2    (0..1 se DVI-D1 e DP1 disabilitati)
            Byte8: priorità VGA1   (0xFF se disabilitato)
            Byte9: priorità VGA2   (0xFF se disabilitato)
            */
            m_dm->setProperty("inputMode",     frame_buff[1]);
            m_dm->setProperty("currentIN",     frame_buff[2]);
            m_dm->setProperty("ddc_ci_Mode",   frame_buff[3]);
            m_dm->setProperty("currentDDC_IN", frame_buff[4]);

            //nota: se ingressi non sono abilitati ricevo 0xFF
            //ma devo convertirli perché sono usati come indici
            //della tabella IN_TYPES[]
            if(frame_buff[9]  == 0xFF) frame_buff[9] =  4;
            if(frame_buff[10] == 0xFF) frame_buff[10] = 5;

            //se DVI-D1 e DP1 disabilitati (ho alcuni 0xFF)
            //per non stravolgere l'impostazione precedente dell'OSD in pagina inputs,
            //lascio le 6 priorità associate alle righe (riga 5 e 6 nascoste)
            //solo le righe 3 e 4 possono scambiarsi di posto, ad esse sono
            //assegnati i valori di priorità 0 e 1 nel pacchetto SOURCE_SET, ma
            //nell'OSD devo avere 0..5 quindi assegno in modo fisso 0 e 1 ai primi
            //due ingressi (DVI-D1 e DP1) che non verranno mai toccati (frecce non
            //appaiono); per le righe 3 e 4 (DVI-D2 e DP2) traslo i valori 0 e 1 in
            //2 e 3, che uso internamente nella logica della pagina Inputs.

            //se DVI-D1 e DP1 disabilitati i primi due bytes della serie delle
            //priorities sono a 0xFF
            if(frame_buff[5] == 0xFF) {
                frame_buff[5] = 0;
                frame_buff[6] = 1;
                frame_buff[7] += 2; //trasla 0..1 -> 2..3
                frame_buff[8] += 2; //trasla 0..1 -> 2..3
                frame_buff[9]  = 4;  //di fatto non usato
                frame_buff[10] = 5;  //di fatto non usato
            }

            m_dm->set_InputTable(frame_buff[5], frame_buff[6], frame_buff[7],
                                 frame_buff[8], frame_buff[9], frame_buff[10]);

            //m_dm->CurrentDDCINChanged(frame_buff[4]);

//            qDebug("-->inputMode: %d",      frame_buff[1]);
//            qDebug("-->ddc_ci_Mode: %d",    frame_buff[3]);
//            qDebug("-->currentIN: %d",      frame_buff[2]);
//            qDebug("-->currentDDC_IN: %d",  frame_buff[4]);
//            qDebug("-->InputTable: %d.%d.%d.%d.%d.%d",
//                   frame_buff[5], frame_buff[6], frame_buff[7],
//                   frame_buff[8], frame_buff[9], frame_buff[10]);

            qDebug("set osdSOURCE end");

            if(m_ql)  //ridondanza di sicurezza (non può essere null qui)
                emit m_ql->newSourceData();
            break;
        }
        //---------------------------------------------
        case cmdRUNTIME_SET: {
            qDebug("-cmdRUNTIME_SET");
            fflush(stderr);
            //minuti a 32 bit
            int val = (frame_buff[4]<<24) + (frame_buff[3]<<16) +
                      (frame_buff[2]<< 8) +  frame_buff[1];
            qDebug() << " + + osdRUN_TIME  val:" << val;
            m_dm->setProperty("totalRunTime", val);
            break;
        }

        //---------------------------------------------
        case cmdSEND_TABLES:
            qDebug("\n--------SEND_TABLES");

            Cmd_EDID_SET();
            Cmd_DIMMING_SET();
            Cmd_GAMMA_SET();
            Cmd_INFO_SET();
            Cmd_PANEL_TIMINGS_SET();
            break;

        //---------------------------------------------
        case cmdSEND_TEMPER:
            qDebug("-cmdSEND_TEMPER");
            fflush(stderr);
            Cmd_TEMPER_SET();
            break;

       //---------------------------------------------
        case cmdSER_LOCK:
            if(m_dm->property("lockOnSerial").toBool()) {
                qDebug("-cmdSER_LOCK");
                m_dm->setProperty("serLockActive", true);
                emit m_dm->closeOSD();
            }
            break;

        //---------------------------------------------
        case cmdINIT_END: {
            qDebug("-cmdINIT_END");
            fflush(stderr);
            //forza visualizzazione iniziale OSD se in INIT_END ho segnale assente
            int videoSignal = frame_buff[1] & 0x01;
            if(videoSignal == 0)
                m_dm->setProperty("doOpenOSD", true);
            else
                m_dm->setProperty("doOpenOSD", false);

            //nasconde le sorgenti DVI1 e DP1
            int in1_Disable = frame_buff[1] & 0x02;
            qDebug("frame_buff[1]: %d", frame_buff[1]);
            if(in1_Disable != 0)
                m_dm->setProperty("inputs1Disable", true);
            else
                m_dm->setProperty("inputs1Disable", false);

            int LPC_sw_rel[4];
            LPC_sw_rel[0] = frame_buff[2];		
            LPC_sw_rel[1] = frame_buff[3];		
            LPC_sw_rel[2] = frame_buff[4];		
            LPC_sw_rel[3] = frame_buff[5];		

            int FPGA_sw_rel[4];
            FPGA_sw_rel[0] = frame_buff[6];		
            FPGA_sw_rel[1] = frame_buff[7];		
            FPGA_sw_rel[2] = frame_buff[8];		
            FPGA_sw_rel[3] = frame_buff[9];		

            qDebug("LPC_sw_rel: %d.%d.%d.%d FPGA_sw_rel: %d.%d.%d.%d",
                   LPC_sw_rel[0],  LPC_sw_rel[1],  LPC_sw_rel[2],  LPC_sw_rel[3],
                   FPGA_sw_rel[0], FPGA_sw_rel[1], FPGA_sw_rel[2], FPGA_sw_rel[3]
                  );
			
			QString lpc_maj_ver = QString::number(((((unsigned int)LPC_sw_rel[0]) << 8) | LPC_sw_rel[1]));
			QString lpc_min_ver = QString::number(((((unsigned int)LPC_sw_rel[2]) << 8) | LPC_sw_rel[3]));
			QString fpga_maj_ver = QString::number(((((unsigned int)FPGA_sw_rel[0]) << 8) | FPGA_sw_rel[1]));
			QString fpga_min_ver = QString::number(((((unsigned int)FPGA_sw_rel[2]) << 8) | FPGA_sw_rel[3]));
			
			m_ql->getEngine()->rootContext()->setContextProperty("LPC_SW_REV_MAJ", lpc_maj_ver);
			m_ql->getEngine()->rootContext()->setContextProperty("LPC_SW_REV_MIN", lpc_min_ver);
			m_ql->getEngine()->rootContext()->setContextProperty("FPGA_SW_REV_MAJ", fpga_maj_ver);
			m_ql->getEngine()->rootContext()->setContextProperty("FPGA_SW_REV_MIN", fpga_min_ver);
			
			QString osd_str_ver = QString(SW_REL_MAJ) + "." + QString(SW_REL_MIN);
			QString lpc_str_ver = lpc_maj_ver + "." + lpc_min_ver;
			QString fpga_str_ver = fpga_maj_ver + "." + fpga_min_ver;
			
			m_dm->setProperty("OSD_SW_Ver_OK", (osd_str_ver.compare(m_fm->osd_ver) == 0));
			m_dm->setProperty("LPC_SW_Ver_OK", (lpc_str_ver.compare(m_fm->lpc_ver) == 0));
			m_dm->setProperty("FPGA_SW_Ver_OK", (fpga_str_ver.compare(m_fm->fpga_ver) == 0));
			
            //inizializza il motore grafico
            //e abilita il mouse per richiamare l'OSD a video
            //nota: va fatto oneshoot all'avvio (comando che arriva da LPC)
            if(initOSDGraphics == false) {
                initOSDGraphics = true;
                qDebug("emit m_ql->doStart()");

                //carica main.qml con property osdVisible: false
                if(m_ql) //ridondanza di sicurezza (non può essere null qui)
                    emit m_ql->doStart();
				m_fwUpgThread->start();
            }
            qDebug("-----> cmdINIT_END");
            break;
        }
        //---------------------------------------------
        default:
            return -1;
    }
    return 0;
}

//-----------------------------------------------------------------------
bool SerialProtocol::parseBuffer()
{
    //for(int i=0; i<25; i++) qDebug(" rxbuff %d: 0x%02X", i, rxbuff[i]);

#ifndef DEBUG_WINDOWS
    int numB = pHead - rxbuff; //available bytes
    char *pFrameEnd = 0;
    //search for the first occurrence of the header word
    char *pFrameStart = (char *)memmem(rxbuff, numB, "\xAA\x55", 2);
    if(pFrameStart == 0) {
        //qDebug("no header yet\n");
        return false;
    }

    //header found, look for command id to get the trailer offset

    //remaining bytes to search within (from header)
    int rem = numB - (pFrameStart - rxbuff);
    //qDebug("rem: %d", rem);

    //is cmd id available?
    if(rem < 3) {
        //qDebug("no cmdID yet\n");
        return false;
    }

    //look up the expected trailer offset
    int exp_off = TAB_OFFS[(int)pFrameStart[2]];
    //qDebug("look for trailer  - cmdID: %d rem: %d exp_off: %d",
          // pFrameStart[2], rem, exp_off);
    //enough bytes to check the trailer?
    if(rem < exp_off + 2) {
        //qDebug("no trailer yet\n");
       return false;
    }
    //is the trailer in place?
    else {
        pFrameEnd = &pFrameStart[exp_off];
        if(memcmp(pFrameEnd, "\x33\xCC", 2) != 0) {
            //qDebug("trailer position error, reset buffer\n");
            pHead = rxbuff;
            return false;
        }
    }

    //extract the frame body and submit to decoding
    pFrameStart += 2; //skip header bytes
    int frameSize = pFrameEnd - pFrameStart;
    memset(frame_buff, 0, sizeof(frame_buff));
    memcpy(frame_buff, pFrameStart, frameSize);

    //shift the remaining bytes to the beginning of the buffer
    pFrameEnd += 2; //skip trailer bytes
    int remB = rxbuff + numB - pFrameEnd;
    memcpy(rxbuff, pFrameEnd, remB);
    memset(rxbuff + remB, 0, sizeof(rxbuff) - remB); //azzero il rimanente
    pHead = rxbuff + remB;

                        //qDebug("\n*** Complete frame extracted ***");
                        //for(int i=0; i<frameSize; i++)
                        //    qDebug("    0x%02X", frame_buff[i]);
                        //qDebug(" ");

    parse_in_serial_frame();
#endif

    return true;
}

//-----------------------------------------------------
// Thread body
//-----------------------------------------------------
bool SerialProtocol::runLoop()
{
    PRINT_FUNC_NAME

    qDebug("Serial comm thread started");
    int ret = openSerialPort();
    if(ret < 0) {
        qDebug("Serial comm open error, terminate thread");
        return true;
    }
    ret = setSerialParams();
    if(ret < 0) {
        qDebug("Serial comm setting error, terminate thread");
        return true;
    }

    pHead = rxbuff; //start of the rx buffer

    //attesa ricezione caratteri
    while(1)
    {
        //inform LPC microcontroller the OSD process is ready
        if(m_dm->sendOSD_READY) {
            m_dm->sendOSD_READY = false; //one shoot
            Cmd_OSD_READY();
        }

        ssize_t rc = read(serial_fd, pHead, 25); //nota: 25 un numero qualunque
        //ricevuto dei caratteri
        if(rc > 0) {
            //qDebug() << "read rc:" << rc;
            //for(int i=0; i<rc; i++) qDebug("  %d: 0x%02X", i, pHead[i]);
            pHead += rc;

            while(parseBuffer());
        }
        else if(rc < 0) {
            qDebug("errore rx: %d", rc);
        }
    }
    return true;
}

/* EOF */





































