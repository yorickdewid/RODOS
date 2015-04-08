
#include "rodos.h"
#include "topics.h"

extern void hwResetAndReboot();

static SyncFifo<Coordinates,10> newPos;
static Subscriber newPosPuter(toActuator, newPos);


class Checker : public Thread {
    void run () {
        long msgCnt = 0;
        long subCounter = 0;
        int resultIndex = 0;
        Coordinates allResults[10];
        int diffs[10];
        Coordinates oneResult;
        bool shallKill = false;

        while(1) {
            newPos.syncGet(oneResult);
            if(getcharNoWait() == 'k') shallKill = true;

            if(msgCnt != oneResult.msgCnt || subCounter != oneResult.subCounter) {

                /*** Chek all results from last cycle **/
                for(int i = 0; i < resultIndex; i++) {
                    diffs[i] = 0;
                    for(int j = 0; j < resultIndex; j++) {
                        if(allResults[i].x != allResults[j].x) diffs[i]++;
                    }
                }
                PRINTF("Diffs: ");
                for(int i = 0; i < resultIndex; i++) {
                    PRINTF(" %d ", diffs[i]);
                    if(diffs[i] >= 2) {
                        PRINTF(" (Faulty Node %ld) ", allResults[i].myNode);
                        if(shallKill) killNode.publish(allResults[i].myNode);
                    }
                }

                /**** Prepare for next cycle ***/
                msgCnt     = oneResult.msgCnt;
                subCounter = oneResult.subCounter;
                resultIndex = 0;

                if(shallKill) PRINTF("\n-------------------\n");
                else          PRINTF("\n------------------- press k to aktivate the kill funktion\n");

            }
            oneResult.printIt();
            allResults[resultIndex] = oneResult;
            if(resultIndex < 10) resultIndex++;

        }
    }
} checker;

