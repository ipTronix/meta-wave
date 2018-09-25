#! /bin/sh
# upgrade script

echo --------------------------------------------------
echo                   upgrade script
echo   USB version
echo --------------------------------------------------

if false; then

echo prepare disk

disk=/dev/mmcblk1

# destroy the partition table
dd if=/dev/zero of=${disk} bs=1024 count=1

# call sfdisk to create partition table
sfdisk --force -uM ${disk} << EOF
10,100,0c
110,3000,83
3110,,0c
EOF

echo format ${disk}p1 FAT
mkfs.vfat ${disk}p1

echo format ${disk}p2 EXT3
mkfs.ext3 -E nodiscard -j ${disk}p2

echo format ${disk}p3 FAT
mkfs.vfat ${disk}p3

fi

echo mounting USB disk
mkdir -p /mnt/usb
mount /dev/sda1 /mnt/usb

echo mounting data partition
mkdir -p /mnt/p3
mount /dev/mmcblk1p3 /mnt/p3


mkdir -p /mnt/iso

if false; then
  echo extract iso from image
  line=$(hexdump -C /mnt/usb/recovery/image -n 8 -s 20)
  pos=0x${line:19:2}${line:16:2}${line:13:2}${line:10:2}
  len=0x${line:31:2}${line:28:2}${line:25:2}${line:22:2}
  ofs=$((pos+len))
  ofs=$((ofs+127))
  ofs=$((ofs/128))
  echo pos:$pos, len:$len, ofs:$ofs
  dd if=/mnt/usb/recovery/image of=/mnt/p3/update.iso bs=128 skip=${ofs}
  echo mounting iso
  mount /mnt/p3/update.iso /mnt/iso
else
  echo mounting iso from image
  line=$(hexdump -C /mnt/p3/image -n 8 -s 28)
  pos=0x${line:19:2}${line:16:2}${line:13:2}${line:10:2}
  len=0x${line:31:2}${line:28:2}${line:25:2}${line:22:2}
  ofs=$((pos+len))
  ofs=$((ofs+127))
  ofs=$((ofs&0xFFFFFF80))
  echo pos:$pos, len:$len, ofs:$ofs
  mount /mnt/p3/image /mnt/iso -o loop,offset=$ofs
fi


# recover
/mnt/iso/update