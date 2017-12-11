/*****************************************************************************
*   Fie:		temp_monitor.h
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#ifndef TEMP_MONITOR_H
#define TEMP_MONITOR_H

#include <QObject>
#include <QTimer>
#include "serialprotocol.h"
#include "temp_monitor.h"

class SerialProtocol;
class DataModel;
class QML_Launcher;

class TempMonitor : public QObject
{
    Q_OBJECT

    QTimer *tempTimer;
    DataModel *m_dm;
    QML_Launcher *m_ql;

public:
    explicit TempMonitor(QObject *parent = 0);
    int curr_temperature;
    void RegisterDm(DataModel *dmodel) { m_dm = dmodel; }
    void RegisterQl(QML_Launcher *ql)  { m_ql = ql; }

public slots:
    int ReadTemperature();
    void UpdateTemperature();
};

#endif // TEMP_MONITOR_H
