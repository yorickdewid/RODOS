
#include "rodos.h"
#include "topics.h"

static SyncFifo<Coordinates,10> coordinates;
static Subscriber coordinatedPutter(fromSensor, coordinates);

class Steering : public Thread {

    void run () {

        Coordinates myTarget;
        Coordinates newPos;

        while(1) {
            coordinates.syncGet(myTarget);
            long deltaX = myTarget.x - myTarget.lastX;
            long deltaY = myTarget.y - myTarget.lastY;

            newPos   = myTarget; // all default values
            newPos.x = myTarget.lastX - (deltaX / 10);
            newPos.y = myTarget.lastY - (deltaY / 10);
            newPos.myNode = MY_NODE_NR();

            toActuator.publish(newPos);
            newPos.printIt();
        }
    }
} steering;


/*********** AutoKill ***/

class AutoKill : public Putter {

    bool putGeneric(unsigned int msgLen, const void* msg) {
        long nodeToKill = *(long*)msg;
        PRINTF(" Node to kill %ld\n", nodeToKill);
        if(nodeToKill == MY_NODE_NR()) hwResetAndReboot(); // thats me! -> bye bye ....
        return true;
    }
} autoKill;
static Subscriber autoKillPutter(killNode, autoKill);



