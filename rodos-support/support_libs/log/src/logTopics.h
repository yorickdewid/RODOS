
#ifndef __LOG_TOPICS_H__
#define __LOG_TOPICS_H__

#include "rodos.h"
#include "LogItem.h"
#include "LogControlItem.h"

#ifndef NO_RODOS_NAMESPACE
using namespace RODOS;
#endif

namespace LOG
{
  extern Topic<LogItem> logTopic;
  extern Topic<LogControlItem> logControlTopic;
} // namespace log

#endif // __LOG_TOPICS_H__
