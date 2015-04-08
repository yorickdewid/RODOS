
#ifndef __CONSOLE_MSG_OUTPUT_SEMAPHORE_H__
#define __CONSOLE_MSG_OUTPUT_SEMAPHORE_H__

#include "semaphore.h"

#ifndef NO_RODOS_NAMESPACE
using namespace RODOS;
#endif

namespace LOG
{
	extern Semaphore consoleMsgOutputSemaphore;
}

#endif // __CONSOLE_MSG_OUTPUT_SEMAPHORE_H__
