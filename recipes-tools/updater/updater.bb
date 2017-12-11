SUMMARY = "USB Updater"
DESCRIPTION = "Updte system from USB."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
    file://linux \
    file://version \
    file://update/update \
    file://update/osd \
    file://update/S90osd \
    file://update/fwupg \
    file://update/arm_sw.bin \
    file://update/fpga_sw.bin \
"

S = "${WORKDIR}"

do_compile() {
  mkisofs -o update.iso update && \
  cp linux image.u && \
  cat update.iso >> image.u  && \
  dd if=version of=image
}

do_install() {
  install -d ${D}
  install -m 0755 image.u ${D}/image
}
