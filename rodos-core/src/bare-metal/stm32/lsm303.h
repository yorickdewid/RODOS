#ifndef __LSM303__H__
#define __LSM303__H__

#include "i2cdevice.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class I2Cbus;

/*
 * - Driver for LSM303DLM (3-axis accelerometer and 3-axis magnetometer) from STMicroelectonics (http://www.st.com/internet/analog/product/251902.jsp)
 * - IC needs to be connected to I2C bus
 */


/*
 *	Accelerometer
 */
class LSM303acc : private I2Cdevice {
public:
	LSM303acc(I2Cbus *bus, unsigned char addr = 0x30) : I2Cdevice(bus, addr)
	{}

	int setup();

	int update();

	short x, y, z;
};

/*
 *	Magnetometer
 */
class LSM303mag : private I2Cdevice {
public:
	LSM303mag(I2Cbus *bus, unsigned char addr = 0x3c) : I2Cdevice(bus, addr)
	{}

	int setup();

	int update();

	short x, y, z;
};


#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* __LSM303__H__ */
