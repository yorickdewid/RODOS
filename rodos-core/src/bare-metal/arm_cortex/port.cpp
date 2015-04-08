#include "rodos.h"

#include "hw/hw_types.h"
#include "hw/hw_memmap.h"
#include "hw_specific.h"
#include "portmacro.h"
#include "src/uart.h"
#include "src/sysctl.h"
#include "src/systick.h"
#include "src/interrupt.h"
#include "src/uart.h"
#include "src/gpio.h"
#include "utils/uartstdio.h"



/* The priority used by the kernel is assigned to a variable to make access
from inline assembler easier. */
extern "C" const unsigned portLONG ulKernelPriority = configKERNEL_INTERRUPT_PRIORITY;
unsigned long long nanoTime=1000LL;
unsigned long long microTimeInterval;
volatile long *contextT;

extern "C" void xPortPendSVHandler( void ) __attribute__ (( naked ));
extern "C" void xPortSysTickHandler( void );// __attribute__ (( naked ));
extern "C" void vPortYieldFromISR( void );


void vPortYieldFromISR( void )
{
	//nanoTime +=(microTimeInterval*1000LL);
	/* Set a PendSV to request a context switch. */
//	*(portNVIC_INT_CTRL) |= portNVIC_PENDSVSET;

	/* This function is also called in response to a __asmSaveContextAndCallScheduler, 
	 * so we want the yield to occur immediately. */
//	portENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

void xPortPendSVHandler( void )
{

	/* Start first task if the stack has not yet been setup. */
	__asm volatile
	(
		        " mrs    r0, psp						\n"                   /*  in PSP - process(task) stack pointer */
		       "	cbz r0, no_save					\n"
				 "									\n"	// Save the context 
		         " stmdb  r0!, {r4-r11}						\n"
		         " push {r14}						\n"
		         " push { r0 }						\n"
				 " ldr r0, =ulKernelPriority						\n"
				 " ldr r0, [r0]						\n"
				 " msr basepri, r0 						\n"
				 " pop { r0 }						\n"
		       " bl schedulerWrapper						\n"				//call scheduler
		         " push { r0 } 						\n"
				 " mov r0, #0 						\n"
				 " msr basepri, r0 						\n"
			  	 " pop	 { r0 }						\n"
				 " pop {r14}						\n"
		       "no_save:\n"
				" ldr r1, =contextT						\n"
		   	    " ldr r0, [r1] 						\n"
		        " ldmia  r0!, {r4-r11} 						\n"
		        " msr    psp, r0     						\n"
		        " orr r14, #0xd 						\n"           /*  Force to new process PSP */
		       " bx     r14						\n"
	);
}
/*-----------------------------------------------------------*/

extern long long timeToTryAgainToSchedule;
extern bool isSchedulingEnabled;

void xPortSysTickHandler( void ) {
	if(!isSchedulingEnabled) return;

	long long timeNow = NOW();  // comment this out to improve performance, but: no time events any more
	TimeEvent::propagate(timeNow); // comment this out to improve performance, but: no time events any more

	if(NOW() < timeToTryAgainToSchedule) return;

	*(portNVIC_INT_CTRL) |= portNVIC_PENDSVSET;
/*	vPortYieldFromISR();
	extern void vPortYieldFromISR( void );

	__asm volatile
	( 
	"	push {r14}							\n"
	"	bl vPortYieldFromISR				\n"
	"	pop {r14}							\n"
	"    bx r14"
	);
*/

}

void __asmSwitchToContext(long* context)
{
	contextT = context;
}

void __asmSaveContextAndCallScheduler()
{
	//SysTickIntEnable();
	/* Set a PendSV to request a context switch. */
	*(portNVIC_INT_CTRL) |= portNVIC_PENDSVSET;

	/* This function is also called in response to a Yield(), so we want
	the yield to occur immediately. */
	portENABLE_INTERRUPTS();
}
