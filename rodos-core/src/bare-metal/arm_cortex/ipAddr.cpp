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
#include "netif/etharp.h"
#include "utils/flash_save.h"

extern PERMANENT_VAR permanentVars;
extern struct netif g_sEMAC_if;

/********* Global Fuctions **************/

bool setMyIP(int ip0, int ip1, int ip2, int ip3)
{
	
	readPermanentsFromFlash(&permanentVars);
	
	IP4_ADDR(&(permanentVars.xIpAddr),ip0,ip1,ip2,ip3);

    permanentVars.progFlag=0xAA55;
    writePermanentsToFlash(permanentVars);
    netif_set_ipaddr(&g_sEMAC_if, &(permanentVars.xIpAddr));
	
    return true;
	
}
bool setMyIPMask(int ip0, int ip1, int ip2, int ip3)
{
	readPermanentsFromFlash(&permanentVars);
	
	IP4_ADDR(&(permanentVars.xNetMask),ip0,ip1,ip2,ip3);

    permanentVars.progFlag=0xAA55;
    writePermanentsToFlash(permanentVars);
    netif_set_netmask(&g_sEMAC_if, &(permanentVars.xNetMask));
	
    return true;
}
