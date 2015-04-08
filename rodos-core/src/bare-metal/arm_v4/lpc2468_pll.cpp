///
// @file lpc2468_pll.cpp
//
//

#include "lpc24xx_registers.h"

// Ref: UM10237 LPC24XX User manual Rev. 04 - 26. August 2009
// Fcco = (2*M*Fin)/N
// allowed Fcco range 275 MHz to 550 MHz
// Fin = 12MHz @ Olimex LPC-E2468 board
// Fcpu max = 72MHz
// for USB usage Fcco target values { 288 MHz, 384 MHz 480 MHz }

// configuration:
//  Fcco = 288 MHz
//  Fcpu =  72 MHz
//  Fin  =  12 MHz
//  M=12  N=1

#define PLL_MValue      11    // multiplier M = 12
#define PLL_NValue       0    // divider N = 1
#define CCLKDivValue     3    // divide Fcco by 4   -->       cclk   = 72  MHz
#define USBCLKDivValue   5    // divide Fcco by 6   -->       usbclk = 48  MHz

#define PLLSTAT_PLLE  ( 1 << 24 )
#define PLLSTAT_PLLC  ( 1 << 25 )
#define PLLSTAT_PLOCK ( 1 << 26 )

void
initPLL(void)
{
  volatile unsigned long MValue;
  volatile unsigned long NValue;

  if ( (PLLSTAT & (PLLSTAT_PLLC)) != 0 ) {
    PLLCON = 1;  /* Enable PLL, disconnected */
    PLLFEED = 0xaa;
    PLLFEED = 0x55;
  }

  PLLCON = 0;  /* Disable PLL, disconnected */
  PLLFEED = 0xaa;
  PLLFEED = 0x55;

  SCS = SCS & ~(1 << 4);        /* SCS.OSCRANGE = 0  (Fosc = 1MHz..20MHz) */
  SCS = SCS |  (1 << 5);        /* Enable main OSC */
  while( !(SCS & 0x40) ) {
     /* Wait until main OSC is usable */
  }

  CLKSRCSEL = 0x1;                 /* select main OSC as the PLL clock source */

  PLLCFG = (PLL_MValue & 0x7FFF) | ((PLL_NValue & 0xFF) << 16);
  PLLFEED = 0xaa;
  PLLFEED = 0x55;

  PLLCON = 1;                   /* Enable PLL, disconnected */
  PLLFEED = 0xaa;
  PLLFEED = 0x55;

  CCLKCFG = CCLKDivValue;       /* Set clock divider */

  while ( ((PLLSTAT & PLLSTAT_PLOCK) == 0) ) {};  /* wait for PLL lock */

  MValue = PLLSTAT & 0x00007FFF;
  NValue = (PLLSTAT & 0x00FF0000) >> 16;
  while ((MValue != PLL_MValue) && ( NValue != PLL_NValue) ) {}

  PLLCON = 3;                  /* enable and connect */
  PLLFEED = 0xaa;
  PLLFEED = 0x55;

  while ( ((PLLSTAT & (PLLSTAT_PLLC)) == 0) ) {} /* Check connect bit status */
}

