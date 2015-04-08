/*-----------------------------------------------------------
  Class Timer - provides start() and stop() methods that are
  intended to disable interrupts over the time of a voluntary
  context switch. If this is handled otherwise it is perfectly
  fine to also leave these methods unimplemented.
-------------------------------------------------------------*/

/*
 * Includes
 */
// Work space includes
#include "workspace.h"
#include "registers/regsepit.h"

// RODOS includes
#include "hw_specific.h"
#include "rodos.h"

extern "C" unsigned int vectNum;

//! @brief Options for low power mode support for the timers.
//! These constants are bit masks that are or'd together to select in which low
//! power modes the timer will continue counting.
enum _timer_low_power_modes
{
    WAIT_MODE_EN = 1,   //!< Timer is enabled in wait mode.
    STOP_MODE_EN = 2    //!< Timer is enabled in stop mode.
};


//! @brief Available clock sources for the timers.
enum _timer_clock_sources
{
    CLKSRC_OFF = 0,     //!< clock source is OFF
    CLKSRC_IPG_CLK = 1, //!< clock source is peripheral clock
    CLKSRC_PER_CLK = 2, //!< clock source is high-freq reference clock
    CLKSRC_CLKIN = 3,   //!< clock source is external from a CLKIN input
    CLKSRC_CKIL = 3     //!< clock source is low-freq reference clock
};


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/*
 * Definitions
 */
unsigned int value_us = 0;

/*
 * Prototypes
 */
extern "C"{
	void leave_irq();
}

/*
 * Methods
 */
void Timer::init(){

	//Disabel interrupts here to avoid preemtion before threads are running.
	// They get enabled by the first context switch to the idle-thread
	hwDisableInterrupts();

	if(cpu_get_current() == 0){
		unsigned int control_reg_tmp = 0;

		// enable the source clocks to the EPIT port
		clock_gating_config(REGS_EPIT1_BASE, CLOCK_ON);

		// start with a known state by disabling and reseting the module
		HW_EPIT_CR_WR(HW_EPIT1, BM_EPIT_CR_SWR);

		// wait for the reset to complete
		while ((HW_EPIT_CR(HW_EPIT1).B.SWR) != 0) ;

		// set the reference source clock for the counter
		control_reg_tmp |= BF_EPIT_CR_CLKSRC(CLKSRC_IPG_CLK);

		// set the counter clock prescaler value - 0 to 4095
		control_reg_tmp |= BF_EPIT_CR_PRESCALAR(get_main_clock(IPG_CLK)/1000000);

		// set and forget reload mode
		control_reg_tmp |= BM_EPIT_CR_RLD;

		// set behavior for low power mode
		control_reg_tmp |= BM_EPIT_CR_WAITEN;
		control_reg_tmp |= BM_EPIT_CR_STOPEN;

		// make the counter start from a known value when enabled
		control_reg_tmp |= BM_EPIT_CR_IOVW | BM_EPIT_CR_ENMOD;

		// finally write the control register
		HW_EPIT_CR_WR(HW_EPIT1, control_reg_tmp);

		gic_set_irq_priority(IMX_INT_EPIT1, 0xff-10);
	    gic_set_irq_security(IMX_INT_EPIT1, false);    // set IRQ as non-secure
	    gic_enable_irq(IMX_INT_EPIT1, true);
	    gic_set_cpu_target(IMX_INT_EPIT1, 0, true);
	}
}

void Timer::start(){
	switch(cpu_get_current()){
	case 0:
		// Start the timer
		// load value
		HW_EPIT_LR_WR(HW_EPIT1, value_us);

		// ensure to start the counter in a proper state
		// by clearing possible remaining compare event
		HW_EPIT_SR_SET(HW_EPIT1, BM_EPIT_SR_OCIF);

		// set the mode for IRQ
		HW_EPIT_CR_SET(HW_EPIT1, BM_EPIT_CR_OCIEN);

		// finally, enable the counter
		HW_EPIT_CR_SET(HW_EPIT1, BM_EPIT_CR_EN);
		break;
	case 1:
		enable_interrupt(SW_INTERRUPT_1, 1, 245);
		break;
	case 2:
		enable_interrupt(SW_INTERRUPT_2, 2, 245);
		break;
	case 3:
		enable_interrupt(SW_INTERRUPT_3, 3, 245);
		break;
	}
}

void Timer::stop(){
	switch(cpu_get_current()){
	case 0:
		/* temporary workaround for the discovered issue when disabling the
		 * counter during end of count/reload/set compare flag ??.
		 * Set to the max value so that it ensures that the counter couldn't
		 * reach 0 when it is disabled.
		 */
		HW_EPIT_LR_WR(HW_EPIT1, 0xFFFFFFFF);

		// disable the counter
		HW_EPIT_CR_CLR(HW_EPIT1, BM_EPIT_CR_EN);

		// ensure to leave the counter in a proper state
		// by disabling the output compare interrupt
		HW_EPIT_CR_CLR(HW_EPIT1, BM_EPIT_CR_OCIEN);

		// and clearing possible remaining compare event
		HW_EPIT_SR_SET(HW_EPIT1, BM_EPIT_SR_OCIF);
		break;
	case 1:
		disable_interrupt(SW_INTERRUPT_1, 1);
		break;
	case 2:
		disable_interrupt(SW_INTERRUPT_2, 2);
		break;
	case 3:
		disable_interrupt(SW_INTERRUPT_3, 3);
		break;
	}
}

void Timer::setInterval(const long long microsecondsInterval){
	/* The timer interval, that is the time between two consecutive
	 * timer interrupts, should be set with the method
	 * setInterval(). Contained in the main function it is called
	 * with the preprocessor directive PARAM_TIMER_INTERVAL as an argument.
	 */
	value_us = (unsigned int)microsecondsInterval;
}

/*
 * Executes the necessary code to leave the irq before starting the context switch.
 */
extern "C" void leave_irq(){
	if(cpu_get_current() == 0){
		// Clear flags of timer it if found set
		if (HW_EPIT_SR_RD(HW_EPIT1) & BM_EPIT_SR_OCIF)
		{
			HW_EPIT_SR_SET(HW_EPIT1, BM_EPIT_SR_OCIF);
		}

		// Signal the other processors about the preemption
		gic_send_sgi(SW_INTERRUPT_1, 2, kGicSgiFilter_UseTargetList);
		gic_send_sgi(SW_INTERRUPT_2, 4, kGicSgiFilter_UseTargetList);
		gic_send_sgi(SW_INTERRUPT_3, 8, kGicSgiFilter_UseTargetList);
	}

	Timer::stop();

	long long timeNow = NOW();  	// comment this out to improve performance, but: no time events any more
	TimeEvent::propagate(timeNow); 	// comment this out to improve performance, but: no time events any more

	// Before calling the func to save the context, is necessary to clear this IRQ
	gic_write_end_of_irq(vectNum);
}

#ifndef NO_RODOS_NAMESPACE
}
#endif

