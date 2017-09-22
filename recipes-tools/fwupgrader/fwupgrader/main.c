//-----------------------------------------------------------------------
//
// Firmware download utility
//
//-----------------------------------------------------------------------
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


#define SERIAL_PORT     "/dev/ttyACM0"
//#define SERIAL_PORT   "/dev/ttymxc1" //lorenzo

//#define DEBUG_DUMP
//#define DEBUG_MODE

#define FPGA_SW_UPDATE_FILE     "fpga_sw_update.bin"
#define ARM_SW_UPDATE_FILE      "arm_sw_update.bin"

#define LPC_BOOTL   60
#define SET_BOOTL   61
#define SW_UPD_ACK  52
#define SW_UPD_NACK 53

#define LO_BYTE(x) (unsigned char)(x % 256)
#define HI_BYTE(x) (unsigned char)(x / 256)


/**
 */
typedef struct {
  uint32_t version; // version of linux-dtb-fs
  uint32_t img_pos; // linux image
  uint32_t img_len;
  uint32_t dtb_pos; // device tree blob
  uint32_t dtb_len;
  uint32_t rfs_pos; // ram file system
  uint32_t rfs_len;
}sImgHdr, *psImgHdr;

sImgHdr imgHdr;


enum e_dest { DEST_LPC = 50, DEST_FPGA = 51 };

int serial_fd;
unsigned char txbuff[264];
unsigned char rxbuff[16];
int chunk_num;

//-----------------------------------------------------------------------
int openSerialPort()
{
    serial_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY);
    if(serial_fd < 0) {
        printf ("Error %d opening serial: %s\n", errno, strerror(errno));
        fflush(stdout);
        return -1;
    }
    int parity = 0;
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (serial_fd, &tty) != 0) {
        printf("error %d from tcgetattr", errno);
        return -1;
    }

    cfsetospeed (&tty, B115200);
    cfsetispeed (&tty, B115200);

    //cfmakeraw(tty);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break as \000 chars
    tty.c_iflag &= ~IGNBRK;         		// disable break processing
    tty.c_lflag = 0;                		// no signaling chars, no echo, no canonical processing
    tty.c_oflag = 0;                		// no remapping, no delays
    tty.c_cc[VMIN] = 0;                     // read doesn't block
    tty.c_cc[VTIME] = 30;            		// 3 seconds read timeout
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                    | INLCR | IGNCR | ICRNL | IXON);
    tty.c_cflag |= (CLOCAL | CREAD);		// ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);		// shut off parity
    tty.c_cflag &= ~INLCR;                  // do not convert 0x0d to 0x0a
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr (serial_fd, TCSANOW, &tty) != 0) {
        //return("error %d from tcsetattr", errno);
        return -1;
    }
    return 0;
}

//-----------------------------------------------------------------------
int send_chunk(unsigned char *chunkData, int chunkNum, int dest)
{
    int i;

    //Checksum: somma a 8 bit complementata
    unsigned char chksum = 0;
    for(i=0; i<256; i++)
        chksum += chunkData[i];
    chksum = (unsigned char)(~chksum);

    //pacchetto
    txbuff[0] = 0xAA;               //header
    txbuff[1] = 0x55;
    txbuff[2] = dest;               //50= software LPC 51= software FPGA
    txbuff[3] = LO_BYTE(chunkNum);
    txbuff[4] = HI_BYTE(chunkNum);
    memcpy(&txbuff[5], chunkData, 256);
    txbuff[261] = chksum;
    txbuff[262] = 0x33;             //tail
    txbuff[263] = 0xCC;

#ifdef DEBUG_DUMP
    for(i=0; i<264; i++) {
        printf("%d\t", txbuff[i]);
        if((i%8) == 0) printf("\n");
    }
    printf("\nChunk checksum: %d\n\n", chksum);
    fflush(stdout);
#endif

    //invio pacchetto
    int ret = write(serial_fd, txbuff, sizeof(txbuff));
    if(ret < 0) {
        printf("write error: %s\n", strerror(errno));
        fflush(stdout);
        return -1;
    }
    return 0;
}

