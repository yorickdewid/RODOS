/*
 * Implementation of the global timer for providing a base time
 * for RODOS.
 */
#include "global_timer/global_timer.h"
#include "regsarmglobaltimer.h"


/**********************************************************************
 * The ARM private peripheral clock is around 400 MHz. This value was measured
 * with experiments since there is not much documentation about the value of
 * the clock that feeds the global timer.
 * prescaler + 1 = PERIPHCLK / f(timer)
 * For a 10ns (100MHz) timer:
 * 		- prescaler + 1 = 500 / 100 = 5
 * 		- prescaler = 4
 */
void global_timer_start(){
	// Clear the Global Timer Control Register
	HW_ARMGLOBALTIMER_CONTROL_WR(0x0);

	// Write the lower 32-bit timer counter register
	HW_ARMGLOBALTIMER_COUNTERn_WR(LOWER_COUNTER_REG, 0x0);

	// Write the upper 32-bit timer counter register
	HW_ARMGLOBALTIMER_COUNTERn_WR(HIGHER_COUNTER_REG, 0x0);

	// Set prescaler - 10ns increments
	HW_ARMGLOBALTIMER_CONTROL.B.PRESCALER = 3;

	// Set the timer enable bit.
	HW_ARMGLOBALTIMER_CONTROL.B.TIMER_ENABLE = 1;
}

/**********************************************************************
 * Careful when using this function.
 * It stops and clears the actual time of RODOS.
 */
void global_timer_stop(){
	// Clear the timer enable bit in the Global Timer Control Register
	gbl_tmr_disable();
}

/**********************************************************************
 * Set the comparator register.
 */
void global_timer_set_comparator(long long value){
	// Clear COMP bit in the control register
	HW_ARMGLOBALTIMER_CONTROL.B.COMP_ENABLE = 0;	// Banked per core

	// Set the comparator value
	HW_ARMGLOBALTIMER_COMPARATORn_WR(0, HW_ARMGLOBALTIMER_COUNTERn_RD(LOWER_COUNTER_REG) + (uint32_t)value);
	HW_ARMGLOBALTIMER_COMPARATORn_WR(1, HW_ARMGLOBALTIMER_COUNTERn_RD(HIGHER_COUNTER_REG) + (uint32_t)(value >> 32));

	// Set the COMP bit in the control register
	HW_ARMGLOBALTIMER_CONTROL.B.COMP_ENABLE = 1;	// Banked per core
}

/**********************************************************************
 * Return the value of the timer.
 */
long long global_timer_get_value(){
	unsigned long low_reg, high_reg, high_reg_ver;
	do {
		// Read the upper 32-bit timer counter register
		low_reg = HW_ARMGLOBALTIMER_COUNTERn_RD(LOWER_COUNTER_REG);

		// Read the lower 32-bit timer counter register
		high_reg = HW_ARMGLOBALTIMER_COUNTERn_RD(HIGHER_COUNTER_REG);

		// Read the upper 32-bit timer counter register again.
		high_reg_ver = HW_ARMGLOBALTIMER_COUNTERn_RD(HIGHER_COUNTER_REG);

		// If the value is different to the 32-bit upper value read previously,
		// go back to step 2. Otherwise the 64-bit timer counter value is correct.
	} while(high_reg != high_reg_ver);

	long long result_microSec = high_reg;
	result_microSec = result_microSec<<32;
	result_microSec += low_reg;

	// Since the time in RODOS is in nanoseconds and the timer is 10 nanoseconds
	result_microSec *= 10;

	return result_microSec;
}

long long global_timer_get_comp(){
	unsigned long low_reg, high_reg;
	low_reg = HW_ARMGLOBALTIMER_COMPARATORn_RD(0);
	high_reg = HW_ARMGLOBALTIMER_COMPARATORn_RD(1);

	long long result_microSec = high_reg;
	result_microSec = result_microSec<<32;
	result_microSec += low_reg;

	return result_microSec;
}

void gbl_tmr_enable(){
	uint32_t reg = HW_ARMGLOBALTIMER_CONTROL_RD();
	uint32_t value = reg | 0x1;
	HW_ARMGLOBALTIMER_CONTROL_WR(value);
}

void gbl_tmr_disable(){
	uint32_t reg = HW_ARMGLOBALTIMER_CONTROL_RD();
	uint32_t value = reg & ~(0x1);
	HW_ARMGLOBALTIMER_CONTROL_WR(value);
}

void gbl_tmr_disable_comp(){
	uint32_t reg = HW_ARMGLOBALTIMER_CONTROL_RD();
	uint32_t value = reg & ~(0x2);
	HW_ARMGLOBALTIMER_CONTROL_WR(value);
}

void gbl_tmr_enable_comp(){
	uint32_t reg = HW_ARMGLOBALTIMER_CONTROL_RD();
	uint32_t value = reg | 0x2;
	HW_ARMGLOBALTIMER_CONTROL_WR(value);
}

void gbl_tmr_disable_irq(){
	uint32_t reg = HW_ARMGLOBALTIMER_CONTROL_RD();
	uint32_t value = reg & ~(0x4);
	HW_ARMGLOBALTIMER_CONTROL_WR(value);
}

void gbl_tmr_enable_irq(){
	uint32_t reg = HW_ARMGLOBALTIMER_CONTROL_RD();
	uint32_t value = reg | 0x4;
	HW_ARMGLOBALTIMER_CONTROL_WR(value);
}

void gbl_tmr_disable_auto_inc(){
	uint32_t reg = HW_ARMGLOBALTIMER_CONTROL_RD();
	uint32_t value = reg & ~(0x8);
	HW_ARMGLOBALTIMER_CONTROL_WR(value);
}

void gbl_tmr_enable_auto_inc(){
	uint32_t reg = HW_ARMGLOBALTIMER_CONTROL_RD();
	uint32_t value = reg | 0x8;
	HW_ARMGLOBALTIMER_CONTROL_WR(value);
}

void gbl_tmr_clear_event(){
	//if(HW_ARMGLOBALTIMER_IRQSTATUS_RD() == 1){
		gbl_tmr_disable_comp();

		while(HW_ARMGLOBALTIMER_IRQSTATUS_RD() == 1){
			//HW_ARMGLOBALTIMER_IRQSTATUS_WR(0);
			BW_ARMGLOBALTIMER_IRQSTATUS_EVENT_FLAG(1);
		}
		gbl_tmr_enable_comp();
	//}
}
