#include "rodos.h"

#include "../api/LogLevel.h"
#include "../api/param.h"

namespace LOG
{
  static const LogLevel theDebugLevel("DEBUG", LOG_LEVEL_DEBUG_VALUE);
  static const LogLevel theInfoLevel("INFO", LOG_LEVEL_INFO_VALUE);
  static const LogLevel theWarnLevel("WARN", LOG_LEVEL_WARN_VALUE);
  static const LogLevel theErrorLevel("ERROR", LOG_LEVEL_ERROR_VALUE);
  static const LogLevel theOffLevel("OFF", LOG_LEVEL_OFF_VALUE);

  extern const LogLevel* const debug = &theDebugLevel;
  extern const LogLevel* const info = &theInfoLevel;
  extern const LogLevel* const warn = &theWarnLevel;
  extern const LogLevel* const error = &theErrorLevel;
  extern const LogLevel* const off = &theOffLevel;
}
