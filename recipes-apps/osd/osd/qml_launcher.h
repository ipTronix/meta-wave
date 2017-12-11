/*****************************************************************************
*   Fie:		qml_launcher.h
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#ifndef QML_LAUNCHER_H
#define QML_LAUNCHER_H

#include <QObject>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>
#include <QtQml>
#include <QTimer>
#include <QDebug>
#include "datamodel.h"
#include "filemanager.h"

class DataModel;
class FileManager;

class QML_Launcher : public QObject
{
    Q_OBJECT
    QQmlApplicationEngine engine;
    DataModel *dm;
    FileManager *fm;

public:
    explicit QML_Launcher(QObject *parent = 0);
    void RegisterClasses(DataModel *pDm, FileManager *pFm);
	QQmlApplicationEngine* getEngine(void);

signals:
    void doStart();
    void newSourceData();
    void settingsCmdSent();
    void updateTemperature();

public slots:
    void startOSD();

};

#endif // QML_LAUNCHER_H
