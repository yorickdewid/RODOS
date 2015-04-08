#ifndef __EPIT_H__
#define __EPIT_H__

#include "registers/regsepit.h"
#include "workspace.h"

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


// Interrupt Service Routine
typedef void (*epit_isr_function) (void);


/***********************************
 * API
 */
#if defined(__cplusplus)
extern "C" {
#endif

/*
 * Initiates the EPIT timer.
 * @Param1: Instance of the module.
 */
void epit_init(int instance);

/*
 * Start the EPIT timer.
 * @Param1: Instance of the module.
 * @Param2: Value in microseconds.
 */
void epit_start(int instance, int value_us);

/*
 * Stops the EPIT timer.
 * @Param1: Instance of the module.
 */
void epit_stop(int instance);

/*
 * Get value of the counter in EPIT timer.
 * @Param1: Instance of the module.
 */
unsigned int epit_get_value(int instance);

/*
 * Clear EPIT flags.
 * @Param1: Instance of the module.
 */
void epit_clear_flags(int instance);

/*
 * Registers a function as a IRQ to handler the interrupt.
 * @Param1: Instance of the module.
 * @Param2: Pointer to the IRQ handler.
 */
void epit_setIRQ(int instance, epit_isr_function isr, int priority);

#if defined(__cplusplus)
}
#endif

#endif // __EPIT_H__
