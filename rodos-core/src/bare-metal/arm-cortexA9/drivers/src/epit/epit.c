#include "epit/epit.h"
#include "workspace.h"

/*
 * Initiates the EPIT timer.
 * @Param1: Instance of the module.
 */
void epit_init(int instance){
	unsigned int control_reg_tmp = 0;

	// enable the source clocks to the EPIT port
	clock_gating_config(REGS_EPIT_BASE(instance), CLOCK_ON);

	// start with a known state by disabling and reseting the module
	HW_EPIT_CR_WR(instance, BM_EPIT_CR_SWR);

	// wait for the reset to complete
	while ((HW_EPIT_CR(instance).B.SWR) != 0) ;

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
	HW_EPIT_CR_WR(instance, control_reg_tmp);

}

/*
 * Start the EPIT timer.
 * @Param1: Instance of the module.
 * @Param2: Value in microseconds.
 */
void epit_start(int instance, int value_us){
	// load value
	HW_EPIT_LR_WR(instance, value_us);

	// ensure to start the counter in a proper state
	// by clearing possible remaining compare event
	HW_EPIT_SR_SET(instance, BM_EPIT_SR_OCIF);

	// set the mode for IRQ
	HW_EPIT_CR_SET(instance, BM_EPIT_CR_OCIEN);

	// finally, enable the counter
	HW_EPIT_CR_SET(instance, BM_EPIT_CR_EN);
}

/*
 * Stops the EPIT timer.
 * @Param1: Instance of the module.
 */
void epit_stop(int instance){
	/* temporary workaround for the discovered issue when disabling the
	 * counter during end of count/reload/set compare flag ??.
	 * Set to the max value so that it ensures that the counter couldn't
	 * reach 0 when it is disabled.
	 */
	HW_EPIT_LR_WR(instance, 0xFFFFFFFF);

	// disable the counter
	HW_EPIT_CR_CLR(instance, BM_EPIT_CR_EN);

	// ensure to leave the counter in a proper state
	// by disabling the output compare interrupt
	HW_EPIT_CR_CLR(instance, BM_EPIT_CR_OCIEN);

	// and clearing possible remaining compare event
	HW_EPIT_SR_SET(instance, BM_EPIT_SR_OCIF);

	// disable interrupts
	switch(instance){
	case HW_EPIT1:
		disable_interrupt(IMX_INT_EPIT1, cpu_get_current());
		break;
	case HW_EPIT2:
		disable_interrupt(IMX_INT_EPIT2, cpu_get_current());
		break;
	}
}

/*
 * Stops the EPIT timer.
 * @Param1: Instance of the module.
 */
unsigned int epit_get_value(int instance){
	return HW_EPIT_CNR_RD(instance);
}

/*
 * Clear EPIT flags.
 * @Param1: Instance of the module.
 */
void epit_clear_flags(int instance){
	// Clear them if activated
	if (HW_EPIT_SR_RD(instance) & BM_EPIT_SR_OCIF)
	{
		HW_EPIT_SR_SET(instance, BM_EPIT_SR_OCIF);
	}
}

/*
 * Registers a function as a IRQ to handler the interrupt.
 * @Param1: Instance of the module.
 * @Param2: Pointer to the IRQ handler.
 */
void epit_setIRQ(int instance, epit_isr_function isr, int priority){
	// Enable and set the interrupt
	switch(instance){
	case HW_EPIT1:
		register_interrupt_routine(IMX_INT_EPIT1, isr);
		enable_interrupt(IMX_INT_EPIT1, cpu_get_current(), priority);
		break;
	case HW_EPIT2:
		register_interrupt_routine(IMX_INT_EPIT2, isr);
		enable_interrupt(IMX_INT_EPIT2, cpu_get_current(), priority);
		break;
	}
}
