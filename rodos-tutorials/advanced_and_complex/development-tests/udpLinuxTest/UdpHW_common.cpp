

/**
 *  UDP Interface for Linux platform, using Sokets
 */

#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "UdpHW_common.h" // Definitions and headers needed for networking



UDPInLx::UDPInLx(int portNr) {


	/* Create socket from which to receive. */
	inputSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (inputSocket < 0)  return;

	/* Create name with wildcards. */
	inputAddr.sin_family = AF_INET;         // IPv4
	inputAddr.sin_addr.s_addr = INADDR_ANY; // allow external and internal address
	inputAddr.sin_port =  htons(portNr);    // TBD use portNr = 0 to get any port
	bind(inputSocket, (sockaddr*)&inputAddr, sizeof(inputAddr));

	/* Find assigned port value and print it out. */
	socklen_t len =  sizeof(inputAddr);
	getsockname(inputSocket, (sockaddr*)&inputAddr, &len);

	/** Set to nonblocking: no wait if no data is ready */
	fcntl(inputSocket,F_SETFL, O_NONBLOCK);
}


/***********************************************/

bool UDPInLx::get(void* userData, int maxLen) {
	int errcode =  read(inputSocket, userData, maxLen);
	if(errcode <= 0) return false;
	return true;
}

/***********************************************/

bool UDPInLx::get(void* userData, int maxLen, unsigned long *ipAddr) {

    sockaddr_in sendAddr;
    hostent* sendHost;
    unsigned long inAddr;

    int ip0, ip1, ip2, ip3;   // Octets of sender address
    char sendHostAddress[50];  // Output address
    
    // Reset sender address information
    memset(&sendAddr, 0, sizeof(sendAddr) );
    sendAddr.sin_family = AF_INET;    // IPv4 
    sendAddr.sin_port = htons(0);     // Receive from any port
    int sendAddrLen = sizeof(sendAddr);
    
    // printf("sendhost: %x \n", sendHost->h_addr);
    int errcode =  recvfrom(inputSocket, userData, maxLen, 0, (sockaddr *) &sendAddr, (socklen_t *) &sendAddrLen);

    if (errcode > 0) {
	*ipAddr = ntohl(sendAddr.sin_addr.s_addr);
	inAddr = ntohl(sendAddr.sin_addr.s_addr);
	// Convert output address to IPv4 dotted decimal notation
	ip3 = inAddr & 0xff; 	inAddr >>= 8;
	ip2 = inAddr & 0xff; 	inAddr >>= 8;
	ip1 = inAddr & 0xff; 	inAddr >>= 8;
	ip0 = inAddr & 0xff; 	inAddr >>= 8;
	xprintf("sendHostAddress %d.%d.%d.%d\n", ip0, ip1, ip2, ip3);
    }

    if(errcode <= 0) {
	return false;
    }
    return true;
}



/***********************************************************************************************/


UDPOutLx::UDPOutLx(long receiverPort, char *receiverHost) {


	outputSocket= socket(AF_INET, SOCK_DGRAM, 0);
	hp = gethostbyname(receiverHost);

	memcpy(&outputAddr.sin_addr, hp->h_addr, hp->h_length);
	outputAddr.sin_family = AF_INET;
	outputAddr.sin_port = htons(receiverPort);

	// 
	xprintf("Established connection to port %ld at %s\n", receiverPort, receiverHost);
}


/*************************************************/


bool UDPOutLx::send(void* data, int len) {


    int sendval = sendto(outputSocket, data, len, 0, (sockaddr*)&outputAddr, sizeof(outputAddr));
    if (sendval == -1) {
	return false;
    } else {
	xprintf("Sending %s \n", (char*)data);
	return true;
    }
}


/**********************************************/

/** 
 * An example how to utilize the sendto() functionality.
 * Here a message is broadcasted to a host not known at compile time but through a
 * pre-defined port.
 * The host address is provided as long.
 */ 
bool UDPOutLx::sendTo(const void* data, const int maxLen, unsigned long ipAddr) {

    // Here we establish a connection procedure like in send, but connecting to another host.
    hostent* hp_;
    sockaddr_in outputAddr_;
    
    int ip0, ip1, ip2, ip3;   // Octets of output address
    char remHostAddress[50];  // Output address

    // TODO Check for message length
    if (maxLen > MAXLEN_UDP) {
	return false;
    }

    // Convert output address to IPv4 dotted decimal notation
    ip3 = ipAddr & 0xff; 	ipAddr >>= 8;
    ip2 = ipAddr & 0xff; 	ipAddr >>= 8;
    ip1 = ipAddr & 0xff; 	ipAddr >>= 8;
    ip0 = ipAddr & 0xff; 	ipAddr >>= 8;
    sprintf(remHostAddress, "%d.%d.%d.%d", ip0, ip1, ip2, ip3);

    // Copy host address into structure hp
    hp_ = gethostbyname(remHostAddress); 

    // Fill in remote host address structure
    memset(&outputAddr_, 0, sizeof(outputAddr_) );
    memcpy(&outputAddr_.sin_addr, hp_->h_addr, hp_->h_length);
    outputAddr_.sin_family = AF_INET;    
    outputAddr_.sin_port = outputAddr.sin_port;

    // Send to a different host, but the same socket.
    int sendval = sendto(outputSocket, data, maxLen, 0, (sockaddr*) &outputAddr_, sizeof(outputAddr_));
    if (sendval == -1) {
	return false;
    } 
    return true;
}


/** 
 * An example how to utilize the sendto() functionality.
 * Here a message is broadcasted to a port on a remote host not known at compile time.
 * Note, the host address is provided as long.
 */ 
bool UDPOutLx::sendVia(const void* data, const int maxLen, unsigned long portNr, unsigned long ipAddr) {

    // Here we establish a connection procedure like in send, but with a variable host.
    hostent* hp_;
    sockaddr_in outputAddr_;
    
    int ip0, ip1, ip2, ip3;   // Octets of output address
    char remHostAddress[50];  // Output address

    // TODO Check for message length
    if (maxLen > MAXLEN_UDP) {
	return false;
    }

    // Convert output address to IPv4 dotted decimal notation
    ip3 = ipAddr & 0xff; 	ipAddr >>= 8;
    ip2 = ipAddr & 0xff; 	ipAddr >>= 8;
    ip1 = ipAddr & 0xff; 	ipAddr >>= 8;
    ip0 = ipAddr & 0xff; 	ipAddr >>= 8;
    sprintf(remHostAddress, "%d.%d.%d.%d", ip0, ip1, ip2, ip3);

    // Copy host address into structure hp
    hp_ = gethostbyname(remHostAddress); 

    // Fill in remote host address structure
    memset(&outputAddr_, 0, sizeof(outputAddr_) );
    memcpy(&outputAddr_.sin_addr, hp_->h_addr, hp_->h_length);
    outputAddr_.sin_family = AF_INET;    
    outputAddr_.sin_port = htons(portNr);

    // Send to a specified port and host, but the same socket.
    int sendval = sendto(outputSocket, data, maxLen, 0, (sockaddr*) &outputAddr_, sizeof(outputAddr_));
    if (sendval == -1) {
	return false;
    } else {
	return true;
    }
}
