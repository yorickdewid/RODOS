
#include "rodos.h"
#include "globals.h"


Semaphore loadProtector;
int minLoadGlobal = 0;
int myLoadGlobal = 0;

Topic <Context> topicX(-1, "topicX");
Topic <Context> topicY(-1, "topicY");
Topic <Context> topicZ(-1, "topicZ");
Topic <NodeState> loadReport(-1, "loadReport");

/********* basis Task for all recovery able tasks ***/

RecoveryAbleTask *task[NR_OF_TASKS];

void RecoveryAbleTask::run() {
    myContext.nodeNr = MY_NODE_NR();
    int missingMsgs = 0;

    TIME_LOOP(1*SECONDS, 500*MILLISECONDS) {
        if(lastContext.getOnlyIfNewData(externContext)) { // is some one else working?
            missingMsgs = 0;
            myContext.value = externContext.value;        // take the official version 
            if(externContext.nodeNr < myContext.nodeNr) { // let the other work
                activated = false;
            }
        } else {                                          // no one else working, then I do!
            missingMsgs++;
            loadProtector.enter();
            {
                if(missingMsgs > 3 && myLoadGlobal == minLoadGlobal) {
                   activated = true;
                   myLoadGlobal++;
                }
            }
            loadProtector.leave();
        }
        loadProtector.enter();
        {
            if(activated && minLoadGlobal <= myLoadGlobal - 2) {
               activated = false;
               myLoadGlobal--;
            }
        }
        loadProtector.leave(); 
        performTask();                                    // In any case, do the job and be ready
    } // time loop
}


/************************** The gateway *******************/

static UDPInOut udp(-50000);
static LinkinterfaceUDP linkinterfaceUDP(&udp);
static Gateway gateway1(&linkinterfaceUDP, true);


