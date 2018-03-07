/**
 *
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

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

/**
 *
 */
int main(int argc, char** argv)
{
  sImgHdr   imgHdr;
  char      buf[256];
  size_t    len = 0U;
  ssize_t   n;
  int       sts;
  int       size;
  int       ofs;

  size = 0;
  ofs = 0;
  sts = 0;

  for(;;){
    len = read(STDIN_FILENO, buf, 256);
    if(len <= 0){
      break;
    }
    size += len;

    switch(sts){
    case 0:
      n = len < sizeof(sImgHdr)? len: sizeof(sImgHdr);
      memcpy((char*)&imgHdr+ofs, buf, n);
      ofs += n;
      if(ofs >= sizeof(sImgHdr)){
        ofs = imgHdr.rfs_pos + imgHdr.rfs_len;
        ofs = (ofs+127) & ~127;
        sts = 1;
      }
      break;

    case 1:
      if(size >= ofs){
        if((size - ofs) > 0){
          write(STDOUT_FILENO, buf + len - (size - ofs), size - ofs);
        }
        sts = 2;
      }
      break;

    case 2:
      write(STDOUT_FILENO, buf, len);
      break;
    }
  }
  return 0;
}
