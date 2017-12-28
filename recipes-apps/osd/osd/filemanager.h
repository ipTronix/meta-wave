/*****************************************************************************
*   Fie:		filemanager.cpp
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QDebug>
#include "global.h"
#include "datamodel.h"

#ifdef DEBUG_IMX_LORENZO
#define CONFIG_PATH     "/opt/osd/config_files/"
#define LOGO_FILE_PATH  "/opt/osd/config_files/"
#else
//punto di montaggio della SDCard
#define CONFIG_PATH     "/mnt/sd/fileset/"
#define LOGO_FILE_PATH  "/mnt/sd/"
#endif

//EDID files sulla SDCard
#define EDID_FILE0    CONFIG_PATH "edid-dvi-d1.txt"
#define EDID_FILE1    CONFIG_PATH "edid-dp1.txt"
#define EDID_FILE2    CONFIG_PATH "edid-dvi-d2.txt"
#define EDID_FILE3    CONFIG_PATH "edid-dp2.txt"
#define EDID_FILE4    CONFIG_PATH "edid-dvi-a1.txt"
#define EDID_FILE5    CONFIG_PATH "edid-dvi-a2.txt"
#define DIMMING_FILE  CONFIG_PATH "dimming.txt"
#define GAMMA_FILE    CONFIG_PATH "gamma.txt"
#define TIMINGS_FILE  CONFIG_PATH "paneltimings.txt"
//#define OSD_CFG_FILE  CONFIG_PATH "wave_cfg.txt"

#ifndef DEBUG_WINDOWS
# define NET_PARAM_FILE "/opt/osd/net_params.bin"
#else
# define NET_PARAM_FILE "C:/LAVORO/ipTRONIX/OSD_SAM_Scaler_software/net_params.bin"
#endif


//file temporaneo segnaposto creato quando udhcpc ha ottenuto il lease
#define DHCP_LEASE_OK_FILE "/var/volatile/dhcp_resp"


class DataModel;

class FileManager : public QObject
{
    Q_OBJECT

    DataModel *m_dm;

public:
    explicit FileManager(QObject *parent = 0);
    void LoadFiles();
    void RegisterDm(DataModel *dmodel) { m_dm = dmodel; }

signals:

public slots:
    bool LoadMonitorinfoFile();
    bool loadGammaFile();
    bool loadDimmingFile();
    bool loadEDIDFile(QString filename, QList<int> &table);
    void loadEDIDFiles();
    bool loadPanelTimingsFile();
    bool loadOSDConfigFile();
    void checkLogoIsPresent();
};

#endif // FILEMANAGER_H
