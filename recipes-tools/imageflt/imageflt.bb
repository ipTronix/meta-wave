DESCRIPTION = "Recover application split usb recovery image"
SECTION = "recovery"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://main.c \
"

S = "${WORKDIR}"

do_compile() {
	${CC} main.c -o imageflt
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 imageflt ${D}${bindir}
}
