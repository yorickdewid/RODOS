#include "rodos.h"
#include "rodosgui-client.h"
#include "random.h"

using namespace RODOS;

#define RAND randomTT800Positive()

/************** Test GUI output ********************/

class GuiTester: public Thread {
public:
	void run() {

		rodosGui.setBackgroundColor(80, 80, 30, 255);
		rodosGui.setPosAndSize(300, 20, 500, 300);

		int x = 10, y = 10;
		char buf[100];

		TIME_LOOP(2*SECONDS, 1*SECONDS)
		{
			xsprintf(buf, " = %3.9f ", SECONDS_NOW());
			PRINTF(" guitest time = %s press any key on GUI to generate events\n", buf);
			rodosGui.drawText(x, y + 15, "TimeNow");
			rodosGui.fillRectangle(x + 60, y + 30, 100, 20, RAND % 255, RAND % 255, RAND % 255, 128);
			rodosGui.drawRectangle(x + 60, y, 100, 20, 10, 10, 10, 255);
			rodosGui.drawText(x + 60, y + 15, buf);

			rodosGui.drawCircle(RAND % 500, RAND % 300, RAND % 20, RAND % 255, RAND % 255, RAND % 255, 255);
			rodosGui.fillCircle(RAND % 500, RAND % 300, RAND % 20, RAND % 255, RAND % 255, RAND % 255, 255);
			rodosGui.drawLine(RAND % 500, RAND % 300, RAND % 500, RAND % 300);
			rodosGui.drawPolygon(255, 10, 10, 255, 4, RAND % 500, RAND % 300, RAND % 500, RAND % 300, RAND % 500, RAND % 300, RAND % 300, RAND % 300);
			//rodosGui.drawPolygon(255, 10, 10, 255, 6, 1, 1, 1, 100, 100, 1, 100,100, 200,100, 100,200);
			//rodosGui.fillPolygon(255, 10, 10, 50, 5, RAND % 500, RAND % 300, RAND % 500, RAND % 300, RAND % 500, RAND % 300, RAND % 300, RAND % 500, RAND % 300, RAND % 300, RAND % 300);
			//rodosGui.fillPolygon(255, 10, 10, 255, 6, 1, 1, 1, 100, 100, 1, 100,100, 200,100, 100,200);

			if (RAND % 100 < 5) {
				bool onOff = RAND % 100 < 50;
				PRINTF("************* Enable mouse events : %d\n", onOff);
				rodosGui.enableMousereports(onOff);
			}

			y += 30;
			if (y >= 300) {
				x += 200;
				y = 0;
			}
			if (x >= 300) {
				rodosGui.clearScreen();
				x = y = 10;
			}
		}
	}
} guiTester;

/********** Tests to recieve events from GUI ******************/

void guiEventPrinter(GuiEvent &event) {
	PRINTF("Gui Event at %3.9f: ", SECONDS_NOW());
	event.printMe();
}

SubscriberReceiver<GuiEvent> noName01(guiEvent, guiEventPrinter, "GuiEventServer");