//-----------------------------------------------------------------------
int send_reset()
{
    unsigned char chunk[256];

    return send_chunk(chunk, 0, 52);
}


//-----------------------------------------------------------------------
//Attendo la conferma (ACK o NAK) da LPC
int get_response(unsigned char respCode)
{
    unsigned char *ptr = rxbuff; //inizio del buffer di ricezione
    int waited = 5;     //pacchetti ACK e NAK hanno 5 bytes
    int received = 0;

    while(1)
    {
        //nota: read timeout 3 sec
        int ret = read(serial_fd, ptr, waited);
        //printf("serial read: %d\n", ret);
        if(ret < 0) {
            printf("read error %d: %s\n", errno, strerror(errno));
            return -1;
        }
        //timeout 3"
        else if(ret == 0) {
            printf("read timeout\n");
            fflush(stdout);
            return -1;
        }
/*{
int i;
printf("Ricevuti: %d bytes ", ret);
for(i=0; i<ret; i++){
  printf(" %02X", ptr[i]);
}
printf("\n");
}*/
        received  += ret;
        ptr       += ret;
        //arrivati 5 caratteri pacchetti ACK o NACK
        if(received >= waited)
        {
#ifdef DEBUG_DUMP
            printf("Arrivati: %X %X %X %X %X \n",
                   rxbuff[0],rxbuff[1],rxbuff[2],rxbuff[3],rxbuff[4]);
#endif

            //controllo se pacchetto valido e se ho un ACK
            if(rxbuff[0] == 0xAA && rxbuff[1] == 0x55 &&
               rxbuff[3] == 0x33 && rxbuff[4] == 0xCC)
            {
                int retCode = rxbuff[2];
                //printf("retCode: %d\n", retCode);
                if(retCode == respCode /*SW_UPD_ACK*/)
                    return 0;
            }
            return -1;
        }
    }
}

//-----------------------------------------------------------------------
int sendFile(const char * filename, enum e_dest dest)
{
    int fp;
    unsigned char chunk_buff[256];
    int last_chunk;

    printf("Open file %s\n", filename);
    fp = open(filename, O_RDONLY);
    if(fp < 0) {
        printf("File open error %d: %s\n", errno, strerror(errno));
        return -1;
    }

    //calculate the number of chunks
    struct stat st;
    stat(filename, &st);
    long int fsize = st.st_size;        //leggo il size
    int totChunks = fsize / 256;        //qui perdo i decimali
    if(totChunks % 256) totChunks++;    //quindi se ultimo chunk parziale lo conteggio
    printf("File size: %ld totChunks: %d\n", fsize, totChunks);

    //---------------- Transmit loop -----------------------------
    chunk_num = 0;
    last_chunk = 0;

    do {
        //vedo se ultimo chunk
        if(chunk_num == (totChunks - 1)) {
            chunk_num = 0xFFFF;
            last_chunk = 1;  //fine download
            printf("-- LAST CHUNK - ");
        }
        else
            printf("-- chunk n. %d - ", chunk_num);

        memset(chunk_buff, 0xFF, 256);

        //leggo il chunk dal file
        int ret = read(fp, chunk_buff, 256);
        if(ret < 0) {
            printf("file chunk read error %d: %s\n", errno, strerror(errno));
            return -1;
        }
        printf("bytes letti dal file: %d\n", ret);

        //flushes data received but not read
        tcflush(serial_fd, TCIFLUSH);

#ifndef DEBUG_MODE
        int numRetry = 3;
        while(1)
        {
            send_chunk(chunk_buff, chunk_num, dest);

            //attendo ACK
//printf("attendo ACK\n");
            ret = get_response(SW_UPD_ACK);
//printf("get_response return %d\n", ret);
            if(ret < 0) {
                if(--numRetry == 0) {
                    printf("-- File transfer error - aborted\n");
                    return -1;
                }
                printf("-- chunk %d transfer error - retry: %d\n", chunk_num, numRetry);
                usleep(20000); //pausa prima di ritentare
            }
            else {
                chunk_num++;
                break;
            }
            fflush(stdout);
        }
#else
        send_chunk(chunk_buff, chunk_num, dest);
        chunk_num++;
#endif
    } while(!last_chunk);

    printf("\n---- File transfer completed -----\n\n");
    return 0;
}

