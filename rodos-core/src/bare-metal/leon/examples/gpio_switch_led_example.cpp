/**
 * Author: Murat Goeksu <Murat.Goeksu@dlr.de>
 *
 * Leon3/Nexys3 HAL GPIO example (led status depending on corresponding switch status)
 */

#include "rodos.h"
#include "hal/hal_gpio.h"


static Application module01("Leon3/Nexys3 GPIO switch/led example", 2000);

class GPIOExampleSwitchLed: public Thread {
    uint64_t period;
    HAL_GPIO *gpio_sw;
    HAL_GPIO *gpio_led;

public:
    GPIOExampleSwitchLed(const char* name, HAL_GPIO* gpio_sw, HAL_GPIO* gpio_led, uint64_t period) :
        Thread(name) {
        this->period = period;
        this->gpio_sw = gpio_sw;
        this->gpio_led = gpio_led;
    }

    void init() {
        // Use all 8 leds/switches
        gpio_sw->init(false,8,0);
        gpio_led->init(true,8,0);
    }

    void run() {
        // Turn led on, if the corresponding switch is on.
        TIME_LOOP(0,period){
            gpio_led->setPins(gpio_sw->readPins());
        }
    }
};

HAL_GPIO gpio032(GPIO_032); // NEXYS3: LED 1
HAL_GPIO gpio040(GPIO_040); // NEXYS3: SWITCH 1

GPIOExampleSwitchLed gpioThread8("SW/LED", &gpio040, &gpio032, 200 * MILLISECONDS);
