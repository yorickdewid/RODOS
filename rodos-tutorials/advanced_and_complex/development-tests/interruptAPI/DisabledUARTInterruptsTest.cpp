/*
 * DisabledUARTInterruptsTest.cpp
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
 * While the thread is running, it's switching the interrupt level of UART 2 off or on every 6 seconds and prints the number of interrupt
 * events on UART 2 every 3 seconds.
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

	//UART 2 is 2 cycles unblocked and 2 cycles blocked.
	//Sending something over UART 2 generates interrupts on the corresponding interrupt level
	//  If the interruptlevel of UART 2 is blocked, only one interrupt is detected after unblocking it again
	virtual void run() {
		int blocked = false;
		int cnt = 0;
		TIME_LOOP(0, 3*SECONDS) {
			blocked = (cnt++ / 2) % 2;
			if(blocked)
				HW_maskInterrupts(HW_INT_UART_2);
			else
				HW_unmaskInterrupts(HW_INT_UART_2);

			xprintf("numInts: %d\n%s\n", numInts, (blocked ? "BLOCKED" : "NOT BLOCKED"));
		}
	}
};

int ModifiedPrinter::numInts = 0;

ModifiedPrinter printer;
