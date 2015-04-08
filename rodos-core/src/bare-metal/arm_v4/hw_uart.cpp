/**
 * @file hw_uart.cc
 * @date 2008/12/03 10:03
 * @author	David Waleczek, Lutz Dittrich, Sergio Montenegro
 * @author	Fabian Greif
 * 
 * Copyright 2008 DLR
 *
 * @brief UART communication
 *
 */

#include <string.h>
#include <rodos.h>

#include "hw_uart.h"
#include "lpc24xx_registers.h"

#include "uart/uart_0.h"
#include "uart/uart_1.h"
#include "uart/uart_2.h"
#include "uart/uart_3.h"
namespace RODOS {
// ----------------------------------------------------------------------------
UART::UART(const char* devname)
{
	// All available UARTs are declared here to make sure that they are
	// fully constructed before the initialize function is called!
	static Uart0 uart0;
	static Uart1 uart1;
	static Uart2 uart2;
	static Uart3 uart3;

	static UartInterface* uarts[4] = { &uart0, &uart1, &uart2, &uart3 };
	
	// Extract the device identifier (0..3) from the given name. If the
	// format is incorrect 0 will be used as identifier.
	uint_fast8_t deviceId = 0;
	if (strlen(devname) == 5 && strncmp(devname, "UART", 4) == 0) {
		deviceId = devname[4] - '0';
		if (deviceId > 3) {
			deviceId = 0;
		}
	}
	
	device = uarts[deviceId];
	device->initialize(2);
}

UART::~UART()
{
}

void UART::writechar(const char c) {
	device->write(c);
}

bool UART::isCharReady() {
	return device->isCharacterAvailable();
}

bool UART::getcharNoWait(char &c) {
	return device->read(c);
}

bool UART::putGeneric(const unsigned int len, const void* msg) {
	unsigned int ctr;
	const char* ptr = reinterpret_cast<const char*>(msg);
	for (ctr = 0; ctr < len; ctr++) {
		device->write(*ptr++);
	}
	return true;
}
}
