#include "rodos.h"
#include "hal.h"


void MAIN(){
		PRINTF("GPIO Test\n");
#ifdef DYNAMIC_MEMORY_ALLOCATION
		PRINTF("- using dynamic memory allocation\n");
#endif
}


class GPIOThread: public Thread {
	HAL_GPIO gpio; // PB27
	long long periode;

public:
	GPIOThread(const char* name, GPIO_PIN idx, long long periode) :
		Thread(name),
		gpio(idx){
		this->periode = periode;
	}

	void init() {
		gpio.init(true, 1);
	}

	void run() {
		int value = 0;
		PRINTF("%s started\n",getName());
		TIME_LOOP(0*SECONDS,periode) {
			value ^= 1;
			gpio.setPins(value);

		}
	}
};

GPIOThread g1("LED1",GPIO_059,100*MILLISECONDS); //PB27
GPIOThread g2("LED2",GPIO_060,200*MILLISECONDS); //PB28
GPIOThread g3("LED3",GPIO_061,300*MILLISECONDS); //PB29
GPIOThread g4("LED4",GPIO_062,400*MILLISECONDS); //PB30

GPIOThread g5("LED5",GPIO_051,700*MILLISECONDS); //PB19
GPIOThread g6("LED6",GPIO_052,800*MILLISECONDS); //PB20
GPIOThread g7("LED7",GPIO_053,900*MILLISECONDS); //PB21
GPIOThread g8("LED8",GPIO_054,1000*MILLISECONDS); //PB22

