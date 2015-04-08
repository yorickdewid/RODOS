#include <string.h>
#include "rodos.h"
//#include "hw_udp.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/udp.h"
#include "utils/eth.h"
#include "netif/etharp.h"



unsigned char UDPReceiver::rcvFlag=0;
int UDPReceiver::rcvLen=0;
char UDPReceiver::rcvMsg[1100];
struct ip_addr UDPReceiver::addrRcv;


UDPReceiver::UDPReceiver(const long portNr)
{
	pcbUdp = udp_new();
	udp_bind(pcbUdp, IP_ADDR_ANY, portNr);
	udp_recv(pcbUdp, udp_recv_func, NULL);
	rcvFlag=0;
	if(pcbUdp == NULL)
		initialised = false;
	else
		initialised = true;
	
}
UDPReceiver::~UDPReceiver()
{
	if(pcbUdp != NULL)
		udp_remove(pcbUdp);
}
void UDPReceiver::reopen(const long portNr)
{
	if(pcbUdp != NULL)
		udp_remove(pcbUdp);
	pcbUdp = udp_new();
	udp_bind(pcbUdp, IP_ADDR_ANY, portNr);
	udp_recv(pcbUdp, udp_recv_func, NULL);
	rcvFlag=0;
	if(pcbUdp == NULL)
		initialised = false;
	else
		initialised = true;
}
/*
bool UDPReceiver::isConnected()
{
	if(pcbUdp == NULL)
		return false;
	else
		return true;
}
*/
bool UDPReceiver::readyToGet()
{
	if(rcvFlag==1)
		return true;
	else
		return false;
}
long UDPReceiver::get(void* userData, int maxLen, unsigned long *ipaddr)
{
	if(rcvLen<maxLen)
	{
		memcpy (userData, rcvMsg, rcvLen);
		*ipaddr=addrRcv.addr;
		rcvFlag=0;
		return rcvLen;
	}
	else
	{
		memcpy (userData, rcvMsg, maxLen);
		*ipaddr=addrRcv.addr;
		rcvFlag=0;
		return maxLen;
	}

}
long UDPReceiver::get(void* userData, int maxLen)
{
	if(rcvLen<maxLen)
	{
		memcpy (userData, rcvMsg, rcvLen);
		rcvFlag=0;
		return rcvLen;
	}
	else
	{
		memcpy (userData, rcvMsg, maxLen);
		rcvFlag=0;
		return maxLen;
	}

}
void  UDPReceiver::udp_recv_func(void  *arg, struct udp_pcb *pcb, struct  pbuf *p, struct  ip_addr *addr, unsigned short  port)
{
	struct  pbuf *q;
	//int i;
	  //
	  // if udp message received
	  //
      if (p != NULL) 
      {
    	  if(rcvFlag==0)
    	  {
    		  rcvLen=0;
    		  q=p;
    		  //
    		  // loop until data is read from tree structure of pbuf
    		  //
    		  while(q != NULL && rcvLen + q->len < 1100)
    		  {
        	  		memcpy (&(rcvMsg[rcvLen]), q->payload, q->len);
        	  		rcvLen+=q->len;
        	  		//i=q->len;
        	  		q=q->next;
    		  }
    		  addrRcv.addr=addr->addr;
    		  //
    		  // set receive flag
    		  //
    		  rcvFlag=1;	  		
    	  }// end if(rcvFlag==0)
    	  
    	  //don't forget clean the buffer
    	  pbuf_free(p);  
      }//end if (p != NULL) 
}


UDPTransmitter::UDPTransmitter(const long _portNr)
{
	portNr=_portNr;
	pcbUdp = udp_new();
	if(pcbUdp == NULL)
		initialised = false;
	else
		initialised = true;
}
UDPTransmitter::UDPTransmitter(const long _portNr,  const char *host)
{
	portNr=_portNr;
	pcbUdp = udp_new();
	if(pcbUdp == NULL)
		initialised = false;
	else
		initialised = true;
}
UDPTransmitter::UDPTransmitter(const long _portNr, unsigned long _ipAddr)
{
	portNr=_portNr;
	ipAddr=_ipAddr;
	pcbUdp = udp_new();
	if(pcbUdp == NULL)
		initialised = false;
	else
		initialised = true;
	
}
UDPTransmitter::UDPTransmitter(const long _portNr, int ip0, int ip1, int ip2, int ip3)
{
	struct ip_addr xIpAddr;
	
	IP4_ADDR(&xIpAddr,ip0,ip1,ip2,ip3);
	portNr=_portNr;
	ipAddr=xIpAddr.addr;
	pcbUdp = udp_new();
	if(pcbUdp == NULL)
		initialised = false;
	else
		initialised = true;
}
UDPTransmitter::~UDPTransmitter()
{
	udp_remove(pcbUdp);
}
bool UDPTransmitter::send(const void* userData, const int maxLen)
{
	err_t ret;
	struct ip_addr xIpAddr;
	struct pbuf *myp = pbuf_alloc(PBUF_TRANSPORT,maxLen, PBUF_RAM);
	
	if(pcbUdp == NULL)
		pcbUdp = udp_new();
	if(pcbUdp == NULL)
	{
		initialised = false;
		return false;
	}
	initialised = true;
	memcpy(myp->payload,userData,maxLen);
	xIpAddr.addr=ipAddr;
	ret = udp_sendto(pcbUdp,myp, &xIpAddr, portNr);
    pbuf_free(myp);
    if(ret == ERR_OK)
    	return true;
    else
    	return false;
	
}
bool UDPTransmitter::sendTo(const void* userData, const int maxLen, unsigned long _ipAddr)
{
	err_t ret;
	struct ip_addr xIpAddr;
	struct pbuf *myp = pbuf_alloc(PBUF_TRANSPORT,maxLen, PBUF_RAM);
	
	if(pcbUdp == NULL)
		pcbUdp = udp_new();
	if(pcbUdp == NULL)
	{
		initialised = false;
		return false;
	}
	initialised = true;
	memcpy(myp->payload,userData,maxLen);
	xIpAddr.addr=_ipAddr;
	ret = udp_sendto(pcbUdp,myp, &xIpAddr, portNr);
    pbuf_free(myp);
    if(ret == ERR_OK)
    	return true;
    else
    	return false;
}
