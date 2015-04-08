#include "rodos.h"

#include "../api/Logger.h"

#include "LogInputThread.h"
#include "LogControlThread.h"
#include "../api/logApp.h"

namespace LOG
{
  size_t Logger::levelValue = LOG_LEVEL_DEBUG_VALUE;
  MetaLogger Logger::mlog = MetaLogger("Logger");

  static bool initLog()
  {
    LogInputThread::getInstance();
    LogControlThread::getInstance();
    logApp.getID();
    return true;
  }

    const bool linkerDangit_lookAtMe = initLog();
} // namespace log
