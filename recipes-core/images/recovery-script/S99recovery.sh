#! /bin/sh

upgrade_script="/usr/bin/upgrade.sh"

if [ -f ${upgrade_script} ]
then
  echo "Run upgrade script"
  sh ${upgrade_script}
fi
