/*
 * Copyright (c) 2012 Uni Wuerzburg, Wuerzburg
 * All rights reserved.
 *
 * @author Michael Ruffer
 */


#ifndef _HAL_ADC_H_
#define _HAL_ADC_H_

#include "hal/genericIO.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


enum ADC_PARAMETER_TYPE {
	//ADC_PARAMETER_RESOLUTION,
	//ADC_PARAMETER_???
};


enum ADC_STATUS_TYPE {

};

enum ADC_IDX {
	ADC_IDX0 = 0
};

enum ADC_CHANNEL {
	ADC_CH0 = 0,	// pin PA21
	ADC_CH1,		// pin PA22
	ADC_CH2,		// pin PA23
	ADC_CH3,		// pin PA24
	ADC_CH4,		// pin PA25
	ADC_CH5,		// pin PA26
	ADC_CH6,		// pin PA27
	ADC_CH7			// pin PA28
};


class HAL_ADC : public GenericIOInterface{
/* public functions */
	void* context;
public:
	HAL_ADC(ADC_IDX adcIdx = ADC_IDX0);

	/* Initialization of ADC */
	int init(ADC_CHANNEL ch);

	/* disable ADC and set all its registers and pins to its reset state */
	void reset();

 	/* Configuration of ADC AFTER initialization */
	int config(ADC_PARAMETER_TYPE type, int paramVal);

	 ///< in case we have an external analog mux
	//void setAssociatedGPIOMultilexer(GPIO* muxControl);

	int status(ADC_STATUS_TYPE type);


	bool isWriteFinished(){return true;}
	bool isDataReady();

	int read();

	/*** Extra only for AD **/
	void selectChannel(ADC_CHANNEL ch); ///< eventaully using the external multiplex and GPIO
	//int readAnalogValue();
};

//extern HAL_ADC adc0; // -> defined and initialized in hw_specific.cpp

#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif // _HAL_ADC_H_