//-----------------------------------------------------------------------
int forceBooloaderMode()
{
    //pacchetto
    txbuff[0] = 0xAA;   //header
    txbuff[1] = 0x55;
    txbuff[2] = 3;      //COMMAND (3)
    txbuff[3] = 5;      //CMD_ID=5 (forza bootloader)
    txbuff[4] = 0x33;   //tail
    txbuff[5] = 0xCC;

    //invio pacchetto
    int ret = write(serial_fd, txbuff, 6);
    if(ret < 0) {
        printf("write error: %s\n", strerror(errno));
        fflush(stdout);
        return -1;
    }
    return 0;
}

//-----------------------------------------------------------------------
int waitBootloaderMode(void)
{
    int numRetry = 3;
    do {
        //attendo segnale LPC è in bootloader mode con timeout 3"
        int ret = get_response(LPC_BOOTL);
        if(ret < 0) {
            printf("Force LPC in bootloader mode...\n");
            fflush(stdout);
            forceBooloaderMode();

            //LPC resettando chiude la USB VC, devo chiudere anche io
            close(serial_fd);
            return 1;
        }
        else {
            printf("LPC is in bootloader mode\n");
            fflush(stdout);
            return 0;
        }
    } while(--numRetry);

    printf("\nLPC bootloader mode timeout\n");
    return -1;
}

