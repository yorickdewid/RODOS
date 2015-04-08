#include "i2cbus.h"
#include "lsm303.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/*
 * - Driver for LSM303DLM (3-axis accelerometer and 3-axis magnetometer) from STMicroelectonics (http://www.st.com/internet/analog/product/251902.jsp)
 * - IC needs to be connected to I2C bus
 */
int LSM303acc::setup() {
	unsigned char reg1a = 0x27;

	setReg(0x20, reg1a);		// ctrl_reg1_a
	setReg(0x23, 0xb0);		// ctrl_reg4_a
	if( getReg(0x20) != reg1a ) {
		return -1;
	}
	return 0;
}

int LSM303acc::update() {
	unsigned char stat = getReg(0x27);

	if( (stat & 0x08) == 0 ) {
		// No new data
		return 1;
	}

	static const unsigned char reg_offset = 0x28 | 0x80;
	unsigned char rbuf[6];

	if( WriteRead(&reg_offset, 1, rbuf, sizeof rbuf) ){
		// I2C failure, please retry
		return -1;
	}

	unsigned char n = 0;
	x = (rbuf[n+1] << 8) | rbuf[n]; n += 2;
	y = (rbuf[n+1] << 8) | rbuf[n]; n += 2;
	z = (rbuf[n+1] << 8) | rbuf[n]; n += 2;

	return 0;
}

/*
 *	Magnetometer
 */
int LSM303mag::setup() {
	if( (getReg(0x0a) != 0x48) || (getReg(0x0b) != 0x34) ) {
		return -1;
	}

	setReg(0x00, 0x18);		// cra_reg_m
	setReg(0x01, 0x20);		// crb_reg_m
	setReg(0x02, 0x80);		// mr_reg_m
	return 0;
}

int LSM303mag::update() {
	unsigned char stat = getReg(0x09);

	if( (stat & 0x01) == 0 ) {
		// No new data
		return 1;
	}

	static const unsigned char reg_offset = 0x03;
	unsigned char rbuf[6];

	if( WriteRead(&reg_offset, 1, rbuf, sizeof rbuf) ){
		// I2C failure, please retry
		return -1;
	}

	unsigned char n = 0;
	x = (rbuf[n] << 8) | rbuf[n+1]; n += 2;
	y = (rbuf[n] << 8) | rbuf[n+1]; n += 2;
	z = (rbuf[n] << 8) | rbuf[n+1]; n += 2;

	return 0;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif

