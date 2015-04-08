/*********************************************************** Copyright 
 **
 ** Copyright (c) 2008, German Aerospace Center (DLR)
 ** All rights reserved.                                                    
 ** 
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 ** 
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 ** 
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 ** 
 ** 3 Neither the name of the German Aerospace Center nor the names of
 **   its contributors may be used to endorse or promote products derived
 **   from this software without specific prior written permission.
 ** 
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **  
 ****************************************************************************/

/**
 * @file hw_specific.cc
 * @date 2008/04/23 7:33
 * @port 2010/02/18 19:14
 * @author Miroslaw Sulejczak, modified by Michael Ruffer
 *
 * Copyright 2010 DLR
 *
 * @brief all hardware specific stuff I have no better place for ...
 *
 */

#include <sys/stat.h>
#include <errno.h>

#include "rodos.h"

#include "params.h"
#include "hw_specific.h"
#include "hal/hal_uart.h"

#include "stm32f4xx.h"
#include "core_cm4.h"

// watchdog related includes
#include "stm32f4xx_iwdg.h"
#include "stm32f4xx_misc.h"

volatile long *contextT;

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/* Constants required to set up the initial stack. */
#define INITIAL_XPSR			( 0x01000000 )
#define INITIAL_EXEC_RETURN		( 0xfffffffd )

// - on interrupt the cortex m3 saves automatically the context of the interrupted function
// - it saves the registers xPSR,PC,LR,r12,r3,r2,r1,r0 on stack
// - that is why it's not necessary that the compiler adds code to save the context, so
//   we have to add the attribute "naked" to avoid this
extern "C" {
void PendSV_Handler(void) __attribute__ (( naked ));
void SVC_Handler(void) __attribute__ (( naked ));
}

volatile long taskRunning = 0;

/** function to do hardware initialization 
 * -> hwInit() is called in main() (src/independent/main.cpp)
 */

HAL_UART uart_stdout(UART_DEBUG);

void TIMx_init(); // timer for system time -> see hw_timer.cpp

/* measure LSI frequency using Timer 5 -> necessary for watchdog
 * -> only once in hwInit() -> afterwards Timer 5 is free
 */
static uint32_t GetLSIFrequency(void);
static uint32_t LSIFrequency = 0;

void hwInit() {
	taskRunning = 0;

	/* Make PendSV, SysTick and Timer2 the same priroity */
	NVIC_SetPriority(PendSV_IRQn, 255);
	NVIC_SetPriority(SysTick_IRQn, 255);
	//NVIC_SetPriority(TIM2_IRQn, 255); // moved to TIMx_init()

	// initialization of stm32f4:
	// set system clock to 168MHz
	// init RCC
	// ...
//	SystemInit(); // is called in startup code before constructor call and main
	SystemCoreClockUpdate(); // update a global variable holding the system clock

	LSIFrequency = GetLSIFrequency();

	//Determine the Node Number from the first bytes of the chips UID
	extern long myNodeNr;
	#define STM32F4_UID ((unsigned char*)(0x1FFF7A10))
	#define STM32F4_UID_SIZE 12
	myNodeNr= *(long*)STM32F4_UID;

	//UART for stdout
	uart_stdout.init(115200);
	//uart_stdout.config(UART_PARAMETER_ENABLE_DMA,32);
	//uart_stdout.config(UART_PARAMETER_HW_FLOW_CONTROL, 1); // necessary for bluetooth module

	TIMx_init(); // Timer for system time

	RCC_ClocksTypeDef  rcc_clocks;
	RCC_GetClocksFreq(&rcc_clocks);
	PRINTF("CPU: %d MHz\n",rcc_clocks.SYSCLK_Frequency/1000000);
}

/**
 *  abort execution
 */
void hwResetAndReboot() {
    //NVIC_SystemReset();
    hwInitWatchdog(0);
    while(1){}
}

/**
  *  global interrupt disable
  */
void hwDisableInterrupts(){
    __disable_irq();
}

/**
  *  global interrupt enable
  */
void hwEnableInterrupts(){
    __enable_irq();
}

/**
 * create context on stack and return a pointer to it
 * - very helpful document: "PM0214 Programming manual - STM32F3xxx and STM32F4xxx Cortex-M4 programming manual"
 * - page 42
 */
long* hwInitContext(long* stack, void* object) {

	stack--; // to maintain the initial stack pointer double word aligned
			 // we have to decrement the stack pointer by an even number (FA-2012.02
	/* Simulate the stack frame as it would be created by a context switch
	 interrupt. */
	*stack = INITIAL_XPSR;					// xPSR
	stack--;
	*stack = (long) (threadStartupWrapper);	// PC
	stack--;
	*stack = 0;								// LR
	stack -= 5;								// R12, R3, R2 and R1
	*stack = (long) object; 				// R0
	stack--;
	*stack = INITIAL_EXEC_RETURN;
	stack -= 8;							// R11, R10, R9, R8, R7, R6, R5 and R4

	return stack;
}

/**
 * In case a network is avialable, the corresponding node nr.
 */

