#include "rodos.h"
#include "hal_uart.h"

class UartTestReceiver: public Thread{
public:
	UartTestReceiver(const char* name) : Thread(name) {
	}

	void run() {
		char uartRXBuf0[10];
		char uartRXBuf1[10];
		uart0.write("UART Test started!\n",19);
		while(1) {
			int rxCnt0 = uart0.read(uartRXBuf0,10);
			if (rxCnt0 > 0){
				uart0.write(uartRXBuf0,rxCnt0);
			}

			int rxCnt1 = uart1.read(uartRXBuf1,10);
			if (rxCnt1 > 0){
				uart1.write(uartRXBuf1,rxCnt1);
			}
			suspendCallerUntil(NOW() + 100*MILLISECONDS);
		}
    }
};

UartTestReceiver u1("u1");


