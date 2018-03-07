DESCRIPTION = "Tests application"
SECTION = "examples"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
    file://main.c \
    file://arm_sw.bin \
    file://fpga_sw.bin \
"

FILES_${PN} += "/usr/*"

S = "${WORKDIR}"

do_compile() {
	${CC} main.c -o fwupg
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 fwupg ${D}${bindir}
  install -d ${D}/usr/fw
  install -m 0644 arm_sw.bin ${D}/usr/fw
  install -m 0644 fpga_sw.bin ${D}/usr/fw
}
