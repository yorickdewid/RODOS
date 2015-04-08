/*
 * Copyright (c) 2011 Uni Wuerzburg, Wuerzburg
 * All rights reserved.
 *
 * @author Michael Ruffer
 */

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "pwm.h"


int pwm_init(TIM_TypeDef * timer, TIM_CHAN_TypeDef channel, int frequency, int defaultPulseWidth)
{
	static char pwmInit[4] = {0,0,0,0};
	char timerIdx = 1;

	// local variables to initialize timer2
	TIM_TimeBaseInitTypeDef timStruct;
	TIM_OCInitTypeDef timOCInitStruct;					// init structure for OC-modes
	unsigned char timClockMultiplier = 0;
	RCC_ClocksTypeDef clocks;
	int timPrescaler = 1;


	/**** configure GPIO for timer-Use ****/

	// Configure timer pin as output, alternate function, push-pull
	GPIO_InitTypeDef Gis;
	GPIO_StructInit(&Gis);
	Gis.GPIO_Mode = GPIO_Mode_AF_PP;
	Gis.GPIO_Speed = GPIO_Speed_50MHz;

	if (timer == TIM1)
	{	// Enable GPIO clock and release reset
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, DISABLE);

		switch (channel)
		{
			case TIM_CHAN1: Gis.GPIO_Pin = GPIO_Pin_8; GPIO_Init(GPIOA, &Gis); break; // PA8 -> CH1
			case TIM_CHAN2: Gis.GPIO_Pin = GPIO_Pin_9; GPIO_Init(GPIOA, &Gis); break; // PA9 -> CH2
			case TIM_CHAN3: Gis.GPIO_Pin = GPIO_Pin_10; GPIO_Init(GPIOA, &Gis); break; // PA10 -> CH3
			case TIM_CHAN4: Gis.GPIO_Pin = GPIO_Pin_11; GPIO_Init(GPIOA, &Gis); break; // PA11 -> CH4
		}

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
		timerIdx = 1;
	}
	else if (timer == TIM2)
	{	// Enable GPIO clock and release reset
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, DISABLE);

		switch (channel)
		{
			case TIM_CHAN1: Gis.GPIO_Pin = GPIO_Pin_0; GPIO_Init(GPIOA, &Gis); break; // PA0 -> CH1
			case TIM_CHAN2: Gis.GPIO_Pin = GPIO_Pin_1; GPIO_Init(GPIOA, &Gis); break; // PA1 -> CH2
			case TIM_CHAN3: Gis.GPIO_Pin = GPIO_Pin_2; GPIO_Init(GPIOA, &Gis); break; // PA2 -> CH3
			case TIM_CHAN4: Gis.GPIO_Pin = GPIO_Pin_3; GPIO_Init(GPIOA, &Gis); break; // PA3 -> CH4
		}

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		timerIdx = 2;
	}
	else if (timer == TIM3)
	{	// Enable GPIO clock and release reset
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, DISABLE);

		switch (channel)
		{
			case TIM_CHAN1: Gis.GPIO_Pin = GPIO_Pin_6; GPIO_Init(GPIOA, &Gis); break; // PA6 -> CH1
			case TIM_CHAN2: Gis.GPIO_Pin = GPIO_Pin_7; GPIO_Init(GPIOA, &Gis); break; // PA7 -> CH2
			case TIM_CHAN3: Gis.GPIO_Pin = GPIO_Pin_0; GPIO_Init(GPIOB, &Gis); break; // PB0 -> CH3
			case TIM_CHAN4: Gis.GPIO_Pin = GPIO_Pin_1; GPIO_Init(GPIOB, &Gis); break; // PB1 -> CH4
		}

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		timerIdx = 3;
	}
	else if (timer == TIM4)
	{	// Enable GPIO clock and release reset
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, DISABLE);

		switch (channel)
		{
			case TIM_CHAN1: Gis.GPIO_Pin = GPIO_Pin_6; GPIO_Init(GPIOB, &Gis); break; // PB6 -> CH1
			case TIM_CHAN2: Gis.GPIO_Pin = GPIO_Pin_7; GPIO_Init(GPIOB, &Gis); break; // PB7 -> CH2
			case TIM_CHAN3: Gis.GPIO_Pin = GPIO_Pin_8; GPIO_Init(GPIOB, &Gis); break; // PB8 -> CH3
			case TIM_CHAN4: Gis.GPIO_Pin = GPIO_Pin_9; GPIO_Init(GPIOB, &Gis); break; // PB9 -> CH4
		}

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		timerIdx = 4;
	}
	else
	{
		return -1; // timer not supported
	}


	/* reset timer before new init, only if it's not already used as PWM source */
	if (pwmInit[timerIdx-1] == 0) {
		TIM_DeInit(timer);
		pwmInit[timerIdx-1] = 1;
	}

	// RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, DISABLE); // is done in TIM_DeInit
	// RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, DISABLE); // is done in TIM_DeInit
	// RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, DISABLE); // is done in TIM_DeInit
	// RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM4, DISABLE); // is done in TIM_DeInit


	// if the APB1 Prescaler is bigger than 1, the PCLK1 is multiplied by 2 infront of the timer
	RCC_GetClocksFreq(&clocks);
	if(clocks.HCLK_Frequency/clocks.PCLK1_Frequency > 1)
	{ timClockMultiplier = 2; }
	else
	{ timClockMultiplier = 1; }

	// Prescaler
	// fCK_CNT = fCK_PSC/(TIM_Prescaler+1)
	// - it's important to set the prescaler properly, because timer is only a 16Bit counter with
	//   a maximum reload-value of 2^16 = 65536
	timPrescaler = (clocks.PCLK1_Frequency*timClockMultiplier) / (frequency * PWM_RESOLUTION) - 1; // e.g. 72000000/(100000*100)-1 = 6
	timStruct.TIM_Prescaler 		= timPrescaler;

	timStruct.TIM_Period = PWM_RESOLUTION;

	// timer mode is "count down"
	timStruct.TIM_CounterMode 		= TIM_CounterMode_Up;

	// TIM_ClockDivision = division ratio between the timer clock (CK_INT)
	// frequency and sampling clock used by the digital filters
	// -> not interesting for our purpose !!!
	timStruct.TIM_ClockDivision		= TIM_CKD_DIV1;

	// only valid for TIM1 and TIM8 -> STM32F10x_HD and better
	timStruct.TIM_RepetitionCounter	= 0;

	TIM_TimeBaseInit(timer,&timStruct);



	/**** pwm output settings ****/
	TIM_OCStructInit(&timOCInitStruct);						// set init structure to default values

	timOCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;        		/*!< Specifies the TIM mode.
																This parameter can be a value of @ref TIM_Output_Compare_and_PWM_modes */

	timOCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	/*!< Specifies the TIM Output Compare state.
																This parameter can be a value of @ref TIM_Output_Compare_state */

	if (defaultPulseWidth >= PWM_RESOLUTION) defaultPulseWidth = PWM_RESOLUTION - 1;
	if (defaultPulseWidth <= 0) defaultPulseWidth = 1;
	timOCInitStruct.TIM_Pulse = defaultPulseWidth;				/*!< Specifies the pulse value to be loaded into the Capture Compare Register.
																This parameter can be a number between 0x0000 and 0xFFFF */

	timOCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;    	/*!< Specifies the output polarity.
	                                   	   	   	   	   	   	   	This parameter can be a value of @ref TIM_Output_Compare_Polarity */

	switch (channel){
		case TIM_CHAN1: TIM_OC1Init(timer, &timOCInitStruct); break; // channel 1 init
		case TIM_CHAN2: TIM_OC2Init(timer, &timOCInitStruct); break; // channel 2 init
		case TIM_CHAN3: TIM_OC3Init(timer, &timOCInitStruct); break; // channel 3 init
		case TIM_CHAN4: TIM_OC4Init(timer, &timOCInitStruct); break; // channel 4 init
		default: return -1;
	}

	//TIM_ITConfig(timer, TIM_IT_Update, ENABLE); // Interrupt enable
	//DBGMCU_Config(DBGMCU_TIM2_STOP, ENABLE);			// stop Timer during debug break
	TIM_Cmd(timer, ENABLE);

	if (timer == TIM1) TIM_CtrlPWMOutputs(timer, ENABLE); 	// this function applies only to timer 1
															// the others timers don't need extra output activation

	// enable timer2-interrupt in interrupt controller
	//NVIC_EnableIRQ(TIM2_IRQn);
	timerIdx = 0;

	return 0;
}



int pwm_setPulseWidth(TIM_TypeDef * timer, TIM_CHAN_TypeDef channel, int pulseWidth)
{
	int localPulseWidth = pulseWidth;

	if (localPulseWidth >= PWM_RESOLUTION) localPulseWidth = PWM_RESOLUTION - 1; // set to maximum, if necessary
	if (localPulseWidth <= 0) localPulseWidth = 1; // set to minimum, if necessary

	if ( (timer == TIM1) || (timer == TIM2) || (timer == TIM3) || (timer == TIM4) )
	{
		switch (channel)
		{
			case TIM_CHAN1: TIM_SetCompare1(timer, localPulseWidth); break;
			case TIM_CHAN2: TIM_SetCompare2(timer, localPulseWidth); break;
			case TIM_CHAN3: TIM_SetCompare3(timer, localPulseWidth); break;
			case TIM_CHAN4: TIM_SetCompare4(timer, localPulseWidth); break;
		}
	}
	else return -1;

	return 0;
}
