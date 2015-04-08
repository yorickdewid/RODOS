/**
* @file hw_udp.h
* @date 2008/07/15 16:35
* @author Sergio Montenegro, Lutz Dittrich
*
* Copyright 2008 DLR
*
* @brief UDP communication via sockets (Linux)
*/

#ifndef __HW_UDP_H__
#define __HW_UDP_H__


#include "hw_datatypes.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
*  UDP interface for Linux platform, using sockets
*/


/**
*	UDP input port for linux
*/
class UDPReceiver {

private:
  int sock;
  //sockaddr_in inputAddr;

public:
  /** Constructor */
  UDPReceiver(const TUDPPortNr port);
  ~UDPReceiver();
  void reopen(const TUDPPortNr port);
  void setAsync(Topic<GenericMsgRef>* associatedTopic);  
  long get(void* userData, const unsigned int maxLen=1300);
  long get(void* userData, int maxLen, unsigned long *ipaddr); // return number of bytes read, or 0, or < 0 if error
  bool readyToGet();
  bool isInitialised()    { return initialised; } // due to windows compatibility
private:
  bool initialised;
};


/**
*	output UDP port for linux
*/
class UDPTransmitter {
private:
  int sock;
  struct hostent* hp;
  //sockaddr_in outputAddr;

public:
  /** init udp communication, can be re-called at any time */
  UDPTransmitter(const TUDPPortNr port,  const char *host = "localhost");
  // UDPTransmitter(const long _portNr); // hostname, or ipaddress if you prefer
  UDPTransmitter(const long _portNr, unsigned long _ipAddr); // _ipAddr as eg. 192.168.0.20
  UDPTransmitter(const long _portNr, int ip0, int ip1, int ip2, int ip3);
  virtual ~UDPTransmitter();

  /** send it as datagram containing "userdata", default length = all bytes */
  virtual bool send(const void*msg, const unsigned int len);
  bool sendTo(const void* userData, const int maxLen, unsigned long ipAddr);
  bool isInitialised()    { return initialised; } // due to windows compatibility
private:
  bool initialised;
};



/********* Global Fuctions **************/

bool setMyIP(int ip0, int ip1, int ip2, int ip3); // eg. setMyIP(192,168,20,10);
bool setMyIPMask(int ip0, int ip1, int ip2, int ip3); // eg. setMyIPMask(255,255,0,0);


#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* __HW_UDP_H__ */



