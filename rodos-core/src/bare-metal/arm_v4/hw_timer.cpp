
#include "lpc24xx_registers.h"

#include "hw_specific.h"
#include "params.h"
namespace RODOS {
long long Timer::microsecondsInterval = PARAM_TIMER_INTERVAL;

void
Timer::init()
{
	/* setup of timer0 */
	/* single shot timer */
	/* CCLK = 72 MHz */
	/* PCLK = CCLK/4 = 18 MHz */
	/* peripheral clock selection */
	PCLKSEL0_p->peripheral.TIMER0 = 0;  /* PCLK = CCLK/4 */
	
	/* power control */
	PCONP_p->peripheral.TIMER0 = 1; /* power timer 0 peripheral  */
	
	T0TCR = 2;               /* timer control: disable timer and reset counter */
	T0IR = ~0;               /* clear pending interrupts */
	T0CTCR = 0;              /* count control: timer mode */
	T0TC = 0;                /* timer counter */
	T0PR = 17;               /* timer prescaler: divide by 18 -> 1us resolution */
	T0PC = 0;                /* timer prescale counter */
	T0MR0 = microsecondsInterval;      /* match register */
	T0MCR = 7;               /* match control: interrupt, reset and stop on MR0 */
	T0CCR = 0;               /* disable capture capabilities */
	
	const unsigned int TIMER0INTERRUPT = 4;
	
	VICIntSelect = (1 << TIMER0INTERRUPT); /* select FIQ for timer0 interrupts */
	VICIntEnable = (1 << TIMER0INTERRUPT);
}

void
Timer::start()
{
	T0TCR = 2;               /* timer control: disable timer */
	T0IR = ~0;               /* clear pending interrupts */
	T0CTCR = 0;              /* count control: timer mode */
	T0TC = 0;                /* reset counter */
	T0PR = 17;               /* timer prescaler: divide by 18 -> 1us resolution */
	T0PC = 0;                /* reset preset counter */
	T0MR0 = microsecondsInterval;
	T0MCR = 7;               /* match control: interrupt, reset and stop on MR0 */
	T0TCR = 1;               /* timer control: enable timer */
}

void
Timer::stop()
{
	T0TCR = 2;               /* timer control: disable timer and reset counter */
	T0IR = ~0;               /* clear pending interrupts */
}

void
Timer::setInterval(const long long microsecondsInterval_)
{
	Timer::microsecondsInterval = microsecondsInterval_;
}
}

