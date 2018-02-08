# North Invent wave Evolution

DESCRIPTION = "bootloader for wave evolution imx6q and imx6s smarc platform"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

PRINC := "${@int(PRINC) + 2}"

SRC_URI_append_waveq = " \
   file://001_add_roj_target.patch \
"

SRC_URI_append_waves = " \
   file://001_add_waves_target.patch \
   file://002_waves_fix_ddr_timings.patch \
"

