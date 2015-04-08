#include <stdio.h>              

#include <sys/types.h>
#include <unistd.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "UdpHW_common.h"


#define REMOTE_PORT 44532

struct hostaddress {
    long ipAddr;
    unsigned char ipAddrOct[50];
    unsigned char name[50];
};

struct hostaddress LOCALHOST = { 0x7f000001, "127.0.0.1", "localhost"};
struct hostaddress REMOTEHOST = { 0x81f73341, "129.247.51.65", "bssc052n.intra.dlr.de"};
struct hostaddress REMOTEHOST2 = { 0x81f73340, "129.247.51.64", "bssc052nv.intra.dlr.de"};


UDPOutLx udpOut(5001);
//UDPOutLx udpOut(5001, "localhost");
//UDPOutLx udpOut(REMOTE_PORT, "localhost");


/** 
 * This is a small testroutine in order to demonstrate sending data via 
 * the UDP abstract interface with variable IP-address and port.
 */

main(int argc, char *argv[]) {
  
    bool sendFlag;
    xprintf("Sending data to port %d\n", REMOTE_PORT);
    char inputline[UDPOutLx::MAXLEN_UDP];
    while(1) {
	xprintf("Data to send:");
	fgets(inputline, UDPOutLx::MAXLEN_UDP, stdin);
	// Send first to address and port as initialized at startup
	sendFlag = udpOut.send(inputline, UDPOutLx::MAXLEN_UDP);
	if (sendFlag == false) {
	    // Data was not send, perhaps hostaddress was invalid.
	    xprintf("Warning: Data not sent to default host!\n");
	}
	// Send to a different host an different host than before.
	sendFlag = udpOut.sendVia(inputline, UDPOutLx::MAXLEN_UDP, REMOTE_PORT, REMOTEHOST2.ipAddr);
	if (sendFlag == false) {
	    // Data was not send, perhaps hostaddress was invalid.
	    xprintf("Warning: Data not sent to %s!\n", REMOTEHOST2.name);
	}
	sendFlag = udpOut.sendTo(inputline, UDPOutLx::MAXLEN_UDP, LOCALHOST.ipAddr);
	if (sendFlag == false) {
	    // Data was not send, perhaps hostaddress was invalid.
	    xprintf("Warning: Data not sent to %s!\n", LOCALHOST.name);
	}
	 
    }
}
