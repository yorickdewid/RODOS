/*!
 * \file    adcExample.cpp
 *
 * \date    12.11.2013
 * \author  Michael Ruffer
 *
 * \brief   To run this example you must connect e.g. a potentiometer
 *          to adc1/adc2 (channel1/channel3) according to the hal_adc.cpp of your target
 *          -> e.g. STM32F4 pin PA1 & PA3
 */

#include "hal/hal_adc.h"
#include "rodos.h"


static Application module01("ADCTest", 2000);

class ADCTest: public Thread {
	uint64_t periode;
	ADC_CHANNEL channel;
	HAL_ADC *adc;

public:
	ADCTest(const char* name, HAL_ADC* adc, ADC_CHANNEL channel, uint64_t periode) :
			Thread(name) {
	    this->adc = adc;
		this->periode = periode;
		this->channel = channel;
	}

	void init() {
        adc->init(channel);
	}

	void run() {
	    int64_t t0,t1;
	    TIME_LOOP(0,periode){
	        t0=NOW();
	        uint16_t adcVal = adc->read(channel);
	        t1=NOW();
		    PRINTF("%s channel %d: %d in %d us\n",this->getName(), channel, adcVal, (t1-t0)/MICROSECONDS);
		}
	}
};

HAL_ADC adc1(ADC_IDX1);
//HAL_ADC adc2(ADC_IDX2);

ADCTest a1("ADC1Test",&adc1,ADC_CH_001, 1000 * MILLISECONDS);
//ADCTest a2("ADC2Test",&adc2,ADC_CH_003, 1000 * MILLISECONDS);
/***********************************************************************/
