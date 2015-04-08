// WARNING: This file is generated automatically, do not edit!
//          Modify 'uart.cpp.in' instead and then run 'python generate.py'

#ifndef UART3_H
#define UART3_H

#include "uart.h"

/**
 * @brief	Driver for UART3 of the LPC2468
 * 
 * To use this class for other LPC2xxx device you need to check the PINSELx
 * and PINMODEx registers to match the location of the UARTs.
 * 
 * This implementation requires the VIC to be active.
 * 
 * @date	May 2011
 * @author	Fabian Greif 
 */
class Uart3 : public UartInterface
{
public:
	virtual void
	initialize(uint8_t level = 2);
	
	virtual bool
	write(char c, bool blocking = true);
	
	virtual bool
	isCharacterAvailable();
	
	virtual bool
	read(char &c, bool blocking = false);
};

#endif // UART3_H