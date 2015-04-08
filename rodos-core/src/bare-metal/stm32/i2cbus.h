#ifndef __I2CBUS_H__
#define __I2CBUS_H__

#include <stdint.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class I2Cbus_spec;

class I2Cbus {
public:
	I2Cbus(I2Cbus_spec const *);

	int Write(uint8_t addr, uint8_t const *wbuf, uint8_t wbytes) const;

	int WriteRead(uint8_t addr, uint8_t const *wbuf, uint8_t wbytes,
	    uint8_t *rbuf, uint8_t rbytes) const;

private:
	void Setup() const;

	void Reset() const;

	int WaitEvent(uint32_t i2c_event) const;

	int WriteNS(uint8_t addr, uint8_t const *wbuf, uint8_t wbytes) const;

	I2Cbus_spec const * const dev_spec;

	int nbusy_wait;
};

I2Cbus * const i2c_bus1();
I2Cbus * const i2c_bus2();


#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* __I2CBUS_H__ */
