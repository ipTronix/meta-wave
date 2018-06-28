/*****************************************************************************
*   Fie:		fw_upgrader.h
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#ifndef FW_UPGRADER_H
#define FW_UPGRADER_H

#include <QObject>
#include <QMutex>
#include <QDebug>
#include <QTimer>
#include "parameters.h"
#include "temp_monitor.h"
#include "datamodel.h"
#include "qml_launcher.h"
#include "filemanager.h"

class DataModel;
class QML_Launcher;

class FwUpgrader : public QObject
{
    Q_OBJECT

    DataModel    *m_dmdl;
	QML_Launcher *m_qlaunch;
	
public:
    explicit FwUpgrader(QObject *parent = 0);
	void RegisterQMLlauncher(QML_Launcher *ql) { m_qlaunch = ql; }
	void RegisterDm(DataModel *dmodel) { m_dmdl = dmodel; }

public slots:
    bool FwUpgraderRunLoop();
};

#endif //FW_UPGRADER_H
