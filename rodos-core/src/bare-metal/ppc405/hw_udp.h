/**
* @file hw_udp.h
* @date 2008/07/15 16:35
* @author Sergio Montenegro, Lutz Dittrich
*
* Copyright 2008 DLR
*
* @brief UDP communication: only stubs on ppc
*/

#ifndef __HW_UDP_H__
#define __HW_UDP_H__

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
*  UDP interface for Linux platform, using sockets
*/

/**
 *    UDP input port for linux
 */
class UDPReceiver {
  public:
    /** Constructor
     * Warning: negativ port number menans more than one can receive on the same
     * @param portNr port number on localhost used for reception , negativ for boradcast
     */
    UDPReceiver(const int port) { }
    /** Destructor */
    ~UDPReceiver() { }

    /** Opens a socket for UDP reception.
     * On success attribute 'initialised' is set true.
     * Warning: negativ port number menans more than one can receive on the same
     * @param portNr port number on localhost, negative for broadcast
     */
    void reopen(const int port) { }
 
    void setAsync(Topic<GenericMsgRef>* associatedTopic) { }

    /**
     * Receives data from Linux UDP socket. Writes data up to maxLen to userData.
     * @param[IN/OUT] userData pointer to input buffer
     * @param[IN] size of input buffer
     * @return length of message written to userData
     */
    long get(void* userData, const unsigned int maxLen=1300) { return 0; }
    long get(void* userData, int maxLen, unsigned long *ipaddr) { return 0; } 
    bool readyToGet() { return false; }
    bool isInitialised()    { return true; } 
};


/**
*	output UDP port for linux
*/
class UDPTransmitter {
  public:
    UDPTransmitter(const int port,  const char *host = "localhost") { }
    UDPTransmitter(const long _portNr, unsigned long _ipAddr) { } 
    UDPTransmitter(const long _portNr, int ip0, int ip1, int ip2, int ip3) { }
    virtual ~UDPTransmitter() { }

    /** Send it as datagram contining "userdata", default length = all bytes
     * @param data pointer to datafield
     * @param length of data field
     */
    virtual bool send(const void*msg, const unsigned int len) {return true; }

    /** Send datagram to a specified host different to the address used during intialization
     * @param data pointer to datafield
     * @param length of data field
     * @param ipAddr of receiving host
     */
    bool sendTo(const void* userData, const int maxLen, unsigned long ipAddr) {return true;}
    bool isInitialised()    { return true; } // due to windows compatibility

  private:
    void openConnection(const int port,  const char *host) { }
};

/********* Global Fuctions **************/

bool setMyIP(int ip0, int ip1, int ip2, int ip3);
bool setMyIPMask(int ip0, int ip1, int ip2, int ip3);

#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* __HW_UDP_H__ */
