#include "rodos.h"

#include "logTopics.h"

namespace LOG
{
  Topic<LogItem> logTopic(42, "log");
  Topic<LogControlItem> logControlTopic(-1, "logControl");
} // namespace log
