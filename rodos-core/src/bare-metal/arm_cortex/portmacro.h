#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef configKERNEL_INTERRUPT_PRIORITY
	#define configKERNEL_INTERRUPT_PRIORITY 255
#endif
/* Constants required to manipulate the NVIC. */
#define portNVIC_SYSTICK_CTRL		( ( volatile unsigned portLONG *) 0xe000e010 )
#define portNVIC_SYSTICK_LOAD		( ( volatile unsigned portLONG *) 0xe000e014 )
#define portNVIC_INT_CTRL			( ( volatile unsigned portLONG *) 0xe000ed04 )
#define portNVIC_SYSPRI2			( ( volatile unsigned portLONG *) 0xe000ed20 )
#define portNVIC_SYSTICK_CLK		0x00000004
#define portNVIC_SYSTICK_INT		0x00000002
#define portNVIC_SYSTICK_ENABLE		0x00000001
#define portNVIC_PENDSVSET			0x10000000
#define portNVIC_PENDSV_PRI			( ( ( unsigned portLONG ) configKERNEL_INTERRUPT_PRIORITY ) << 16 )
#define portNVIC_SYSTICK_PRI		( ( ( unsigned portLONG ) configKERNEL_INTERRUPT_PRIORITY ) << 24 )
/*-----------------------------------------------------------
 * Port specific definitions.  
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	unsigned portLONG
#define portBASE_TYPE	long

#if( configUSE_16_BIT_TICKS == 1 )
	typedef unsigned portSHORT portTickType;
	#define portMAX_DELAY ( portTickType ) 0xffff
#else
	typedef unsigned portLONG portTickType;
	#define portMAX_DELAY ( portTickType ) 0xffffffff
#endif
/*-----------------------------------------------------------*/	

/* Architecture specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_RATE_MS			( ( portTickType ) 1000 / configTICK_RATE_HZ )		
#define portBYTE_ALIGNMENT			4
/*-----------------------------------------------------------*/	

/*-----------------------------------------------------------*/


/* Critical section management. */

#define vPortSetInterruptMask()							\
	__asm volatile										\
	(													\
		"	push { r0 }								\n"	\
		"	ldr r0, =ulKernelPriority 				\n"	\
		"	ldr r0, [r0]							\n"	\
		"	msr basepri, r0							\n" \
		"	pop { r0 }								"	\
	)
	
/*-----------------------------------------------------------*/

#define vPortClearInterruptMask()						\
	__asm volatile										\
	(													\
		"	push { r0 }								\n"	\
		"	mov r0, #0								\n"	\
		"	msr basepri, r0							\n"	\
		"	pop	 { r0 }								"	\
	)

/*-----------------------------------------------------------*/



#define portDISABLE_INTERRUPTS()	vPortSetInterruptMask();
#define portENABLE_INTERRUPTS()		vPortClearInterruptMask();



#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
