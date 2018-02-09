SUMMARY = "USB Updater"
DESCRIPTION = "Updte system from USB."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
    file://mkimg \
    file://version \
    file://zImage \
    file://zImage-waveq.dtb \
    file://zImage-waves.dtb \
    file://recovery-waves.cpio.gz.u-boot \
    file://dev-fb-qt5-waves \
"

S = "${WORKDIR}"

FILES_${PN} += "/zzz/*"

do_compile() {
  ./mkimg
}

do_install() {
  install -d ${D}
  install -m 0755 image.u ${D}/image
}
