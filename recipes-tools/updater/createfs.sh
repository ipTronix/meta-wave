#!/bin/sh
#
echo "Create filesystem"

oldver=`cut -b-4 ../sources/meta-wave/recipes-core/images/script-install/version`
oldver=`expr $oldver + 0`
newver=`expr $oldver + 1`
echo $oldver to $newver
sed -i "s/$oldver\$/$newver/g" ../sources/meta-wave/recipes-core/images/script-install/version

bitbake dev-fb-qt5