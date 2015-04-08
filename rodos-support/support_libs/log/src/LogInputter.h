
#ifndef __LOG_INPUTTER_H__
#define __LOG_INPUTTER_H__

#include "application.h"
#include "thread.h"
#include "../api/LogLevel.h"
#include "../api/Endl.h"
#include "meta/MetaLogger.h"

namespace LOG
{
  class LogInputter
  {
    public:

    inline LogInputter();
    LogInputter(const Application* const app, const Thread* const thread,
      const LogLevel* level);
    LogInputter& operator<<(const char* cStringInput);
    LogInputter& operator<<(const Endl& endlInput);
    LogInputter& operator<<(short shortInput);
    LogInputter& operator<<(int intInput);
    LogInputter& operator<<(long longInput);
    LogInputter& operator<<(char charInput);
    LogInputter& operator<<(void* pointerInput);

    private:

    static MetaLogger mlog;
    const Application* const app;
    const Thread* const thread;
    const bool activated;

  }; // class LogInputter

  LogInputter::LogInputter()
  : app(NULL), thread(NULL), activated(false)
  {
  }
} // namespace logging

#endif // __LOG_INPUTTER_H__
