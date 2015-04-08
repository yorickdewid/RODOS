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

#include "topic.h"
#include "hw_datatypes.h"

/**
*  UDP interface for CM3, using sockets
*/


/**
*	UDP input port for CM3
*/
///To receive data from a single UDP port (socket).
class UDPReceiver {

	//	here you can write all privat data you need for each port to receive data
	private:
		//*****************************************************************************
		//
		// struct of udp configuration 
		//
		//*****************************************************************************
		struct udp_pcb *pcbUdp;
		//*****************************************************************************
		//
		// flag indictae that message receive
		//
		//*****************************************************************************
		static unsigned char rcvFlag;
		//*****************************************************************************
		//
		// length of receive message
		//
		//*****************************************************************************
		static int rcvLen;				
		//*****************************************************************************
		//
		// variable for storage ip_addr of message receive
		//
		//*****************************************************************************
		static struct ip_addr addrRcv;
		//*****************************************************************************
		//
		// bufor fog receive message
		//
		//*****************************************************************************
		static char rcvMsg[1100];
		//*****************************************************************************
		//
		// !This function call up automatically when udp message is received
		// ! /param arg - pointer to argument
		// ! /param pcb - pointer to structure storage udp settings
		// ! /param p - pointer to structure storage udp buffer
		// ! /param addr - pointer to structure storage ip address from message source
		// ! /port - number of port from message source
		// ! /return - none
		//
		//*****************************************************************************

		static void udp_recv_func(void  *arg, struct udp_pcb *pcb, struct  pbuf *p, struct  ip_addr *addr, unsigned short  port);
public:
	/** Constructor */
	UDPReceiver(const long portNr);
  	~UDPReceiver();
   
	void reopen(const long portNr);
	void setAsync(Topic<GenericMsgRef>* associatedTopic);
	long get(void* userData, int maxLen, unsigned long *ipaddr); // return number of bytes read, or 0, or < 0 if error
	long get(void* userData, int maxLen); // return number of bytes read, or 0, or < 0 if error
	bool readyToGet(); // true -> i will get data the next time i call get
//	bool isConnected();
  bool isInitialised()    { return initialised; } // due to windows compatibility
private:
	bool initialised;
};


/**
*	output UDP port for linux
*/
class UDPTransmitter {
  private:
	//*****************************************************************************
	//
	// struct of udp configuration 
	//
	//*****************************************************************************
	struct udp_pcb *pcbUdp;
	//*****************************************************************************
	//
	// variable for storage ip_addr for send message
	//
	//*****************************************************************************
	unsigned long ipAddr;
	//*****************************************************************************
	//
	// variable for storage port number for send message
	//
	//*****************************************************************************
	long portNr;

public:
  /** init udp communication, can be re-called at any time */
    UDPTransmitter(const long _portNr,  const char *host = "localhost"); 
  	UDPTransmitter(const long _portNr); // hostname, or ipaddress if you prefer
  	UDPTransmitter(const long _portNr, unsigned long _ipAddr); // _ipAddr as eg. 192.168.0.20
  	UDPTransmitter(const long _portNr, int ip0, int ip1, int ip2, int ip3);
	~UDPTransmitter();
	bool send(const void* userData, const int maxLen);
	bool sendTo(const void* userData, const int maxLen, unsigned long ipAddr);
//	bool isConnected();
  bool isInitialised()    { return initialised; } // due to windows compatibility
private:
	bool initialised;
};

/********* Global Fuctions **************/

bool setMyIP(int ip0, int ip1, int ip2, int ip3); // eg. setMyIP(192,168,20,10);
bool setMyIPMask(int ip0, int ip1, int ip2, int ip3); // eg. setMyIPMask(255,255,0,0);

#endif /* __HW_UDP_H__ */
