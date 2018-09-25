/**
 * copia image e image.md5
 * fa il sync
 * verifica md5
 * crea boot.scr
 *
 */

#include <stdio.h>
#include <openssl/md5.h>


/**
 */
typedef struct {
  void   *buf;
  size_t  len;
}sFile, *psFile;



unsigned char md5_str[MD5_DIGEST_LENGTH];

int md5cp(char* tgtfile, char* dstfile, unsigned char* md5);

/**
 */
int main(int argc, char** argv)
{
  int     ret;
  psFile  md5_file;

  printf("\n-----------------------------\n"
           " Update image copier utility\n"
           "-----------------------------\n");
  printf("Compiled: "__DATE__ " " __TIME__ "\n\n");

  printf("mounting partition 3\n");
  ret = system("mkdir -p /mnt/p3");
  if(ret){
    printf("ERROR: creating directory /mnt/p3\n");
    return -1;
  }
  ret = system("mount -t vfat /dev/mmcblk1p3 /mnt/p3");
  if(ret){
    printf("ERROR: mounting partition 3\n");
    return -1;
  }

  printf("coping image and md5\n");
  ret = md5cp("/mnt/usb/recovery/image", "/mnt/p3", md5_str)
  if (ret) {
    return -1;
  }

  ret = md5cp("/mnt/usb/recovery/image.md5", "/mnt/p3", md5_str)
  if (ret) {
    return -1;
  }

  printf("verifiing image md5\n");
//image.md5
//4637b6c06f111c25e9ffbb0d95b83b4f  image

  md5_file = fileLoad("/mnt/p3/image.md5");
  if (!md5_file) {
    return -1;
  }
  fileFree(md5_file);

  return 0;
}

/**
 */
int md5cp(char* tgtfile, char* dstfile, unsigned char* md5)
{
  char   *buf;
  int     tgtfd;
  int     dstfd;
  int     ret;
  size_t  rb;
  size_t  wb;
  MD5_CTX mdctx;  // MD5 context

  tgtfd = open(tgtfile, O_RDONLY);
  if(tgtfd<0){
    printf("errore apertura %s\n", tgtfile);
    return -1;
  }

  dstfd = open(dstfile, O_RDWR | O_CREAT | O_TRUNC, 0666);
  if(dstfd<0){
    printf("errore apertura %s\n", dstfile);
    close(tgtfd);
    return -1;
  }

  buf = (char*)malloc(1024);
  if(!buf){
    printf("errore malloc\n");
    close(tgtfd);
    close(dstfd);
    return -1;
  }

  MD5_Init(&mdctx);
  for(;;){
    rb = read(tgtfd, buf, 1024);
    if(!rb){
      break;
    }
    wb = write(dstfd, buf, rb);
    if(wb!=rb){
      printf("errore scrittura\n");
      free(buf);
      close(tgtfd);
      close(dstfd);
      return -1;
    }
    MD5_Update(&mdctx, buf, rb);
  }

  free(buf);
  close(tgtfd);
  close(dstfd);
  if (mdctx) {
    MD5_Final(md5, &mdctx);
  }

  return 0;
}

/**
 */
psFile fileLoad(char* file)
{
  psFile  pHnd;
  int     fd;
  int     ret;

  pHnd = (psFile)malloc(sizeof(sFile));
  if(!pHnd){
    return NULL;
  }
  memset(pHnd, 0, sizeof(sFile));

  fd = open(file, O_RDONLY);
  if(fd<0){
    free(pHnd);
    return NULL;
  }

  pHnd->len = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);

  pHnd->buf = (void*)malloc(pHnd->len);
  if(!pHnd->buf){
    close(fd);
    free(pHnd);
    return NULL;
  }

  ret = read(fd, pHnd->buf, pHnd->len);
  if(ret!=pHnd->len){
    close(fd);
    free(pHnd->buf);
    free(pHnd);
    return NULL;
  }

  close(fd);

  return pHnd;
}

/**
 */
int fileFree(psFile pHnd)
{
  free(pHnd->buf);
  free(pHnd);
  return 0;
}
