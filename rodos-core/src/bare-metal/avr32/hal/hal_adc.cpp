/*
 * Copyright (c) 2012 Uni Wuerzburg, Wuerzburg
 * All rights reserved.
 *
 * @author Michael Ruffer
 */



#include "params.h"
#include "hal_adc.h"

extern "C"
{
#include <avr32/io.h>
//#include "evk1100.h"
//#include "power_clocks_lib.h"
#include "gpio.h"
#include "adc.h"
//#include "intc.h"
}


/* EVK1100 - ADC0:
   CHANNEL 0: temperature sensor (NTC, 100k)	- pin PA21
   CHANNEL 1: potentiometer (100k) 				- pin PA22
   CHANNEL 2: phototransistor 					- pin PA23
   CHANNEL 3: - pin PA24
   CHANNEL 4: - pin PA25
   CHANNEL 5: - pin PA26
   CHANNEL 6: - pin PA27
   CHANNEL 7: - pin PA28
 */

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif



struct ADC_context {
	ADC_IDX idx;
	int selectedChannel;
	//int resolution;
};

#define MAX_ADC_OBJECTS 2

static ADC_context adcContext[MAX_ADC_OBJECTS];
static int adcCtxArrIdx = 0;

HAL_ADC::HAL_ADC(ADC_IDX adcIdx){
	context = &adcContext[adcCtxArrIdx];
	if (adcCtxArrIdx < (MAX_ADC_OBJECTS-1) ) adcCtxArrIdx++;

	ADC_context* adcCtx = (ADC_context*)context;
	adcCtx->idx = adcIdx;
	adcCtx->selectedChannel = 0;
}

/* Initialization of ADC */
int HAL_ADC::init(ADC_CHANNEL ch){
	ADC_context* adcCtx = (ADC_context*)context;
	adcCtx->selectedChannel = ch;

	switch (ch){
		case ADC_CH0:
			gpio_enable_module_pin(AVR32_ADC_AD_0_PIN,AVR32_ADC_AD_0_FUNCTION);
			break;
		case ADC_CH1:
			gpio_enable_module_pin(AVR32_ADC_AD_1_PIN,AVR32_ADC_AD_1_FUNCTION);
			break;
		case ADC_CH2:
			gpio_enable_module_pin(AVR32_ADC_AD_2_PIN,AVR32_ADC_AD_2_FUNCTION);
			break;
		case ADC_CH3:
			gpio_enable_module_pin(AVR32_ADC_AD_3_PIN,AVR32_ADC_AD_3_FUNCTION);
			break;
		case ADC_CH4:
			gpio_enable_module_pin(AVR32_ADC_AD_4_PIN,AVR32_ADC_AD_4_FUNCTION);
			break;
		case ADC_CH5:
			gpio_enable_module_pin(AVR32_ADC_AD_5_PIN,AVR32_ADC_AD_5_FUNCTION);
			break;
		case ADC_CH6:
			gpio_enable_module_pin(AVR32_ADC_AD_6_PIN,AVR32_ADC_AD_6_FUNCTION);
			break;
		case ADC_CH7:
			gpio_enable_module_pin(AVR32_ADC_AD_7_PIN,AVR32_ADC_AD_7_FUNCTION);
			break;
		default:
			break;
	}

	// configure ADC
	// Lower the ADC clock to match the ADC characteristics:
	// 8Bit resolution: max. ADCClock = 8MHz
	// 10Bit resolution: max. ADCClock = 5MHz
	// Atmel-Example settings: CLK_ADC = PBA_CLK = 12MHz -> ADCClock = CLK_ADC/((PRESCAL+1)*2) = 12MHz/((1+1)*2)  = 3MHz
	// RODOS settings: CLK_ADC = PBA_CLK = 16,5MHz -> ADCClock = CLK_ADC/((PRESCAL+1)*2) = 16,5MHz/((2+1)*2)  = 2,75MHz
	AVR32_ADC.mr |= 0x2 << AVR32_ADC_MR_PRESCAL_OFFSET;
	adc_configure(&AVR32_ADC);

	// Enable the ADC channel.
	adc_enable(&AVR32_ADC,ch);

	return 1;
}

/* disable ADC and set all its registers and pins to its reset state */
// TBD
void HAL_ADC::reset(){}

/* Configuration of ADC AFTER initialization */
// TBD
int HAL_ADC::config(ADC_PARAMETER_TYPE type, int paramVal){
	return 0;
}

 ///< in case we have an external analog mux
//void HAL_ADC::setAssociatedGPIOMultilexer(GPIO* muxControl){}

// TBD
int HAL_ADC::status(ADC_STATUS_TYPE type){
	return 0;
}

bool HAL_ADC::isDataReady(){
	ADC_context* adcCtx = (ADC_context*)context;
	return adc_check_eoc(&AVR32_ADC,adcCtx->selectedChannel);
}

int HAL_ADC::read(){
	ADC_context* adcCtx = (ADC_context*)context;

    adc_start(&AVR32_ADC);
	return ((int)adc_get_value(&AVR32_ADC, adcCtx->selectedChannel));
}

void HAL_ADC::selectChannel(ADC_CHANNEL ch){
	ADC_context* adcCtx = (ADC_context*)context;
	adcCtx->selectedChannel = ch;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
