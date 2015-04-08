#include "rodos.h"
#include "hal.h"
#include "params.h"
#include "hw_specific.h"



namespace RODOS{

extern HAL_UART UART_DEBUG;
}





HAL_PWM pwm3(PWM_IDX01);


class MotorControl: public Thread{
public:
	MotorControl(const char* name) : Thread(name) {
	}
	void init(){
		pwm3.init(1000,9);
	}
	void run() {
		int i = 0;
		PRINTF("PWM Test started!\nPress +/- to decrease/increase duty cycle\n");
		while(1) {
			int input = UART_DEBUG.getcharNoWait();
			if (input != 0){
				switch (input){
				case '+':
					i+=10;
					if (i>512) i=512;
					pwm3.write(i);
					PRINTF("%d\n",i);
					break;
				case '-':
					i-=10;
					if (i<0) i=0;
					pwm3.write(i);
					PRINTF("%d\n",i);
					break;
				}
			}
			suspendCallerUntil(NOW() + 100*MILLISECONDS);
		}
    }
};

MotorControl m("motorcontrol");



