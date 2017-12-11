/*****************************************************************************
*   Fie:		SCOMProtocol.h
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#ifndef SCOMPROTOCOL_H
#define SCOMPROTOCOL_H

#include <QObject>
#include <QMutex>
#include <QDebug>
#include <QTimer>
#include <QByteArray>
#include "parameters.h"
#include "temp_monitor.h"
#include "datamodel.h"
#include "qml_launcher.h"
#include "CircBuff.h"

#define SCOM_ATT			0x07
#define ACK_VAL				0x06
#define NAK_VAL				0x15

#define BROADCAST_ADDR		0xFF

class DataModel;
class QML_Launcher;

#define KEEP_PARSE_FLAG		0x01
#define SEND_ANSWER_FLAG	0x02

typedef enum
{
	scomTMP,
	scomMAN,
	scomTYP,
	scomBRT,
	scomSLI,
	scomSWI,
	scomETC,
	scomSNB,
	scomLFD,
	scomBZZ,
	scomMCC,
	scomINVALID
}eSCOM_CMD;

typedef struct
{
	char 			acCmd[3];
	unsigned char	len;
	unsigned char	answer_len;
}SCOM_STRUCT;

class SCOMProtocol : public QObject
{
    Q_OBJECT

    DataModel    *m_dm;
	QML_Launcher *m_ql;
	
	eSCOM_CMD FindScomCmd(char* pacCmd, unsigned char *pcLen);
	void GetScomLabel(eSCOM_CMD eCmd, char* pLabel);
	unsigned char GetScomAnswerLen(eSCOM_CMD eCmd);
	unsigned char GetCHK(unsigned char* pacData, unsigned char len);
	void GetAnswer(bool bACK, eSCOM_CMD eCmd, QByteArray* pAnswer);

public:
    explicit SCOMProtocol(QObject *parent = 0);
    void RegisterDm(DataModel *dmodel) { m_dm = dmodel; }
	void RegisterQMLlauncher(QML_Launcher *ql) { m_ql = ql; }
    unsigned char Parse(CircBuff* scom_buff, QByteArray* pAnswer);
};

#endif // SCOMPROTOCOL_H