// TW: already defind in rodos/src/independent/globaldata.cpp
//long myNodeNr = -1 ;
//long getNodeNumber() { return myNodeNr; }
void startIdleThread() __attribute__ (( naked ));
void startIdleThread() {
	__asm volatile(
			" ldr r0, =0xE000ED08 	\n" /* Use the NVIC offset register to locate the stack. */
			" ldr r0, [r0] 			\n"
			" ldr r0, [r0] 			\n"
			" msr msp, r0			\n" /* Set the msp back to the start of the stack. */
			" cpsie i				\n" /* Globally enable interrupts. */
			" svc 0					\n" /* System call to start first task. */
			" nop					\n"
	);
}

extern "C" {
/*
 * SVC_Handler is only used to start the first thread
 * - We do this procedure to load rest of context of first thread (idleThread) and switch to PSP
 * -> PSP (Process Stack Pointer) is used in threads
 * -> MSP (Main Stack Pointer) is used in exception handlers (so in scheduler)
 * - MSP is the default stack pointer after reset
 */
void SVC_Handler(void) {
	__asm volatile (
			"	ldr	r1, =contextT				\n" // after start "contextT" holds the stack pointer of first thread
			"	ldr r0, [r1]					\n"// load stack pointer of first thread (idleThread)
			"	ldmia r0!, {r4-r11, r14}		\n"// Pop the registers that are not automatically saved on exception entry and the critical nesting count.
			"	msr psp, r0						\n"// load stack pointer of first thread to PSP -> PSP is used after leaving ISR
			"	bx r14							\n"
			"									\n"
			"	.align 2						\n"
	);
}

/* When the ISR is entered the following registered are saved automatically:
 * FPSCR, S15 ... S0, xPSR, PC, LR, R12, R3, R2, R1, R0
 * -> the other registers (R11 ... R4, S31 ... S16) must be saved and restored manually
 * -> very helpful document: "PM0214 Programming manual - STM32F3xxx and STM32F4xxx Cortex-M4 programming manual" page 42
 */
void PendSV_Handler(void) {
	__asm volatile
	(
			"	cpsid f								\n" // disable all interrupts
			"	mrs r0, psp							\n"// load stack pointer of interrupted thread in r0 -> r0 is used to save all not automatically saved core and fpu registers
			"										\n"
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
			"	tst r14, #0x10						\n" // Is the task using the FPU context?  If so, push high vfp registers.
			"	it eq								\n"
			"	vstmdbeq r0!, {s16-s31}				\n"// save FPU registers on stack of interrupted thread
#endif
			"										\n"
			"	stmdb r0!, {r4-r11, r14}			\n" // save core registers on stack of interrupted thread
			"										\n"
			"	bl schedulerWrapper					\n"
			"										\n"
			"	ldr r1, =contextT					\n"// "contextT" holds the stack pointer of next thread
			"	ldr r0, [r1]						\n"// load stack pointer of next thread
			"										\n"
			"	ldmia r0!, {r4-r11, r14}			\n"// Pop the core registers
			"										\n"
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
			"	tst r14, #0x10						\n" // Is the task using the FPU context?  If so, pop the high vfp registers too
			"	it eq								\n"
			"	vldmiaeq r0!, {s16-s31}				\n"
#endif
			"										\n"
			"	msr psp, r0							\n" // load psp with stack pointer of next thread -> PSP (process stack pointer) is used after leaving ISR
			"	cpsie f								\n"// enable all interrupts
			"	bx r14								\n"
			"										\n"
			"	.align 2							\n"
	);
}

void __asmSwitchToContext(long* context) {
	contextT = context;
}

void __asmSaveContextAndCallScheduler() {
	/* Set a PendSV-interrupt to request a context switch. */
	SCB ->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
}

/*
 * Initialize and start HW-watchdog
 * - it is used the IWDG (Independent Watchdog) of the STM32F4
 * - it is clocked by it's own independent low-speed clock (LSI)
 * - LSI can vary between 17 kHz and 47 kHz (32kHz typical)
 *   -> so it must be measured using Timer 5 -> done in hwInit
 * - max. interval@47kHz: 1/(47kHz/256) * 0xFFF = 22305 ms
 * - max. interval@32kHz: 1/(32kHz/256) * 0xFFF = 32768 ms
 * - max. interval@17kHz: 1/(17kHz/256) * 0xFFF = 61666 ms
 */
void hwInitWatchdog(long intervalMilliseconds) {
	/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable );

	/* set IWDG counter clock prescaler depending on intervalMilliseconds to improve range and accuracy*/
	uint32_t prescaler = 0;
	if (intervalMilliseconds < 1000) {
		IWDG_SetPrescaler(IWDG_Prescaler_16 );
		prescaler = 16;
	} else {
		IWDG_SetPrescaler(IWDG_Prescaler_256 );
		prescaler = 256;
	}

	/* Set counter reload value
	 Counter Reload Value = intervalMilliseconds / IWDG counter clock period
	 = intervalMilliseconds / (LsiFreq/prescaler)
	 */
	double wdgClk = (double) LSIFrequency / (double) prescaler;
	double intervalSec = (double) intervalMilliseconds / (double) 1000;
	uint32_t reloadVal = intervalSec * wdgClk;
	if (reloadVal > 0xFFF) {
		reloadVal = 0xFFF; // set to maximum reload value
	}
	IWDG_SetReload(reloadVal);

	/* Reload IWDG counter */
	IWDG_ReloadCounter();

	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG_Enable();
}

