
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <cstddef> // size_t
#include "application.h"
#include "thread.h"
#include "LogLevel.h"
#include "../src/LogInputter.h"
#include "param.h"

namespace LOG
{
  class Logger
  {
    public:

    static inline size_t getLevelValue();
    static inline void setLevelValue(size_t value);

    inline Logger(const Application* _app);

    inline LogInputter log(const LogLevel* const level) const;

    private:

    static size_t levelValue;
    static MetaLogger mlog;

    const Application* app;

  }; // class Logger

  size_t Logger::getLevelValue()
  {
    return Logger::levelValue;
  }

  void Logger::setLevelValue(size_t value)
  {
    mlog.info << "Setting level value from " << Logger::levelValue
      << " to " << value << mendl;
    Logger::levelValue = value;
  }

  Logger::Logger(const Application* _app)
  : app(_app)
  {
  }

  LogInputter Logger::log(const LogLevel* const level) const
  {
    if (level->value >= Logger::levelValue)
    {
      mlog.debug << level->value << ">=" << Logger::levelValue
        << " - creating activated log inputter" << mendl;
      return LogInputter(this->app, Thread::getCurrentThread(), level);
    }
    else
    {
      mlog.debug << level->value << "<" << Logger::levelValue
        << " - creating deactivated log inputter" << mendl;
      return LogInputter();
    }
  }
} // namespace log

#endif // __LOGGER_H__
