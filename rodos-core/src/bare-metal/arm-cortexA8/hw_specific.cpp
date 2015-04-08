/**
* @file hw_specific.cc
* @date 2008/04/23 7:33
* @port 2013/03/12 17:50
* @author Tiago Duarte
*
* Copyright 2010 DLR
*
*/

/************************************/
#include <sys/stat.h>
#include <errno.h>

// RODOS includes
#include "params.h"
#include "hw_specific.h"  //bare-metal-generic
#include "context.h"
#include "rodos.h"

#include "dmtimer.h"
#include "soc_AM335x.h"
#include "interrupt.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "cpu.h"

TContext context_to_be_saved;
unsigned int context_pointer;
#define GPIO_INSTANCE_ADDRESS           (SOC_GPIO_1_REGS)
#define GPIO_INSTANCE_PIN_NUMBER        (6)

extern void GpioPinMuxSetup(unsigned int offsetAddr, unsigned int padConfValue);

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif
static void DMTimerIsr(void);
/* Initially we have no threads */
long volatile taskRunning;

uint32_t   volatile systemTime;
HAL_UART uart_stdout(UART_DEBUG);

/*-----------------------------------------------------------*/
/* It is only used on processors that support special
 * exceptions for context switching.
 */
void startIdleThread(){
}

/*-----------------------------------------------------------*/
/* This is the 1st function to be called after main().
 * This should initialize the uart, timer, etc.
 */
void hwInit(){
//	GPIO1ModuleClkConfig();
    /* Selecting GPIO1[23] pin for use. */
//    GpioPinMuxSetup(GPIO_INSTANCE_ADDRESS, GPIO_INSTANCE_PIN_NUMBER);
    //GPIO1Pin23PinMuxSetup();
    /* Enabling the GPIO module. */
//    GPIOModuleEnable(GPIO_INSTANCE_ADDRESS);
    /* Resetting the GPIO module. */
//    GPIOModuleReset(GPIO_INSTANCE_ADDRESS);
    /* Setting the GPIO pin as an output pin. */
 //   GPIODirModeSet(GPIO_INSTANCE_ADDRESS,
//                   GPIO_INSTANCE_PIN_NUMBER,
//                   GPIO_DIR_OUTPUT);



    /* Initialize the ARM interrupt control */
    IntAINTCInit();

    uart_stdout.init();
}


/*-----------------------------------------------------------*/
/* This function is called by initSystem(), in main(),
 * and it should initialize the process for time in RODOS.
 * initSystem is the 2nd function to be called in main().
 *
 */
void hwInitTime(){
#define TICKS_PER_USEC 24
#define TIME_IRQ_EVERY_US 1000000
#define TIME_TIMER_STARTVALUE (0xFFFFFFFF-(TIME_IRQ_EVERY_US*TICKS_PER_USEC))

	systemTime = 0;

	DMTimer3ModuleClkConfig();

  /* Load the load register with the reload count value */
   DMTimerReloadSet(SOC_DMTIMER_3_REGS, TIME_TIMER_STARTVALUE);
   DMTimerCounterSet(SOC_DMTIMER_3_REGS, TIME_TIMER_STARTVALUE);
   DMTimerModeConfigure(SOC_DMTIMER_3_REGS, DMTIMER_AUTORLD_NOCMP_ENABLE);
   DMTimerEnable(SOC_DMTIMER_3_REGS);
    /* Enable the DMTimer interrupts */
    DMTimerIntEnable(SOC_DMTIMER_3_REGS, DMTIMER_INT_OVF_EN_FLAG);
  
    /* Registering DMTimerIsr */
    IntRegister(SYS_INT_TINT3, DMTimerIsr);
    /* Set the priority */
    IntPrioritySet(SYS_INT_TINT3, 0, AINTC_HOSTINT_ROUTE_IRQ);
    /* Enable the system interrupt */
    IntSystemEnable(SYS_INT_TINT3);
}

