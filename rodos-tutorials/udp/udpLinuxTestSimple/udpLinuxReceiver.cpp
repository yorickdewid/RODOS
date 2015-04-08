
#include "UdpHW_common.h"

/*************************************************/


UDPInLx udpIn(-5001);  // negatrive to allow other to hear the same port
char  userData[1300];

main(int argc, char *argv[]) {

  printf("UDPInMsg Tester 3.5 waiting\n");

  long waiting = 0;
  while(1) {

    bool ok = udpIn.get(userData);
    if(ok) printf("%ld Data: '%s'\n", waiting, userData);
    else { 
	waiting++;
    	usleep(100000);
    }
  }
}
