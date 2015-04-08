/**
* @file context.h
* @date 2013/03/21 15:12
* @author Tiago Duarte
*
* @brief memory layout for context saving in ARMv7 architecture
*
*/

#ifndef ARMv7_CONTEXT_H
#define ARMv7_CONTEXT_H

namespace RODOS {
	typedef struct s_context
	{
		/* @{ The context of the thread. */
		long CPSR; 	/* Current Status Register */

		/* @{ Argument, result or scratch register, named a1-a4 */
		long r0;
		long r1;
		long r2;
		long r3;
		/* @}} */

		/* @{ Variable-register, named v1-v8 */
		long r4;
		long r5;
		long r6;
		long r7;
		long r8;
		long r9;	/* SB - Static base register */
		long r10;
		long r11;
		/* @} */
		long r12;	/* IP - Intra-Procedure-call scratch register */
		long SP;	/* Stack Pointer */
		long LR;	/* Link Register - store the return address */
		long PC;	/* Program Counter */
	} TContext;
}

// Offsets
#define offset_CPSR		0
#define offset_R0		4
#define offset_R1		8
#define offset_R2		12
#define offset_R3		16
#define offset_R4		20
#define offset_R5		24
#define offset_R6		28
#define offset_R7		32
#define offset_R8		36
#define offset_R9		40
#define offset_R10		44
#define offset_R11		48
#define offset_R12		52
#define offset_SP		56
#define offset_LR		60
#define offset_PC		64

#endif /* ARMv7_CONTEXT_H */
