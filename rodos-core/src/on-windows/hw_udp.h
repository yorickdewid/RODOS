

/*********************************************************** Copyright 
 **
 ** Copyright (c) 2008, German Aerospace Center (DLR)
 ** All rights reserved.
 ** 
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 ** 
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 ** 
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 ** 
 ** 3 Neither the name of the German Aerospace Center nor the names of
 **   its contributors may be used to endorse or promote products derived
 **   from this software without specific prior written permission.
 ** 
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **  
 ****************************************************************************/


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

//#include <sys/types.h>
//#include <netinet/in.h>

#include "hw_datatypes.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class UDPReceiver {
private:
  void *m_pvHandle;
  bool IsConnected (void);

public:
  /** Constructor 
   * @param portNr port number on localhost used for reception 
   */
  UDPReceiver(const TUDPPortNr port);
  /** Destructor */
  ~UDPReceiver();

  /** Opens a socket for UDP reception. 
   * On success attribute 'initialised' is set true.
   * @param portNr port number on localhost
   */
  void reopen(const TUDPPortNr port);

  void setAsync(Topic<GenericMsgRef>* associatedTopic);

  /** 
   * Receives data from Linux UDP socket. Writes data up to maxLen to userData. 
   * @param[IN/OUT] userData pointer to input buffer
   * @param[IN] size of input buffer
   * @return length of message written to userData
   */
  long get(void* userData, const unsigned int maxLen=1300);
  long get(void* userData, int maxLen, unsigned long *ipaddr); // return number of bytes read, or 0, or < 0 if error
  bool readyToGet();
  bool isInitialised()    { return IsConnected(); }
};


/**
*	output UDP port for linux
*/
class UDPTransmitter {
private:
  void *m_pvHandle;

private:
  void openConnection(const TUDPPortNr port,  const char *host);
  bool IsConnected (void);

public:
	
  /** init udp communication, can be re-called at any time */
  UDPTransmitter(const TUDPPortNr port,  const char *host = "localhost");
  UDPTransmitter(const long _portNr, unsigned long _ipAddr); // _ipAddr as eg. 192.168.0.20
  UDPTransmitter(const long _portNr, int ip0, int ip1, int ip2, int ip3);
  virtual ~UDPTransmitter();

  /** Send it as datagram contining "userdata", default length = all bytes
   * @param data pointer to datafield
   * @param length of data field
   */
  virtual bool send(const void*msg, const unsigned int len);

  /** Send datagram to a specified host different to the address used during intialization
   * @param data pointer to datafield
   * @param length of data field
   * @param ipAddr of receiving host
   */
  bool sendTo(const void* userData, const int maxLen, unsigned long ipAddr);
  bool isInitialised()    { return IsConnected(); } // due to windows compatibility
};


   
/********* Global Fuctions **************/

bool setMyIP(int ip0, int ip1, int ip2, int ip3); // eg. setMyIP(192,168,20,10);
bool setMyIPMask(int ip0, int ip1, int ip2, int ip3); // eg. setMyIPMask(255,255,0,0);


#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* __HW_UDP_H__ */



