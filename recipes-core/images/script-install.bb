SUMMARY = "Add an init script to handle gpio for tera-he"
SECTION = "base"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/COPYRIGHT;md5=ae06596d194c8fcae93f44a22bcd2e15"
RDEPENDS_${PN} = "initscripts"
PR = "r0"
SRC_URI = " \
           file://strace \
           file://S90osd \
           file://fstab \
           file://20-sd_mount.rules \
           file://50default \
           file://wave_cfg.txt \
           file://COPYRIGHT \
"
FILES_${PN} += "/opt/*"

S = "${WORKDIR}"

do_install () {
  install -d ${D}${sysconfdir}
  install -m 0755 fstab ${D}${sysconfdir}
  install -d ${D}${sysconfdir}/udev/rules.d/
  install -m 0755 20-sd_mount.rules ${D}${sysconfdir}/udev/rules.d/
  install -d ${D}${sysconfdir}/rc5.d
  install -m 0755 S90osd ${D}${sysconfdir}/rc5.d
  install -d ${D}/usr/bin
  install -m 0755 strace ${D}/usr/bin
  install -d ${D}/opt/osd
  install -m 0644 wave_cfg.txt ${D}/opt/osd
  install -d ${D}/etc/udhcpc.d
  install -m 0755 50default ${D}/etc/udhcpc.d
  mkdir -p ${D}/mnt
  mkdir -p ${D}/mnt/sd
}

FILES_${PN} += "/mnt/sd"
