#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"

#include "i2cbus.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

struct I2Cbus_spec {
	/* Config */
	uint32_t	cfg_speed;
	uint16_t	cfg_own_address7;

	/* I2C */
	I2C_TypeDef	*i2c_dev;
	uint32_t	i2c_clk;

	/* GPIO */
	GPIO_TypeDef	*gpio_dev;
	uint32_t	gpio_clk;
	uint16_t	gpio_scl;
	uint16_t	gpio_sda;
};


#define GET_DEV()	(this->dev_spec->i2c_dev)

I2Cbus::I2Cbus(const I2Cbus_spec *ds) : dev_spec(ds) {
	nbusy_wait = (256 * (400*1000)) / ds->cfg_speed;
	Setup();
}

void I2Cbus::Setup() const {
	I2Cbus_spec const * const ds = dev_spec;

	/* I2C Periph clock enable */
	RCC_APB1PeriphClockCmd(ds->i2c_clk, ENABLE);

	/* GPIO Periph clock enable */
	RCC_APB2PeriphClockCmd(ds->gpio_clk, ENABLE);

	/*
	 *	Configure the used I/O ports pin
	 */
	{
	GPIO_InitTypeDef is;

	/* Configure I2C pins: SCL and SDA */
	is.GPIO_Pin = ds->gpio_scl | ds->gpio_sda;
	is.GPIO_Speed = GPIO_Speed_50MHz;
	is.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(ds->gpio_dev, &is);
	}

	/*
	 *	I2C configuration
	 */
	{
	I2C_InitTypeDef is;

	/* I2C configuration */
	is.I2C_Mode = I2C_Mode_I2C;
	is.I2C_DutyCycle = I2C_DutyCycle_2;
	is.I2C_OwnAddress1 = ds->cfg_own_address7;
	is.I2C_Ack = I2C_Ack_Enable;
	is.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	is.I2C_ClockSpeed = ds->cfg_speed;

	/* I2C Peripheral Enable */
	I2C_Cmd(GET_DEV(), ENABLE);

	/* Apply I2C configuration after enabling it */
	I2C_Init(GET_DEV(), &is);
	}
}

void I2Cbus::Reset() const {
	I2Cbus_spec const * const ds = dev_spec;
	int i;

	/* Hold I2C in reset */
	I2C_SoftwareResetCmd(GET_DEV(), ENABLE);

	/* Configure I2C pins as GPIO */
	{
	GPIO_InitTypeDef is;

	is.GPIO_Pin = ds->gpio_scl | ds->gpio_sda;
	is.GPIO_Speed = GPIO_Speed_50MHz;
	is.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(ds->gpio_dev, &is);
	}

	/* Toggle SCL until SDA not driven low */
	ds->gpio_dev->BSRR = ds->gpio_sda;
	for(i = 1024; i; --i) {
		ds->gpio_dev->BSRR = ds->gpio_scl;
		if( ds->gpio_dev->IDR & ds->gpio_sda ) {
			break;
		}
		ds->gpio_dev->BRR = ds->gpio_scl;
	}

	/* Release I2C from reset ... */
	I2C_SoftwareResetCmd(GET_DEV(), DISABLE);

	/* ... and setup again */
	Setup();
}

int I2Cbus::WaitEvent(uint32_t i2c_event) const {
	I2C_TypeDef * const I2Cx = GET_DEV();

	for( int n = nbusy_wait; n; --n ) {
		uint32_t stat;

		stat = I2Cx->SR1 | (I2Cx->SR2 << 16);

		if( (stat & i2c_event) == i2c_event ) {
			return 0;
		}
	}

	Reset();
	return -1;
}

int I2Cbus::WriteNS(uint8_t addr, uint8_t const *wbuf, uint8_t wbytes) const {
	I2C_TypeDef * const I2Cx = GET_DEV();

	/* Send START condition */
	I2Cx->CR1 |= 0x0100;

	/* Test on EV5 and clear it */
	if( WaitEvent(I2C_EVENT_MASTER_MODE_SELECT) ){
		return -1;
	}

	/* Send address for write */
	I2Cx->DR = addr;

	/* Test on EV6 and clear it */
	if( WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) ){
		return -1;
	}

	/* While there is data to be written */
	while( wbytes-- ) {
		I2Cx->DR = *wbuf++;

		/* Test on EV8 and clear it */
		if( WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING) ){
			return -1;
		}
	}

	return 0;
}

int I2Cbus::Write(uint8_t addr, uint8_t const *wbuf, uint8_t wbytes) const {
	I2C_TypeDef * const I2Cx = GET_DEV();

	if( WriteNS(addr, wbuf, wbytes) ) {
		return -1;
	}

	/* Send STOP condition */
	I2Cx->CR1 |= 0x0200;

	return 0;
}

int I2Cbus::WriteRead(uint8_t addr, uint8_t const *wbuf, uint8_t wbytes, uint8_t *rbuf, uint8_t rbytes) const {
	I2C_TypeDef * const I2Cx = GET_DEV();

	if( WriteNS(addr, wbuf, wbytes) ) {
		return -1;
	}

	/* Send (RE-)START condition */
	I2Cx->CR1 |= 0x0100;

	/* Test on EV5 and clear it */
	if( WaitEvent(I2C_EVENT_MASTER_MODE_SELECT) ){
		return -1;
	}

	/* Send address for read */
	I2Cx->DR = addr | 0x01;

	/* Test on EV6 and clear it */
	if( WaitEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) ){
		return -1;
	}

	/* While there is data to be read */
	while( rbytes-- ) {
		if( rbytes == 0) {
			/*
			 * Receiving last byte
			 *	- disable acknowledgement
			 * 	- send STOP condition
			 */
			I2Cx->CR1 = (I2Cx->CR1 & ~0x0400) | 0x0200;
		}

		/* Test on EV7 and clear it */
		if( WaitEvent(I2C_EVENT_MASTER_BYTE_RECEIVED) ){
			return -1;
		}

		/* Read the byte */
		*rbuf++ = I2Cx->DR;
	}

	/* Enable Acknowledgement to be ready for another reception */
	I2Cx->CR1 |= 0x0400;

	return 0;
}

/**************************/

I2Cbus * const i2c_bus1() {
	static const I2Cbus_spec spec = {
		(100 * 1000),
		0xA0,

		I2C1,
		RCC_APB1Periph_I2C1,

		GPIOB,
		RCC_APB2Periph_GPIOB,
		GPIO_Pin_6,
		GPIO_Pin_7,
	};

	static I2Cbus bus(&spec);

	return &bus;
}

I2Cbus * const i2c_bus2() {
	static const I2Cbus_spec spec = {
		(100 * 1000),
		0xA0,

		I2C2,
		RCC_APB1Periph_I2C2,

		GPIOB,
		RCC_APB2Periph_GPIOB,
		GPIO_Pin_10,
		GPIO_Pin_11,
	};

	static I2Cbus bus(&spec);

	return &bus;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif

