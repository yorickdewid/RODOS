

/**
* @file interruptservers.cc
* @date 2011/12/22 17:08
* @author Sergio Montenegro
*
*/
#include "rodos.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/*********************************************/
void sigtermHandler(int sig)   { interruptSigterm.publishFromInterrupt(0); }
void uartHandler(int sig)      { interruptUart.publishFromInterrupt(0); }
void timerHandler(int sig)     { interruptTimer.publishFromInterrupt(0); }



#ifndef NO_RODOS_NAMESPACE
}
#endif


