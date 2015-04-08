
#include "rodos.h"

#define LOW_STACK_LIMIT 300

/** Checks percent usage of each stack **/

namespace RODOS { // may not be in a nams spcae

class ThreadChecker : public Thread {
public:
    ThreadChecker() : Thread("ThreadChecker") { }

    void init() { xprintf(SCREEN_RED "NOT TO FLY!!!!! checks stack of all threads " SCREEN_RESET); }

    void run() {
        PRINTF(" \n\n\n****************************************** IMPORTANT \n");
        PRINTF(" ****** to use this, set EMPTY_MEMORY_MARKER in thread_on_hw.cpp to 0 and recompile rodos\n");
        PRINTF(" ******************************************\n");
        long minStack = DEFAULT_STACKSIZE;
        Thread* dangerousThread = 0;
        TIME_LOOP(0, 3*SECONDS) {
            PRINTF("TST: Threads and stacks:\n");
            ITERATE_LIST(Thread, Thread::threadList) {
                long index = 0;
                char* stk = iter->stackBegin;
                for(index = 16; index < iter->stackSize; index++) if(stk[index]!= 0) break;
                if(index < minStack) minStack = index;
                if(index < LOW_STACK_LIMIT) dangerousThread = iter;

                PRINTF("	%s  Prio = %7ld Stack = %6ld StackFree = %6ld, (min = %6ld)\n",
                       iter->getName(), iter->priority, iter->stackSize, index, minStack);
            }
            if(minStack < LOW_STACK_LIMIT) {
                PRINTF("TST: Dager! Stack to low\n");
                PRINTF("\nTST: Recommend to suspend '%s'****\n\n", dangerousThread->getName());
            }
        }
    }
} threadChecker;


/**************************************************/
class Innocent1 : public Thread {
public:
    Innocent1() : Thread("Innocent1") { }
    void run() { }
} innocent1;
class Innocent2 : public Thread {
public:
    Innocent2() : Thread("Innocent2") { }
    void run() { }
} innocent2;
class Innocent3 : public Thread {
public:
    Innocent3() : Thread("Innocent3") { }
    void run() { }
} innocent3;



/*************************/


/*********** Otehre Threads to test stack occupation ***/

void stackUser(int len) {
    char variableOnStack[len];
    for(int i = 0; i < len; i++) variableOnStack[i] = 0x5a;
}


/** Consumes more an more stack until it crases *****/

class StackConsumer : public Thread {
public:
    StackConsumer() : Thread("StackConsumer", 400, 2000) { }
    void run() {
        long consumer = 1000;
        TIME_LOOP(6*SECONDS, 1*SECONDS) {
            stackUser(consumer);
            consumer+=100;
            PRINTF("TST: StackConsumer using %ld stack bytes\n", consumer);
        }
    }
} stackConsumer;

} // namespace
