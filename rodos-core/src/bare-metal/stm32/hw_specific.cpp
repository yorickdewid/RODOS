

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
#include "led.h"
#include "adc.h"

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_dbgmcu.h"
#include "core_cm3.h"
// watchdog related includes
#include "stm32f10x_gpio.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_misc.h"

#include "../../api/yprintf.h"

volatile long *contextT;

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/* timer2 is used to generate the system time
 * -> TIM2_PRESCALER_VALUE is the value which is written in the prescaler register of timer2
 * -> TIM2_PRESCALER_VALUE must be set properly to avoid that the value written in the period register
 *    doesn't exceed 0xFFFF
 * -> max. clock = 72MHz, timerClock = 72Mhz/(PRESCALER+1) = 72MHz/12 = 6MHz
 * -> max. timerCount = timerClock/100 (if interrupt every 10ms) = 60000
 */
#define TIM2_PRESCALER_VALUE 11

// global variable that holds the clock timer 2 is running at (after hwInit() was called)
static unsigned int tim2Clock = 0;

/* Constants required to set up the initial stack. */
#define INITIAL_XPSR			( 0x01000000 )


// - on interrupt the cortex m3 saves automatically the context of the interrupted function
// - it saves the registers xPSR,PC,LR,r12,r3,r2,r1,r0 on stack
// - that is why it's not necessary that the compiler adds code to save the context, so
//   we have to add the attribute "naked" to avoid this
extern "C" void PendSV_Handler( void ) __attribute__ (( naked ));

// don't use "naked" for the following ISRs, because the system crashes
// -> maybe because not all registers are saved automatically
// -> it works when the compiler puts prologue and epilogue in the ISR
//    -> but this can be a problem when the ISR can interrupt each other
//    -> this can happen when they don't have the same priority
extern "C" void SysTick_Handler( void );// __attribute__ (( naked ));
extern "C" void TIM2_Handler( void );

void prvSetMSP( unsigned long ulValue ) __attribute__ (( naked ));
void prvSetPSP( unsigned long ulValue ) __attribute__ (( naked ));


volatile long taskRunning = 0;

extern long long timeToTryAgainToSchedule;
extern bool isSchedulingEnabled;

extern "C"
{
unsigned long long nanoTime;



void SysTick_Handler() {
	if(!isSchedulingEnabled) return;

	long long timeNow = NOW();  // comment this out to improve performance, but: no time events any more
	TimeEvent::propagate(timeNow); // comment this out to improve performance, but: no time events any more

	if(NOW() < timeToTryAgainToSchedule) return;

	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; // set SW-IRQ to call scheduler
}



void TIM2_Handler() 
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	//TIM2->SR = (uint16_t)~TIM_IT_Update;
	NVIC_ClearPendingIRQ(TIM2_IRQn);
    nanoTime += 10000000; // 10M ns for each 10ms-tick
}

}


unsigned long long hwGetNanoseconds() {

	int count = 0;
	long long returnTime = 0;

	// to get the correct time, we have to disable interrupts, while taking a snapshot of the
	// time variables
	// -> current time = nanoTime + 1 000 000 000 * countRegister/tim2Clock
	unsigned int faultmask = __get_FAULTMASK(); // get global interrupt mask to restore value afterwards
	__set_FAULTMASK(1);			// disable interrupt
	count = TIM_GetCounter(TIM2);
	returnTime = nanoTime;
	__set_FAULTMASK(faultmask);	// set faultmask to its old value -> so interrupts will only
								// be enabled if they have been enabled before

	/** low precision
	 * - nanos = 166ns * count (for tim2Clock = 6MHz)
	 * ! startup time might be wrong due to integer overflow !
	 */
	//int nanos = 1000000000/tim2Clock * count;

	/** high precision
	 * - nanos = 166,666666ns * count (for tim2Clock = 6MHz)
	 * - nanos = ns * count (for tim2Clock = 6MHz)
	 * - takes 4 times longer than low precision
	 */
	long long nanos = 0;
	nanos = 1000000000/(tim2Clock/1000000);
	nanos *= count;
	nanos /= 1000000;

	return returnTime + nanos;
}



void hwInitTime()
{
	nanoTime=0;
}



unsigned long long hwGetAbsoluteNanoseconds(void) 
{
	return hwGetNanoseconds();// + timeAtStartup;
}



