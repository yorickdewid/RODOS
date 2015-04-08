
#ifndef ARMv4_CONSTANTS_H
#define ARMv4_CONSTANTS_H

#define MODE_MASK		0x1F	/* lower 5 bits of CPSR */

#define MODE_USR		0x10
#define MODE_FIQ		0x11
#define MODE_IRQ		0x12
#define MODE_SVC		0x13
#define MODE_ABT		0x17
#define MODE_UND		0x1B
#define MODE_SYS		0x1F

#define CPSR_N_BIT		(1 << 31)
#define CPSR_Z_BIT		(1 << 30)
#define CPSR_C_BIT		(1 << 29)
#define CPSR_V_BIT		(1 << 28)
#define CPSR_I_BIT		(1 << 7)
#define CPSR_F_BIT		(1 << 6)
#define CPSR_T_BIT		(1 << 5)

#define IRQ_DISABLE		(1 << 7)	/* when I bit is set, IRQ is disabled */
#define FIQ_DISABLE		(1 << 6)	/* when F bit is set, FIQ is disabled */
#define THUMB_ENABLE 	(1 << 5)	/* when T bit is set, Thumb mode is enabled */

/* constant to pre-fill the stack */
#define STACK_FILL				0xAAAAAAAA
#define END_OF_STACK_MARKER 	0xDEADBEEF

#ifdef __ASSEMBLER__

// Register locations
#define T0IR			0xE0004000
#define VIC_BASE		0xFFFFF000
#define VIC_VectAddr	0xF00

#endif

#endif	// ARMv4_CONSTANTS_H
