
#include "rodos.h"
#include "random.h"
#include "topics.h"


static Application ioName("Sensor_Actuators");

long lastX = 0; // updated by actuator
long lastY = 0; // updated by actuator

class Sensor : public Thread {

    void run () {
        Coordinates target;

        TIME_LOOP(1*SECONDS, 200*MILLISECONDS) {
            static long long loopCnt = 0;
            if((loopCnt % 50) == 0)  {
                target.msgCnt++;
                target.subCounter = 0;
                target.x = randomTT800Positive() % 500;
                target.y = randomTT800Positive() % 500;
            }
            loopCnt++;

            target.lastX = lastX;
            target.lastY = lastY;
            target.subCounter++;

            PRINTF("SEN: (%3ld, %3ld) -> (%3ld, %3ld)\r", target.x, target.y, lastX, lastY);
            fromSensor.publish(target);
        }
    }
} sensor;


/*****************************************************/

static SyncFifo<Coordinates,10> coordinates;
static Subscriber coordinatesPutter(toActuator, coordinates);

class Actuator : public Thread {
public:
    void run () {
        Coordinates myPos;
        while(1) {
            coordinates.syncGet(myPos);
            if(myPos.x < 0 || myPos.x > 500) continue;
            if(myPos.y < 0 || myPos.y > 500) continue;
            lastX = myPos.x;
            lastY = myPos.y;
            // PRINTF("ACT: (%ld, %ld)- %lld.%ld\n", myPos.x, myPos.y, myPos.msgCnt, myPos.subCounter);
        }
    }
} actuator;

