#include <string.h>
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
#include "../hw/hw_types.h"
#include "../utils/ustdlib.h"

extern int atoi(const char *str); // SergioM

//*****************************************************************************
//
// !Auxiliary function for change address from ip structure to string
// ! \param ipaddr - ip addres as a long value
// ! \param pucBuf - write ip as a string
// ! \return none
//
//*****************************************************************************
void ChangeAddress(unsigned long ipaddr, char *pucBuf)
{
    int iIndex = 0;
    int iTemp;
    unsigned char *pucTemp;
	
    pucTemp = (unsigned char *)&ipaddr;
    iTemp = usprintf(&pucBuf[iIndex], "%d.", pucTemp[0]);
    iIndex += iTemp;
    iTemp = usprintf(&pucBuf[iIndex], "%d.", pucTemp[1]);
    iIndex += iTemp;
    iTemp = usprintf(&pucBuf[iIndex], "%d.", pucTemp[2]);
    iIndex += iTemp;
    iTemp = usprintf(&pucBuf[iIndex], "%d", pucTemp[3]);
    iIndex += iTemp;
    pucBuf[iIndex] = 0;

}
//*****************************************************************************
//
// This is auxiliary function which change ip adress from string 
// to ip_addr structure.
//
//*****************************************************************************
int getIpAddress(struct ip_addr *addr, char *str)
{
	int ad1,ad2,ad3,ad4;
	int i;
	char tab[4];
	char tmp[16];
	
	i=strcspn(str,".");
	strncpy(tab,str,i);
	tab[i]=0;
	ad1=atoi(tab);
	strcpy(tmp,&(str[i+1]));
	strcpy(str,tmp);
	i=strcspn(str,".");
	strncpy(tab,str,i);
	tab[i]=0;
	ad2=atoi(tab);
	strcpy(tmp,&(str[i+1]));
	strcpy(str,tmp);
	i=strcspn(str,".");
	strncpy(tab,str,i);
	tab[i]=0;
	ad3=atoi(tab);
	strcpy(tmp,&(str[i+1]));
	strcpy(str,tmp);
	ad4=atoi(str);
	if(ad1<0 || ad1>255 || ad2<0 || ad2>255 || ad3<0 || ad3>255 || ad4<0 || ad4>255)
		return (1);
    IP4_ADDR(addr,ad1,ad2,ad3,ad4);
	return (0);
}
