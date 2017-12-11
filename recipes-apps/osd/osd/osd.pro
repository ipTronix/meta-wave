TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    datamodel.cpp \
    filemanager.cpp \
    serialprotocol.cpp \
    temp_monitor.cpp \
	CircBuff.cpp \
	scom_tcp_server.cpp \
	scom_tcp_thread.cpp \
	SCOMProtocol.cpp \
    qml_launcher.cpp

RESOURCES += qml.qrc \
    qrc2.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

#per target Linux
#target.path = /opt/$${TARGET}/bin
#INSTALLS += target

OTHER_FILES +=

HEADERS += \
    datamodel.h \
    filemanager.h \
    serialprotocol.h \
    parameters.h \
    global.h \
    temp_monitor.h \
	CircBuff.h \
	scom_tcp_server.h \
	scom_tcp_thread.h \
	SCOMProtocol.h \
    qml_launcher.h
