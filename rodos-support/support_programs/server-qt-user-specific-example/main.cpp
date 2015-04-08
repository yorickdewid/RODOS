
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "interfaceToMyGui.h"
#include <QApplication>

#include "rodosgui-server.h"
#include "udp_common.h"

using namespace RODOS;

extern void *controller(void *param);

int main(int parn, char* parv[]) {
    pthread_t controllerThread;
    long dummyParam = 0;

    pthread_create(&controllerThread, NULL, controller, &dummyParam);

    QApplication app(parn, parv);
    InterfaceToMyGUI gui;
    gui.setStyleSheet("background-color:gray;");
    gui.show();
    app.exec();
    return 0;
}



/** controller as pthread from main ***/

UDPInLx udpIn(6021);

extern void executeGuiCommand(GuiCommand& gCmd);

void *controller(void *param){
    GuiCommand gCmd; 
    sleep(2); // Else it crahsesQ

    while(1) {
      usleep(50000); // if it is too fast, it crashes! 
      bool ok = udpIn.get(&gCmd);
      if(ok) { executeGuiCommand(gCmd); }
   }
  return param;
}



