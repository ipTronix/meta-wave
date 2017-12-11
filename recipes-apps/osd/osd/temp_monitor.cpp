/*****************************************************************************
*   Fie:		temp_monitor.cpp
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#include <fcntl.h>
#include <unistd.h>
#include "parameters.h"
#include "global.h"
#include "temp_monitor.h"

//--------------------------------------------------------
TempMonitor::TempMonitor(QObject *parent) :
    QObject(parent)
{
    PRINT_FUNC_NAME

    m_dm = 0;
    m_ql = 0;
    curr_temperature = 0;
    //aggiornamento cadenzato temperatura
    tempTimer = new QTimer(this);
    tempTimer->setInterval(TEMPER_UPDATE_TIME * 1000);
    QObject::connect(tempTimer, SIGNAL(timeout()), this, SLOT(UpdateTemperature()));
    tempTimer->start();
}

//--------------------------------------------------------
int TempMonitor::ReadTemperature()
{
    unsigned char buf[32];
    int temp_fd = open("/sys/class/thermal/thermal_zone0/temp", O_RDONLY);
    if(temp_fd == -1) {
        qDebug("temperature open error");
        return -1; //error
    }
    int ret = read(temp_fd, buf, 32);
    //temperatura: numero in millesimi di °C (stringa di 6 caratteri)
    if(ret < 6) {
        qDebug("temperature read error");
        return -1; //error
    }
    //converto da ASCII a decimale e compongo la cifra
    int temp = (buf[0]-'0')*100 + (buf[1]-'0')*10 + (buf[2]-'0');
    //arrotondo a intero
    temp += 5; //sommo metà divisore
    temp /= 10;
    //clamp
    if(temp > 127) temp = 127; //nota: nella realtà i.MX6 non può superare i 120C
    //qDebug("Temperatura processore: %dC", temp);

    close(temp_fd);
    return temp;
}

//--------------------------------------------------------
void TempMonitor::UpdateTemperature()
{
#ifndef DEBUG_WINDOWS
    if(m_dm) {
        m_dm->Curr_TemperatureC = ReadTemperature();
        if(m_ql)
            emit m_ql->updateTemperature();
    }
#endif
}


/* EOF */






























