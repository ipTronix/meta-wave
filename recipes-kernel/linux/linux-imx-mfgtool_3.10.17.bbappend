# North Invent wave Evolution

DESCRIPTION = "kernel for roj imx6q smarc platform"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

PRINC := "${@int(PRINC) + 2}"

SRC_URI_append_waveq = " \
   file://waveq-dts.patch \
   file://ltc3676.patch \
   file://004-fix-mmc-addresses.patch \
   file://defconfig \
"

SRC_URI_append_waves = " \
   file://waves-dts.patch \
   file://ltc3676.patch \
   file://004-fix-mmc-addresses.patch \
   file://defconfig \
"
