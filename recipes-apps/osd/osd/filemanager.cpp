/*****************************************************************************
*   Fie:		filemanager.cpp
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#include "filemanager.h"
#include "global.h"
#include <QDir>
#include <QFile>

//------------------------------------------------------
FileManager::FileManager(QObject *parent) : QObject(parent)
{
    PRINT_FUNC_NAME

    m_dm = 0;
}

//------------------------------------------------------
void FileManager::LoadFiles()
{
    PRINT_FUNC_NAME

    loadEDIDFiles();
    loadGammaFile();
    loadDimmingFile();
    LoadMonitorinfoFile();
    loadPanelTimingsFile();
    loadOSDConfigFile();

    //data loaded, send OSD_READY
    m_dm->sendOSD_READY = true;
}

//------------------------------------------------------
/* esempio:
#Monitor identification data
SOFTWARE_ID=1.2.3.4
MONITOR_SERIAL=B270000084
MANUFACTURER_ID=NIN
MONITOR_TYPE=EXAMPLE
*/

bool FileManager::LoadMonitorinfoFile()
{
    PRINT_FUNC_NAME
    qDebug(" ");

    QString line;
    QStringList buff;
    QByteArray temp;

    QFile file(CONFIG_PATH "monitorinfo.txt");

    if(file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);

        while(!in.atEnd())
        {
            line = in.readLine();

            //salto righe di commento e righe vuote
            if(line.startsWith("#") || line == "") {
                continue;
            }
            //separate parameter name from value
            buff = line.split("=", QString::KeepEmptyParts, Qt::CaseInsensitive);
            //devo avere 2 campi, altrimenti riga non valida
            if(buff.count() != 2) {
                //qDebug("riga non valida");
                continue;
            }

            //-------------------------------------------------
            if(buff[0] == "MONITOR_SERIAL")
            {
                //qDebug("imposto MONITOR_SERIAL");
                if(m_dm) {
                    QString ser = buff[1];
                    //tutto maiuscole - serve per prima lettera
                    ser = ser.toUpper();
                    //stringa ASCII originale visualizzata nel tab status
                    m_dm->setProperty("monitorSerialN", ser);

                    if(ser.length() != 10) {
                        qDebug("Errore lunghezza stringa seriale monitor");
                        //default 0000
                        temp = QByteArray(4, '\0');
                    }
                    //stringa valida
                    else {
                        qDebug() << "seriale monitor: " << ser;
                        //converto il primo carattere nel relativo indice (A=1, B=2)
                        temp = ser.toLatin1().constData();
                        char firstLetter = temp.at(0) - 'A' + 1;

                        //ricompongo la stringa per ottenere un seriale in forma numerica
                        char numBuff[11];
                        numBuff[0] = '0' + firstLetter; //primo numero in ASCII
                        for(int i=1; i<10; i++)
                            numBuff[i] = temp.at(i);
                        numBuff[10] = 0; //terminatore della stringa
                        unsigned long int val  = strtoul(numBuff, NULL, 0);
                        qDebug() << "numBuff: " << numBuff << "val: " << val;

                        unsigned char serBuff[4];
                        serBuff[0] = (unsigned char)((val & 0xFF000000) >> 24);
                        serBuff[1] = (unsigned char)((val & 0x00FF0000) >> 16);
                        serBuff[2] = (unsigned char)((val & 0x0000FF00) >> 8);
                        serBuff[3] = (unsigned char)( val & 0x000000FF);

//printf("serBuff hex: %02x %02x %02x %02x"
//       ,serBuff[0]
//       ,serBuff[1]
//       ,serBuff[2]
//       ,serBuff[3]);
//fflush(stdout);

                        //qDebug() << "Stringa seriale monitor: " << temp;
                        //vettore di 4 bytes per DDC/CI
                        temp.clear();
                        temp = QByteArray::fromRawData((const char*)serBuff, 4);
                    }
                    m_dm->array_MonitorSerial.clear();
                    m_dm->array_MonitorSerial.append(temp.at(0));
                    m_dm->array_MonitorSerial.append(temp.at(1));
                    m_dm->array_MonitorSerial.append(temp.at(2));
                    m_dm->array_MonitorSerial.append(temp.at(3));
                    //m_dm->array_MonitorSerial = temp; //non funziona, sposta il puntatore
                }
            }
            //-------------------------------------------------
            else if(buff[0] == "SOFTWARE_ID")
            {
                qDebug("imposto SOFTWARE_ID");
                if(m_dm) {
                    //valore originale visibile nel tab status
                    m_dm->setProperty("osd_softwareID", buff[1]);

                    //estraggo le 4 cifre
                    QStringList numbers = buff[1].split(".",
                                                        QString::KeepEmptyParts,
                                                        Qt::CaseInsensitive);

                    //vettore di 4 bytes che sarà inviato con DDC-CI
                    temp.clear();

                    //devo avere 2 campi, altrimenti riga non valida
                    if(numbers.count() != 4) {
                        qDebug("campo versione sw non valido");
                        //default 0000
                        temp = QByteArray(4, '\0');
                    }
                    else {
                        for(int i=0; i<4; i++)
                        {
                            int num = numbers[i].toInt();
                            if(num > 255) {
                                qDebug("campo versione sw out of range");
                                temp = QByteArray(4, '\0');
                                break;
                            }
                            temp.append((unsigned char)numbers[i].toInt());
                        }
                    }
                    m_dm->array_SoftwareID = temp;

                    qDebug("SW VER: %d %d %d %d"
                           ,m_dm->array_SoftwareID.at(0)
                           ,m_dm->array_SoftwareID.at(1)
                           ,m_dm->array_SoftwareID.at(2)
                           ,m_dm->array_SoftwareID.at(3));
                }
            }
            //-------------------------------------------------
            else if(buff[0] == "MANUFACTURER_ID")
            {
                QString manID = buff[1];

                //copio max 16 caratteri
                m_dm->array_manufacturer_ID = manID.left(16).toLocal8Bit();

                int len = manID.length();
                //padding con \0
                if(len < 16) {
                    m_dm->array_manufacturer_ID.resize(16);
                    for(int i=len; i<16; i++)
                        m_dm->array_manufacturer_ID[i] = 0;
                }
                qDebug() << "array_manufacturer_ID:" << m_dm->array_manufacturer_ID.constData();
            }
            //-------------------------------------------------
            else if(buff[0] == "MONITOR_TYPE")
            {
                QString monType = buff[1];

                //copio max 16 caratteri
                m_dm->array_monitor_type = monType.left(16).toLocal8Bit();

                int len = monType.length();
                //padding con \0
                if(len < 16) {
                    m_dm->array_monitor_type.resize(16);
                    for(int i=len; i<16; i++)
                        m_dm->array_monitor_type[i] = 0;
                }
                qDebug() << "array_monitor_type:" << m_dm->array_monitor_type.constData();
            }
        }
        return true;
    }
    //file monitorinfo.txt not found
    qDebug() << "File open error:" << file.errorString();
