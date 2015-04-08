/*
 *
 */

#include "rodos.h"
#include "workspace.h"
#include "registers/regsuart.h"

#include <new>
#include "hw_specific.h"


#define MAX_DEBUG_LINE_LENGTH 256

struct DebugLine{
	int core;
	char line[MAX_DEBUG_LINE_LENGTH];
};

DebugLine currentLine;
int currentLinePointer=0;

HAL_UART uart_stdout(UART_DEBUG);
HAL_Sharedmemory shm_stdout((RODOS::Sharedmemory_IDX)SHM_DEBUG);

typedef BlockFifo<DebugLine,256> DebugFifo;
DebugFifo* debug_fifo;

extern "C" {

int putcharu(int c);
int puts(const char* s);

	/* This function is responsible for printing the characters in the console
	 * is used in the Yprintf class in the yputc(c) method */
	int putchar(int c){

		if(!debug_fifo){
			return c;
		}
		//putcharu(c);

		//We don't know if we are in an interrupt or not, so we cant use semaphore or hwDisabelInterrupts. So we have to do this
		// TODO

		if(c != '\n'){
			currentLine.line[currentLinePointer]=c;
			currentLinePointer++;
		}

		if(c == '\n' || currentLinePointer+1 == MAX_DEBUG_LINE_LENGTH){
			currentLine.line[currentLinePointer]=0;

			shm_stdout.ensureLock();
			debug_fifo->put(currentLine);
			shm_stdout.unlock();
			shm_stdout.raiseSharedMemoryChanged();

			currentLinePointer=0;
		}


		return c;

	}
/*
int putcharu(int c){


		int instance = 0;

		int cpu = cpu_get_current();
		switch(cpu){
		case 0:
			instance = HW_UART2;
			break;
		case 1:
		case 2:
		case 3:
			instance = HW_UART1;
			break;
		}

		while (HW_UART_UTS(instance).B.TXFULL);

		HW_UART_UTXD_WR(instance, (char)c);

		if(((unsigned char)c) == '\n'){
			// wait to write the return character
			while (HW_UART_UTS(instance).B.TXFULL);
			HW_UART_UTXD_WR(instance, '\r');
		}

		return c;

	}


*/

	/* This function uses putchar(c) to print a string. */
	int puts(const char* s) {
		if (s == 0) {
			return 0;
		}
		while (*s != 0) {
			putchar(*s);
			s++;
		}
		//putchar('\n');
		return 1;
	}

} /* EXTERN "C" END */

void RODOS::FFLUSH() {
	if(currentLinePointer > 0)
		putchar('\n');
}


void initDebugOutput(){
	int32_t size=sizeof(DebugFifo),maxmembers=4;
	char* shMem = (char*) shm_stdout.init(&size,&maxmembers);
	if(cpu_get_current()==0){
		debug_fifo= new (shMem) DebugFifo();
		uart_stdout.init();

	}else{
		debug_fifo = (DebugFifo*) shMem;

	}

	currentLine.core=cpu_get_current();
}



class stdOutDeamon : public Thread {

	void puts_uart(const char* str){
		while(*str){
			while(uart_stdout.putcharNoWait(*str) == -1){}
			str++;
		}
	}



	void run(){
		if(cpu_get_current() != 0){
			return;
		}

		while(1){
			shm_stdout.suspendUntilDataReady();
			shm_stdout.resetDataReady();

			int len;
			DebugLine* line = debug_fifo->getBufferToRead(len);
			while(len>0){
				char label[20];
				xsprintf(label,"Core %d: ",line->core);
				puts_uart(label);
				puts_uart(line->line);
				puts_uart("\n");
				debug_fifo->readConcluded(1);

				line = debug_fifo->getBufferToRead(len);
			}

		}
	}

}stdOutDeamonInstance;
