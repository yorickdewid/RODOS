/**
* @file context.h
* @date 2009/10/07 15:36
* @author David Waleczek
*
* Copyright 2009/10 DLR
*
* @brief memory layout for context saving
*
* description of memory layout for context saving
* for ARM 32-bit architecture
*
*/

#ifndef ARMv4_CONTEXT_H
#define ARMv4_CONTEXT_H

namespace RODOS {
/**
* @struct s_context
* @brief memory layout for context saving
*
* @internal
*
* This struct describes the memory layout used for context saving.
* I omitted the SPSR because it is only available in exception mode,
* reading or writing to it in system or user mode can cause unpredictable behavior
*  See "Procedure Call Standard for the ARM Architecture"
*/
typedef struct s_context
{
  /* @{ The context of the thread. */
  long CPSR; /*Current Status Register*/

  /* @{ Argument / scratch register */
  long r0; /*can contain result*/
  long r1; /*can contain result*/
  long r2;
  long r3;
  /* @}} */
  
  /* @{ Variable-register */
  long r4;
  long r5;
  long r6;
  long r7;
  long r8;
  long r9;
  long r10;
  long r11;
  /* @} */
  long r12;		/* the Intra-Procedure-call scratch register */
  long SP;		/* the Stack Pointer */
  long LR;		/* the Link Register */
  long PC;		/* the Program Counter */

} TContext;

}
#endif /* ARMv4_CONTEXT_H */
