UPDATER
================================================================================
Descrizione della procedura da segure per preparare una penna USB che effettui l'aggionamento del sistema.
La penna USB deve essere preparata con un'unica partizione FAT32. Su questa partizione dovrà essere copiato il file immagine con il nome:
   /recovery/image

Preparazione dell'aggiornamento
================================================================================
nella cartella:
  NIwave/sources/meta-wave/recipes-tools/updater/updater/update
editare lo script file: update

Esempio:
# ! /bin/sh
# upgrade script
mkdir -p /mnt/hd mount /dev/mmcblk1p2 /mnt/hd
cp /mnt/iso/osd /mnt/hd/usr/bin chmod +x /mnt/hd/usr/bin/osd
cp /mnt/iso/s90osd /mnt/hd/etc/rc5.d/S90osd chmod +x /mnt/hd/etc/rc5.d/S90osd
sync && sync
cd /mnt/iso
/mnt/iso/fwupg

Questo script, dopo aver montato l'immagine iso di aggiornamento:
  copia il nuovo binario osd in /usr/bin copia lo scrip di avvio S900sd in /etc/rc5.d/S90osd
  chiama fwupg che provvede a:
    aggiornare il fw FPGA e LPC;
    aggiornare il file /mnt/p1/version (partizione VFAT dell'EMMC);
    riavviare tutto inviando il comando di reset all'LPC.

Una volta modificato il file script update copiare nella directory i files da aggiornare, se necessario modificare il file:
    NIwave/sources/meta-wave/recipes-tools/updater/updater.bb
riportando la lista dei file necessari per l'aggiornamento

Esempio:
file://update/update \                         script di aggiornamento
file://update/osd \                              binario on screen display
file://update/S90osd \                        script avvio OSD
file://update/fwupg \                         programma aggiornamento fw
file://update/arm_sw.bin \                firmware LPC
file://update/fpga_sw.bin \               firmware FPGA

Generare il file version contenente la versione dell'aggiornamento nella cartella:
     NIwave/sources/meta-wave/recipes-tools/updater/updater

Il file versione deve avere lunghezza pari a 4 caratteri

Esempio:
Per generare il file version con la versione 1.1.1.1 eseguire:
   echo -n -e '\x01\x01\x01\x01' > version


Una volta eseguite le modifiche necessarie eseguire la ricetta: bitbake updater
L'immagine da copiare sulla chiavetta USB è in: NIwave/build/tmp/work/cortexa9hf-vfp-neon-poky-linux-gnueabi/updater/1.0-r0/image