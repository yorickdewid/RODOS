/*
 * Copyright (c) 2011 Fraunhofer FIRST, Berlin
 * All rights reserved.
 *
 * @author Bernd Oestmann
 */
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "led.h"

/*
 *	LED at PC.12
 */
void led_init() {
	// Enable GPIO clock and release reset
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,
	    ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,
	    DISABLE);

	// Configure PC.12 as output push-pull (LED)
	GPIO_InitTypeDef Gis;

	GPIO_StructInit(&Gis);
	Gis.GPIO_Pin = GPIO_Pin_12;
	Gis.GPIO_Mode = GPIO_Mode_Out_PP;
	Gis.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &Gis);
}

void led(int on) {
	if( on ) {
		GPIOC->BRR  |= 0x00001000;
	} else {
		GPIOC->BSRR |= 0x00001000;
	}
}
