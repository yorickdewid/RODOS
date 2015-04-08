/*
 * Copyright (c) 2011 Fraunhofer FIRST, Berlin
 * All rights reserved.
 *
 * @author TW
 */
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"

#include "adc.h"

/*
 *	ADC
 */


void adc_init() {

	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_DeInit(ADC1);

	//Config ----

	// Enable GPIO clock and release reset
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	//RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, DISABLE);

	// Pin C5 input 2MHz
	{
	GPIO_InitTypeDef Gis;
	GPIO_StructInit(&Gis);
	Gis.GPIO_Pin = GPIO_Pin_5;  	//Pin C5
	GPIO_Init(GPIOC, &Gis);		//Port C
	}



	// ADC15
	{
	ADC_InitTypeDef Ais;

	ADC_StructInit(&Ais);
	Ais.ADC_Mode = ADC_Mode_Independent;
	Ais.ADC_ScanConvMode=DISABLE;
	Ais.ADC_ContinuousConvMode = DISABLE; //continuous mode
	Ais.ADC_ExternalTrigConv =ADC_ExternalTrigConv_None;
	Ais.ADC_NbrOfChannel = 1;

	ADC_Init(ADC1, &Ais);
	}

	ADC_Cmd(ADC1, ENABLE);
	/*Enable Temp Sensor*/
	ADC_TempSensorVrefintCmd(ENABLE);
	/* Enable ADC1 reset calibaration register */
  	ADC_ResetCalibration(ADC1);
  	/* Check the end of ADC1 reset calibration register */
  	while(ADC_GetResetCalibrationStatus(ADC1));
  	/* Start ADC1 calibaration */
  	ADC_StartCalibration(ADC1);
  	/* Check the end of ADC1 calibration */
  	while(ADC_GetCalibrationStatus(ADC1));

}


int adc_getvalue(int adc_chan){
	ADC_RegularChannelConfig(ADC1,adc_chan,1,ADC_SampleTime_239Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);
	return ADC_GetConversionValue(ADC1);
}