void prvSetPSP( unsigned long ulValue )
{
	__asm volatile( "msr psp, r0" );
	__asm volatile( "bx lr" );
}



void prvSetMSP( unsigned long ulValue )
{
	__asm volatile( "msr msp, r0" );
	__asm volatile( "bx lr" );
}



void startIdleThread()
{
	/* Start the first task. */
	prvSetPSP( 0 );
	
	// calling the Scheduler the first time by rising a SW-IRQ
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; 
}

HAL_UART uart_stdout(UART_DEBUG);

/** function to do hardware initialization 
* -> hwInit() is called in main() (src/independent/main.cpp)
*/
void hwInit()
{
	// local variables to initialize timer2
	TIM_TimeBaseInitTypeDef timStruct;
	unsigned char timClockMultiplier = 0;
	RCC_ClocksTypeDef clocks;


	taskRunning = 0;

   	/* Make PendSV, SysTick and Timer2 the same priroity */
	NVIC_SetPriority (PendSV_IRQn,255);
	NVIC_SetPriority (SysTick_IRQn,255);
	NVIC_SetPriority (TIM2_IRQn,255);

	// initialization of stm32:
	// set system clock to 72MHz
	// init RCC
	// ...
	SystemInit();
	SystemCoreClockUpdate(); // update a global variable holding the system clock

	//led_init();	// inits GPIO PC12 as Output -> on olimex evalboard is a LED connected to this pin
	//adc_init();

	uart_stdout.init(115200);

	// init timer for system time
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, DISABLE);
	RCC_GetClocksFreq(&clocks);

	TIM_DeInit(TIM2);

	// if the APB1 Prescaler is bigger than 1, the PCLK1 is multiplied by 2 infront of the timer
	if(clocks.HCLK_Frequency/clocks.PCLK1_Frequency > 1)
	{ timClockMultiplier = 2; }
	else
	{ timClockMultiplier = 1; }
	
	// TIM_Period = timer-clk divided by 100 to get an interrupt every 10ms
	tim2Clock = (clocks.PCLK1_Frequency*timClockMultiplier) / (TIM2_PRESCALER_VALUE+1);
	timStruct.TIM_Period = tim2Clock / 100;
	
	// Prescaler
	// fCK_CNT = fCK_PSC/(TIM_Prescaler+1)
	// - it's important to set the prescaler properly, because TIM2 is only a 16Bit counter with
	//   a maximum reload-value of 2^16 = 65536
	timStruct.TIM_Prescaler 		= TIM2_PRESCALER_VALUE;		

	// timer mode is "count down"
	//timStruct.TIM_CounterMode 		= TIM_CounterMode_Down;
	timStruct.TIM_CounterMode 		= TIM_CounterMode_Up;
	
	// TIM_ClockDivision = division ratio between the timer clock (CK_INT) 
	// frequency and sampling clock used by the digital filters 
	// -> not interesting for our purpose !!!
	timStruct.TIM_ClockDivision		= TIM_CKD_DIV1; 

	// only valid for TIM1 and TIM8
	timStruct.TIM_RepetitionCounter	= 0; 			

	TIM_TimeBaseInit(TIM2,&timStruct);	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	DBGMCU_Config(DBGMCU_TIM2_STOP, ENABLE);			// stop Timer during debug break
	TIM_Cmd(TIM2, ENABLE);
	// enable timer2-interrupt in interrupt controller	
	NVIC_EnableIRQ(TIM2_IRQn);
}



