

/**
 *  UDP Interface for Linux platfrom, using Sokets
 */

#include "udp_common.h"

#include <sys/types.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>





/**
 *	UDP imput port for linux
 */

/*************************************************/

UDPInLx::UDPInLx(int portNr) {
	isInitialized = false;

	/* Create socket from which to receive. */
	inputSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (inputSocket < 0)  return;

	/* Create name with wildcards. */
	inputAddr.sin_family = AF_INET;
	inputAddr.sin_addr.s_addr = INADDR_ANY;
	inputAddr.sin_port =  htons(portNr);   // 0 to get any port
	int ok = bind(inputSocket, (sockaddr*)&inputAddr, sizeof(inputAddr));
        if(ok < 0) return;

	/* Find assigned port value and print it out. */
	socklen_t len =  sizeof(inputAddr);
	getsockname(inputSocket, (sockaddr*)&inputAddr, &len);

	/** Set to nonblocking: no wait if no data is ready */
	//fcntl(inputSocket,F_SETFL, O_NONBLOCK);
        isInitialized = true;
}


/***********************************************/

bool UDPInLx::get(void* userData, int maxLen) {
	int errcode =  read(inputSocket, userData, maxLen);
	if(errcode <= 0) return false;
	return true;
}



/***********************************************************************************************/



/**
 *	Output UDP ports for Linux
 */


/*************************************************/

UDPOutLx::UDPOutLx(long receiverPort, char *receiverHost) {


	outputSocket= socket(AF_INET, SOCK_DGRAM, 0);
	hp = gethostbyname(receiverHost);

	memcpy(&outputAddr.sin_addr, hp->h_addr, hp->h_length);
	outputAddr.sin_family = AF_INET;
	outputAddr.sin_port = htons(receiverPort);
}


/*************************************************/


bool UDPOutLx::send(void* data, int len) {

	sendto(outputSocket, data, len, 0, (sockaddr*)&outputAddr, sizeof(outputAddr));
	return true;
}


/**********************************************/






