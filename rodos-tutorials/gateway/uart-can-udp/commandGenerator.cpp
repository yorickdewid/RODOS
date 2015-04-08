#include "rodos.h"
#include "topics.h"

/******************************/

static Application senderName("command-generator", 2016);

UserCmd cmdToSend;

class CommandGenerator : public Thread {
public:
    CommandGenerator() : Thread("CommandGenerator") { }
    void run () {
        char* s;
        PRINTF("Enter Command:");
        TIME_LOOP(0*SECONDS, 100*MILLISECONDS) {
            if((s = getsNoWait())!= 0 ) { 
                int len = strlen(s);
                cmdToSend.cmdLen = len;
                int lenToSend = len + 1 + sizeof(int32_t);
                strcpy(cmdToSend.cmdLine, s);
                PRINTF("Sending %d chars: %s\nEnter Command:", cmdToSend.cmdLen, s);
                command.publishMsgPart(cmdToSend, lenToSend, true);
            }
        }
    }
} commandGenerator;

