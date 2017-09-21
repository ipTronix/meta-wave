#! /bin/sh

emmc_device="/dev/mmcblk2"
emmc_mnt_path="/mnt/p1/"
recovery_script=${emmc_mnt_path}"recovery/recovery.sh"
upgrade_script="/usr/bin/upgrade.sh"

mkdir -p ${emmc_mnt_path}
mount ${emmc_device}"p1" ${emmc_mnt_path}

if [ -f ${recovery_script} ]
then
  echo "Run recovery script"
  sh ${recovery_script}
fi
rm -rf ${emmc_mnt_path}"boot.scr*"
umount ${emmc_mnt_path}

if [ -f ${upgrade_script} ]
then
  echo "Run upgrade script"
  sh ${upgrade_script}
fi
