#include "i2cbus.h"
#include "itg3200.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/*
 * - Driver for ITG3200 Integrated Triple-Axis Digital-Output Gyroscope from InvenSense (http://invensense.com/mems/gyro/itg3200.html)
 * - IC needs to be connected to I2C bus
 */

int ITG3200::setup() {
	unsigned char srd = 8 - 1;

	setReg(0x15, srd);		// sample rate divider
	setReg(0x16, (0x03 << 3) | 2);	// FS_SEL, DLPF
	setReg(0x17, 0xf1);		// Interrupt CFG
	setReg(0x3e, 0x01);		// CLK_SEL

	if( getReg(0x15) != srd ) {
		return -1;
	}
	return 0;
}

int ITG3200::update() {
	static const unsigned char reg_offset = 0x1b;	//0x1d
	unsigned char rbuf[8];							//[6]

	if( WriteRead(&reg_offset, 1, rbuf, 8) ){		//rbuf,6
		// I2C failure, please retry
		return -1;
	}

	unsigned char n = 0;
	temp=(rbuf[n] << 8) | rbuf[n+1]; n += 2;		//...
	x = (rbuf[n] << 8) | rbuf[n+1]; n += 2;
	y = (rbuf[n] << 8) | rbuf[n+1]; n += 2;
	z = (rbuf[n] << 8) | rbuf[n+1]; n += 2;

	return 0;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif

