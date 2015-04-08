/*
 * gpioExample.cpp
 *
 * Created on: 25.11.2013
 * Author: Michael Ruffer
 *
 */
#include "rodos.h"
#include "hal/hal_gpio.h"


static Application module01("HAL GPIO example", 2000);

class GPIOExampleOut: public Thread {
	uint64_t periode;
	HAL_GPIO *gpio;

public:
	GPIOExampleOut(const char* name, HAL_GPIO* gpio, uint64_t periode) :
			Thread(name) {
		this->periode = periode;
		this->gpio = gpio;
	}


	void init() {
	    gpio->init(true,1,0);
	}


	void run() {
		uint32_t output = 0;
		TIME_LOOP(0,periode){
			/* Toggle pin */
		    output ^= 1;
		    gpio->setPins(output);
		}
	}
};

HAL_GPIO gpio060(GPIO_060); // STM32F4-PD12 (STM32F4DISCOVERY: LED green)
HAL_GPIO gpio061(GPIO_061); // STM32F4-PD13 (STM32F4DISCOVERY: LED orange)
HAL_GPIO gpio062(GPIO_062); // STM32F4-PD14 (STM32F4DISCOVERY: LED red)
HAL_GPIO gpio063(GPIO_063); // STM32F4-PD15 (STM32F4DISCOVERY: LED blue)

GPIOExampleOut gpioThread0("GPIO 60",&gpio060, 100 * MILLISECONDS);
GPIOExampleOut gpioThread1("GPIO 61",&gpio061, 200 * MILLISECONDS);
GPIOExampleOut gpioThread2("GPIO 62",&gpio062, 500 * MILLISECONDS);
GPIOExampleOut gpioThread3("GPIO 63",&gpio063, 1000 * MILLISECONDS);
/***********************************************************************/




/***********************************************************************/
class GPIOExampleIn: public Thread {
    uint64_t periode;
    HAL_GPIO *gpio;

public:
    GPIOExampleIn(const char* name, HAL_GPIO* gpio, uint64_t periode) :
            Thread(name) {
        this->periode = periode;
        this->gpio = gpio;
    }


    void init() {
        gpio->init(false,1,0);
    }


    void run() {
        TIME_LOOP(0,periode){
            /* Read pin */
            PRINTF("GPIO level: %d\n",(int)gpio->readPins());
        }
    }
};

HAL_GPIO gpio000(GPIO_000); // STM32F4-PA0 (STM32F4DISCOVERY: user pushbutton "B1")

GPIOExampleIn gpioThread4("GPIO 0",&gpio000, 500 * MILLISECONDS);

/***********************************************************************/
