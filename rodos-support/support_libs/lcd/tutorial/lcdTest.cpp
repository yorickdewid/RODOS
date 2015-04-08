/*
 * blinky.cpp
 *
 * Created on: 30.10.2012
 * Author: Michael Ruffer
 *
 */

#include "rodos.h"
#include <stdio.h>
#include "lcd.h"

/****** Use modulId 2000 just be be found by other example: ceiler **/

static Application module01("LCD Test", 2000);

LCD_HD44780 lcd;

class LCDTest: public Thread {
public:
	LCDTest() : Thread("LCD Test") {}

	void init(){
		lcd.init();
	}

	void run() {

		//long long nextTime = NOW();
		char* buffer = "Hallo Welt";
		lcd.write(buffer,10,0);
		suspendCallerUntil();
		while (1) {

			//nextTime += periode;
			//suspendCallerUntil(nextTime);
		}
	}
};

LCDTest lcdTest;
/***********************************************************************/
