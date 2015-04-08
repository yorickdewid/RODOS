#ifndef __I2CDEVICE_H__
#define __I2CDEVICE_H__

#include <stdint.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class I2Cbus;

class I2Cdevice {
protected:
	I2Cdevice(I2Cbus *bus, unsigned char addr);

	// basic access
	int Write(uint8_t const *wbuf, uint8_t wbytes) const;
	int WriteRead(uint8_t const *wbuf, uint8_t wbytes,
	    uint8_t *rbuf, uint8_t rbytes) const;

	// common I2C-device-register access
	int getReg(unsigned char reg) const;
	int setReg(unsigned char reg, unsigned char val) const;

private:
	I2Cbus * const i2c_bus;			// I2C-bus
	unsigned char const i2c_addr;		// I2C-address
};


#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* __I2CDEVICE_H__ */
