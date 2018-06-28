/*****************************************************************************
*   File:		fw_upgrader.cpp
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
#include "fw_upgrader.h"

FwUpgrader::FwUpgrader(QObject *parent) :
    QObject(parent)
{
    PRINT_FUNC_NAME
	m_qlaunch = 0;
}

//-----------------------------------------------------
// Thread body
//-----------------------------------------------------
bool FwUpgrader::FwUpgraderRunLoop()
{
	bool bKeyUnplugged;
	
    PRINT_FUNC_NAME

    qDebug("FW upgrader run loop started");
	
	QFile img_file("/mnt/usb/recovery/image");
	
	bKeyUnplugged = true;
	
    while(1)
    {
		if (img_file.exists())
		{
			if (bKeyUnplugged)
			{
				bKeyUnplugged = false;
				qDebug("image file detected");
				m_dmdl->setProperty("FwUpgdRequested", true);
				emit m_qlaunch->reqDisplayOfUpgradePopup();
			}
		}
		else
		{
			if (!bKeyUnplugged)
			{
				bKeyUnplugged = true;
				qDebug("image file detection lost");
				m_dmdl->setProperty("FwUpgdRequested", false);
				emit m_qlaunch->reqRemoveOfUpgradePopup();
			}
		}
		
		//qDebug("FW upgrader still alive");
		usleep(350000);
    }
	
    return true;
}
