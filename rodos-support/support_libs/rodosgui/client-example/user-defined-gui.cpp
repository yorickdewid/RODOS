#include "rodos.h"
#include "rodosgui-client.h"
#include "random.h"

#define RAND randomTT800Positive()

using namespace RODOS;
/************** Test GUI output ********************/

class UserDefGuiTester: public Thread{
public:
  void run() {

    GuiCommand myCmd;

    char buf[100];
    myCmd.commandNr = GUICMD_USER_COMMANDS;
    int cnt = 0;

    TIME_LOOP(2*SECONDS, 1*SECONDS) {
        xsprintf(buf, " = %3.9f ", SECONDS_NOW());
        PRINTF(" guitest time = %s press any button on GUI to generate events\n", buf);

        myCmd.commandNr++;
        myCmd.param[0] = RAND % 100;
        strcpy((char*)&(myCmd.param[1]), buf);

        /*************/
            // rodosGui.userCommand(myCmd); // both ar equivalent, use as prefered
            guiCommand.publish(myCmd); // both ar equivalent, use as prefered
        /*************/

	if(cnt++ % 20 == 0) myCmd.commandNr = GUICMD_USER_COMMANDS;
    }
  }
} userDefGuiTester;


/********** Tests to recieve events from GUI ******************/

void guiEventPrinter(GuiEvent &event) {
    PRINTF("Gui Event at %3.9f: ",  SECONDS_NOW());
    event.printMe();
}

SubscriberReceiver<GuiEvent> noName01(guiEvent, guiEventPrinter, "GuiEventServer");


