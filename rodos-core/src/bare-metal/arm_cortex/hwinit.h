#ifndef HWINIT_H_
#define HWINIT_H_

#ifdef __cplusplus
extern "C" {
#endif

//*****************************************************************************
//
// !Function initialize all hardware need for udp server work.
// !This function initialize UART if RS232 is declared.
// !If RS232 is declared this function write to console information
// ! and path to current directory
// ! \param none
// ! \return 0 when is OK and 1 when error occour
//
//*****************************************************************************
extern int hardwareInit();
//*****************************************************************************
//
// !Function should be called by the top-level application to perform the needed 
// ! lwIP TCP/IP initialization
// ! \param none
// ! \return none
//
//*****************************************************************************
extern void lwip_init(void);
//*****************************************************************************
//
// !Ethernet Interrupt handler.
// ! \param none
// ! \return none
//
// This function is defined in ../udp_srv.c 
//*****************************************************************************
extern void EthernetIntHandler(void);
//*****************************************************************************
//
// The interrupt handler for the first timer interrupt.
//
//*****************************************************************************
extern void Timer0IntHandler(void);


#ifdef __cplusplus
 }
#endif

#endif /*HWINIT_H_*/