#if 0
    //default: tutti 0
    temp.clear();
    temp = QByteArray(4, '\0');
    m_dm->array_MonitorSerial = temp;
    m_dm->array_SoftwareID =    temp;

    //LOLLo gestire anche il caso di parametri non presenti nel file
    //LOLLO default per manufacturer_ID e monitor_type
    temp.clear();
    temp = QByteArray(8, '0'); //caratteri 'O'
    m_dm->array_manufacturer_ID = temp;
    m_dm->array_monitor_type = temp;
#endif //0
    return false;
}

//------------------------------------------------------
bool FileManager::loadGammaFile()
{
    qDebug() << COL_YELLOW "FileManager::LoadGammaFile:" CONFIG_PATH "gamma.txt"
             << COL_NORMAL;

//    //debug temporary
//    struct s_temp {
//        int redVal;
//        int greenVal;
//        int blueVal;
//    };
//    struct s_temp GammaTable[256];
//    memset(GammaTable, 0, 256 * sizeof(struct s_temp));

    QStringList buff;

    QFile file(GAMMA_FILE);
    if(file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);
        QString line;

        while(!in.atEnd())
        {
            line = in.readLine();

            //salto righe di commento e righe vuote
            if(line.startsWith("#") || line == "")
                continue;

            //changes all whitespaces to single instance of ASCII 32
            line = line.simplified();
            //remove spaces and tabs
            line.replace(" ", "" ).replace('\t', "" );

            //extract all fields
            buff = line.split(";", QString::KeepEmptyParts, Qt::CaseInsensitive);
            //devo avere 4 campi, altrimenti riga non valida
            if(buff.count() != 4) {
                qDebug("riga non valida");
                continue;
            }
//            qDebug() << "trovato:" << buff[0] << buff[1]
//                     << buff[2] << buff[3];

            //converto da ASCII a int
            int idx = buff[0].toInt();
            //qDebug() << "inpIdx:" << idx;

            //sanity check
            if(idx <= 255) {
                m_dm->GammaTable[idx].redVal =   buff[1].toInt();
                m_dm->GammaTable[idx].greenVal = buff[2].toInt();
                m_dm->GammaTable[idx].blueVal =  buff[3].toInt();

//                GammaTable[idx].redVal =   buff[1].toInt();
//                GammaTable[idx].greenVal = buff[2].toInt();
//                GammaTable[idx].blueVal =  buff[3].toInt();
            }
        }

//debug dump
//for(int i=0; i<256; i++)
//qDebug() << GammaTable[i].redVal
//             << GammaTable[i].greenVal
//             << GammaTable[i].blueVal;

        return true;
    }
    qDebug() << "File open error:" << file.errorString();
    return false;
}