/*
 * trigger HW-watchdog -> must be done periodically with less than "intervalMilliseconds" ms
 */
void hwTriggerWatchdog() {
	IWDG_ReloadCounter();
}

/**
 * @brief  This function handles TIM5 global interrupt during LSI measurement on startup.
 *         This is done only once in hwInit().
 * @param  None
 * @retval None
 */
static volatile unsigned int PeriodValue = 0;
static volatile bool captureRdy = false;
static uint32_t CaptureNumber = 0;

void TIM5_ISR4LSI(){
    static uint16_t tmpCC4[2] = { 0 };

    if (TIM_GetITStatus(TIM5, TIM_IT_CC4 ) != RESET) {
        /* Get the Input Capture value */
        tmpCC4[CaptureNumber++] = TIM_GetCapture4(TIM5 );

        /* Clear CC4 Interrupt pending bit */
        TIM_ClearITPendingBit(TIM5, TIM_IT_CC4 );

        if (CaptureNumber >= 2) {
            /* Compute the period length */
            PeriodValue = (uint16_t) (0xFFFF - tmpCC4[0] + tmpCC4[1] + 1);
            CaptureNumber = 0;
            captureRdy = true;
        }
    }
}


/* when redefining TIM5_ISR_USER remember to put it in namespace RODOS
 * Example:
    #ifndef NO_RODOS_NAMESPACE
    namespace RODOS {
    #endif
        void TIM5_ISR_USER(){
            // do your stuff
        }
    #ifndef NO_RODOS_NAMESPACE
    }
    #endif
 */
void TIM5_ISR_USER()__attribute__((weak));
void TIM5_ISR_USER(){}

/**
 * @brief  This function handles TIM5 global interrupt request.
 * @param  None
 * @retval None
 */
extern "C" {
void TIM5_IRQHandler(void) {
    if (LSIFrequency == 0){
        TIM5_ISR4LSI();
    }else{
        TIM5_ISR_USER();
    }
}
} // end extern "C"

/**
 * @brief  Configures TIM5 to measure the LSI oscillator frequency.
 * 		The LSI frequency is used to init the Independent Watchdog.
 * @param  None
 * @retval LSI Frequency
 */
static uint32_t GetLSIFrequency(void) {
    if (LSIFrequency != 0) return LSIFrequency;

	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	RCC_ClocksTypeDef RCC_ClockFreq;

	// initialize all global variables -> that's really important if function is called more than one time
	PeriodValue = 0;
	captureRdy = false;
	CaptureNumber = 0;

	/* Enable the LSI oscillator ************************************************/
	RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY ) == RESET) {
	}

	/* TIM5 configuration *******************************************************/
	/* Enable TIM5 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* Connect internally the TIM5_CH4 Input Capture to the LSI clock output */
	TIM_RemapConfig(TIM5, TIM5_LSI );

	/* Configure TIM5 presclaer */
	TIM_PrescalerConfig(TIM5, 0, TIM_PSCReloadMode_Immediate );

	/* TIM5 configuration: Input Capture mode ---------------------
	 The LSI oscillator is connected to TIM5 CH4
	 The Rising edge is used as active edge,
	 The TIM5 CCR4 is used to compute the frequency value
	 ------------------------------------------------------------ */
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInit(TIM5, &TIM_ICInitStructure);

	/* Enable TIM5 Interrupt channel */
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable TIM5 counter */
	TIM_Cmd(TIM5, ENABLE);

	/* Reset the flags */
	TIM5->SR = 0;

	/* Enable the CC4 Interrupt Request */
	TIM_ITConfig(TIM5, TIM_IT_CC4, ENABLE);

	/* Wait until the TIM5 get 2 LSI edges (refer to TIM5_IRQHandler() ) */
	while (!captureRdy) {
	}

	/* Deinitialize the TIM5 peripheral registers to their default reset values */
	TIM_DeInit(TIM5 );

	/* Compute the LSI frequency, depending on TIM5 input clock frequency (PCLK1)*/
	/* Get SYSCLK, HCLK and PCLKx frequency */
	RCC_GetClocksFreq(&RCC_ClockFreq);

	/* Get PCLK1 prescaler */
	if ((RCC ->CFGR & RCC_CFGR_PPRE1 )== 0){
		/* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
		return ((RCC_ClockFreq.PCLK1_Frequency / PeriodValue) * 8);
	} else { /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
		return (((2 * RCC_ClockFreq.PCLK1_Frequency) / PeriodValue) * 8);
	}
}

#ifndef NO_RODOS_NAMESPACE
}
#endif

