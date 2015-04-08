/*****************************************************************************
Copyright:   DLR RM-IS Berlin Adlerhof  
             The copyright of this software source code is the property of 
             RM-IS. The program(s) may be used and/or copied only with the 
             written permission of RM-IS and in accordance with the terms and 
             conditions stipulated in the agreement/contract under which the 
             software has been supplied. 
******************************************************************************
File       : WinSocket.h
Author     : MS - Martin Schlicker
Description: Include file for RODOS Socket
             Inspired by Socket Classes from Vijay Mathew Pandyalakal
******************************************************************************
Modify list: 
08.11.2011   MS start implementation
*****************************************************************************/

#ifndef WIN_SOCKET_H
#define WIN_SOCKET_H


#ifndef NO_RODOS_NAMESPACE
#ifndef RODOS_NAMESPACE_ALREADY_SET
namespace RODOS {
#endif
#endif

#define ERR_BASE_BASEIF_SOCK  0x1000

const long ERRSOCK_NO_INIT               = -(ERR_BASE_BASEIF_SOCK + 0x01);
const long ERRSOCK_CONNECTION_DESRUPTED  = -(ERR_BASE_BASEIF_SOCK + 0x02);
const long ERRSOCK_CONNECT               = -(ERR_BASE_BASEIF_SOCK + 0x03);
const long ERRSOCK_CONNECT_ILL_ADDR      = -(ERR_BASE_BASEIF_SOCK + 0x04);
const long ERRSOCK_CONNECT_RESOLVE_HOST  = -(ERR_BASE_BASEIF_SOCK + 0x05);
const long ERRSOCK_SOCKET_CREATE         = -(ERR_BASE_BASEIF_SOCK + 0x06);
const long ERRSOCK_PORT_BIND             = -(ERR_BASE_BASEIF_SOCK + 0x07);
const long ERRSOCK_ILLEGAL_PORT          = -(ERR_BASE_BASEIF_SOCK + 0x08);
const long ERRSOCK_ILLEGAL_ADRESS        = -(ERR_BASE_BASEIF_SOCK + 0x09);
const long ERRSOCK_NO_PORT_BIND          = -(ERR_BASE_BASEIF_SOCK + 0x0A);
const long ERRSOCK_ACCEPT                = -(ERR_BASE_BASEIF_SOCK + 0x0B);
const long ERRSOCK_ACCEPT_BIND           = -(ERR_BASE_BASEIF_SOCK + 0x0C);
const long ERRSOCK_ACCEPT_LISTEN         = -(ERR_BASE_BASEIF_SOCK + 0x0D);
const long ERRSOCK_ACCEPT_ILL_CLIENTSOCK = -(ERR_BASE_BASEIF_SOCK + 0x0E);
const long ERRSOCK_SEND                  = -(ERR_BASE_BASEIF_SOCK + 0x0F);
const long ERRSOCK_RECEIVE               = -(ERR_BASE_BASEIF_SOCK + 0x10);
const long ERRSOCK_RECEIVE_TIMEOUT       = -(ERR_BASE_BASEIF_SOCK + 0x11);


#ifndef WIN_SOCKET_UDP_CPP
  typedef void t_WinSocket_UDP_LocalVars;
#endif

// ***************************************************************************
// Author    : MS
// Date      : 08.11.2011
// Descript  : socket-class to transfere messages per UDP
//             usLocalPort:       local port to send/receive messages
//             pchLocalAdress:    local IP-adress of a dedicated interfacecard
//             bEnableBroadcasts: true=broadcasts can be send. 
//                                A destination broadcast-adress is:
//                                x.y.z.255, x.y.255.255, x.255.255.255, 255.255.255.255
// usPort | pchAdress || Remark
//--------|-----------++--------------------------------------
//    0   |    NULL   || If first message will be send, port and adress are set automaticly by OS
//    0   | "a.b.c.d" || If first message will be send, port            is  set automaticly by OS 
//  >=1   |    NULL   || Receives messages on dedicated port and any interface-card
//  >=1   | "a.b.c.d" || Receives messages on dedicated port and     interface-card

class WinSocket_UDP
{
  public:
    WinSocket_UDP (long *plOk, unsigned short usLocalPort=0,
                    char *pchLocalAdress=0, bool bEnableBroadcasts=false);
    ~WinSocket_UDP();

    long GetLocalAddress (char *pchIPAdress, unsigned short *pusPort);

    static unsigned long IPAdress2Value (char         *pchIPAdress);
    static char         *Value2IPAdress (unsigned long ulValue);

    // Send datablock
    // pchToAdress: send packet to adress
    // usToPort:    send packet to port
    long Send (char *pchData, long lSize,   
               unsigned short usToPort, char *pchToAdress);

    // Read Data from UDP-socket
    // pchFromAdress: (return-value) packet received from source-adress
    // usFromPort:    (return-value) packet received from source-port
    // ui64Timeout:   ==0: wait for ever, ==1: nowait, >1 waittime
    long Read (char *pchBuffer, long lSize,  // Reads data from the UDP-socket
               unsigned short *pusFromPort=0, char *pchFromAdress=0, long lTimeout_ms=1);  

  public:
    static char m_achIPAdress[128];

  private:
    t_WinSocket_UDP_LocalVars *m_pcLocalVars;
    bool m_bInitOK;

  private:
    long OpenSocket (void);
};

// ***************************************************************************


#ifndef NO_RODOS_NAMESPACE
#ifndef RODOS_NAMESPACE_ALREADY_SET
}
#endif
#endif

#endif  // WIN_SOCKET_H

