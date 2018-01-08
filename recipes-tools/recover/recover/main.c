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
  uint32_t dt1_pos; // device tree blob
  uint32_t dt1_len;
  uint32_t dt2_pos;
  uint32_t dt2_len;
  uint32_t rfs_pos; // ram file system
  uint32_t rfs_len;
}sImgHdr, *psImgHdr;


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
          "   recovery appliaction\n"
          "Compiled: "__DATE__ " " __TIME__ "\n"
          "-----------------------------\n");

  printf("Create /mnt/usb\n");
  mkdir("/mnt/usb", 0777);

  printf("Create /mnt/iso\n");
  mkdir("/mnt/iso", 0777);

  printf("Mount USB Disk\n");
  ret = system("mount /dev/sda1 /mnt/usb");
  if(ret){
    printf("ERROR:Mount /mnt/usb FAILED\n");
    return -1;
  }

  printf("Get image header\n");
  pImgHdr = imgHdrGet("/mnt/usb/recovery/image");
  if(!pImgHdr){
    printf("ERROR:reading image header\n");
    return -1;
  }

  ofs = pImgHdr->rfs_pos + pImgHdr->rfs_len;
  ofs = (ofs+127) & ~127;
  printf("ofs: %d %08X\n", ofs, ofs);

  sprintf(cmd,
          "dd if=/mnt/usb/recovery/image of=/home/root/update.iso bs=128 skip=%d",
          ofs/128);
  ret = system(cmd);
  if(ret){
    printf("ERROR: splitting image file\n");
    return -1;
  }

  ret = system("mount /home/root/update.iso /mnt/iso");
  if(ret){
    printf("ERROR: mounting iso image\n");
    return -1;
  }
//TODO ret = exec("/mnt/iso/update");
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
