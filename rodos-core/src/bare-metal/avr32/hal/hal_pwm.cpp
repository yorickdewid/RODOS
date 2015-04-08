/*
 * Copyright (c) 2012 Uni Wuerzburg, Wuerzburg
 * All rights reserved.
 *
 * @author Michael Ruffer
 */
#include "hal/hal_pwm.h"
#include "params.h"

extern "C"
{
#include <avr32/io.h>
#include "evk1100.h"
#include "gpio.h"
#include "pwm.h"
}


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/* EVK1100:
   PWM0 -
   PWM1 -
   PWM2 - Pin PB21
   PWM3 - Pin PB22
   PWM4 -
   PWM5 -
   PWM6 -
 */


class HW_HAL_PWM {
public:
	PWM_IDX idx;
	unsigned int frequency;
	unsigned int increments;
};


// provide memory for context of HAL_UART-objects 
// -> maybe someone likes to create more than one object per UART-interface
//    so we provide more space (10) than there are real UART-interfaces (4)
#define MAX_PWM_OBJECTS 10
static HW_HAL_PWM HW_HAL_PWMArray[MAX_PWM_OBJECTS];
static int pwmCtxArrIdx = 0;


HAL_PWM::HAL_PWM(PWM_IDX pwmIdx){
	context = &HW_HAL_PWMArray[pwmCtxArrIdx];// static mem alloc
    // make sure that we don't exeed array
    if (pwmCtxArrIdx < (MAX_PWM_OBJECTS-1) ) pwmCtxArrIdx++;

	HW_HAL_PWM *pContext = (HW_HAL_PWM*)context;
	pContext->idx = pwmIdx;
};


/*
 * PWM
 * fixed to frequency = 125.9 Hz, increments = 512 !!!
 */
int HAL_PWM::init(int freq, int increments) {
	HW_HAL_PWM *pContext = (HW_HAL_PWM*)context;
	PWM_IDX idx = pContext->idx;

	if ((idx < PWM_IDX00) || (idx > PWM_IDX06)) {return -1;}

	pContext->frequency = freq; // not used yet
	//pContext->increments = increments;
	pContext->increments = 512;

	switch (idx){
		case PWM_IDX00:
			gpio_enable_module_pin(AVR32_PWM_0_PIN, AVR32_PWM_0_FUNCTION);
			break;
		case PWM_IDX01:
			gpio_enable_module_pin(AVR32_PWM_1_PIN, AVR32_PWM_1_FUNCTION);
			break;
		case PWM_IDX02:
			gpio_enable_module_pin(AVR32_PWM_2_PIN, AVR32_PWM_2_FUNCTION);
			break;
		case PWM_IDX03:
			gpio_enable_module_pin(AVR32_PWM_3_PIN, AVR32_PWM_3_FUNCTION);
			break;
		case PWM_IDX04:
			gpio_enable_module_pin(AVR32_PWM_4_0_PIN, AVR32_PWM_4_0_FUNCTION);
			break;
		case PWM_IDX05:
			gpio_enable_module_pin(AVR32_PWM_5_0_PIN, AVR32_PWM_5_0_FUNCTION);
			break;
		case PWM_IDX06:
			gpio_enable_module_pin(AVR32_PWM_6_PIN, AVR32_PWM_6_FUNCTION);
			break;
        default:
            return -1;
	}

	// PWM controller configuration
	pwm_opt_t pwm_opt;             		// PWM option config.
	pwm_opt.diva = AVR32_PWM_DIVA_CLK_OFF;
	pwm_opt.divb = AVR32_PWM_DIVB_CLK_OFF;
	pwm_opt.prea = AVR32_PWM_PREA_MCK;
	pwm_opt.preb = AVR32_PWM_PREB_MCK;
	pwm_init(&pwm_opt);

	// PWM channel configuration
	avr32_pwm_channel_t pwm_channel = { {0},0,0,0,0 };
	pwm_channel.CMR.calg = PWM_MODE_LEFT_ALIGNED;       // Channel mode.
	pwm_channel.CMR.cpol = PWM_POLARITY_HIGH;           // Channel polarity.
	pwm_channel.CMR.cpd = PWM_UPDATE_DUTY;              // Not used the first time.
	pwm_channel.CMR.cpre = AVR32_PWM_CPRE_MCK_DIV_256;  // Channel prescaler.
	pwm_channel.cdty = 0;   					// Channel Duty Cycle
	pwm_channel.cprd = pContext->increments;  	// Channel period.
	pwm_channel.cupd = 0;   					// Channel update is not used here.
	// With these settings, the output waveform frequency will be :
	// (MCK/prescaler)/period = (16500000/256)/512 = 125.9Hz (with MCK = PBA_CLK = 16.5MHz,
	// prescaler = 256, period = 512)
	pwm_channel_init(idx, &pwm_channel); // configure channel with index "idx"

	pwm_start_channels(1 << idx);  // Start channel 0.

	return 1;
}


