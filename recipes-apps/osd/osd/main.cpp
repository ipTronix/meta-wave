/*****************************************************************************
*   Fie:		main.cpp
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#include "global.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>
#include <QtQml>
#include <QDebug>
#include <qthread.h>
#include "datamodel.h"
#include "filemanager.h"
#include "serialprotocol.h"
#include "temp_monitor.h"
#include "qml_launcher.h"
#include "scom_tcp_server.h"
#include "SCOMProtocol.h"

int main(int argc, char *argv[])
{
    qDebug("\n----------------------------------------");
    qDebug(" OSD Wave --");
    qDebug("----------------------------------------");
    qDebug("Rev. " SW_REL_MAJ "." SW_REL_MIN);
    qDebug() << "Compiled:"  << __DATE__ << __TIME__ << '\n';

    QApplication app(argc, argv);

    int id1 = QFontDatabase::addApplicationFont(":/fonts/Myriad-Pro_31655.ttf");
    int id2 = QFontDatabase::addApplicationFont(":/fonts/Myriad-Pro-Bold_31631.ttf");
    qDebug() << "AddApplicationFont ids:" << id1 << id2;

    DataModel   *data_model   = new DataModel;
    FileManager *file_manager = new FileManager;
    TempMonitor *tMon =         new TempMonitor;

    //avvio ritardato grafica OSD all'arrivo di INIT_END
    QML_Launcher *launcher = new QML_Launcher();

    //Serial comm thread
    QThread *commThread = new QThread;
    SerialProtocol *sp = new SerialProtocol();

    sp->RegisterDm(data_model);
    sp->RegisterQMLlauncher(launcher);

    sp->moveToThread(commThread);
    QObject::connect(commThread, SIGNAL(started()), sp, SLOT(runLoop()));
    qDebug("Serial comm thread created");
	
#ifndef DEBUG_WINDOWS
    commThread->start();
#endif
    //scambio dei puntatori delle classi
    data_model->RegisterFm(file_manager);
    data_model->RegisterSp(sp);
    file_manager->RegisterDm(data_model);
    file_manager->LoadFiles();
    file_manager->checkLogoIsPresent();  //to enable logo timeout setting
    data_model->ComputeTablesCRC();
    tMon->RegisterDm(data_model);
    launcher->RegisterClasses(data_model, file_manager);
    tMon->RegisterQl(launcher);
    tMon->UpdateTemperature(); //prima lettura

    //nasconde il cursore all'avvio del QML
    QGuiApplication::setOverrideCursor(Qt::BlankCursor);

#ifdef DEBUG_AVVIA_SUBITO
    //per attivare subito senza attendere INIT_END
    launcher->startOSD();
#endif
	scom_tcp_server m_scom_tcp_server;
	SCOMProtocol* m_scom_protocol;
	
	if (data_model->property("eth_enabled").toBool())
	{
		m_scom_protocol = new SCOMProtocol();
		m_scom_protocol->RegisterDm(data_model);
		m_scom_protocol->RegisterQMLlauncher(launcher);
		m_scom_tcp_server.RegisterScomProtocol(m_scom_protocol);
	
		m_scom_tcp_server.startServer();
	}

 return app.exec();

}

/* EOF */













