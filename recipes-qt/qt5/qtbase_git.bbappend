DESCRIPTION = "patches to compile qt"

QT_CONFIG_FLAGS += " -no-xcb -qpa eglfs "

FILESEXTRAPATHS_prepend := "${THISDIR}/qtbase:"

PACKAGECONFIG_append_pn-qtbase = " accessibility"

SRC_URI_append_waveq = " \
    file://000_fix_compilation_error.patch \
    "
SRC_URI_append_waves = " \
    file://000_fix_compilation_error.patch \
    "

