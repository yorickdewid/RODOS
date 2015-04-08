

/**
 *  UDP Interface for Linux platfrom, using Sokets
 */


#include <netdb.h>


/**
 *	UDP imput port for linux
 */


class UDPInLx {
	int inputSocket;
	sockaddr_in inputAddr;
        bool isInitialized;
public:
	UDPInLx(int portNr);
	bool get(void* userData, int maxLen=1300);
        bool isConnected() { return isInitialized; }
};



class UDPOutLx {
	hostent *hp;
	int outputSocket;
	sockaddr_in outputAddr;
public:
	/// init UDP communication, can be re-called at any time
	UDPOutLx(long receiverPort, char *receiverHost = (char*)"localhost");
	// Send it as datagrm contining "userdata", dafault length = all bytes
	bool send(void* data, int len=1300);
};



