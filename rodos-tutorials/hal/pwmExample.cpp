/*!
 * \file    pwmExample.cpp
 *
 * \date    30.10.2012
 * \author  Michael Ruffer
 *
 * \brief   To run this example you must connect e.g. an oscilloscope
 *          to the pwm pins according the hal_pwm.cpp of your target
 *          -> e.g. STM32F4 pin PB0 (Timer 3 Chan 3)
 */

#include "rodos.h"
#include "hal/hal_pwm.h"


HAL_PWM pwm10(PWM_IDX10);

class PWMExample: public Thread {
	uint64_t periode;
	HAL_PWM *pwm;
public:
	PWMExample(const char* name, HAL_PWM* pwm, uint64_t periode) :
			Thread(name) {
		this->periode = periode;
		this->pwm = pwm;
	}

	void init() {
	    pwm->init(1000,100);
	}

	void run() {
		uint32_t pulseWidth = 0;
		TIME_LOOP(0,periode){
			pwm->write(pulseWidth);
			pulseWidth += 10;
			if (pulseWidth >= 100) pulseWidth = 0;
		}
	}
};

PWMExample pwmThread("PWMExample", &pwm10, 100 * MILLISECONDS);
/***********************************************************************/
