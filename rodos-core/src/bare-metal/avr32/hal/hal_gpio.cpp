/*
 * Copyright (c) 2012 Uni Wuerzburg, Wuerzburg
 * All rights reserved.
 *
 * @author Michael Ruffer
 */


#include "rodos.h"

extern "C" {
	#include "gpio.h"
	#include "intc.h"
}

#include "hal/hal_gpio.h"

#include <stdlib.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

#define DYNAMIC_MEMORY_ALLOCATION

// define the maximum amount of GPIO objects you will use
// -> this value sets up an array to save the context of every GPIO object
// -> to save RAM memory choose the value as small as possible
// -> this is only necessary when dynamic memory allocation is not used
// e.g. MAX_NUM_GPIO 110 -> at least 10 byte * 110 = approx. 1kB of RAM will be used
#define MAX_NUM_GPIO 50



class HW_HAL_GPIO {
public:
	GPIO_PIN pinIdx;
	unsigned int numOfPins;
	bool isOutput;
	unsigned int pinMask;
};

/* gpio_isr_default()
 * This function is the default interrupt handler for gpio-interrupts. It is always called when
 * the gpio-interrupt is enabled and no user-specific handler is defined.
 * -> there are 14 gpio-interrupt-lines at uc3a0512:
 * 	  AVR32_GPIO_IRQ_0 -> GPIO000-007
 * 	  AVR32_GPIO_IRQ_1 -> GPIO008-015
 * 	  ...
 * 	  AVR32_GPIO_IRQ_13 -> GPIO104-109
 * -> 8 gpio-interrupt-signals are ORed together per line
 * -> when defining new interrupt service routines use __attribute__((__interrupt__))
	  e.g.
	  __attribute__((__interrupt__))
	  static void gpio_isr_GPIO000_007(void) {
	  }
*/

extern "C"{
	void gpio_isr_default(void);
}
#ifndef DYNAMIC_MEMORY_ALLOCATION
	HW_HAL_GPIO HW_HAL_GPIOArray[MAX_NUM_GPIO+1];
	static int gpioContextArrayIndex = 0;
#endif


void GPIO_setPinMask(HW_HAL_GPIO* pContext){

	if (pContext->numOfPins+(pContext->pinIdx & 0x1f) > 32) { // pin-group exceeds port boundary ! only the pins up to most significant pin of port will be set
		pContext->pinMask = 0xFFFFFFFF << (pContext->pinIdx & 0x1f);
	}
	else{
		pContext->pinMask = 0xFFFFFFFF << (pContext->pinIdx & 0x1f);
		pContext->pinMask &= 0xFFFFFFFF >> (32-pContext->numOfPins-(pContext->pinIdx & 0x1f));
	}
}


HAL_GPIO::HAL_GPIO(GPIO_PIN pinIdx) {
	// allocate memory for gpio context directly when creating the object
	// -> it is very important because we don't want to use an uninitialized pointer in any method
#ifndef DYNAMIC_MEMORY_ALLOCATION
	context = &HW_HAL_GPIOArray[gpioContextArrayIndex];// static memory allocation
	if (gpioContextArrayIndex < MAX_NUM_GPIO) gpioContextArrayIndex++; // make sure that we don't exceed the array
#else
	//context = malloc(sizeof(HW_HAL_GPIO)); // dynamic memory allocation
	context = (HW_HAL_GPIO*)xmalloc(sizeof(HW_HAL_GPIO)); // dynamic memory allocation with RODOS function
#endif


	HW_HAL_GPIO* pContext = (HW_HAL_GPIO*)context;
	pContext->pinIdx = pinIdx;

	//set meaningful default values
	pContext->numOfPins = 1;
	pContext->isOutput = false;
	GPIO_setPinMask(pContext);
}


int32_t HAL_GPIO::init(bool isOutput, uint32_t numOfPins, uint32_t initVal){
	HW_HAL_GPIO* pContext = (HW_HAL_GPIO*)context;

	pContext->isOutput = isOutput;

	if (numOfPins != 0) pContext->numOfPins = numOfPins; // numOfPins has to be > 0 -> if new value is 0 don't change the default value

	GPIO_setPinMask(pContext);

	volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[pContext->pinIdx >> 5];
	gpio_port->oderc = pContext->pinMask; 	// disable GPIO output driver for the pins
											// -> all pins are input first to avoid damages to hardware due to driving pins to reset value (all zero)
	gpio_port->gpers = pContext->pinMask; 	// enable GPIO function of pins


	return 0;
}


