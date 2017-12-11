/*****************************************************************************
*   Fie:		qml_launcher.cpp
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#include "qml_launcher.h"

QML_Launcher::QML_Launcher(QObject *parent) :
    QObject(parent)
{
    qDebug("QML_Launcher");

    //passo alla parte QML il numero degli ingressi (default 4)
    engine.rootContext()->setContextProperty("N_INPUTS", NUM_INPUTS);
    engine.rootContext()->setContextProperty("OSD_CLOSE_TOUT", OSD_TIMEOUT);
    //nota: solo per debug
    engine.rootContext()->setContextProperty("OSD_SW_REV_MAJ", SW_REL_MAJ);
    engine.rootContext()->setContextProperty("OSD_SW_REV_MIN", SW_REL_MIN);

    engine.rootContext()->setContextProperty("IMG_PATH_WIN", _IMG_PATH_WIN);
#ifdef DEBUG_WINDOWS
    engine.rootContext()->setContextProperty("DEB_WIN", 1);
#else
    engine.rootContext()->setContextProperty("DEB_WIN", 0);
#endif

    //usato per inviare signals da C++ a QML
    engine.rootContext()->setContextProperty("sig_conn", this);
    connect(this, SIGNAL(doStart()), this, SLOT(startOSD()));
}

void QML_Launcher::RegisterClasses(DataModel *pDm, FileManager *pFm)
{
    qDebug(" --> RegisterClasses");
    dm = pDm;
    fm = pFm;
    engine.rootContext()->setContextProperty("data_model",   dm);
    engine.rootContext()->setContextProperty("file_manager", fm);
}

//load the OSD main
void QML_Launcher::startOSD()
{
    qDebug("\n\n ------ QML_Launcher::startOSD ------");

#ifndef DEBUG_WINDOWS
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
#else
    engine.load("C:/LAVORO/ipTRONIX/OSD_SAM_Scaler_software/SW/OSD_Desktop_SCOM/proj/main.qml");
#endif
}

QQmlApplicationEngine* QML_Launcher::getEngine(void)
{
	return &engine;
}
