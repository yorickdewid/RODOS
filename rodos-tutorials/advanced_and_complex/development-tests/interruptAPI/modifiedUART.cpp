/*
 * modfiedUART.cpp
 *
 * Tests used in LEON-Port modification
 *
 *  Created on: Feb 20, 2012
 *      Author: kraf_ni
 */

#include <rodos.h>
#include <hw_uart.h>
#include <interruptHandling.h>

/*
 * This class sets the interrupt handler for UART 2 to a static function in itself, which just increments a counter on every interrupt.
 * Every 3 seconds the thread prints how often the function was called.
 */
class ModifiedPrinter : public Thread {
	static int numInts;

	static void uartHandling2(void) {
		++numInts;			//If there is an interrupt on UART 2, just increment the counter.
	}
public:
	ModifiedPrinter() : Thread("Interrupt counter UART 2") {}

	virtual void init() {
		HW_setInterruptHandler(0x12, uartHandling2);
	}

	virtual void run() {
		TIME_LOOP(0, 3*SECONDS) {
			xprintf("numInts: %d\n", numInts);
		}
	}
};

int ModifiedPrinter::numInts = 0;

ModifiedPrinter printer;