//------------------------------------------------------
bool FileManager::loadDimmingFile()
{
    PRINT_FUNC_NAME

//    //debug temporary
//    struct s_temp {
//        int on_time;
//        int duty_cycle;
//        int dac;
//        int led;
//        int osd;
//    };
//    struct s_temp DimmingTable[100];
//    memset(DimmingTable, 0, 100 * sizeof(struct s_temp));

    QStringList buff;

    QFile file(DIMMING_FILE);
    if(file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);
        QString line;

        //ora posso sovrascrivere i defaults con i valori nel file (colonna OSD)
        m_dm->m_osd_dimming_table.clear();

        while(!in.atEnd())
        {
            line = in.readLine();

            //salto righe di commento e righe vuote
            if(line.startsWith("#") || line == "")
                continue;

            //changes all whitespaces to single instance of ASCII 32
            line = line.simplified();
            //remove spaces and tabs
            line.replace(" ", "" ).replace('\t', "" );

            //extract all fields
            buff = line.split(";", QString::KeepEmptyParts, Qt::CaseInsensitive);
            //devo avere 6 campi, altrimenti riga non valida
            if(buff.count() != 7) {
                qDebug("riga dimming non valida - ho %d campi", buff.count());
                continue;
            }
//            qDebug() << "trovato:"
//                     << buff[0] << buff[1]
//                     << buff[2] << buff[3]
//                     << buff[4] << buff[5];

            //converto da ASCII a int
            int idx = buff[0].toInt();

            //sanity check
            if(idx < 101) {
                m_dm->DimmingTable[idx].wPeriod =   buff[1].toInt();
                m_dm->DimmingTable[idx].wONTime =   buff[2].toInt();
                m_dm->DimmingTable[idx].cDAC =      buff[3].toInt();
                m_dm->DimmingTable[idx].cLED =      buff[4].toInt();
                m_dm->DimmingTable[idx].cOSD =      buff[5].toInt();
                //per cifra slider OSD
                m_dm->m_osd_dimming_table << buff[5].toInt();
            }
        }

        //debug dump
        //for(int i=0; i<100; i++)
        //qDebug() << DimmingTable[i].on_time << DimmingTable[i].duty_cycle
        //         << DimmingTable[i].dac     << DimmingTable[i].led << DimmingTable[i].osd;

        return true;
    }
    qDebug() << "File open error:" << file.errorString();
    return false;
}

