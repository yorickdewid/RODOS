/**
 * @file switch.S
 * @author Tobias Mikschl <tobias.mikschl@uni-wuerzburg.de>
 *
 * @brief SaveContext and SwitchToContext functions for the Scheduler.
 */


#include "context.h"
  .section  .text
  .align    2

__asmSwitchToContext:
    .globl __asmSwitchToContext
    .func __asmSwitchToContext
    .type __asmSwitchToContext,"function"
    /* void __asmSwitchToContext(long* context) */

    mr 1, 3 /* load pointer to context data (long* context) */
		
		/* Load MSRS */
    mfmsr 0
    /* Disable external Interrupts */
		wrteei 0
		/* Save MSR */
    mtsrr1 0 /* MSR */

    lwz 0, +164(1)
    mtsrr0 0 /* PC */

    LOAD_CONTEXT_FROM_STACK

    rfi /* return to the thread */
      /* (MSR) <= (SRR1) */
      /* (PC) <= (SRR0) */

.endfunc

__asmSaveContextAndCallScheduler:
    .globl __asmSaveContextAndCallScheduler
    .func __asmSaveContextAndCallScheduler
    .type __asmSaveContextAndCallScheduler,"function"
    /* void __asmSaveContextAndCallScheduler() */


		SAVE_CONTEXT_TO_STACK

    mflr 0
    stw 0,164(1) /* store LR as new PC */
		
		/* call void schedulerWrapper(long* ctx) */
    mr 3, 1 /* pass pointer to context data (long* ctx) */
    b schedulerWrapper /* call */
		

.endfunc

