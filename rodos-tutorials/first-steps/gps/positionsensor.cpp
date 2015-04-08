#include "rodos.h"
#include "topics.h"
#include "random.h"

class PositionSensor : public Thread {
public:
    PositionSensor() : Thread("PositionSensor") { }

    void run () {
        Pos p = {0.0, 0.0, 0.0};
        TIME_LOOP(2*SECONDS, 3*SECONDS) {
            p.x+= (randomTT800Positive() % 40)*0.05-1;
            p.y+= (randomTT800Positive() % 40)*0.05-1;
            p.z+= (randomTT800Positive() % 40)*0.05-1;
            position.publish(p);
        }
    }
} positionSensor;


