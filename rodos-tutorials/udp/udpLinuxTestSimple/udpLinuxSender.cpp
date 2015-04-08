
#include "UdpHW_common.h"

UDPOutLx udpOut(-5001, "localhost"); // minus for multicast to this port

main(int argc, char *argv[]) {
  
  printf("Sending to localhost:xxxx\n");
  char inputline[1300];
  while(1) {
    printf("Data to send:");
    gets(inputline);
    
     udpOut.send(inputline, 1300);
  }
}
