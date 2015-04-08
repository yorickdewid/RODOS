#ifndef ETH_H_
#define ETH_H_
#ifdef __cplusplus
extern "C" {
#endif

	
//*****************************************************************************
//
// This is auxiliary function which change ip adress from string 
// to ip_addr structure.
//
//*****************************************************************************
int getIpAddress(struct ip_addr *addr, char *str);
//*****************************************************************************
//
// !Auxiliary function for change address from ip structure to string
// ! \param ipaddr - ip addres as a long value
// ! \param pucBuf - write ip as a string
// ! \return none
//
//*****************************************************************************
void ChangeAddress(unsigned long ipaddr, char *pucBuf);

#ifdef __cplusplus
	}
#endif
#endif /*ETH_H_*/
