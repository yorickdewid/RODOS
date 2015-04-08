#ifndef __ITG3200__H__
#define __ITG3200__H__

#include "i2cdevice.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class I2Cbus;

/*
 * - Driver for ITG3200 Integrated Triple-Axis Digital-Output Gyroscope from InvenSense (http://invensense.com/mems/gyro/itg3200.html)
 * - IC needs to be connected to I2C bus
 */

// use I2C address 0xd0 for FIRST Multisensorboard
// use I2C address 0xd2 for Olimex STM32-P103, when using Sparkfun ITG-3200 breakout board

class ITG3200 : private I2Cdevice {
public:
	ITG3200(I2Cbus *bus, unsigned char addr = 0xd0) : I2Cdevice(bus, addr)
	{}

	int setup();

	int update();

	short x, y, z, temp;
};


#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* __ITG3200__H__ */