int32_t HAL_GPIO::config(GPIO_CFG_TYPE type, uint32_t paramVal){
	HW_HAL_GPIO* pContext = (HW_HAL_GPIO*)context; //  only to avoid casting of "context" every time ...
	volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[pContext->pinIdx >> 5];

	switch (type){
		case GPIO_CFG_OUTPUT_ENABLE:
			if (paramVal > 0){
				pContext->isOutput = true;
				gpio_port->oders = pContext->pinMask; // enable GPIO output driver
			}
			else {
				pContext->isOutput = false;
				gpio_port->oderc = pContext->pinMask; // disable GPIO output driver
			}
			return 0;

		case GPIO_CFG_NUM_OF_PINS:
			if (paramVal != 0) {
				pContext->numOfPins = paramVal; // numOfPins has to be > 0
				GPIO_setPinMask(pContext);
				return 0;
			}
			return -1;

		case GPIO_CFG_IRQ_SENSITIVITY:
            switch (paramVal) {
            case GPIO_IRQ_SENS_BOTH:
                gpio_port->imr0c = pContext->pinMask;
                gpio_port->imr1c = pContext->pinMask;
                break;

            case GPIO_IRQ_SENS_RISING:
                gpio_port->imr0s = pContext->pinMask;
                gpio_port->imr1c = pContext->pinMask;
                break;

            case GPIO_IRQ_SENS_FALLING:
                gpio_port->imr0c = pContext->pinMask;
                gpio_port->imr1s = pContext->pinMask;
                break;

            default:
                gpio_port->imr0c = pContext->pinMask;
                gpio_port->imr1c = pContext->pinMask;
            }
            return 0;

		default: return -1;
	}
}


void HAL_GPIO::reset(){
	HW_HAL_GPIO* pContext = (HW_HAL_GPIO*)context; // only to avoid casting of "context" every time ...
	volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[pContext->pinIdx >> 5];

	if (pContext->numOfPins == 0) return; // config is not called yet

	config(GPIO_CFG_OUTPUT_ENABLE, 0);
	interruptEnable(false);

	PRIORITY_CEILING{
		gpio_port->ovr  &= ~pContext->pinMask; // set outputvalue of pins to resetvalue 0
	}
}


void HAL_GPIO::setPins(uint32_t val) {
	HW_HAL_GPIO* pContext = (HW_HAL_GPIO*)context; // only to avoid casting of "context" every time ...
	volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[pContext->pinIdx >> 5];
	unsigned int newPinVal = 0;

	if (pContext->numOfPins == 0) return; // config is not called yet

	if (pContext->isOutput){
		PRIORITY_CEILING{
			newPinVal = gpio_port->ovr & ~pContext->pinMask;	// get current pinvalues of whole port and clear pinvalues we want to set new
			newPinVal |= (val << (pContext->pinIdx & 0x1f) ) & pContext->pinMask; // set new pinvalues
			gpio_port->ovr  = newPinVal; // write new pinvalues to GPIO output register
			gpio_port->oders = pContext->pinMask; // enable GPIO output driver
		}
	}
}


uint32_t HAL_GPIO::readPins(){
	HW_HAL_GPIO* pContext = (HW_HAL_GPIO*)context; // only to avoid casting of "context" every time ...
	volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[pContext->pinIdx >> 5];

	if (pContext->numOfPins == 0) return -1; // config is not called yet

	return (gpio_port->pvr & pContext->pinMask) >> (pContext->pinIdx & 0x1f);
}


extern "C" void _unhandled_interrupt(); // defined in avr32_framework/intc.c

void HAL_GPIO::interruptEnable(bool enable){
	HW_HAL_GPIO* pContext = (HW_HAL_GPIO*)context; // only to avoid casting of "context" every time ...
	volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[pContext->pinIdx >> 5];

	if (pContext->numOfPins == 0) return; // config is not called yet

	PRIORITY_CEILING{
		if (enable){
			// default GPIO_IRQ_SENS_BOTH:
            gpio_port->imr0c = pContext->pinMask;
            gpio_port->imr1c = pContext->pinMask;;

			gpio_port->gfers = pContext->pinMask; // Enable the glitch filter.
//			gpio_port->iers = pContext->pinMask;  // Enable interrupt
//
//			unsigned irq = pContext->pinIdx / 8 + AVR32_GPIO_IRQ_0;
//			INTC_register_interrupt(&gpio_isr_default, irq, AVR32_INTC_INT0);
		}
		else{
			gpio_port->gferc = pContext->pinMask; // disable the glitch filter.
			gpio_port->ierc = pContext->pinMask;  // disable interrupt
			gpio_port->imr0c = pContext->pinMask; // set interrupt mode register to reset value
			gpio_port->imr1c = pContext->pinMask;	// set interrupt mode register to reset value
			unsigned irq = pContext->pinIdx / 8 + AVR32_GPIO_IRQ_0;
			INTC_register_interrupt(&_unhandled_interrupt, irq, AVR32_INTC_INT0);
		}
	}
}

extern "C" {
/*! \brief GPIO interrupt service routine
 */
__attribute__((__interrupt__))
void gpio_isr_default(void) {
	int i = 0;
	// clear Interrupt Flag Register of all 4 ports
	for (i = 0; i<=3; i++) {
		AVR32_GPIO.port[i].ifrc = AVR32_GPIO.port[i].ifr;
	}
}
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
