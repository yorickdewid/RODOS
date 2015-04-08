
//*****************************************************************************
//
// startup_gcc.c - Startup code for use with GNU tools.
//
// Copyright (c) 2007 Luminary Micro, Inc.  All rights reserved.
// 
// Software License Agreement
// 
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's microcontroller products.
// 
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  You may not combine
// this software with "viral" open-source software in order to form a larger
// program.  Any use in violation of the foregoing restrictions may subject
// the user to criminal sanctions under applicable laws, as well as to civil
// liability for the breach of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 1900 of the Stellaris Peripheral Driver Library.
//
//*****************************************************************************



#ifdef __cplusplus
extern "C"
{
#endif


extern void __libc_init_array(void);  /* calls CTORS of static objects */
//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
void ResetISR(void);
static void NmiSR(void);
static void FaultISR(void);
static void IntDefaultHandler(void);

//*****************************************************************************
//
// External declaration for the interrupt handler used by the application.
//
//*****************************************************************************
extern void SysTick_Handler(void);
extern void PendSV_Handler(void) __attribute__ (( naked ));
extern void TIM2_Handler( void );
extern void USART1_IRQHandler ( void );
extern void USART2_IRQHandler ( void );
extern void USART3_IRQHandler ( void );
#ifndef STM32F10X_MD
extern void TIM5_IRQHandler(void);
extern void USART4_IRQHandler ( void );
extern void USART5_IRQHandler ( void );
#endif
//*****************************************************************************
//
// The entry point for the application.
//
//*****************************************************************************
extern int main(void);

//*****************************************************************************
//
// System stack is located at the end of RAM.
// the variable "_estack" is provided by the linker -> see linker script
// _estack = RAM-Start + RAM-Length
//
//*****************************************************************************
extern unsigned long _estack;

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{
	(void (*)(void))(&_estack),				// The initial stack pointer
	ResetISR,                               // The reset handler
    NmiSR,                                  // The NMI handler
    FaultISR,                               // The hard fault handler
    IntDefaultHandler,                      // The MPU fault handler
    IntDefaultHandler,                      // The bus fault handler
    IntDefaultHandler,                      // The usage fault handler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    IntDefaultHandler,                      // SVCall handler
    IntDefaultHandler,                      // Debug monitor handler
    0,                                      // Reserved
    PendSV_Handler,                      	// The PendSV handler
    SysTick_Handler,                        // The SysTick handler
    IntDefaultHandler,                      // 0
    IntDefaultHandler,                      // 1
    IntDefaultHandler,                      // 2
    IntDefaultHandler,                      // 3
    IntDefaultHandler,                      // 4
    IntDefaultHandler,                      // 5
    IntDefaultHandler,                      // 6
    IntDefaultHandler,                      // 7
    IntDefaultHandler,                      // 8
    IntDefaultHandler,                      // 9
    IntDefaultHandler,                      // 10
    IntDefaultHandler,                      // 11
    IntDefaultHandler,                      // 12
    IntDefaultHandler,                      // 13
    IntDefaultHandler,                      // 14
    IntDefaultHandler,                      // 15
    IntDefaultHandler,                      // 16
    IntDefaultHandler,                      // 17
    IntDefaultHandler,                      // 18
    IntDefaultHandler,						// 19
    IntDefaultHandler,                      // 20
    IntDefaultHandler,                      // 21
    IntDefaultHandler,                      // 22
    IntDefaultHandler,                      // 23
    IntDefaultHandler,                 		// 24 Timer 1  break interrupt
    IntDefaultHandler,                      // 25
    IntDefaultHandler,                      // 26
    IntDefaultHandler,                 		// 27
    TIM2_Handler, 		                	// 28 Timer 2 global interrupt
    IntDefaultHandler,                      // 29
    IntDefaultHandler,                      // 30
    IntDefaultHandler,                      // 31
    IntDefaultHandler,                      // 32
    IntDefaultHandler,                      // 33
    IntDefaultHandler,                      // 34
    IntDefaultHandler,                      // 35
    IntDefaultHandler,                      // 36
    USART1_IRQHandler,                      // 37
    USART2_IRQHandler,                      // 38
    USART3_IRQHandler,                      // 39
    IntDefaultHandler,                      // 40
    IntDefaultHandler,                      // 41
    IntDefaultHandler,						// 42
    IntDefaultHandler,						// 43
    IntDefaultHandler,						// 44
    IntDefaultHandler,						// 45
    IntDefaultHandler,						// 46
    IntDefaultHandler,						// 47
    IntDefaultHandler,						// 48
    IntDefaultHandler,						// 49
#ifndef STM32F10X_MD
    TIM5_IRQHandler,						// 50
    IntDefaultHandler,						// 51
    USART4_IRQHandler,						// 52
    USART5_IRQHandler,						// 53
#else
    IntDefaultHandler,						// 50
    IntDefaultHandler,						// 51
    IntDefaultHandler,						// 52
    IntDefaultHandler,						// 53
#endif
    IntDefaultHandler,						// 54
    IntDefaultHandler,						// 55
    IntDefaultHandler,						// 56
    IntDefaultHandler,						// 57
    IntDefaultHandler,						// 58
    IntDefaultHandler						// 59
};

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern unsigned long _etext;
extern unsigned long _sdata;
extern unsigned long _edata;
extern unsigned long _sbss;
extern unsigned long _ebss;
extern unsigned long _sidata;

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
void
ResetISR(void)
{
    unsigned long *pulSrc, *pulDest;

    //
    // Copy the data segment initializers from flash to SRAM.
    //
    pulSrc = &_sidata;
    for(pulDest = &_sdata; pulDest < &_edata; )
    {
        *pulDest++ = *pulSrc++;
    }

    //
    // Zero fill the bss segment.
    //
    for(pulDest = &_sbss; pulDest < &_ebss; )
    {
        *pulDest++ = 0;
    }

    //
    //Call the constructors
    //
	__libc_init_array();

    //
    // Call the application's entry point.
    //
    main();
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
static void
NmiSR(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
FaultISR(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
IntDefaultHandler(void)
{
    //
    // Go into an infinite loop.
    //
    while(1)
    {
    }
}


#ifdef __cplusplus
}
#endif
