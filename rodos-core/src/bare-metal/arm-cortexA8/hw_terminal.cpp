/*
 *
 */

#include "rodos.h"
#include "uartStdio.h"

namespace RODOS {
extern HAL_UART uart_stdout;

extern "C" {
	/* This function is responsible for printing the characters in the console
	 * is used in the Yprintf class in the yputc(c) method */
	int putchar(int ic){
		char c = (char) (ic & 0xff);

		if(Scheduler::isSchedulerRunning()){
			while(uart_stdout.putcharNoWait(c) == -1){
				uart_stdout.suspendUntilWriteFinished();
			}
		}else{
			UARTPutc(c);
		}

		return c;
	}

	/* This function uses putchar(c) to print a string. */
	int puts(const char* s) {
				if (s == 0) {
			return 0;
		}
		while (*s != 0) {
			putchar(*s);
			s++;
		}
		putchar('\n');
		return 1;
	}

	/*
	 * Flush the uart by reading all the characters until no more are available
	 */
	void flush() {
		
		return;
	}
} /* EXTERN "C" END */

}

void RODOS::FFLUSH() {
}
