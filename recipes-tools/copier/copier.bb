DESCRIPTION = "Copier application, copy update image from USB"
SECTION = "tools"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
    file://main.c \
"

FILES_${PN} += "/usr/*"

S = "${WORKDIR}"

do_compile() {
    ${CC} main.c -o copier
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 copier ${D}${bindir}
}
