#include <stdio.h>

#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "UdpHW_common.h"


struct hostaddress {
    long ipAddr;
    unsigned char ipAddrOct[50];
    unsigned char name[50];
};

struct hostaddress LOCALHOST = { 0x7f000001, "127.0.0.1", "localhost"};
struct hostaddress REMOTEHOST = { 0x81f73341, "129.247.51.65", "bssc052n.intra.dlr.de"};
struct hostaddress REMOTEHOST2 = { 0x81f73340, "129.247.51.64", "bssc052nv.intra.dlr.de"};


/*************************************************/
#define REMOTE_PORT 44532
#define INPUT_PORT_NR 5001 // 0
#define PORT_USED INPUT_PORT_NR

UDPInLx udpIn(PORT_USED);

/** 
 * A simple demonstration of an UDP receiver using an abstract UDP interface.
 * Aim is to show that data is received from different hosts, by checking 
 * the senders address.
 */  
main(int argc, char *argv[]) {

  long waiting = 0;
  char  userData[UDPInLx::MAXLEN_UDP];
  char  userDataRemote[UDPInLx::MAXLEN_UDP];
  unsigned long ipAddr;

  xprintf("UDPInMsg Tester 3.5 waitingport %d\n", PORT_USED);


  // Poll the UDP port
  while(1) {

    bool ok;
#if 0
    ok = udpIn.get(userData);
    if(ok) {
	// Write data and look again for new data.
	xprintf("%ld Data: %s\n", waiting, userData);
    	usleep(100);
    }
    else { 
	// No data yet, wait a little.
	waiting++;
    	usleep(100);
    }
    /* 
     * receive data from a separate host. 
     */
    ok = udpIn.get(userDataRemote, UDPInLx::MAXLEN_UDP, REMOTEHOST.ipAddr);
    if(ok) {
	// Write data and look again for new data.
	xprintf("%ld Data from %s: %s\n", waiting, REMOTEHOST.name, userDataRemote);
    	usleep(1000000);
    }
    else { 
	// No data yet, wait a little.
	waiting++;
    	usleep(100000);
    }
#endif
    /* 
     * receive data from a separate host. 
     */
    ok = udpIn.get(userDataRemote, UDPInLx::MAXLEN_UDP, &ipAddr);
    if(ok) {
	// Write data and look again for new data.
	

	xprintf("%ld Data from %lx: %s\n", waiting, ipAddr, userDataRemote);
    	usleep(100000);
    }
    else { 
	// No data yet, wait a little.
	waiting++;
    	usleep(100000);
    }
  }
}