//----------------------------------------------------------------
//carico le 4 tabelle, oppure lascio i defaults
//note: called at startup. EDID tables are already
//initialized with default data in DataModel::loadDefaultSettings()
//so DataModel class must be instantiated before FileManager class
//----------------------------------------------------------------
void FileManager::loadEDIDFiles()
{
    PRINT_FUNC_NAME
    loadEDIDFile(EDID_FILE0, m_dm->EDID_table_DVID1);
    loadEDIDFile(EDID_FILE1, m_dm->EDID_table_DP1);
    loadEDIDFile(EDID_FILE2, m_dm->EDID_table_DVID2);
    loadEDIDFile(EDID_FILE3, m_dm->EDID_table_DP2);
}

//------------------------------------------------------
//carica un file EDID nel formato dell'utility "Phoenix EDID designer tool"
//------------------------------------------------------
bool FileManager::loadEDIDFile(QString filename, QList<int>& table)
{
    PRINT_FUNC_NAME

    qDebug() << "loadEDIDFile filename" << filename;
    QStringList buff;
    QFile file(filename);
    if(file.open(QFile::ReadOnly))
    {
        table.clear();

        QTextStream in(&file);
        QString line;

        while(!in.atEnd())
        {
            line = in.readLine();

            //salto righe di commento e righe vuote
            //prendo solo le righe con il "|" che sono quelle dei dati
            if(!line.contains("|"))
                continue;

            line.replace("|", "" );
            line = line.simplified();

            //separate parameter name from value
            buff = line.split(" ", QString::KeepEmptyParts, Qt::CaseInsensitive);
            //devo avere 17 campi, altrimenti riga non valida
            if(buff.count() != 17) {
                qDebug("riga non valida");
                continue;
            }
            //convert from HEX to int and add to global list
            bool bStatus = false;
            for(int i=0; i<16; i++) {
                table << buff[i+1].toUInt(&bStatus, 16);
            }
        }
//qDebug() << "table:  (len=" << table.length() << ")";
//qDebug() << table;

//for(int i=0; i<128; i++)
//    printf("EDID[%d]: 0x%02X\n", i, table.at(i));
//fflush(stdout);

        return true;
    }
    qDebug() << "EDID file open error:" << file.errorString();
    return false;
}

//------------------------------------------------------
//File di configurazione (generico)
//al momento usato solo per impostare la Ethernet con il parametro:
//ETHERNET=YES
//predisposto per leggere altri parametri
//------------------------------------------------------
bool FileManager::loadOSDConfigFile()
{
    PRINT_FUNC_NAME

    QString line;
    QStringList buff;
	QFile* file;
	
    //mettere ricerca prima nella SD Card
    //LOLLO todo

    QFile file1(CONFIG_PATH "wave_cfg.txt"); //Sulla SD Card
    QFile file2("/opt/osd/wave_cfg.txt"); //Default nel filesystem
	
    if(file1.exists())
	{
		file = &file1;
	}
	else
	{
		file = &file2;
	}

    if(file->open(QFile::ReadOnly))
    {
        QTextStream in(file);

        while(!in.atEnd())
        {
            line = in.readLine();
            qDebug() << "readLine";
            //salto righe di commento e righe vuote
            if(line.startsWith("#") || line == "") {
                continue;
            }
            //separate parameter name from value
            buff = line.split("=", QString::KeepEmptyParts, Qt::CaseInsensitive);
            //devo avere 2 campi, altrimenti riga non valida
            if(buff.count() != 2) {
                //qDebug("riga non valida");
                continue;
            }

            if(buff[0] == "ETHERNET")
            {
                QString eth = buff[1];
                if(eth == "YES") {
                    qDebug("ETHERNET abilitata");
                    m_dm->setProperty("eth_enabled", true);
                }
                else {
                    qDebug("ETHERNET disabilitata");
                    m_dm->setProperty("eth_enabled", false);
                }
            }
        }
    }
    else {
        qDebug("wave_cfg.txt non trovato, ETHERNET disabilitata");
        m_dm->setProperty("eth_enabled", false);
    }
    return true;
}

