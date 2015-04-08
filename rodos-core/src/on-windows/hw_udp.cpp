

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
* @file hw_udp.cpp
* @date 2011/11/11 12:00
* @author Martin Schlicker
*
* Copyright 2011 DLR
*
* @brief UDP communication via sockets (Windows)
*/
#include "rodos.h"
#include "hw_udp.h"
#include "WinSocket_UDP.h"
#include <stdio.h>
#include <string.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

#define LOCAL_IP_ADRESS  "192.168.5.10"  // "localhost" ist nicht mit externen Netzwerk-Interfaces verbunden
#define CHECK_PORT(P) ((((P) <= 1024) || ((P) > 0xFFFF)) ? 0 : (unsigned short) (P))

typedef struct
{ WinSocket_UDP *pcSock;
  char           achLocalAdr [64];
  char           achRemoteAdr[64];
  unsigned short usLocalPort;
  unsigned short usRemotePort;
  bool           bSendBroadcasts;
} t_UDP_Socket;

// ***************************************************************************

void UDPReceiver::reopen(const TUDPPortNr port) 
{ unsigned short usPort = CHECK_PORT (port);
  t_UDP_Socket  *pcHnd  = (t_UDP_Socket *) m_pvHandle;
  long           lOk;
  
  if (pcHnd->pcSock) 
  { delete pcHnd->pcSock;
    memset (pcHnd, 0, sizeof(t_UDP_Socket));
  }
  if (usPort == 0) return;

  strncpy (pcHnd->achLocalAdr, LOCAL_IP_ADRESS, 64);  pcHnd->achLocalAdr[63] = 0;
  pcHnd->achRemoteAdr[0] = 0;
  pcHnd->usLocalPort     = usPort;
  pcHnd->usRemotePort    = 0;
  pcHnd->bSendBroadcasts = false;
  pcHnd->pcSock          = new WinSocket_UDP (&lOk, pcHnd->usLocalPort, pcHnd->achLocalAdr);

  if (lOk <= 0)
  { delete pcHnd->pcSock;
    memset (pcHnd, 0, sizeof(t_UDP_Socket));
  }
}

// ***************************************************************************

UDPReceiver::UDPReceiver(const TUDPPortNr port) 
{ 
  m_pvHandle = new t_UDP_Socket;
  memset (m_pvHandle, 0, sizeof(t_UDP_Socket));

  reopen (port);
}

// ***************************************************************************

UDPReceiver::~UDPReceiver() 
{ t_UDP_Socket *pcHnd = (t_UDP_Socket *) m_pvHandle;

  if (pcHnd->pcSock) delete pcHnd->pcSock;
  delete pcHnd;
}

// ***************************************************************************

bool UDPReceiver::IsConnected (void)
{ t_UDP_Socket *pcHnd = (t_UDP_Socket *) m_pvHandle;

  return (pcHnd->pcSock) ? true : false;
}

// ***************************************************************************

long UDPReceiver::get (void *userData, const unsigned int maxLen)
{ t_UDP_Socket *pcHnd = (t_UDP_Socket *) m_pvHandle;

  if (pcHnd->pcSock == 0) return -1;

  return pcHnd->pcSock->Read ((char *) userData, (long) maxLen, 0, 0, 1);  // no wait
}

// ***************************************************************************

long UDPReceiver::get (void* userData, int maxLen, unsigned long *ipaddr)
{ t_UDP_Socket  *pcHnd = (t_UDP_Socket *) m_pvHandle;
  long           lByteCount;
  char           achIPAdr[64];
  unsigned long  ulIPAdr;

  if (pcHnd->pcSock == 0) return -1;

  lByteCount = pcHnd->pcSock->Read ((char *) userData, (long) maxLen, 0, achIPAdr, 1);  // no wait
  ulIPAdr    = WinSocket_UDP::IPAdress2Value (achIPAdr);

  if (ipaddr)
  { ipaddr[0] = (ulIPAdr >> 24) & 0xFF;
    ipaddr[1] = (ulIPAdr >> 16) & 0xFF;
    ipaddr[2] = (ulIPAdr >>  8) & 0xFF;
    ipaddr[3] = (ulIPAdr >>  0) & 0xFF;
  }
  return lByteCount;
}

// ***************************************************************************

bool UDPReceiver::readyToGet ()
{ t_UDP_Socket *pcHnd = (t_UDP_Socket *) m_pvHandle;
  
  return (pcHnd->pcSock == 0) ? false : true;
}

// ***************************************************************************
// ***************************************************************************
// ***************************************************************************

void UDPTransmitter::openConnection (const TUDPPortNr port, const char *host) 
{ t_UDP_Socket  *pcHnd  = (t_UDP_Socket *) m_pvHandle;
  unsigned short usPort = CHECK_PORT (port);
  long           lOk;

  if (pcHnd->pcSock) 
  { delete pcHnd->pcSock;
    memset (pcHnd, 0, sizeof(t_UDP_Socket));
  }
  if (usPort == 0) return;

  strncpy (pcHnd->achLocalAdr,  LOCAL_IP_ADRESS, 64);  pcHnd->achLocalAdr [63] = 0;
  strncpy (pcHnd->achRemoteAdr, host,            64);  pcHnd->achRemoteAdr[63] = 0;
  pcHnd->usLocalPort     = 0;
  pcHnd->usRemotePort    = usPort;
  pcHnd->bSendBroadcasts = false;
  pcHnd->pcSock          = new WinSocket_UDP (&lOk, pcHnd->usLocalPort, pcHnd->achLocalAdr, pcHnd->bSendBroadcasts);

  if (lOk <= 0)
  { delete pcHnd->pcSock;
    memset (pcHnd, 0, sizeof(t_UDP_Socket));
  }
}

// ***************************************************************************

UDPTransmitter::UDPTransmitter(const TUDPPortNr port, const char *host)
{ unsigned short usPort = CHECK_PORT (port);

  m_pvHandle = new t_UDP_Socket;
  memset (m_pvHandle, 0, sizeof(t_UDP_Socket));

  openConnection(usPort, host);
}

// ***************************************************************************

UDPTransmitter::UDPTransmitter(const long portNr, unsigned long ipAddr)
{ unsigned short usPort = CHECK_PORT (portNr);
  char achIPAdr[64];

  m_pvHandle = new t_UDP_Socket;
  memset (m_pvHandle, 0, sizeof(t_UDP_Socket));

  strncpy (achIPAdr, WinSocket_UDP::Value2IPAdress (ipAddr), 64);  achIPAdr[63] = 0;

  openConnection(usPort, achIPAdr);
}	

// ***************************************************************************

UDPTransmitter::UDPTransmitter(const long portNr, int ip0, int ip1, int ip2, int ip3) 
{ unsigned short usPort = CHECK_PORT (portNr);
  char achIPAdr[64];

  m_pvHandle = new t_UDP_Socket;
  memset (m_pvHandle, 0, sizeof(t_UDP_Socket));

  sprintf (achIPAdr, "%03ld.%03ld.%03ld.%03ld", ip0, ip1, ip2, ip3);

  openConnection(usPort, achIPAdr);
}

// ***************************************************************************

UDPTransmitter::~UDPTransmitter()
{ t_UDP_Socket *pcHnd = (t_UDP_Socket *) m_pvHandle;

  if (pcHnd->pcSock) delete pcHnd->pcSock;
  delete pcHnd;
}

// ***************************************************************************

bool UDPTransmitter::IsConnected (void)
{ t_UDP_Socket *pcHnd = (t_UDP_Socket *) m_pvHandle;

  return (pcHnd->pcSock) ? true : false;
}

// ***************************************************************************

bool UDPTransmitter::send(const void* msg, const unsigned int len) 
{ t_UDP_Socket *pcHnd = (t_UDP_Socket *) m_pvHandle;
  long          lSendBytes;
  
  if (pcHnd->pcSock == 0) return false;

  lSendBytes = pcHnd->pcSock->Send ((char *) msg, (long) len, 
                                    pcHnd->usRemotePort, pcHnd->achRemoteAdr);
 
  return (lSendBytes > 0) ? true : false; 
}

// ***************************************************************************

bool UDPTransmitter::sendTo(const void* userData, const int maxLen, unsigned long ipAddr) 
{ t_UDP_Socket *pcHnd = (t_UDP_Socket *) m_pvHandle;
  long          lSendBytes;
  char          achIPAdr[64];
  
  if (pcHnd->pcSock == 0) return false;

  strncpy (achIPAdr, WinSocket_UDP::Value2IPAdress (ipAddr), 64);  achIPAdr[63] = 0;
  lSendBytes = pcHnd->pcSock->Send ((char *) userData, (long) maxLen, pcHnd->usRemotePort, achIPAdr);
 
  return (lSendBytes > 0) ? true : false; 
}

// ***************************************************************************



#ifndef NO_RODOS_NAMESPACE
}
#endif
