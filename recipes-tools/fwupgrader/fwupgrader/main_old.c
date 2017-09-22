/**
 */
#include <stdio.h>
#include <string.h>

#include "uart.h"

/**
 */
int main(int argc, char** argv)
{
  char     *device = "/dev/ttyACM0";
  int       uart;
  char      buffer[4096];
  int       wb;
  int       rb;
  int       len;
  int       i;
  unsigned short         j;
  int       bps;
  char      line[4096];
  char      rx_buff[5];
  unsigned char chksum = 0;

  printf("ACM test test version 1.0 "__DATE__" "__TIME__"\n");

  // open UART
  bps = 115200;
  printf("Open uart %s @ %d bps\n", device, bps);
  uart = uartOpen(device, bps);
  if(device<0){
    printf("ERROR while open %s\n", device);
    return -1;
  }

  for(;;){
    printf("Digitare il numero di caratteri da inviare\n");
    
    if(fgets(line, sizeof(line), stdin)==NULL){
      break;
    }
    len = atoi(line);
    if(!len){
      break;
    }

    for (j=0; j<len; j++)
    {
        buffer[0] = 0xAA;
        buffer[1] = 0x55;
        buffer[2] = 50;
        
        if (j==(len-1))
        {
           buffer[3] = 0xFF;
           buffer[4] = 0xFF;
        }
        else
        {
            buffer[3] = j&0xFF;
            buffer[4] = j>>8;
        }

        for(i=5; i<261; i++){
                 buffer[i] = i-5;
                 chksum += buffer[i];
        }
        buffer[i] = ~chksum;
        buffer[i+1] = 0x33;
        buffer[i+2] = 0xCC;
        
        wb = write(uart, buffer, 264);
        if(wb!=264){
            printf("ERROR:writing on %s\n", device);
        }
        
        do
        {
          rb = read(uart, rx_buff, 5);
        }while(rb == 0)
        
        if (rb < 5 || rx_buff[2] != 0x52)
        {
            printf("ERROR:NACK\n");
        }
    }
    
  }

  uartClose(uart);
  return 0;
}
