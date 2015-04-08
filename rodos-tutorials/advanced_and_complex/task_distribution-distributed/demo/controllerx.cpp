#include "rodos.h"
#include "globals.h"
#include "mask.h"

/******************************/

static Application controllerXApplic("controllerX", 5001);

class ControllerX : public RecoveryAbleTask {
public:
    void init() { task[TASK_X] = this; }
    void performTask() {
        myContext.value++;
        if(activated) PRINTF("%s%7ld\n",    ROTX, myContext.value);
        else          PRINTF("%s-------\n", ROTX);
        if(activated) topicX.publish(myContext);      
    }
} controllerX;

static Subscriber nameNotImportant(topicX, controllerX.lastContext);

