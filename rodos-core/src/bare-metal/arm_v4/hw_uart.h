/**
 * @file hw_uart.h
 * @date 2008/12/03 9:55
 * @author Lutz Dittrich, Sergio Montenegro
 *
 * Copyright 2008 DLR
 *
 * @brief UART communication
 *
 */

#ifndef ARMv4_HW_UART_H
#define ARMv4_HW_UART_H

#undef putchar

#include "timeevent.h"
#include "putter.h"

#include "uart/uart.h"
namespace RODOS {
/**
 * @brief	UART driver
 * 
 * Uses the event ids from EVENT_UART to EVENT_UART+8, see event.h
 */
class UART: public Putter
{
public:
	UART(const char* devname);
	virtual ~UART();
	
	/**
	 * @brief	Write a character
	 * 
	 * Thread is suspended until the character could be written
	 */
	void writechar(const char c);
	bool isCharReady();
	bool getcharNoWait(char &c);

	virtual bool putGeneric(const unsigned int len, const void* msg);
	
private:
	UartInterface *device;
};
}
#endif // ARMv4_HW_UART_H
