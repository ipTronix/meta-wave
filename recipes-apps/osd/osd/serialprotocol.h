/*****************************************************************************
*   Fie:		serialprotocol.cpp
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#ifndef SERIALPROTOCOL_H
#define SERIALPROTOCOL_H

#include <QObject>
#include <QMutex>
#include <QDebug>
#include <QTimer>
#include "parameters.h"
#include "temp_monitor.h"
#include "datamodel.h"
#include "qml_launcher.h"

class DataModel;
class TempMonitor;

enum {
    cmdPARAM_SET      = 0,
    cmdSOURCE_SET	  = 1,
    cmdRUNTIME_SET	  = 2,
    cmdCOMMAND		  = 3,
    cmdOSD_READY      = 4,
    cmdSEND_TABLES    = 5,
    cmdINIT_END       = 6,
    cmdSEND_TEMPER    = 7,
    cmdSER_LOCK       = 8,
    cmdSEND_KEY       = 9,
    cmdEEDID_SET_IN0  = 10, //DVI-D 1
    cmdEEDID_SET_IN1  = 11, //DP1
    cmdEEDID_SET_IN2  = 12, //DVI-D 2
    cmdEEDID_SET_IN3  = 13, //DP2
    cmdDIMMING_SET    = 20,
    cmdGAMMA_SET      = 30,
    cmdINFO_SET       = 40,
    cmdPANEL_TIMINGS  = 41,
    cmdTEMPER_SET     = 42,
    cmdOSD_STATUS_SET = 43
};

enum {
    _reserved_	        = 0,
    cmd_SETTINGS_SAVE	= 1,
    cmd_SETTINGS_RECALL = 2,
    cmd_FACTORY_RESET	= 3,
    cmd_RESET_COUNTERS	= 4,
    cmd_reboot 	        = 5, //nota: usato dall'utility bootloader
    cmd_BRT_UP 	        = 6,
    cmd_BRT_DWN 	    = 7,
    cmd_BRT_BROAD_UP    = 8,
    cmd_BRT_BROAD_DWN   = 9,
    cmd_SRC_CHAG 	    = 10
};

class QML_Launcher;

class SerialProtocol : public QObject
{
    Q_OBJECT

    int serial_fd;
    QTimer *tempTimer;
    DataModel    *m_dm;
    TempMonitor  *m_tm;
    QML_Launcher *m_ql;

    char txbuff[256];

    //scratch area where raw bytes from serial inpunt are accumulated.
    //On every read() cycle, additional data (if available) is appended, then the
    //parser is called to search for valid and complete frames.
    //Frames are searched scanning the header and trailer
    //Packets found are cyclically extracted and the remaining incomplete
    //frame is then shifted to start from buffer's start. No ring buffering is
    //required since all data is processed right after each read() call
    char rxbuff[256];

    char frame_buff[256];   //contiene il frame estratto da inviare al processing
    char *pHead;
    QMutex ser_mutex;
    bool initOSDGraphics;

    bool openSerialPort();
    bool setSerialParams();
    bool parseBuffer();
    int Send_chunk(int cmdID, int chunkNum, unsigned char *chunk);
    int Send_bytes(int len);

public:
    explicit SerialProtocol(QObject *parent = 0);
    void RegisterDm(DataModel *dmodel) { m_dm = dmodel; }
    void RegisterTm(TempMonitor *tmon) { m_tm = tmon; }
    void RegisterQMLlauncher(QML_Launcher *ql) { m_ql = ql; }
    bool setPacketWaited;

signals:
    void Postpone_Temperature_packet();

public slots:
    bool runLoop();
    int parse_in_serial_frame();

    int Cmd_PARAM_SET(enum e_params parID, int par_val);
    int Cmd_TEMPERATURE_SET(int temp);
    int Cmd_SOURCE_SET(int inMode,  int currIn,
                       int ddcMode, int currDDC,
                       QVariantList inIndex, bool inDisabled);

    int Cmd_COMMAND(int cmdID);
    int Cmd_OSD_READY();
    int Cmd_EDID_SET();
    int Cmd_DIMMING_SET();
    int Cmd_GAMMA_SET();
    int Cmd_INFO_SET();
    int Cmd_PANEL_TIMINGS_SET();
    int Cmd_TEMPER_SET();
    int Cmd_SEND_KEY(int keyCode, int dest_adr);
    int Cmd_OSD_STATUS_SET(bool open);
};

#endif // SERIALPROTOCOL_H










































