#ifndef FLASH_H_
#define FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif
	
#include "lwip/tcpip.h"
	
//*****************************************************************************
//Define a structure where you can put all the
//variables you want to save in the flash.
//The maximum structure size is 1024 byte.
//*****************************************************************************
typedef struct tagPERMANENT_VAR
{
	struct ip_addr xIpAddr, xNetMask, xGateway;
    int portListen;
    int portSend;
    unsigned int progFlag;
    
} PERMANENT_VAR;

//*****************************************************************************
//
// !This function writes the variables to the flash.
// ! \param permanentVars keep data write to flash
// ! \return none
//
//*****************************************************************************
void writePermanentsToFlash(PERMANENT_VAR permanentVars);

//*****************************************************************************
//
// !This function reads the variables from the flash.
// ! \param permanentVars keep data read from flash
// ! \return none
//
//*****************************************************************************
void readPermanentsFromFlash(PERMANENT_VAR *permanentVars);

#ifdef __cplusplus
 }
#endif

#endif /*FLASH_H_*/