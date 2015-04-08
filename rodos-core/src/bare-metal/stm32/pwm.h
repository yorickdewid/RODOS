/*
 * pwm.h
 *
 *  Created on: 07.11.2011
 *      Author: Michael Ruffer
 */

#ifndef PWM_H_
#define PWM_H_

#include "stm32f10x.h"


/* timers with pwm feature:
 * STM32F103_MD: 1,2,3,4,5 (Olimex STM32-P103) ! Timer 5 has no output pins!
 * STM32F103_HD: 1,2,3,4,5,8 (FIRST Multisensorboard)
 *
 * every timer has 4 channels = 4 outputs/pins
 * all channels share the same clock -> so the choosen pwm frequency is valid for all channels
 *
 * !!! When using RODOS, Timer 2 is already in use for time base !!!
 *
 * only timer 1 to 4 are supported by this driver
 */


/**
  * @brief  number of steps one period is divided in
  */
#define PWM_RESOLUTION 256


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	TIM_CHAN1,
	TIM_CHAN2,
	TIM_CHAN3,
	TIM_CHAN4
} TIM_CHAN_TypeDef;


/**
  * @brief  Initializes timer in PWM mode
  * @param  TIMx: where x can be 1,2,3,4 to select the TIMx peripheral.
  * @note
  *   - timer 5 has also a PWM mode, but there is no output pin on LD and MD devices
  * @param  channel: specifies the output channel of TIMx to be used
  *   This parameter can be one of the following values:
  *     @arg TIM_CHAN1: channel 1
  *     @arg TIM_CHAN2: channel 2
  *     @arg TIM_CHAN3: channel 3
  *     @arg TIM_CHAN4: channel 4
  * @param  frequency: frequency of PWM signal
  *   	This parameter can be any value between: 10 and 100000 with PWM_RESOLUTION set to 256.
  * @param defaultPulseWidth: pulsewidth of high-pulse at starttime
  *   	This parameter can be any value between 1 and PWM_RESOLUTION-1.
  * @retval int
  *  	0  - everything is fine
  *  	-1 - a parameter is not in the valid range
  */
int pwm_init(TIM_TypeDef * TIMx, TIM_CHAN_TypeDef channel, int frequency, int defaultPulseWidth);

/**
  * @brief  Set pulsewidth of high-pulse of PWM-signal
  * @param  TIMx: where x can be 1,2,3,4 to select the TIMx peripheral.
  * @note
  *   - timer 5 has also a PWM mode, but there is no output pin on LD and MD devices
  * @param  channel: specifies the output channel of TIMx to be used
  *   This parameter can be one of the following values:
  *     @arg TIM_CHAN1: channel 1
  *     @arg TIM_CHAN2: channel 2
  *     @arg TIM_CHAN3: channel 3
  *     @arg TIM_CHAN4: channel 4
  * @param pulseWidth: pulsewidth of high-pulse
  *   	This parameter can be any value between 1 and PWM_RESOLUTION-1.
  * @retval int
  *  	0  - everything is fine
  *  	-1 - a parameter is not in the valid range
  */
int pwm_setPulseWidth(TIM_TypeDef * timer, TIM_CHAN_TypeDef channel, int pulseWidth);

#ifdef __cplusplus
}
#endif



#endif /* PWM_H_ */
