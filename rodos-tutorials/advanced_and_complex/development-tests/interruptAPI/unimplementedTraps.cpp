/*
 * unimplementedTraps.cpp
 *
 * Tests used in LEON-Port modification
 *
 *  Created on: Feb 21, 2012
 *      Author: kraf_ni
 */

#include <rodos.h>
#include <interruptHandling.h>

/*
 * This class calls an unhandled user trap and polls the value of the counter of the unhandled trap events.
 * Testing this for a system trap could be done by removing the hwHandleTrap0x12()-Function from hw_uart.cpp and sending
 * something to UART 2, for example. That should increment the counter too, if everything is correct.
 */
class PrinterOfNumUnhandledInterrupts : public Thread {
public:
	PrinterOfNumUnhandledInterrupts() : Thread("Interrupt counter UART 2") {}

	virtual void run() {
		TIME_LOOP(0, 3*SECONDS) {
			xprintf("numFailedTraps: %ld\n", HW_getUnhandledInterruptsCounter());
			__asm__ __volatile__("ta 25");	//Calling an undefined user trap
		}
	}
};

PrinterOfNumUnhandledInterrupts printer;