/**
*  abort execution
*/
void hwResetAndReboot() {
    hwDisableInterrupts();
    hwInitWatchdog(1);
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
*/
long* hwInitContext(long* stack, void* object) {

	stack--; // to maintain the initial stack pointer double word aligned
	         // we have to decrement the stack pointer by an even number (FA-2012.02

	/* Simulate the stack frame as it would be created by a context switch
	interrupt. */
	*stack = INITIAL_XPSR;	/* xPSR */
	stack--;
	*stack = (long)(threadStartupWrapper);	/* PC */
	stack--;
	*stack = 0;	/* LR */
	stack -= 5;	/* R12, R3, R2 and R1. */
	*stack = (long) object; 	/* R0 */
	stack -= 8;	/* R11, R10, R9, R8, R7, R6, R5 and R4. */

	return stack;
}



/** 
* the timer interval 
*/
long long Timer::microsecondsInterval = PARAM_TIMER_INTERVAL;


void Timer::init() 
{
	SysTick_Config(SystemCoreClock * Timer::microsecondsInterval / 1000000); // initialization of systick timer
}



/**
* start timer 
*/
void Timer::start() 
{
	SysTick_Config(SystemCoreClock * Timer::microsecondsInterval / 1000000); // initialization of systick timer
	SysTick_Enable();
}



/**
* stop timer 
*/
void Timer::stop() 
{
	SysTick_Disable();
}



/**
* set timer interval 
*/
void Timer::setInterval(const long long microsecondsInterval) {
  Timer::microsecondsInterval = microsecondsInterval;
}



extern "C"
{
void PendSV_Handler( void )
{
	__asm volatile
	(
		        " cpsid f						\n" // disable all interrupts
		        " mrs   r0, psp					\n" // in PSP - process(task) stack pointer
		       	" cbz 	r0, no_save				\n"
		        " stmdb r0!, {r4-r11}			\n"	// Save context 
		        " push 	{r14}					\n" // Save context 
		        " push 	{ r0 }					\n" // Save context 
		       	" bl 	schedulerWrapper		\n"	// call scheduler
			  	" pop	{ r0 }					\n" 
				" pop 	{r14}					\n" 
		       	" no_save:						\n" 
				" ldr 	r1, =contextT			\n" // get context of next thread
		   	    " ldr 	r0, [r1] 				\n"	// get context of next thread
		        " ldmia r0!, {r4-r11} 			\n" // get context of next thread
		        " msr   psp, r0     			\n"
		        " orr 	r14, #0xd 				\n"	// force to new process PSP
		        " cpsie f						\n" // enable all interrupts
		       	" bx 	r14						\n"
	);
}



void __asmSwitchToContext(long* context)
{
	contextT = context;
}



void __asmSaveContextAndCallScheduler()
{
	/* Set a PendSV-interrupt to request a context switch. */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
}


/*
 * Initialize and start HW-watchdog
 * - it is used the IWDG (Independent Watchdog) of the STM32F1
 * - it is clocked by it's own independent low-speed clock (LSI)
 * - LSI can vary between 30 kHz and 60 kHz (40kHz typical)
 *   -> so it must be measured using Timer 5 (only possible on CL, HD and XL devices)
 * - max. interval@60kHz: 1/(60kHz/256) * 0xFFF = 17472 ms
 * - max. interval@40kHz: 1/(40kHz/256) * 0xFFF = 26208 ms
 * - max. interval@30kHz: 1/(30kHz/256) * 0xFFF = 34944 ms
 */
#ifndef STM32F10X_MD
static uint32_t GetLSIFrequency(void);
#endif

void hwInitWatchdog(long intervalMilliseconds) {
	/* IWDG timeout may varies due to LSI frequency dispersion) */
	/* Get the LSI frequency:  TIM5 is used to measure the LSI frequency */
	uint32_t LsiFreq = 40000;
#ifndef STM32F10X_MD
	LsiFreq = GetLSIFrequency();
#endif

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
	double wdgClk = (double) LsiFreq / (double) prescaler;
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
 * @brief  This function handles TIM5 global interrupt request.
 * @param  None
 * @retval None
 */
#ifndef STM32F10X_MD
static volatile unsigned int PeriodValue = 0;
static volatile bool captureRdy = false;
static uint32_t CaptureNumber = 0;

extern "C" {
void TIM5_IRQHandler(void) {
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
} // end extern "C"

/**
 * @brief  Configures TIM5 to measure the LSI oscillator frequency.
 * 		The LSI frequency is used to init the Independent Watchdog.
 * @param  None
 * @retval LSI Frequency
 */
static uint32_t GetLSIFrequency(void) {
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

	/* Enable clock for AFIO component. Otherwise GPIO-remap will not work */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	/* Connect internally the TIM5_CH4 Input Capture to the LSI clock output */
	GPIO_PinRemapConfig(GPIO_Remap_TIM5CH4_LSI, ENABLE);

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
#endif // STM32F10X_MD

#ifndef NO_RODOS_NAMESPACE
}
#endif

