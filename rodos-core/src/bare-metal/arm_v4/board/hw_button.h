
#ifndef HW_BUTTON_H
#define HW_BUTTON_H

#include "lpc24xx_registers.h"

class Button
{
public:
	/* return \c true if the button is pressed */
	static bool
	isButton1Pressed()
	{
		return (!(FIO2PIN & (1 << 13)));
	}
	
	static bool
	isButton2Pressed()
	{
		return (!(FIO2PIN & (1 << 21)));
	}
};

#endif // HW_BUTTON_H
