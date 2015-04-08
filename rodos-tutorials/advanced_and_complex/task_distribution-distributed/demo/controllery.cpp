#include "rodos.h"
#include "globals.h"
#include "mask.h"

/******************************/

static Application controllerYApplic("controllerY", 5001);

class ControllerY : public RecoveryAbleTask {
public:
    void init() { task[TASK_Y] = this; }
    void performTask() {
        myContext.value++;
        if(activated) PRINTF("%s%7ld\n",    ROTY, myContext.value);
        else          PRINTF("%s-------\n", ROTY);
        if(activated) topicY.publish(myContext);      
    }
} controllerY;

static Subscriber nameNotImportant(topicY, controllerY.lastContext);

