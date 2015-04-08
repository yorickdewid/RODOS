#include "rodos.h"
#include "globals.h"
#include "mask.h"

/******************************/

static Application controllerZApplic("controllerZ", 5001);

class ControllerZ : public RecoveryAbleTask {
public:
    void init() { task[TASK_Z] = this; }
    void performTask() {
        myContext.value++;
        if(activated) PRINTF("%s%7ld\n",    ROTZ, myContext.value);
        else          PRINTF("%s-------\n", ROTZ);
        if(activated) topicZ.publish(myContext);      
    }
} controllerZ;

static Subscriber nameNotImportant(topicZ, controllerZ.lastContext);

