/**
 * Author: Murat Goeksu <Murat.Goeksu@dlr.de>
 *
 * Leon3/Nexys3 HAL GPIO example (blinking leds)
 */

#include "rodos.h"
#include "hal/hal_gpio.h"

static Application module03("Leon3/Nexys3 GPIO led example", 2000);

class GPIOExampleLed: public Thread {
    uint64_t period;
    HAL_GPIO *gpio;

public:
    GPIOExampleLed(const char* name, HAL_GPIO* gpio, uint64_t period) :
        Thread(name) {
        this->period = period;
        this->gpio = gpio;
    }

    void init() {
        gpio->init(true,1,0);
    }

    void run() {
        uint32_t output = 0;
        TIME_LOOP(0,period){
            /* Toggle pin */
            output ^= 1;
            gpio->setPins(output);
        }
    }
};

HAL_GPIO gpio032(GPIO_032); // NEXYS3: LED 1
HAL_GPIO gpio033(GPIO_033); // NEXYS3: LED 2
HAL_GPIO gpio034(GPIO_034); // NEXYS3: LED 3
HAL_GPIO gpio035(GPIO_035); // NEXYS3: LED 4
HAL_GPIO gpio036(GPIO_036); // NEXYS3: LED 5
HAL_GPIO gpio037(GPIO_037); // NEXYS3: LED 6
HAL_GPIO gpio038(GPIO_038); // NEXYS3: LED 7
HAL_GPIO gpio039(GPIO_039); // NEXYS3: LED 8

GPIOExampleLed gpioThread0("GPIO 32", &gpio032, 100 * MILLISECONDS);
GPIOExampleLed gpioThread1("GPIO 33", &gpio033, 200 * MILLISECONDS);
GPIOExampleLed gpioThread2("GPIO 34", &gpio034, 300 * MILLISECONDS);
GPIOExampleLed gpioThread3("GPIO 35", &gpio035, 400 * MILLISECONDS);
GPIOExampleLed gpioThread4("GPIO 36", &gpio036, 500 * MILLISECONDS);
GPIOExampleLed gpioThread5("GPIO 37", &gpio037, 600 * MILLISECONDS);
GPIOExampleLed gpioThread6("GPIO 38", &gpio038, 700 * MILLISECONDS);
GPIOExampleLed gpioThread7("GPIO 39", &gpio039, 800 * MILLISECONDS);
