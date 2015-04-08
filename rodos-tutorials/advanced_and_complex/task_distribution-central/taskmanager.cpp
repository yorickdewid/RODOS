#include "rodos.h"
#include "globals.h"
#include "mask-screen.h"
#include "mask.h"

/******************************/

static Application taskManager("taskManager", TASK_TASKMANAGER);

static CommBuffer<DistributionTable> distributionTable;
static Subscriber nameNotImportant(distributionTableTopic, distributionTable);


/*************************************************/

static Application* taskReferences[NUM_OF_TASKS]; 
static DistributionTable myDistributionTable;

class TaskManagerCore : public Thread {
    NodeState nodeState;

public:
    TaskManagerCore() : Thread("TaskManagerCore") { }

    void run () {

        /*** Init output maks on terminal **/
        INIT_MASK();

	long numOfTasks = 0;
    long nodeNumber = getNodeNumber() % MAX_NUMBER_OF_NODES;
	PRINTF("%s%ld\n", NN, nodeNumber); // \n just to force fflush

	nodeState.nodeNumber = nodeNumber;
	nodeState.nodeSane   = true;

	TIME_LOOP(10*MILLISECONDS, 500*MILLISECONDS) {

	    /** Get and execute the distribution table **/
	    distributionTable.get(myDistributionTable);
	    PRINTF("%s%ld%s%ld%s%ld%s%ld\n",
                  T3, myDistributionTable.nodeToExecute[3],
                  T4, myDistributionTable.nodeToExecute[4],
                  T5, myDistributionTable.nodeToExecute[5],
                  T6, myDistributionTable.nodeToExecute[6]);

	    numOfTasks = 0;
	    for(int i = FIRST_USERTASK; i < NUM_OF_TASKS; i++) {

	        if(myDistributionTable.nodeToExecute[i]  == nodeNumber) {
		    numOfTasks++;
	    	    if(taskReferences[i]) taskReferences[i]->activate();
	        } else {
	    	    if(taskReferences[i]) taskReferences[i]->deactivate();
	        }

	    } // For all tasks: check the distribution table
	    
            /** Report me: nodeState **/
	    nodeState.reportTime = NOW();
	    nodeState.nodeSane   = true;
	    nodeState.cpuLoad    = numOfTasks;
	    nodeStateTopic.publish(nodeState);
	    PRINTF("%s%ld%s%d%s%ld%s%5.3f\n",
			    NR,   nodeState.nodeNumber,
			    S,    nodeState.nodeSane,
			    L,    nodeState.cpuLoad,
			    TIME, (double)nodeState.reportTime / SECONDS);

	} // timeloop: Every second
   } // run
} taskManagerCore;


/******************/

void MAIN() {
    /***** applications discovery **/
    for(int i = 1; i < NUM_OF_TASKS; i++) {
        taskReferences[i] = Application::findApplication(i);
    }

}