int HAL_PWM::config(PWM_PARAMETER_TYPE type, int paramVal) {
/*	UART_context *pContext = (UART_context*)context;
	UART_IDX idx = pContext->idx;

	if ((idx < 0) || (idx > 3)) {return -1;}

	volatile avr32_usart_t* uart = uart_getRegPointer(idx);

	usart_options_t USART_OPTIONS =
	{
		pContext->baudrate,	// set current baudrate
		8, 					// charlength
		USART_NO_PARITY, 	// paritytype
		USART_1_STOPBIT, 	// stopbits
		USART_NORMAL_CHMODE // channelmode
	};
	
	switch (type) {
		case UART_PARAMETER_BAUDRATE:
			if (paramVal > 0) 
			{
				USART_OPTIONS.baudrate = paramVal; // set new baudrate
				pContext->baudrate = paramVal;
				if (usart_init_rs232(uart, &USART_OPTIONS, PBA_CLK) == USART_SUCCESS) {
					// re-enable USART Rx interrupt.
					uart->ier = AVR32_USART_IER_RXRDY_MASK;
					return 0;
				}
			}
			return -1;
#if 0 // old stuff from stm32-driver -> must be adapted for avr32
		case UART_PARAMETER_HW_FLOW_CONTROL:
			if (idx > 3) {return -1;} // HW flow control is only supported by USART1..3

			GPIO_StructInit(&gpioRTS);
			GPIO_StructInit(&gpioCTS);

			if (paramVal > 0){ // enable HW Flow Control
				Uis.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
				this->hwFlowCtrl = USART_HardwareFlowControl_RTS_CTS;
				gpioRTS.GPIO_Mode = GPIO_Mode_AF_PP;
				gpioRTS.GPIO_Speed = GPIO_Speed_50MHz;
				gpioCTS.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			}
			else {  // disable HW Flow Control
				Uis.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
				this->hwFlowCtrl = USART_HardwareFlowControl_None;
				gpioRTS.GPIO_Mode = GPIO_Mode_IN_FLOATING; // set gpio to default state
				gpioCTS.GPIO_Mode = GPIO_Mode_IN_FLOATING; // set gpio to default state
			}

			switch (idx)	{
				case 1: // USART1: CTS PA11; RTS PA12;
					gpioCTS.GPIO_Pin = GPIO_Pin_11;
					GPIO_Init(GPIOA, &gpioCTS);
					gpioRTS.GPIO_Pin = GPIO_Pin_12;
					GPIO_Init(GPIOA, &gpioRTS);
					break;
				case 2: // USART2: CTS PA0; RTS PA1;
					gpioCTS.GPIO_Pin = GPIO_Pin_0;
					GPIO_Init(GPIOA, &gpioCTS);
					gpioRTS.GPIO_Pin = GPIO_Pin_1;
					GPIO_Init(GPIOA, &gpioRTS);
					break;
				case 3: // USART3: CTS PB13; RTS PB14;
					gpioCTS.GPIO_Pin = GPIO_Pin_13;
					GPIO_Init(GPIOB, &gpioCTS);
					gpioRTS.GPIO_Pin = GPIO_Pin_14;
					GPIO_Init(GPIOB, &gpioRTS);
					break;
			}
			break; // end case UART_PARAMETER_HW_FLOW_CONTROL
#endif
		default: return -1;
	}
	*/

	return 1;
}


void HAL_PWM::reset(){
}

int HAL_PWM::write(unsigned int highPulseWidth) {
	HW_HAL_PWM *pContext = (HW_HAL_PWM*)context;
	PWM_IDX idx = pContext->idx;

	if ((idx < PWM_IDX00) || (idx > PWM_IDX06)) {return -1;}

	avr32_pwm_channel_t pwm_channel = { {0},0,0,0,0 };

	if (highPulseWidth < 0) highPulseWidth = 0;
	if (highPulseWidth > 512) highPulseWidth = 512;

	pwm_channel.CMR.calg = PWM_MODE_LEFT_ALIGNED;       // Channel mode.
	pwm_channel.CMR.cpol = PWM_POLARITY_HIGH;           // Channel polarity.
	pwm_channel.CMR.cpd = PWM_UPDATE_DUTY;              // Update duty cycle register with value in cupd
	pwm_channel.CMR.cpre = AVR32_PWM_CPRE_MCK_DIV_256;  // Channel prescaler.
	pwm_channel.cdty = 0;								// Channel duty cycle - not used here -> see cupd
	pwm_channel.cprd = 0;  								// Channel period - not used here
	pwm_channel.cupd = highPulseWidth;					// new duty cyle

	pwm_sync_update_channel(idx, &pwm_channel);

	return 1;
}



#ifndef NO_RODOS_NAMESPACE
}
#endif
