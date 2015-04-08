// header for network
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

/**
 * UDP input port interface for linux
 */
class UDPInLx {

	int inputSocket;
	sockaddr_in inputAddr;

public:

	/// Transferring datagrams via ethernet limits the size.
	static const int MAXLEN_UDP = 1300;

	UDPInLx(int portNr);
	/** 
	 * Receives data from UDP socket. Writes data up to maxLen to userData. 
	 * @param[IN/OUT] userData pointer to input buffer
	 * @param[IN] size of input buffer
	 * @return length of message written to userData
	 */
	bool get(void* userData, int maxLen = MAXLEN_UDP);
//	long get(void* userData, int maxLen);

	/** 
	 * Receives data from UDP socket. Writes data up to maxLen to userData. 
	 * @param[IN/OUT] userData pointer to input buffer
	 * @param[IN] size of input buffer
	 * @param[OUT] address of sender
	 * @return length of message written to userData
	 */
	bool get(void* userData, int maxLen, unsigned long* ipAddr);

};



/*************************************************/


/**
 * UDP output port interface for linux
 */
class UDPOutLx {

    hostent *hp;         
    int outputSocket;
    sockaddr_in outputAddr;
    

public:

    /// Transferring datagrams via ethernet limits the size.
    static const int MAXLEN_UDP = 1300;
    
    /// init udp communication, can be re-called at any time
    UDPOutLx(long receiverPort, char* hostName = "localhost");

    /** Send it as datagram contining "userdata", default length = all bytes
     * @param data pointer to datafield
     * @param length of data field
     */
    bool send(void* data, int len = MAXLEN_UDP);

    /** Send datagram to a specified host different to the address used during intialization
     * @param data pointer to datafield
     * @param length of data field
     * @param ipAddr of receiving host
     */
    bool sendTo(const void* data, const int maxLen, unsigned long ipAddr);

    /** Send datagram to a specified port on a host different to the initialized one.
     * @param data pointer to datafield
     * @param length of data field
     * @param portNr of receiving host
     * @param ipAddr of receiving host
     */
    bool sendVia(const void* data, const int maxLen, unsigned long portNr, unsigned long ipAddr);

};
