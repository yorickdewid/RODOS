/* -*- Mode: C++ -*- */

/**
* @file
* @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
*
* Copyright 2008-2010 DLR
*
* @brief Implementation of time stuff.
* 
* Changed in Jun 2013: removed second timer requirement to keep
* the system time; we can do everything with a single timer, see
* context.S for part of the irq related tick increments. -AndreB
*/

#include <rodos.h>
#include "hw_specific.h"
#include "hw_core.h"

unsigned long long volatile hwTickerBase = 0;

// we access this in the preemption handler in context.S
uint32_t hwTickerReload = HW_INIT_TIMER_1_RELOAD;

void hwInitTime()
{
  /* Nothing: initialisation is done during boot-up already. */
}

void Timer::init()
{
  /* enable timer interrupts */
  *HW_OCR(HW_OCR_TIMER_1_CONTROL) = HW_INIT_TIMER_1_CONTROL;
}

/**
 * Timer::stop() and Timer::start() are ignored.  The low-level
 * implementation does not require interrupts to be disabled manually.
 * The low-level code checks for supervisor/user mode and only
 * preempts if we're running in user mode, so the scheduler is safe.
 * Also, the stack switching code itsef is properly protected against
 * any interrupts.
 *
 * There are UART interrupts, too, so switching off the timer is no
 * guarantee that no interrupts come in.
 *
 * So there is no reason to switch off the timer.
 */
void Timer::start() {}

void Timer::stop() {}

/**
 * We incremented hwTickerBase in each timer interrupt by the reload
 * register's value + 1. This is this the time since boot at which
 * the next timer interrupt will occur.  To get the current time, one
 * only needs to subtract the value of the current counter register,
 * which counts downto 0 (and causes an interrupt then).
 * 
 * For the hwTickerBase increments see the beginning of context.S
 */
unsigned long long hwGetNanoseconds()
{
  unsigned long long base1;
  unsigned long long base2;
  unsigned long long offset1;
  unsigned long long offset2;
  do {
    offset1= *HW_OCR(HW_OCR_TIMER_1_COUNTER);
    base1=   hwTickerBase;
    offset2= *HW_OCR(HW_OCR_TIMER_1_COUNTER);
    base2=   hwTickerBase;
  } while (base1 != base2 || offset2 > offset1); // interrupt or underrun in between? => try again

  unsigned long long ticks= base2 - offset2;
  return ticks * (1000000000ULL / HW_CONFIG_TIMER_CLOCK_HZ);
}


unsigned long long hwGetAbsoluteNanoseconds()
{
  return hwGetNanoseconds();  // we have no real time clock
}

void Timer::setInterval(const long long us) // us is in microseconds
{
  /* Set the reload value */
  hwTickerReload =  HW_CROP(
                            HW_TIMER_COUNTER_MIN,
                            HW_TIMER_COUNTER_MAX,
                            HW_DIV_FLOOR(
                                         us * ((long long)(HW_CONFIG_TIMER_CLOCK_HZ)),
                                         1000*1000 /* microseconds */
                            ) - 1);

  *HW_OCR(HW_OCR_TIMER_1_RELOAD) = hwTickerReload;

  // make sure hwGetNanoseconds() does not return a negative value
  if(hwTickerReload > hwTickerBase)
	  hwTickerBase = hwTickerReload;

  /* Reinitialise/reload/reenable/restart timer now: */
  *HW_OCR(HW_OCR_TIMER_1_CONTROL)= HW_INIT_TIMER_1_CONTROL;
}

