/**
 * Author: Murat Goeksu <Murat.Goeksu@dlr.de>
 *
 * HAL GPIO device driver interface for Nexys3/Leon3.
 */

#include <new>
#include "rodos.h"
#include "hal/hal_gpio.h"
#include "hw_defs.h"
#include "hw_core.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

    class HW_HAL_GPIO
    {
    public:
        HW_HAL_GPIO(GPIO_PIN pinIdx, uint8_t numOfPins, bool isOutput):
            pinIdx(pinIdx),numOfPins(numOfPins),isOutput(isOutput)
        {
            setGPIOPortVars();
        };
        GPIO_PIN pinIdx;
        uint8_t numOfPins;
        bool isOutput;
        uint32_t pinMask;
        int32_t port;

        void setPinMask();

    private:
        void setGPIOPortVars();
    };

    void HW_HAL_GPIO::setPinMask()
    {
        if (numOfPins + (pinIdx & 0x1F) > 32)
        {
            // Pin-group exceeds port boundary! Only pins up to most significant pin of port will be set
            pinMask = 0xFFFFFFFF << (pinIdx & 0x1F);
        }
        else
        {
            pinMask = 0xFFFFFFFF >> (32 - numOfPins);
            pinMask = pinMask << (pinIdx & 0x1F);
        }
    }

    void HW_HAL_GPIO::setGPIOPortVars()
    {
        // For addresses see leon3-nexys3/hw_defs.h
        if( (pinIdx >= 32) && (pinIdx < 52) )
        {
            port = HW_OCR_LED_SW_BTN;
        }
        else if( (pinIdx >= 64) && (pinIdx < 68) )
        {
            port = HW_OCR_GPIO;
        }
        else
        {
            port = -1;
        }
    }

    HAL_GPIO::HAL_GPIO(GPIO_PIN pinIdx)
    {
        context = (HW_HAL_GPIO*)xmalloc(sizeof(HW_HAL_GPIO)); // dynamic memory allocation with RODOS function
        context = new (context) HW_HAL_GPIO(pinIdx,1,false); // placement new
    }

    int32_t HAL_GPIO::init(bool isOutput, uint32_t numOfPins, uint32_t initVal)
    {
        // Return if the selected GPIO pin is not valid
        if (context->port == -1)
        {
            return -1;
        }
        // numOfPins has to be >0 -> If new value is 0, keep the default value
        if (numOfPins > 0)
        {
            context->numOfPins = numOfPins;
        }
        context->isOutput = isOutput;
        context->setPinMask();
        if (context->isOutput)
        {
            setPins(initVal);
        }

        return 0;
    }

    int32_t HAL_GPIO::config(GPIO_CFG_TYPE type, uint32_t paramVal)
    {
        //TODO
        return -1;
    }

    void HAL_GPIO::reset(){
        init(context->isOutput, context->numOfPins, 0);
    }


    void HAL_GPIO::setPins(uint32_t val)
    {
        //FIXME: Exception with PrioCeil? Why?
        uint32_t newPinVal = 0;
        if (context->isOutput)
        {
            // Read the whole port, change only the selected pins and write the value
            //
            //PRIORITY_CEILING{
                // Get current pin values of the whole port and clear pin values we want to set new
                newPinVal = *HW_OCR(context->port) & ~context->pinMask;
                // Set new pin values
                newPinVal |= (val << (context->pinIdx & 0x1F) ) & context->pinMask;
                // Write new pin values
                *HW_OCR(context->port) = newPinVal;
            //}
        }
    }


    uint32_t HAL_GPIO::readPins()
    {
        return ( (*HW_OCR(context->port) & context->pinMask) >> (context->pinIdx & 0x1f) );
    }

#ifndef NO_RODOS_NAMESPACE
}
#endif

