
#include "rodos.h"
#include "topics.h"
#include "rodosgui-client.h"


static Application ioName("Sensor_Actuators");

long lastX = 0; // updated by actuator
long lastY = 0; // updated by actuator


class GuiSensor : public Thread {
    Coordinates target;

    void run() {
        TIME_LOOP(1*SECONDS, 200*MILLISECONDS) {
            target.msgCnt++;
            target.subCounter = 0;
            target.x = rodosGui.lastEvent.x;
            target.y = rodosGui.lastEvent.y;
            target.lastX = lastX;
            target.lastY = lastY;
            PRINTF("SEN: (%3ld, %3ld) -> (%3ld, %3ld)\r", lastX, lastY, target.x, target.y);
            fromSensor.publish(target);
        }
    }
} guiSensor;



/*****************************************************/

static SyncFifo<Coordinates,10> coordinates;
static Subscriber coordinatesPutter(toActuator, coordinates);

class GuiActuator : public Thread {
    Coordinates myPos;
public:
    void run () {
        while(!rodosGui.isGuiReady()) {
            suspendCallerUntil(NOW() + 1*SECONDS);
        }

        rodosGui.setBackgroundColor(80, 80, 80, 255);
        rodosGui.setPosAndSize(200, 20, 500, 500);
        rodosGui.drawText(50, 50, "mouse-click any where to set next target position");

        while(1) {
            coordinates.syncGet(myPos);
            if(myPos.x < 0 || myPos.x > 500) continue;
            if(myPos.y < 0 || myPos.y > 500) continue;
            rodosGui.drawLine  (lastX, lastY, myPos.x, myPos.y);
            rodosGui.drawCircle(myPos.x, myPos.y, 5, 255,0,0,255);
            lastX = myPos.x;
            lastY = myPos.y;


        }
    }
} guiActuator;