//-----------------------------------------------------------------------
int imageGet(void)
{
  int     fd;
  int     rb;
  int     wb;
  size_t  ofs;
  uint8_t buf[1024];
  int     dstfd;
  int     ret;
  char   *imgfile = "/mnt/usb/recovery/image";
  char   *isofile = "/home/root/update.iso";
  //char   *imgfile = "image";
  //char   *isofile = "/home/max/update.iso";
  //char   *isofile = "update.iso";

  mkdir("/mnt/usb", 0777);
  ret = mount("/dev/sda1", "/mnt/usb", "vfat", 0, "");
  if(ret){
    printf("ERROR:Mount /mnt/usb FAILED\n");
    return -1;
  }
  printf("Mount created at /mnt/usb...\n");

  fd = open(imgfile, O_RDONLY);
  if(fd<0){
    printf("File open error %d: %s\n", errno, strerror(errno));
    umount("/mnt/usb");
    return -1;
  }
  rb = read(fd, (void*)&imgHdr, sizeof(sImgHdr));
  if(rb!=sizeof(sImgHdr)){
    printf("Error reading image file header\n");
    umount("/mnt/usb");
    return -1;
  }

  printf( "Image Header\n"
          " version %08X\n"
          " img_pos %08X\n"
          " img_len %08X\n"
          " dtb_pos %08X\n"
          " dtb_len %08X\n"
          " rfs_pos %08X\n"
          " rfs_len %08X\n",
          imgHdr.version,
          imgHdr.img_pos,
          imgHdr.img_len,
          imgHdr.dtb_pos,
          imgHdr.dtb_len,
          imgHdr.rfs_pos,
          imgHdr.rfs_len);

  // copia iso in home
  ofs = imgHdr.rfs_pos + imgHdr.rfs_len;
  ofs = (ofs+127) & ~127;
printf("ofs: %d %08X\n", ofs, ofs);
  ret = lseek(fd, ofs, SEEK_SET);
printf("retofs: %08X\n", ret);
  if(ret!=ofs){
    printf("ERROR: lseek %d %d %d %s\n", ret, ofs, errno, strerror(errno));
    close(fd);
    return -1;
  }

  printf("open %s\n", isofile);
  dstfd = open(isofile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if(dstfd<0){
    printf("ERROR:open %s\n", isofile);
    close(fd);
    return -1;
  }
int size=0;
  for(;;){
    rb = read(fd, (void*)buf, 1024);
    if(rb<=0){
    printf("ERROR: read %d %d %s\n", rb, errno, strerror(errno));
      break;
    }
    wb = write(dstfd, buf, rb);
    if(wb!=rb){
      printf("errore scrittura\n");
      close(fd);
      close(dstfd);
      return -1;
    }
    size += rb;
  }
  close(dstfd);
  close(fd);
printf("copied %d %08X\n", size, size);

  // monta la iso in /mnt/iso
  mkdir("/mnt/iso", 0777);
  sprintf(buf, "mount %s /mnt/iso", isofile);
  ret = system(buf);
  if(ret){
    printf("ERROR: mounting iso image %d %d %s\n", ret, errno, strerror(errno));
    return -1;
  }

//  printf("Unmount /mnt/usb\n");
//  umount("/mnt/usb");
  return 0;
}

//-----------------------------------------------------------------------
unsigned long version_get()
{
  unsigned long version = -1;
  int fd;
  int rb;
  int ret;

  mkdir("/mnt/usb", 0777);
  ret = mount("/dev/sda1", "/mnt/usb", "vfat", 0, "");
  if(ret){
    return -1;
  }

  printf("Mount created at /mnt/usb...\n");

  fd = open("/mnt/usb/recovery/image", O_RDONLY);
  if(fd<0){
    printf("File open error %d: %s\n", errno, strerror(errno));
    umount("/mnt/usb");
    return -1;
  }
  rb = read(fd, (void*)&version, 4);
  if(rb!=4){
    printf("Error reading version\n");
  }
  close(fd);

  printf("Unmount /mnt/usb\n");
  umount("/mnt/usb");

  return version;
}

//-----------------------------------------------------------------------
int main(void)
{
    unsigned long version;
    int ret;

    printf("\n-----------------------------\n"
             " Firmware download utility\n"
             "-----------------------------\n");
    printf("Compiled: "__DATE__ " " __TIME__ "\n\n");

/*
 divide il file /recovery/image estrae update.iso e la monta in /mnt/iso
    ret = imageGet();
    if(ret){
        printf("image get FAIL\n");
        return -1;
    }
 */
/**/    imgHdr.version = version_get();/**/
    printf("Upgrade version: %08X\n", imgHdr.version);

    fflush(stdout);

    ret = openSerialPort();
    if(ret < 0) {
        printf("Serial comm open error\n");
        return -1;
    }

    printf("Wait for LPC bootloader mode\n");
    ret = waitBootloaderMode();
    if(ret < 0){
        goto err;
    //ho chiuso la seriale, la riapro per proseguire la comunicazione
    }else if(ret == 1) {
        sleep(2);
        printf("riapro la porta comm\n");
        fflush(stdout);
        ret = openSerialPort();
        if(ret < 0) {
            printf("- Serial comm open error\n");
            return -1;
        }
        ret = waitBootloaderMode();
        if(ret != 0)
            goto err;
    }

    printf("Upgrade FPGA\n");
    ret = sendFile(FPGA_SW_UPDATE_FILE, DEST_FPGA);
    if(ret < 0) goto err;

    printf("Upgrade LPC\n");
    ret = sendFile(ARM_SW_UPDATE_FILE, DEST_LPC);
    if(ret < 0) goto err;

    /* aggiornamento file versione */
    printf("Update Version file\n");
    printf("mount EMMC VFAT partition\n");

    ret = mount("/dev/mmcblk1p1", "/mnt/p1", "vfat", 0, "");
    if(ret == 0){
      int fd;

      printf("Mount created at /mnt/p1...\n");

      fd = open("/mnt/p1/version", O_WRONLY | O_CREAT | O_TRUNC);
      if(fd>=0){
        write(fd, (void*)&imgHdr.version, 4);
        close(fd);
      }else{
        printf("File open error %d: %s\n", errno, strerror(errno));
      }

      printf("Unmount /mnt/p1\n");
      umount("/mnt/p1");
      printf("Sync\n");
      sync();
      sleep(2);
    }else{
      printf("Mount /mnt/p1 failed\n");
    }

    send_reset();

    return 0;

err:
    close(serial_fd);
    printf("File transfer error\n");
    return -1;
}
