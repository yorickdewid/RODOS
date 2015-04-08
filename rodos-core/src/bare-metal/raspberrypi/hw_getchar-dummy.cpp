

#include "rodos.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

char getcharNoWait() { return 0; }
bool isCharReady() { return false; }
char* getsNoWait() { return 0; }
void activateTopicCharInput() { }

#ifndef NO_RODOS_NAMESPACE
}
#endif

