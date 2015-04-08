#include "i2cbus.h"
#include "i2cdevice.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

I2Cdevice::I2Cdevice(I2Cbus *b, unsigned char a) : i2c_bus(b), i2c_addr(a) {
}

int I2Cdevice::Write(uint8_t const *wbuf, uint8_t wbytes) const {
	return i2c_bus->Write(i2c_addr, wbuf, wbytes);
}

int I2Cdevice::WriteRead(uint8_t const *wbuf, uint8_t wbytes,
    uint8_t *rbuf, uint8_t rbytes) const {
	return i2c_bus->WriteRead(i2c_addr, wbuf, wbytes, rbuf, rbytes);
}

int I2Cdevice::getReg(unsigned char reg) const {
	unsigned char ret;

	if( WriteRead(&reg, 1, &ret, 1) ) {
		return -1;
	}
	return ret;
}

int I2Cdevice::setReg(unsigned char reg, unsigned char val) const {
	unsigned char wbuf[2];

	wbuf[0] = reg;
	wbuf[1] = val;
	if( Write(wbuf, 2) ) {
		return -1;
	}
	return 0;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif

