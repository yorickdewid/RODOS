/**
* @file hw_specific.cpp
* @date 2012/03/09
* @author Michael Ruffer
*
*
* @brief all hardware specific stuff I have no better place for ...
*
*/

//#include <sys/stat.h>
//#include <errno.h>

#include "rodos.h"

#include "hw_specific.h"
#include "params.h"
#include "hal/hal_uart.h"
#include "hal_interrupt.h"

namespace RODOS {
extern "C"
{
//#include <avr32/io.h>
#include "evk1100.h"
#include "power_clocks_lib.h"
#include "intc.h"
#include "cycle_counter.h"
}



/*			timer-stop          timer-start         timer-running                 timer-stop    timer-start
 *    |---------------------------|-----|-----------------------------------|--------------------|-----|
 * powerup                   Timer::  1.Timer                             Timer::               Timer::  1.Timer
 *                            start    IRQ                                 stop                  start    IRQ
 */
enum TimerState {
	TIMER_STOP = 0,
	TIMER_START,
	TIMER_RUNNING
};



volatile static TimerState timerState = TIMER_STOP;
volatile static long long nanoTime = 0;
volatile long taskRunning = 0;



extern volatile long long timeToTryAgainToSchedule;
extern volatile bool isSchedulingEnabled;

extern "C" void schedulerWrapper(long* ctx);


// the following variables are only used in compare_irq_handler
// but they must be global, because compiler does not produce working
// code when they are defined locally
static bool timerStart;
static int timerInterval;
static long *pStack;
//static bool glReturn;

__attribute__((__naked__))
//__attribute__((__interrupt__))
static void compare_irq_handler(void) {

	asm ("ssrf 16");		// disable interrupts - Set Status Register Flag "Global Interrupt Mask"
	asm ("pushm r0-r7"); 	// save the rest of context on stack -> SP,PC,LR,R12-R8 are automatically saved, when entering ISR
	asm ("mov r7,sp");		// gcc expects sp in r7!!!

	timerStart = false;
	timerInterval = Timer::getInterval();
	pStack = 0;
//	glReturn = false;

	if (timerState == TIMER_START) {
		timerStart = true;
		timerState = TIMER_RUNNING;
	}
	else
	{
		nanoTime += timerInterval * 1000; // global time in nanoseconds
	}

	// Clear the pending interrupt(writing a value to the COMPARE register clears
	// any pending compare interrupt request).
	Set_sys_compare( (CPU_CLK/1000000) * timerInterval);

	// return from ISR without calling the scheduler when:
	// 1. scheduling is disabled globally with globalAtomarLock()
	// 2. timeToTryAgainToSchedule is not reached (to avoid to much and unnecessary scheduler calls)
	// 3. this is the first interrupt after calling Timer::start() in Thread::activate()
	//    -> scheduler was just called, so it's not necessary to call it again
	//    -> When calling Thread::activate() and interrupts are enabled (what can only happen accidently
	//		 during system start up) a timer interrupt will occur immediately after calling Timer::start().
	//       This would lead to a new scheduler call and context switch before the first one is finished
	//		 -> the system is broken! So we have to avoid a scheduler call after Timer::start()!
	if ( (!isSchedulingEnabled) || timerStart){
		asm ("popm r0-r7");		// load r0-r7 from stack
		asm ("rete");			// return
	}

	long long timeNow = NOW();  // comment this out to improve performance, but: no time events any more
	TimeEvent::propagate(timeNow); // comment this out to improve performance, but: no time events any more

	if (NOW() < timeToTryAgainToSchedule){
		asm ("popm r0-r7");		// load r0-r7 from stack
		asm ("rete");			// return
	}

	/*if (!isSchedulingEnabled) {
		glReturn = true;
	}

	if (NOW() < timeToTryAgainToSchedule){
		glReturn = true;
	}

	if (timerStart){
		glReturn = true;
	}*/

	//asm ("cp.w %0,0" : : "r" (glReturn));
	//asm ("breq LABEL");
	//asm ("popm r0-r7");		// load r0-r7 from stack
	//asm ("rete");			// return
	//asm ("LABEL:");

	/* prepare scheduler call */
	asm ("mov %0,sp" : "=r" (pStack)); 			// get stack pointer of current thread
	asm ("mov sp, LO(__schedulerStackFrame__)");// load address of scheduler stack
	asm ("orh sp, HI(__schedulerStackFrame__)");// load address of scheduler stack
	asm ("ld.w sp, sp");						// set SP-register to scheduler stack
	schedulerWrapper(pStack);					// call scheduler
}



// time resolution (at CPU_CLK = 66 MHz): 1/66MHz = 15,15 ns
unsigned long long hwGetNanoseconds() {

	int count = 0;
	long long returnTime = 0;

	// to get the correct time, we have to disable interrupts, while taking a snapshot of the
	// time variables
	// -> current time = nanoTime + 1 000 000 000 * countRegister/CPU_CLK
	AVR32_ENTER_CRITICAL_REGION(); // disable interrupts
	count = Get_sys_count();
	returnTime = nanoTime;
	AVR32_LEAVE_CRITICAL_REGION();  // enable interrupts, if have been enabled before AVR32_ENTER_CRITICAL_REGION

	/** low precision
	 * - nanos = 15ns * count (for CPU_CLK = 66MHz)
	 * ! startup time might be wrong due to integer overflow !
	 */
	//int nanos = 1000000000/CPU_CLK * count;

	/** high precision
	 * - nanos = 15,151515ns * count (for CPU_CLK = 66MHz)
	 * - takes 4 times longer than low precision
	 */
	long long nanos = 0;
	nanos = 1000000000/(CPU_CLK/1000000);
	nanos *= count;
	nanos /= 1000000;

	// during timer-stop- and timer-running-phase we return
	// nanoTime + elapsed time since last interrupt
	// !!! make sure that there is now overflow in count-register due to a very long
	// stop-phase: max. @ 66MHz: 2^32 * 1/66MHz = 65s !!!
	return returnTime + nanos;
}



void hwInitTime(){}



unsigned long long hwGetAbsoluteNanoseconds(void){
	return hwGetNanoseconds();
}



void startIdleThread() {}


HAL_UART uart0(UART_IDX0);
HAL_UART uart1(UART_IDX1);

/** function to do hardware initialization 
* -> hwInit() is called in main() (src/independent/main.cpp)
*/
void hwInit(){
	// Input and output parameters when initializing PM clocks using pm_configure_clocks().
	// - parameters are defined in params.h and evk1100.h
	pm_freq_param_t clkParams = {
		CPU_CLK, // unsigned long cpu_f;
		PBA_CLK, // unsigned long pba_f; ! PBA frequency (input/output argument).
		FOSC0, // unsigned long osc0_f; ! Oscillator 0's external crystal(or external clock) frequency (board dependant) (input argument).
		OSC0_STARTUP // unsigned long osc0_startup; ! Oscillator 0's external crystal(or external clock) startup time: AVR32_PM_OSCCTRL0_STARTUP_x_RCOSC (input argument).
	};

	pm_configure_clocks(&clkParams);

	Disable_global_interrupt();

	INTC_init_interrupts(); 	// Initialize interrupt vectors.

	uart0.init(115200);
	uart1.init(115200);

	// interrupts will be enabled when loading the context of the first thread (idlethread)
	//Enable_global_interrupt();
}


/**
*  abort execution
*/
void hwResetAndReboot() 
{
//  xprintf("hw_resetAndReboot() not implemented yet\n");
  //abort();
}

void hwDisableInterrupts(){
    Disable_global_interrupt();
}

void hwEnableInterrupts(){
    Enable_global_interrupt();
}

/**
* create context on stack and return a pointer to it
*/
long* hwInitContext(long* stack, void* object) {
	/* create first thread context as it would be created by a context switch interrupt. */
	stack -= 4; 							// R8-R11
	*stack = (long) object; 				// R12
	stack--;
	*stack = 0;								// LR
	stack--;
	*stack = (long)(threadStartupWrapper);	// PC
	stack--;
	*stack = 0x00600000; 					// start value of processor status register (SR)
											// mode bits M3-M0: Bit24-Bit22 = 001 -> supervisor mode
											// exception mask: Bit21 = 1 -> exceptions disabled
											// global interrupt mask: Bit16 = 0 -> interrupts enabled
	stack -= 8;								// R1-R7

	return stack;
}



/** 
* the timer interval 
*/
long long Timer::microsecondsInterval = PARAM_TIMER_INTERVAL;

void Timer::init()
{
	INTC_register_interrupt(&compare_irq_handler, AVR32_CORE_COMPARE_IRQ, AVR32_INTC_INT0);
}



/**
* start timer 
*/
void Timer::start() 
{
	if (timerState == TIMER_STOP)
	{
		long long elapTime = 0;
		unsigned int countReg = 0;

		timerState = TIMER_START;

		AVR32_ENTER_CRITICAL_REGION(); // disable interrupts
		countReg = Get_sys_count();
		Set_sys_compare(countReg + 10); // the next timer irq will occur before leaving this function
		AVR32_LEAVE_CRITICAL_REGION();  // enable interrupts, if have been enabled before AVR32_ENTER_CRITICAL_REGION

		elapTime = 1000000000/(CPU_CLK/1000000);
		elapTime *= (countReg + 10);
		elapTime /= 1000000;

		nanoTime += elapTime;
	}
}



/**
* stop timer 
*/
void Timer::stop() 
{
	timerState = TIMER_STOP;
	Set_sys_compare(0);
}



/**
* set timer interval 
*/
void Timer::setInterval(const long long microsecondsInterval) {
  Timer::microsecondsInterval = microsecondsInterval;
}



/**
 * In case a network is avialable, the corresponding node nr.
 */
//long myNodeNr = -1 ;

//long getNodeNumber() { return myNodeNr; }

void sp_partition_yield() {}

void FFLUSH() { }
}
