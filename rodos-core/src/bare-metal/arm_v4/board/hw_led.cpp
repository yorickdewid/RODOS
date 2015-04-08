
#include "../lpc24xx_registers.h"
#include "hw_led.h"

// ----------------------------------------------------------------------------
void
LedGreen::init()
{
	off();

	// change pin to output
	FIO4DIR |= 1<<17;
}

void
LedGreen::on()
{
	FIO4CLR |= 1<<17;
}

void
LedGreen::off()
{
	FIO4SET |= 1<<17;
}

void
LedGreen::set(bool enable)
{
	if (enable) {
		on();
	}
	else {
		off();
	}
}

void
LedGreen::toggle()
{
	if (FIO4PIN & (1<<17) ) {
		on();
	}
	else {
		off();
	}
}

// ----------------------------------------------------------------------------
void
LedRed::init()
{
	off();

	// change pin to output
	FIO4DIR |= 1<<16;
}

void
LedRed::on()
{
	FIO4CLR |= 1<<16;
}

void
LedRed::off()
{
	FIO4SET |= 1<<16;
}

void
LedRed::set(bool enable)
{
	if (enable) {
		on();
	}
	else {
		off();
	}
}

void
LedRed::toggle()
{
	if (FIO4PIN & (1<<16) ) {
		on();
	}
	else {
		off();
	}
}