//------------------------------------------------------
bool FileManager::loadPanelTimingsFile()
{
    qDebug() << COL_YELLOW "FileManager::loadPanelTimingsFile:" TIMINGS_FILE
             << COL_NORMAL;

    QStringList buff;

    QFile file(TIMINGS_FILE);
    if(file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);
        QString line;

        while(!in.atEnd())
        {
            line = in.readLine();

            //salto righe di commento e righe vuote
            if(line.startsWith("#") || line == "")
                continue;

            //changes all whitespaces to single instance of ASCII 32
            line = line.simplified();
            //remove spaces and tabs
            line.replace(" ", "" ).replace('\t', "" );

            //extract all fields
            buff = line.split(";", QString::KeepEmptyParts, Qt::CaseInsensitive);
            //devo avere 10 campi, altrimenti riga non valida
            if(buff.count() != 11) {
                qDebug("riga non valida count=%d", buff.count());
                continue;
            }

//             qDebug() << "trovato:"
//                          << buff[0] << buff[1] << buff[2] << buff[3] << buff[4]
//                          << buff[5] << buff[6] << buff[7] << buff[8] << buff[9]
//                         ;

            //converto da ASCII a int
            int idx = buff[0].toInt() - 1; //1 based index
            //qDebug() << "inpIdx:" << idx;

            //sanity check
            if(idx <= 15) {
                m_dm->PanelTimingsTable[idx].Width =         buff[1].toInt();
                m_dm->PanelTimingsTable[idx].Height =		 buff[2].toInt();
                m_dm->PanelTimingsTable[idx].H_front_porch = buff[3].toInt();
                m_dm->PanelTimingsTable[idx].H_sync =		 buff[4].toInt();
                m_dm->PanelTimingsTable[idx].H_back_porch =	 buff[5].toInt();
                m_dm->PanelTimingsTable[idx].V_front_porch = buff[6].toInt();
                m_dm->PanelTimingsTable[idx].V_sync =		 buff[7].toInt();
                m_dm->PanelTimingsTable[idx].clock_idx =	 buff[8].toInt();
                m_dm->PanelTimingsTable[idx].V_back_porch =	 buff[9].toInt();
            }
        }

            //debug dump
//             for(int i=0; i<15; i++)
//                qDebug() << m_dm->PanelTimingsTable[i].Width
//                         << m_dm->PanelTimingsTable[i].Height
//                         << m_dm->PanelTimingsTable[i].H_front_porch
//                         << m_dm->PanelTimingsTable[i].H_sync
//                         << m_dm->PanelTimingsTable[i].H_back_porch
//                         << m_dm->PanelTimingsTable[i].V_front_porch
//                         << m_dm->PanelTimingsTable[i].V_sync
//                         << m_dm->PanelTimingsTable[i].clock_idx
//                         << m_dm->PanelTimingsTable[i].V_back_porch;

        return true;
    }
    qDebug() << "File open error:" << file.errorString();
    return false;
}

//-------------------------------------------------------
//Controlla se sulla SD card è presente il file del logo
//per abilitare il relativo parametro "logo timeout"
//-------------------------------------------------------
void FileManager::checkLogoIsPresent()
{
    PRINT_FUNC_NAME

    QStringList filter("*.bmp");
    QDir dir(LOGO_FILE_PATH);
    QStringList pngfiles =  dir.entryList(QStringList(filter), QDir::Files);
    qDebug() << "\nbmpfiles" << pngfiles;

    if(pngfiles.size() != 0) {
        m_dm->setProperty("logo_present", true);
        qDebug("logo_present = TRUE");
    }
    else {
        m_dm->setProperty("logo_present", false);
        qDebug("logo_present = FALSE");
    }
}

/* EOF */



























































