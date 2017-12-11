/*****************************************************************************
*   Fie:		datamodel.h
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QObject>
#include <QDebug>
#include "global.h"
#include "serialprotocol.h"
#include "filemanager.h"
#include "parameters.h"

class FileManager;
class SerialProtocol;

class DataModel : public QObject
{
    Q_OBJECT

    //obsoleto bool m_osd_enable;
    bool m_logo_present;
    bool m_doOpenOSD;
    bool m_inputs1Disable; //Disabilita DVI1 e DP1. Nel tab “inputs” sono in grigietto
    int m_DimLevel;
    int m_ContrastLevel;
    int m_BuzzerVol;
    int m_ScalingMode;
    int m_InputMode;
    int m_CurrentIN;
    int m_CurrentDDC_IN;
    int m_DDC_CI_Mode;
    int m_SplashLogoToutIdx;
    int m_StdbyToutIdx;
    int m_NoSigToutIdx;
    int m_TotalRunTime;
    int m_MaxTemperature;
    int m_MinTemperature;

    bool m_lockOnSerial;
    bool m_mdimMaster;
    bool m_keyRelayOn;
    bool m_serLockActive;
    int m_monSerialAdr;
    int m_remoteSerialAdr;
    int m_j24config;
    int m_lockPIN;

    QString m_MonitorSerialN;
    QString m_SoftwareID;

    //Position of inputs in the rows.
    //Each input has an associated costant index, used to retrieve the input's name
    QVariantList m_inputIndex;

    //current active input's relative position
    QVariantList m_inputStates;

    QVariantList m_inputTypes;
    QVariantList m_ddcCiStates;
    QVariantList m_anaEDIDStates;

    bool m_eth_enabled;
    bool m_dhcp_on;
    bool m_scom_srv;

    //nota gli IP sono rappresentati con 4 interi separati
    //perché in QML non si riesce ad editare oggetti aggregati
    int m_ipAddr0;
    int m_ipAddr1;
    int m_ipAddr2;
    int m_ipAddr3;

    int m_netMask0;
    int m_netMask1;
    int m_netMask2;
    int m_netMask3;

    int m_defGW0;
    int m_defGW1;
    int m_defGW2;
    int m_defGW3;

    int m_dns0;
    int m_dns1;
    int m_dns2;
    int m_dns3;

    int m_scomSrvIP0;
    int m_scomSrvIP1;
    int m_scomSrvIP2;
    int m_scomSrvIP3;

    FileManager *m_fm;
    SerialProtocol *m_sp;

    void CRC_AddByte(unsigned char data);

public:
    explicit DataModel(QObject *parent = 0);

    void RegisterFm(FileManager *fmanager) { m_fm = fmanager; }
    void RegisterSp(SerialProtocol *serialprot);

    //obsoleto Q_PROPERTY(bool osd_enable          MEMBER m_osd_enable         NOTIFY osd_enableChanged)
    Q_PROPERTY(bool logo_present        MEMBER m_logo_present       NOTIFY logo_presentChanged)
    Q_PROPERTY(int dimLevel             MEMBER m_DimLevel           NOTIFY DimLevelChanged)
    Q_PROPERTY(int contrastLevel        MEMBER m_ContrastLevel      NOTIFY ContrastLevelChanged)
    Q_PROPERTY(int buzzerVol            MEMBER m_BuzzerVol          NOTIFY BuzzerVolChanged)
    Q_PROPERTY(int scalingMode          MEMBER m_ScalingMode        NOTIFY ScalingModeChanged)
    Q_PROPERTY(int inputMode            MEMBER m_InputMode          NOTIFY InputModeChanged)
    Q_PROPERTY(int currentIN            MEMBER m_CurrentIN          NOTIFY CurrentINChanged)
    Q_PROPERTY(int currentDDC_IN        MEMBER m_CurrentDDC_IN      NOTIFY CurrentDDCINChanged)
    Q_PROPERTY(int ddc_ci_Mode          MEMBER m_DDC_CI_Mode        NOTIFY DDC_CI_ModeChanged)
    Q_PROPERTY(int splashLogoToutIdx    MEMBER m_SplashLogoToutIdx  NOTIFY SplashLogoToutIdxChanged)
    Q_PROPERTY(int noSigToutIdx         MEMBER m_NoSigToutIdx       NOTIFY NoSigToutIdxChanged)
    Q_PROPERTY(int stdbyToutIdx         MEMBER m_StdbyToutIdx		NOTIFY StdbyToutIdxChanged)
    Q_PROPERTY(QString monitorSerialN   MEMBER m_MonitorSerialN     NOTIFY MonitorSerialNChanged)
    Q_PROPERTY(QString osd_softwareID   MEMBER m_SoftwareID         NOTIFY SoftwareIDChanged)
    Q_PROPERTY(int totalRunTime         MEMBER m_TotalRunTime       NOTIFY TotalRunTimeChanged)
    Q_PROPERTY(int maxTemperature       MEMBER m_MaxTemperature     NOTIFY MaxTemperatureChanged)
    Q_PROPERTY(int minTemperature       MEMBER m_MinTemperature     NOTIFY MinTemperatureChanged)

    Q_PROPERTY(QVariantList input_Index    READ input_Index    WRITE setinput_Index    NOTIFY inputIndexChanged)
    Q_PROPERTY(QVariantList input_States   READ input_States   WRITE setinput_States   NOTIFY inputStatesChanged)
    Q_PROPERTY(QVariantList input_Types    READ input_Types    WRITE setinput_Types    NOTIFY inputTypesChanged)
    Q_PROPERTY(QVariantList ddcCi_States   READ ddcCi_States   WRITE setddcCi_States   NOTIFY ddcCiStatesChanged)
    Q_PROPERTY(QVariantList anaEDID_States READ anaEDID_States WRITE setanaEDID_States NOTIFY anaEDIDStatesChanged)

    //forza visualizzazione iniziale OSD se in INIT_END ho segnale assente
    Q_PROPERTY(bool doOpenOSD MEMBER m_doOpenOSD  NOTIFY doOpenOSDChanged)
    //non sono presenti sulla scheda gli ingressi DVI1 e DP1, vanno rimossi dalla tab Inputs
    Q_PROPERTY(bool inputs1Disable MEMBER m_inputs1Disable  NOTIFY inputs1DisableChanged)

    Q_PROPERTY(int monSerialAdr         MEMBER m_monSerialAdr       NOTIFY monSerialAdrChanged)
    Q_PROPERTY(int remoteSerialAdr      MEMBER m_remoteSerialAdr    NOTIFY remoteSerialAdrChanged)
    Q_PROPERTY(int j24config            MEMBER m_j24config          NOTIFY j24configChanged)
    Q_PROPERTY(bool lockOnSerial        MEMBER m_lockOnSerial       NOTIFY lockOnSerialChanged)
    Q_PROPERTY(int lockPIN              MEMBER m_lockPIN            NOTIFY lockPINChanged)
    Q_PROPERTY(bool mdimMaster          MEMBER m_mdimMaster         NOTIFY mdimMasterChanged)
    Q_PROPERTY(bool keyRelayOn          MEMBER m_keyRelayOn         NOTIFY keyRelayOnChanged)
    Q_PROPERTY(bool serLockActive       MEMBER m_serLockActive      NOTIFY serLockActiveChanged)

    Q_PROPERTY(bool eth_enabled        MEMBER m_eth_enabled        NOTIFY eth_enabled_Changed)
    Q_PROPERTY(bool dhcp_on            MEMBER m_dhcp_on            NOTIFY dhcp_onChanged)
    Q_PROPERTY(bool scom_srv           MEMBER m_scom_srv           NOTIFY scom_srvChanged)

    Q_PROPERTY(int ipAddr0             MEMBER m_ipAddr0            NOTIFY ipAddr0Changed)
    Q_PROPERTY(int ipAddr1             MEMBER m_ipAddr1            NOTIFY ipAddr1Changed)
    Q_PROPERTY(int ipAddr2             MEMBER m_ipAddr2            NOTIFY ipAddr2Changed)
    Q_PROPERTY(int ipAddr3             MEMBER m_ipAddr3            NOTIFY ipAddr3Changed)

    Q_PROPERTY(int netMask0            MEMBER m_netMask0           NOTIFY netMask0Changed)
    Q_PROPERTY(int netMask1            MEMBER m_netMask1           NOTIFY netMask1Changed)
    Q_PROPERTY(int netMask2            MEMBER m_netMask2           NOTIFY netMask2Changed)
    Q_PROPERTY(int netMask3            MEMBER m_netMask3           NOTIFY netMask3Changed)

    Q_PROPERTY(int defGW0              MEMBER m_defGW0             NOTIFY defGW0Changed)
    Q_PROPERTY(int defGW1              MEMBER m_defGW1             NOTIFY defGW1Changed)
    Q_PROPERTY(int defGW2              MEMBER m_defGW2             NOTIFY defGW2Changed)
    Q_PROPERTY(int defGW3              MEMBER m_defGW3             NOTIFY defGW3Changed)

    Q_PROPERTY(int dns0                MEMBER m_dns0               NOTIFY dns0Changed)
    Q_PROPERTY(int dns1                MEMBER m_dns1               NOTIFY dns1Changed)
    Q_PROPERTY(int dns2                MEMBER m_dns2               NOTIFY dns2Changed)
    Q_PROPERTY(int dns3                MEMBER m_dns3               NOTIFY dns3Changed)

    Q_PROPERTY(int scomSrvIP0          MEMBER m_scomSrvIP0         NOTIFY scomSrvIP0Changed)
    Q_PROPERTY(int scomSrvIP1          MEMBER m_scomSrvIP1         NOTIFY scomSrvIP1Changed)
    Q_PROPERTY(int scomSrvIP2          MEMBER m_scomSrvIP2         NOTIFY scomSrvIP2Changed)
    Q_PROPERTY(int scomSrvIP3          MEMBER m_scomSrvIP3         NOTIFY scomSrvIP3Changed)


    QVariantList getPrioList() { return m_inputIndex; }

    QVariantList m_osd_dimming_table;

    static const int DEFAULT_EDID[128];

    QList<int> EDID_table_DVID1;
    QList<int> EDID_table_DP1;
    QList<int> EDID_table_DVID2;
    QList<int> EDID_table_DP2;

    struct s_rgb {
        int redVal;
        int greenVal;
        int blueVal;
    };
    struct s_rgb GammaTable[256];

    struct s_dimm {
        int wPeriod;
        int wONTime;
        int cDAC;
        int cLED;
        int cOSD;
    };
    static const struct s_dimm DEFAULT_DIMMING_TABLE[101];
    struct s_dimm DimmingTable[101];
    Q_PROPERTY(QVariantList osd_dimming_table READ osd_dimming_table WRITE setosd_dimming_table NOTIFY osd_dimming_tableChanged)

    struct s_timings {
        int Width;
        int Height;
        int H_front_porch;
        int H_sync;
        int H_back_porch;
        int V_front_porch;
        int V_sync;
        int clock_idx;
        int V_back_porch;
    };
    struct s_timings PanelTimingsTable[15];
    static const struct s_timings DEFAULT_PANEL_T_TABLE[15];

    static const QString IN_TYPES[6];

    //usati per computo CRC ed invio sulla seriale
    QByteArray array_MonitorSerial;
    QByteArray array_SoftwareID;
    QByteArray array_manufacturer_ID;
    QByteArray array_monitor_type;

    unsigned short tabs_crc16;
    unsigned short ComputeTablesCRC();
    bool sendOSD_READY;
    int Curr_TemperatureC;

signals:
    //obsoleto void osd_enableChanged(bool);
    void logo_presentChanged(bool);
    void doOpenOSDChanged(bool);
    void inputs1DisableChanged(bool);
    void DimLevelChanged(int);
    void ContrastLevelChanged(int);
    void BuzzerVolChanged(int);
    void ScalingModeChanged(int);
    void InputModeChanged(int);
    void CurrentINChanged(int);
    void CurrentDDCINChanged(int);
    void DDC_CI_ModeChanged(int);
    void SplashLogoToutIdxChanged(int);
    void NoSigToutIdxChanged(int);
    void StdbyToutIdxChanged(int);
    void MonitorSerialNChanged(QString);
    void SoftwareIDChanged(QString);
    void TotalRunTimeChanged(int);
    void MaxTemperatureChanged(int);
    void MinTemperatureChanged(int);
    void inputIndexChanged(QVariantList);
    void inputStatesChanged(QVariantList);
    void inputTypesChanged(QVariantList);
    void ddcCiStatesChanged(QVariantList);
    void anaEDIDStatesChanged(QVariantList);
    void osd_dimming_tableChanged(QVariantList);

    void lockOnSerialChanged(bool);
    void mdimMasterChanged(bool);
    void keyRelayOnChanged(bool);
    void serLockActiveChanged(bool);
    void monSerialAdrChanged(int);
    void remoteSerialAdrChanged(int);
    void j24configChanged(int);
    void lockPINChanged(int);

    void ipAddr0Changed(int);
    void ipAddr1Changed(int);
    void ipAddr2Changed(int);
    void ipAddr3Changed(int);

    void netMask0Changed(int);
    void netMask1Changed(int);
    void netMask2Changed(int);
    void netMask3Changed(int);

    void defGW0Changed(int);
    void defGW1Changed(int);
    void defGW2Changed(int);
    void defGW3Changed(int);

    void dns0Changed(int);
    void dns1Changed(int);
    void dns2Changed(int);
    void dns3Changed(int);

    void scomSrvIP0Changed(int);
    void scomSrvIP1Changed(int);
    void scomSrvIP2Changed(int);
    void scomSrvIP3Changed(int);

    void eth_enabled_Changed(bool);
    void dhcp_onChanged(bool);
    void scom_srvChanged(bool);

    void closeOSD();

public slots:
    void loadDefaultSettings();

    QVariantList input_Index() { return m_inputIndex; }
    void setinput_Index(QVariantList lista) {
        m_inputIndex = lista;
        inputIndexChanged(m_inputIndex);
    }
    void setinput_Index_at(int pos, int val) {
        if(pos < m_inputIndex.length()) {
            m_inputIndex.replace(pos, val);
            inputIndexChanged(m_inputIndex);
        }
    }

    QVariantList input_States() { return m_inputStates; }
    void setinput_States(QVariantList lista) {
        m_inputStates = lista;
        inputStatesChanged(m_inputStates);
    }

    void setinput_States_at(int pos, int val) {
        if(pos < m_inputStates.length()) {
            m_inputStates.replace(pos, val);
            inputStatesChanged(m_inputStates);
        }
    }

    QVariantList input_Types() { return m_inputTypes; }
    void setinput_Types(QVariantList lista) {
        m_inputTypes = lista;
        inputTypesChanged(m_inputTypes);
    }

    QVariantList ddcCi_States() { return m_ddcCiStates; }
    void setddcCi_States(QVariantList lista) {
        m_ddcCiStates = lista;
        ddcCiStatesChanged(m_ddcCiStates);
    }

    void setddcCi_States_at(int pos, int val) {
        if(pos < m_ddcCiStates.length()) {
            m_ddcCiStates.replace(pos, val);
            ddcCiStatesChanged(m_ddcCiStates);
        }
    }

    QVariantList anaEDID_States() { return m_anaEDIDStates; }
    void setanaEDID_States(QVariantList lista) {
        m_anaEDIDStates = lista;
        anaEDIDStatesChanged(m_anaEDIDStates);
    }

    QVariantList osd_dimming_table() { return m_osd_dimming_table; }
    void setosd_dimming_table(QVariantList table) {
        m_osd_dimming_table = table;
        osd_dimming_tableChanged(m_osd_dimming_table);
    }

    void set_InputTable(int p0, int p1, int p2, int p3, int p4, int p5);

    int getTemperature() { return Curr_TemperatureC; }

    void doKeyRelay(bool xx) {
        qDebug() << "doKeyRelay" << xx;
    }

    //trigger serial command to MCU
    void updateSetting(int parID, int par_val);
    void updateSourceSettings();
    void doSettingsSave();
    void doSettingsRecall();
    void doFactoryReset();
    void doResetCounters();
    void doSendKey(int key, int dest_adr);
    void doOSDstatus(bool open);
    void invalidateWaitSet();
    void doBrightUP();
    void doBrightDOWN();
    void doBrightBroadUP();
    void doBrightBroadDOWN();
    void doSourceChange();

    void setDefaultNetwParams();
    bool saveNetwParams();
    void startDHCPclient();
    bool isNetworkUp();
    bool isDHCP_leased();
    void stopDHCPclient();
    void loadNetwParams();
    bool getDHCPresults();
};

#endif // DATAMODEL_H
























































