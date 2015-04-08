
#include "lpc24xx_registers.h"
namespace RODOS {
void
hwInitTime()
{
	/* setup of timer1 as clock */
	/* CCLK = 72 MHz */
	/* PCLK = CCLK/4 = 18 MHz */
	/* use prescaler for one second clock generation */
	
	/* peripheral clock selection */
	PCLKSEL0_p->peripheral.TIMER1 = 0;  /* PCLK = CCLK/4 */
	
	/* power control */
	PCONP_p->peripheral.TIMER1 = 1; /* power timer 1 peripheral  */
	
	T1TCR = 2;               /* timer control: disable timer and reset counter */
	T1IR = ~0;               /* clear pending interrupts */
	T1CTCR = 0;              /* count control: timer mode */
	T1TC = 0;                /* timer counter */
	T1PR = 18000000-1;       /* timer prescaler: divide by 18000000 -> 1s resolution */
	T1PC = 0;                /* timer prescale counter */
	T1MCR = 0;               /* match control: interrupt, reset and stop on MR0 */
	T1CCR = 0;               /* disable capture capabilities */
	T1TCR = 1;               /* timer control: enable clock */
}



unsigned long long
hwGetNanoseconds()
{
  unsigned long seconds;
  unsigned long partial1;
  unsigned long partial2;
  do {
    partial1 = T1PC ;       /* prescaler counter */
    seconds =  T1TC ;       /* timer counter */
    partial2 = T1PC ;
  } while (partial1 > partial2);

  return ((((unsigned long long) seconds)*1000000000ULL)+
          (((unsigned long long) partial2/9)*500ULL));
}
}

