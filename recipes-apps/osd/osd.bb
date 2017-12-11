SUMMARY = "Qt5 OSD"
DESCRIPTION = "On Screen Display."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS = "qtdeclarative qtgraphicaleffects"

SRCREV = "0.15"

SRC_URI = " \
    file://Dashboard1.pro.user \
    file://osd.pro.user \
    file://deployment.pri \
    file://main.cpp \
    file://temp_monitor.cpp \
    file://qml_launcher.cpp \
    file://serialprotocol.cpp \
	file://CircBuff.cpp \
	file://scom_tcp_server.cpp \
	file://scom_tcp_thread.cpp \
	file://SCOMProtocol.cpp \	
    file://datamodel.cpp \
    file://filemanager.cpp \
    file://osd.pro \
    file://datamodel.h \
    file://debug.h \
    file://filemanager.h \
    file://global.h \
    file://parameters.h \
    file://qml_launcher.h \
    file://temp_monitor.h \
    file://serialprotocol.h \
	file://CircBuff.h \
	file://scom_tcp_server.h \
	file://scom_tcp_thread.h \
	file://SCOMProtocol.h \	
    file://global.js \
    file://main.qml \
    file://CustomBtn.qml \
    file://InputRowDelegate.qml \
    file://OsdComboBox.qml \
    file://OsdNumBox.qml \
    file://OsdRadioButtons.qml \
    file://OsdCheckBox.qml \
    file://OsdSlider.qml \
    file://OsdSpinBox.qml \
    file://PopupPIN.qml \
    file://TabComm.qml \
    file://TabInputs.qml \
    file://TabLoadSave.qml \
    file://TabSettings.qml \
    file://TabStatus.qml \
    file://TabTimeouts.qml \
	file://OsdTabBtn.qml \
	file://OsdIpEditor.qml \
	file://Debug_VirtKeyboard.qml \
    file://qrc2.qrc \
    file://qml.qrc \
    file://fonts/Myriad-Pro_31655.ttf \
    file://fonts/Myriad-Pro-Bold_31631.ttf \
    file://img/ico_arrow_dwn.png \
    file://img/ico_arrow_up.png \
    file://img/ico_dp.png \
    file://img/ico_dvi.png \
    file://img/logo.png \
	file://img/ico_no_netw.png \
"

S = "${WORKDIR}"

inherit qmake5

do_install() {
    install -d ${D}${bindir}
    install -m 0755 osd ${D}${bindir}
}
