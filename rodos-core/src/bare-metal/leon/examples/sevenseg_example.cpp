/**
 * Author: Murat Goeksu <Murat.Goeksu@dlr.de>
 *
 * Leon3/Nexys3 7 segment display example
 */

#include "rodos.h"
#include "hal/hal_gpio.h"
#include "hw_sevensegment.h"


static Application module02("Leon3/Nexys3 7 segment display example", 2000);

class GPIOExampleButtonDisplay: public Thread {
    uint64_t period;
    HAL_GPIO *gpio_btn;
    HW_SEVENSEGMENT *hw_sevensegment;

public:
    GPIOExampleButtonDisplay(const char* name, HAL_GPIO* gpio_btn, HW_SEVENSEGMENT* hw_sevenseg, uint64_t period) :
        Thread(name) {
        this->period = period;
        this->gpio_btn = gpio_btn;
        this->hw_sevensegment = hw_sevensegment;
    }

    void init() {
        // Use all 4 buttons
        gpio_btn->init(false,4,0);
        // 7 segment display doesn't need to be initialized
    }

    void run() {
        // Display the currently pressed button, "LEO3" if no button is pressed
        uint32_t val = 0;
        TIME_LOOP(0,period){
            val = gpio_btn->readPins();
            switch(val){
                case 1:
                    hw_sevensegment->writeText("L");
                    break;
                case 2:
                    hw_sevensegment->writeText("D");
                    break;
                case 4:
                    hw_sevensegment->writeText("R");
                    break;
                case 8:
                    hw_sevensegment->writeText("U");
                    break;
                default:
                    hw_sevensegment->writeText("LEO3");
                    break;
            }
        }
    }
};

HAL_GPIO gpio048(GPIO_048); // NEXYS3: BUTTON 1 (left)
HW_SEVENSEGMENT sevenseg;   // NEXYS3: 7 segment display
GPIOExampleButtonDisplay gpioThread9("BTN/DISP", &gpio048, &sevenseg, 100 * MILLISECONDS);