static void DMTimerIsr(void)
{
    /* Clear the status of the interrupt flags */
    DMTimerIntStatusClear(SOC_DMTIMER_3_REGS, DMTIMER_INT_OVF_IT_FLAG);
    systemTime ++;

/*
    static bool toggle=false;
    toggle = !toggle;
    if(toggle){
    GPIOPinWrite(GPIO_INSTANCE_ADDRESS,
                     GPIO_INSTANCE_PIN_NUMBER,
                     GPIO_PIN_LOW);
    }else{
    	            GPIOPinWrite(GPIO_INSTANCE_ADDRESS,
    	                     GPIO_INSTANCE_PIN_NUMBER,
    	                     GPIO_PIN_HIGH);

    }
  */
}


/*-----------------------------------------------------------*/
/* Returns the number of passed nanoseconds since the
 * system was booted up.
 */
unsigned long long hwGetNanoseconds(){
  uint32_t localTime,localTime2,cur;

  do{
	  localTime=systemTime;
	  cur = DMTimerCounterGet(SOC_DMTIMER_3_REGS);
	  localTime2=systemTime;
  }while(localTime != localTime2);//Try again if interupt occured

  return (uint64_t)localTime*SECONDS + ((uint64_t)(cur-TIME_TIMER_STARTVALUE) * MICROSECONDS / TICKS_PER_USEC);
}

/*-----------------------------------------------------------*/
/* While time events are mostly based on the value returned
 * by this function, a pseudo random number generator greatly
 * benefits from increased entropy and relies on
 * hwGetAbsoluteNanoseconds() which adds the value of an
 * internal real-time clock if present. Otherwise, it just
 * returns the same value as hwGetNanoseconds().
 */
unsigned long long hwGetAbsoluteNanoseconds(){
	return hwGetNanoseconds();
}

void hwResetAndReboot() {
}
/**
  *  global interrupt disable
  */
void hwDisableInterrupts(){
	CPUirqd();
}
/**
  *  global interrupt enable
  */
void hwEnableInterrupts(){
	CPUirqe();
}


/*-----------------------------------------------------------*/
/* It is only used on top of other operating systems such as
 * Linux to allow additional space/time partitions in terms
 * of processes.
 */
void sp_partition_yield(){
	return;
}


/*-----------------------------------------------------------*/
/*
 * Create context on stack and return a pointer to it
 * For every thread this function is called.
 *
 * @ params:
 * -> *stack is the pointer to the thread's stack and provides the basic to
 * compute an appropriate stack pointer.
 * -> *object is the thread's object pointer that is passed to the callback function.
 * It's stored in the register that makes up the first argument on a function call.
 */
long* hwInitContext(long* stack, void* object) {
  	long* tos = stack;		/* Top Of Stack */
	*tos = 0xdeaddead;		/* debug */
	TContext *ctx = (TContext*) (tos - sizeof(TContext)/sizeof(long));

	ctx->r0 = (long) object;
	ctx->r1 = 0;
	ctx->r2 = 0;
	ctx->r3 = 0;
	ctx->r4 = 0;
	ctx->r5 = 0;
	ctx->r6 = 0;
	ctx->r7 = 0;
	ctx->r8 = 0;
	ctx->r9 = 0;
	ctx->r10 = 0;
	ctx->r11 = 0;
	ctx->r12 = 0;
	ctx->SP = ((long) ctx) & (~7);
	ctx->LR = 0;
	ctx->PC = (long) threadStartupWrapper;	/* this method is defined in /bare-meta-generic/thread_on_hw.cpp */

	/* ctx->CPSR = CPSR */
	asm volatile (
		".equ I_BIT, 0x80\n\t"
		"mrs  %0, CPSR     \n\t"
		"bic  %0, %0, #I_BIT     \n\t"
		: "=r"(ctx->CPSR)       // Output registers
	);

	return (long*) ctx;
}


/*-----------------------------------------------------------*/
/*
 */

long getNodeNummer(){
	return 0;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif

