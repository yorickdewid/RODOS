/*****************************************************************************
Copyright:   DLR RM-IS Berlin Adlerhof
             The copyright of this software source code is the property of 
             RM-IS. The program(s) may be used and/or copied only with the 
             written permission of RM-IS and in accordance with the terms and 
             conditions stipulated in the agreement/contract under which the 
             software has been supplied. 
******************************************************************************
File       : WinSocket_UDP.cpp
Author     : MS - Martin Schlicker
Description: 
******************************************************************************
Modify list: 
08.11.2011   MS start implementation
*****************************************************************************/

#ifndef WIN_SOCKET_UDP_CPP
#define WIN_SOCKET_UDP_CPP

// ***************************************************************************
// INCLUDE

#include <windows.h>
#include <stdio.h>


#ifndef NO_RODOS_NAMESPACE
#define RODOS_NAMESPACE_ALREADY_SET
namespace RODOS {
#endif

typedef struct
{
  SOCKET Socket;
  bool   bBroadcastsEnabled;
  bool   bIsServer;
  unsigned short usPort;
  char           achAdress[256];
}t_WinSocket_UDP_LocalVars;


#include "WinSocket_UDP.h"

char WinSocket_UDP::m_achIPAdress[128] = { 0,0,0,0 };

// ***************************************************************************
// DEFINITION

// ***************************************************************************
// DECLARATION

// ***************************************************************************
// IMPLEMENTATION

// ***************************************************************************
// Author    : MS
// Date      : 08.11.2011
// Descript  : 

unsigned long WinSocket_UDP::IPAdress2Value (char *pchIPAdress)
{
  if (pchIPAdress == NULL) return 0;
  if (pchIPAdress[0] == 0) return 0;

  return inet_addr(pchIPAdress);
}

// ***************************************************************************
// Author    : MS
// Date      : 08.11.2011
// Descript  : 

char *WinSocket_UDP::Value2IPAdress (unsigned long ulValue)
{ in_addr cAdress;

  cAdress.S_un.S_addr = ulValue;

  strncpy (m_achIPAdress, inet_ntoa (cAdress), 128); m_achIPAdress[127] = 0;

  return m_achIPAdress;
}

// ***************************************************************************
// Author    : MS
// Date      : 08.11.2011
// Descript  : 

long WinSocket_UDP::OpenSocket (void)
{ long lSockError;
  long lSetOptError;
  BOOL bBroadcastsEnabled = m_pcLocalVars->bBroadcastsEnabled;

  m_pcLocalVars->Socket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (m_pcLocalVars->Socket == INVALID_SOCKET)
  { lSockError = WSAGetLastError();
    printf("ERROR open socket(): %ld\n", lSockError);
    return ERRSOCK_SOCKET_CREATE;
  }

  lSetOptError = setsockopt (m_pcLocalVars->Socket, SOL_SOCKET, SO_BROADCAST, (char*) &bBroadcastsEnabled, sizeof(BOOL));
  if (lSetOptError == SOCKET_ERROR)
  { printf("ERROR: setsockopt() failed: %ld\n", WSAGetLastError());
    return ERRSOCK_SOCKET_CREATE;
  }

  return TRUE;
}

// ***************************************************************************
// Author    : MS
// Date      : 08.11.2011
// Descript  : 

WinSocket_UDP::WinSocket_UDP (long *plOk, unsigned short usLocalPort,
                                char *pchLocalAdress, bool bEnableBroadcasts)
{ long        lError, lBindError;
  sockaddr_in RecvAddr;

  *plOk         = FALSE;
  m_bInitOK     = false;
  m_pcLocalVars = NULL;

  m_pcLocalVars = new t_WinSocket_UDP_LocalVars;
  memset (m_pcLocalVars, 0, sizeof(t_WinSocket_UDP_LocalVars));

  m_pcLocalVars->bIsServer          = false;
  m_pcLocalVars->bBroadcastsEnabled = bEnableBroadcasts;
  m_pcLocalVars->usPort             = usLocalPort;
  strncpy (m_pcLocalVars->achAdress, pchLocalAdress, 256); m_pcLocalVars->achAdress[255] = 0;

  if ((lError = OpenSocket()) <= 0) { *plOk = lError; return; } 

  RecvAddr.sin_family      = AF_INET;
  RecvAddr.sin_addr.s_addr = (pchLocalAdress) ? inet_addr(pchLocalAdress) : htonl(INADDR_ANY);
  RecvAddr.sin_port        = htons(usLocalPort);

  if (usLocalPort != 0)
  { lBindError = bind(m_pcLocalVars->Socket, (SOCKADDR *) &RecvAddr, sizeof(RecvAddr));
    if (lBindError == SOCKET_ERROR)
    { printf("ERROR: bind() failed: %ld\n", WSAGetLastError());
      *plOk = ERRSOCK_PORT_BIND;
      return;
    }
    m_pcLocalVars->bIsServer = true;
  }

  m_bInitOK = true;
  *plOk     = TRUE;
}

// ***************************************************************************
// Author    : MS
// Date      : 08.11.2011
// Descript  : 

WinSocket_UDP::~WinSocket_UDP()
{
  m_bInitOK = false;

  if (m_pcLocalVars)
  { if (m_pcLocalVars->Socket) closesocket(m_pcLocalVars->Socket);
    delete m_pcLocalVars;
  }
}

// ***************************************************************************
// Author    : MS
// Date      : 08.11.2011
// Descript  : 

long WinSocket_UDP::GetLocalAddress (char *pchIPAdress, unsigned short *pusPort)
{ 
  if (!m_bInitOK) return ERRSOCK_NO_INIT;

  if (pchIPAdress) strcpy (pchIPAdress, m_pcLocalVars->achAdress);
  if (pusPort) *pusPort = m_pcLocalVars->usPort;

  return TRUE;
}

// ***************************************************************************
// Author    : MS
// Date      : 08.11.2011
// Descript  : 

long WinSocket_UDP::Send (char *pchData, long lSize,   
                           unsigned short usToPort, char *pchToAdress)
{ sockaddr_in ToAddr, LocAddr;
  long        lBytesSend;
  int         iSize = sizeof(LocAddr);

  if (!m_bInitOK)          return ERRSOCK_NO_INIT;
  if (usToPort == 0)       return ERRSOCK_ILLEGAL_PORT;
  if (pchToAdress == NULL) return ERRSOCK_ILLEGAL_ADRESS;
  if (pchToAdress[0] == 0) return ERRSOCK_ILLEGAL_ADRESS;

  ToAddr.sin_family      = AF_INET;
  ToAddr.sin_addr.s_addr = inet_addr(pchToAdress);
  ToAddr.sin_port        = htons(usToPort);

  lBytesSend = sendto (m_pcLocalVars->Socket, pchData, lSize, 0, (SOCKADDR *) &ToAddr, sizeof(ToAddr));
  if (lBytesSend == SOCKET_ERROR)
  { printf("ERROR sendto(): %ld\n", WSAGetLastError());
    return ERRSOCK_SEND;
  }

  if ((m_pcLocalVars->usPort==0) || (m_pcLocalVars->achAdress[0] == 0))
  {
    if (getsockname (m_pcLocalVars->Socket, (SOCKADDR *) &LocAddr, &iSize) == SOCKET_ERROR)
    { printf("ERROR sendto(): %ld\n", WSAGetLastError());
      return ERRSOCK_SEND;
    }
    m_pcLocalVars->usPort = ntohs(LocAddr.sin_port);
    strcpy (m_pcLocalVars->achAdress, inet_ntoa(LocAddr.sin_addr));

    if (m_pcLocalVars->usPort != 0) m_pcLocalVars->bIsServer = true;
  }

  return lBytesSend;
}

// ***************************************************************************
// Author    : MS
// Date      : 08.11.2011
// Descript  : 

long WinSocket_UDP::Read (char *pchBuffer, long lSize,
                           unsigned short *pusFromPort, char *pchFromAdress, long lTimeout_ms)
{ fd_set      cReceiver;
  timeval     cTime;
  sockaddr_in FromAddr;
  int         iFromAddrSize = sizeof(FromAddr);
  long        lByteCount, lHandleCount;

  if (!m_bInitOK)                return ERRSOCK_NO_INIT;
  if (!m_pcLocalVars->bIsServer) return ERRSOCK_NO_PORT_BIND;

  // wait next message
  cReceiver.fd_count    = 1;
  cReceiver.fd_array[0] = m_pcLocalVars->Socket;
  cTime.tv_sec          =  lTimeout_ms / 1000;
  cTime.tv_usec         = (lTimeout_ms % 1000) * 1000;

  switch (lTimeout_ms)
  { case 0:  lHandleCount = select (0, &cReceiver, NULL, NULL, NULL);   break;
    case 1:  lHandleCount = 1;                                          break;
    default: lHandleCount = select (0, &cReceiver, NULL, NULL, &cTime);
  }
  if (lHandleCount == SOCKET_ERROR)
  { printf("ERROR select(): %ld\n", WSAGetLastError());
    return ERRSOCK_RECEIVE;
  }
  if (lHandleCount == 0) return 0;

  // receive a message
  FromAddr.sin_family      = AF_INET;
  FromAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  FromAddr.sin_port        = htons(0);

  lByteCount = recvfrom (m_pcLocalVars->Socket, pchBuffer, lSize, 0, 
                         (SOCKADDR *)&FromAddr, &iFromAddrSize);
  if (lByteCount == SOCKET_ERROR)
  { printf("ERROR receive packet: %ld\n", WSAGetLastError());
    return ERRSOCK_RECEIVE;
  }

  // extract the source-adress
  if (pchFromAdress) strcpy (pchFromAdress, inet_ntoa (FromAddr.sin_addr));
  if (pusFromPort)   *pusFromPort = FromAddr.sin_port;

  return lByteCount;
}

// ***************************************************************************


#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif  // WIN_SOCKET_UDP_CPP

