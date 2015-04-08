#include <stdio.h>
#include "udp_common.h"

/*************************************************/

UDPInLx udpIn(7022);


main(int argc, char *argv[]) {
  char inString[100];
  while(1) {
       udpIn.get(inString);
       printf("got %s\n", inString);
  }
}
