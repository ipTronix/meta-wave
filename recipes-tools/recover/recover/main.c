/**
 *
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <linux/loop.h>

/**
 */
typedef struct {
  uint32_t version; // version of linux-dtb-fs
  uint32_t img_pos; // linux image
  uint32_t img_len;
  uint32_t dt1_pos; // device tree blob 1
  uint32_t dt1_len;
  uint32_t dt2_pos; // device tree blob 2
  uint32_t dt2_len;
  uint32_t rfs_pos; // ram file system
  uint32_t rfs_len;
}sImgHdr, *psImgHdr;
/*
#define SOURCE_DIR "/mnt/fat"
#define SOURCE_DEV "/dev/mmcblk1p1"
#define IMAGE_FILE "/mnt/fat/image"
*/

#define SOURCE_DIR "/mnt/usb"
#define SOURCE_DEV "/dev/sda1"
#define IMAGE_FILE "/mnt/usb/recovery/image"

sImgHdr imgHdr;

psImgHdr imgHdrGet(char* imgfile);

/**
 *
 */
int main(int argc, char** argv)
{
  psImgHdr  pImgHdr;
  char      cmd[256];
  size_t    ofs;
  int       ret;

  printf( "\n"
          "-----------------------------\n"
          "   recovery application\n"
          "Compiled: "__DATE__ " " __TIME__ "\n"
          "-----------------------------\n");

  printf("Create %s\n", SOURCE_DIR);
//  mkdir("/mnt/usb", 0777);
  mkdir(SOURCE_DIR, 0777);

  printf("Create /mnt/iso\n");
  mkdir("/mnt/iso", 0777);

  printf("Mount source device\n");
  sprintf(cmd,
          "mount %s %s",
          SOURCE_DEV,
          SOURCE_DIR);
  ret = system(cmd);//"mount /dev/sda1 /mnt/usb");
  if(ret){
    printf("ERROR:Mount %s FAILED\n", SOURCE_DEV);
    return -1;
  }

  printf("Get image header\n");
  pImgHdr = imgHdrGet(IMAGE_FILE);//"/mnt/usb/recovery/image");
  if(!pImgHdr){
    printf("ERROR:reading image header\n");
    return -1;
  }

  if (!pImgHdr->rfs_pos && !pImgHdr->rfs_len) {
    pImgHdr->rfs_pos = pImgHdr->dt2_pos;
    pImgHdr->rfs_len = pImgHdr->dt2_len;
  }
  ofs = pImgHdr->rfs_pos + pImgHdr->rfs_len;
  ofs = (ofs+127) & ~127;
  printf("ofs: %d %08X\n", ofs, ofs);

#if 1
  printf("splitting image file in /home/root/update.iso\n");
  sprintf(cmd,
          "dd if=%s of=/home/root/update.iso bs=128 skip=%d",
          IMAGE_FILE,
          ofs/128);
  ret = system(cmd);
  if(ret){
    printf("ERROR: splitting image file\n");
    return -1;
  }

  printf("mounting iso image\n");
  ret = system("mount /home/root/update.iso /mnt/iso");
  if(ret){
    printf("ERROR: mounting iso image\n");
    return -1;
  }
//TODO ret = exec("/mnt/iso/update");
#else
  sprintf(cmd,
          "dd if=%s "
             "of=/home/root/dev-fb-qt5-wave.tar.bz2 "
             "bs=128 skip=%d",
          IMAGE_FILE,
          ofs/128);
  ret = system(cmd);
  if(ret){
    printf("ERROR: splitting image file\n");
    return -1;
  }

  ret = system("mkfs.ext3 -E nodiscard -j /dev/mmcblk1p2");
  ret = system("mkdir -p /mnt/mmcblk1p2");
  ret = system("mount -t ext3 /dev/mmcblk1p2 /mnt/mmcblk1p2");
  ret = system("tar -xjvf /home/root/dev-fb-qt5-wave.tar.bz2 -C /mnt/mmcblk1p2");
//  ret = system("umount /mnt/mmcblk1p2");
 //cat /mnt/usb/recovery/image > imageflt  | tar -xjvf  -C /mnt/mmcblk1p2

#endif
  return 0;
}

/**
 */
psImgHdr imgHdrGet(char* imgfile)
{
  int     fd;
  int     rb;
  int     ret;

  fd = open(imgfile, O_RDONLY);
  if(fd<0){
    printf("File open error %d: %s\n", errno, strerror(errno));
    return NULL;
  }
  rb = read(fd, (void*)&imgHdr, sizeof(sImgHdr));
  if(rb!=sizeof(sImgHdr)){
    printf("Error reading image file header\n");
    close(fd);
    return NULL;
  }
  close(fd);

  if( (imgHdr.rfs_pos==0) &&  (imgHdr.rfs_len==0) ){
    imgHdr.rfs_pos = imgHdr.dt2_pos;
    imgHdr.rfs_len = imgHdr.dt2_len;
    imgHdr.dt2_pos = imgHdr.dt1_pos;
    imgHdr.dt2_len = imgHdr.dt1_len;
  }

  printf( "Image Header\n"
          " version %08X\n"
          " img_pos %08X\n"
          " img_len %08X\n"
          " dt1_pos %08X\n"
          " dt1_len %08X\n"
          " dt2_pos %08X\n"
          " dt2_len %08X\n"
          " rfs_pos %08X\n"
          " rfs_len %08X\n",
          imgHdr.version,
          imgHdr.img_pos,
          imgHdr.img_len,
          imgHdr.dt1_pos,
          imgHdr.dt1_len,
          imgHdr.dt2_pos,
          imgHdr.dt2_len,
          imgHdr.rfs_pos,
          imgHdr.rfs_len);

  return &imgHdr;
}
