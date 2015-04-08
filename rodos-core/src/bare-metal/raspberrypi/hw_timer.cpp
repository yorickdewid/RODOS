/*
 * @file hw_timer.cpp
 * @date 2012/08/22
 * @author Michael Ruffer, modified by Johannes Freitag
 *
 * Copyright 2012 University of Wuerzburg
 *
 * @brief Timer for system time and preemption
 */
#include "rodos.h"
#include "hw_specific.h"

#include "params.h"
#include "bcm2835.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/** 
 * the timer interval
 */
int64_t Timer::microsecondsInterval = PARAM_TIMER_INTERVAL;

void Timer::init() {

	uint32_t count;
	count = GET32(CLOL); //get current counter value
	count += Timer::microsecondsInterval*10;  //add period

	PUT32(C1, count);  //set compare value
	PUT32(CS, (1 << COUNTER1));  //reset counter interrupt
	PUT32(0x2000B210, (1 << COUNTER1)); //activate timer interrupt

	enable_irq(); //enable interrupts global

}

/**
 * start timer
 * not necessary in this port because interrupts are disabled in swi routine contextSwitch
 */
void Timer::start() {
}

/**
 * stop timer
 * not necessary in this port because interrupt are disabled in swi routine contextSwitch
 */
void Timer::stop() {
}

/**
 * set timer interval
 */
void Timer::setInterval(const int64_t microsecondsInterval) {
	Timer::microsecondsInterval = microsecondsInterval;
}

/**
 * time at bootup
 */
uint64_t initNanoTime = 0;

/**
 * RODOS time
 */
uint64_t hwGetNanoseconds() {
	return (((uint64_t) GET32(CLOL))
			| ((uint64_t) GET32(CLOH) << 32)) * 1000 - initNanoTime;
}

/**
 * set time at bootup
 */
void hwInitTime() {
	initNanoTime = (((uint64_t) GET32(CLOL))
			| ((uint64_t) GET32(CLOH) << 32)) * 1000;
}

uint64_t hwGetAbsoluteNanoseconds(void) {
	return hwGetNanoseconds(); // + timeAtStartup;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif

