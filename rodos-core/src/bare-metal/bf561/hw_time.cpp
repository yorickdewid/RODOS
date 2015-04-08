/***********************************************************************************************//**
 * \ingroup HK
 * \file hw_time.cpp
 * \version 0.1
 * \date 18.02.2014
 * \author Georg Raith
 *
 * \brief 
 * \b History: \n
 *
 * 18.02.2014 rg: initial version
 */


#include "hw_config.h"
#include "rodos.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

volatile u64 core_timer;

extern bool isSchedulingEnabled; // from scheduler
__attribute__((interrupt_handler)) void system_core_timer_interrupt() {
	core_timer++;

	//support for time events
	TimeEvent::propagate(NOW());


	if (isSchedulingEnabled) {//if the scheduler is locked don't raise interrupt 14 for the context switch
		__asm__ (" RAISE 14;");//raise software interrupt for the scheduler
	}
	return;
}
void system_force_schedule() {
	///\todo implement
	xprintf("Not implemented");
//	if (isSchedulingEnabled) {//if the scheduler is locked don't raise interrupt 14 for the context switch
//		__asm__ (" RAISE 14;");//raise software interrupt for the scheduler
//	}
}

void hwInitTime() {
	//10ms intervall to system core timer interrupt
	core_timer = 0;
}

///returns a 64bit Timer value
///\todo verify this function
unsigned long long hwGetNanoseconds() {
	u32 timer_count1;
	u32 timer_count2;
	u64 return_value;

	//get Reliable values from Register
	//maybe it is possible to make this faster by referencing the ILAT Register + critical Region
	do {
	timer_count1 = *pTCOUNT;
	csync();
	return_value = core_timer;
	csync();
	timer_count2 = *pTCOUNT;
	csync();
	} while (timer_count1 < timer_count2);//Make sure that the TCOUNT Register is not reloaded during the method
#warning use system configuration for this values
	return_value = (return_value+1)*PARAM_TIMER_INTERVAL*1000 - timer_count2;
	return return_value;
}

unsigned long long hwGetAbsoluteNanoseconds() {
#warning not implemented
	return hwGetNanoseconds();
}

long long Timer::microsecondsInterval = PARAM_TIMER_INTERVAL;
void Timer::init() {
	*pEVT6 = (void (*)) (& system_core_timer_interrupt); //interrupt service routine
	*pTSCALE = MFC_CT_TSCALE - 1; //cclock is devided by pTSCALE-1
#warning use system configuration for this values
	*pTPERIOD = microsecondsInterval*10;
	*pTCOUNT = microsecondsInterval*10;
	*pTCNTL = 0;
	//*pTCNTL = (TMPWR | TAUTORLD);
	*pILAT = EVT_IVTMR; //clear pending timer interrupts
	*pIMASK |= EVT_IVTMR; //core timer interrupt enable

//	*pIPEND &= ~EVT_IRPTEN; //global interrupt enable 0 is enabled

}

void Timer::start() {

	*pTCNTL = (TMPWR | TMREN | TAUTORLD);
}

void Timer::stop() {
	//*pTCNTL = (TMPWR | TAUTORLD);
}

void Timer::setInterval(long long int microsecondInterval) {
	*pTPERIOD = microsecondsInterval*10;
	Timer::microsecondsInterval = microsecondInterval;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif
