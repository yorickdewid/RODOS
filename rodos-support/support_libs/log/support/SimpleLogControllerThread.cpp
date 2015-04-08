#include "SimpleLogControllerThread.h"

#include <iostream> // cin
#include "debug.h"
#include "../api/LogLevel.h"
#include "LogOutput.h"

namespace LOG
{
  SimpleLogControllerThread::SimpleLogControllerThread()
  : Thread("SimpleLogControllerThread", LOG_THREAD_PRIO_SIMPLE_CONTROLLER),
    running(false)
  {
  }

  SimpleLogControllerThread::~SimpleLogControllerThread()
  {
  }

  void SimpleLogControllerThread::init()
  {
    std::cin.tie(NULL);
      // Otherwise, cout is flushed every time cin reads something. And that
      // would interfere with log output
    PRINTF("Provides simple console interface to publish log controls");
  }

  void SimpleLogControllerThread::run()
  {
    while (true)
    {
      if (!this->running)
      {
        mlog.debug << "Suspending myself" << mendl;
        AT(END_OF_TIME);
      }

      mlog.info << "Awaiting console input" << mendl;
      char ch = '\0';
      std::cin >> ch;
      mlog.debug << "Got char \"" << ch << "\"" << mendl;
      if (!this->running)
      {
        mlog.debug << "Suspending myself" << mendl;
        AT(END_OF_TIME);
        continue;
      }

      const LogLevel* level = NULL;
      switch (ch)
      {
        case 'd':
        case 'D':
        {
          level = debug;
          break;
        }
        case 'i':
        case 'I':
        {
          level = info;
          break;
        }
        case 'w':
        case 'W':
        {
          level = warn;
          break;
        }
        case 'e':
        case 'E':
        {
          level = error;
          break;
        }
        case 'o':
        case 'O':
        {
          level = off;
          break;
        }
        default:
        {
          break;
        }
      }
      if (!this->running)
      {
        mlog.debug << "Suspending myself" << mendl;
        AT(END_OF_TIME);
        continue;
      }

      if (level != NULL)
      {
        LogOutput::getInstance().setInputLevel(level);
      }
    }
  }

  bool SimpleLogControllerThread::isRunning() const
  {
    return this->running;
  }

  void SimpleLogControllerThread::setRunning(bool running)
  {
    bool wasRunningBefore = this->running;
    this->running = running;
    if (!wasRunningBefore && running)
    {
      mlog.info << "Starting" << mendl;
      this->resume();
    }
    else if (wasRunningBefore && !running)
    {
      mlog.info << "Stopping" << mendl;
    }
  }

  MetaLogger SimpleLogControllerThread::mlog("SimpleLogControllerThread");
} // namespace log
