#include <stdio.h>              

#include "udp_common.h"


UDPOutLx udpOut(7022, (char*)"localhost");

main(int argc, char *argv[]) {
  
  printf("Sending to localhost:7022\n");
  char inputline[1300];
  while(1) {
    printf("Data to send:");
    gets(inputline);
    
     udpOut.send(inputline, 1300);
  }
}
