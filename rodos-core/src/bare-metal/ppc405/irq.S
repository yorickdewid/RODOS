/**
 * @file irq.S
 * @author Tobias Mikschl <tobias.mikschl@uni-wuerzburg.de>
 *
 * @brief Interrupt routines in assembly. May branch to 
 *        a C-Function after context is saved.
 */


#include "context.h"


  .section .data
  .align 2
		ctxptr: .word 0  /* Context Pointer need for PIT Interrupt */

  .section  .text
  .align    2


/**
 * @brief Programmable Interval Timer (PIT) Exception.
 */
  .globl    _pitInterrupt
_pitInterrupt:

  SAVE_CONTEXT_TO_STACK

  /* Clear Pit Interrupt */
  lis 0, 0x0800
  mttsr 0 /* clear  */

  /* Save Pointer to Context */
  lis 4, ctxptr@h
  ori 4,4,ctxptr@l
  stw 1,0(4)

  /* Call C Service Routine */
  bl interruptPit

  LOAD_CONTEXT_FROM_STACK
  
  /* rfi: PC <- SRR0
          MSR <- SRR1 
    -> External Interrupts reenabled */
  rfi /* return */


/**
 * @brief Function for scheduler call.
 *        Necessary for preemption interrupt (PIT)
 */
.globl    __asmCallScheduler
__asmCallScheduler:  
  /* call scheduler */

  /* Get Pointer to Context and
     put in r3 */
  lis 4, ctxptr@h
  ori 4,4,ctxptr@l
  lwz 3,0(4)

  mfsrr0 31
  stw 31, +164(3) /* Save PC */
  
  /* Reset MSR -> Caution: External Interrupts reenabled */
  mfsrr1 0
  mtmsr 0
   
  /* call void schedulerWrapper(long* ctx) */
  /* r3 (pointer to context) is parameter */
  b schedulerWrapper /* call */


/**
 * @brief External Interrupt Exception.
 */
  .globl _externalInterrupt
_externalInterrupt:
  SAVE_CONTEXT_TO_STACK
  bl interruptExternal
  LOAD_CONTEXT_FROM_STACK
  rfi

