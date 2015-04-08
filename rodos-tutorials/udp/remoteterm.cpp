
#include "rodos.h"

static Application  receiverName("remoteterm");

static UDPIn in(-6001); // negative to allow several to listen the same port
static UDPOut out(-6001); // Negative port means broadcast to this port

class Term : public Thread {
    char userData[2000];
    char *s;
    unsigned long senderIp;
    unsigned short senderPort;
    void run () {
        PRINTF("my node nr = %ld\n", getNodeNumber());
        TIME_LOOP(1*SECONDS, 100*MILLISECONDS) {
            while(int len = in.get(userData, 200, &senderIp)) {
                userData[len] = 0;
                senderPort = (short)senderIp;
                PRINTF("from %lx %s\n", senderIp, userData);
            }
            while((s = getsNoWait() ) != 0 ) {
                int len = strlen(s);
                out.send(s,len);
            }
        } // TIME_LOOP
    } // run
} term;


