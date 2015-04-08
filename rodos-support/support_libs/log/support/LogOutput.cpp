#include "rodos.h"

#include "LogOutput.h"

#include "thread.h"
#include "LogOutputThread.h"
#include "SimpleLogControllerThread.h"
#include "../src/LogControlItem.h"
#include "../src/logTopics.h"
#include "logOutputApp.h"

namespace LOG
{
  LogOutput::LogOutput()
  : Singleton<LogOutput>()
  {
    this->levels.insert(
      std::pair<size_t, const LogLevel* const>(debug->value, debug));
    this->levels.insert(
      std::pair<size_t, const LogLevel* const>(info->value, info));
    this->levels.insert(
      std::pair<size_t, const LogLevel* const>(warn->value, warn));
    this->levels.insert(
      std::pair<size_t, const LogLevel* const>(error->value, error));
    this->levels.insert(
      std::pair<size_t, const LogLevel* const>(off->value, off));
  }

  const std::vector<Appender*>& LogOutput::getAppenders() const
  {
    return this->appenders;
  }

  void LogOutput::addAppender(Appender* appender)
  {
    mlog.debug << "Adding appender "
      << (appender == NULL ? "NULL" : appender->getName()) << mendl;
    PROTECT_WITH_SEMAPHORE(this->msgProcessingSem)
    {
      this->appenders.push_back(appender);
    }
  }

  bool LogOutput::removeAppender(Appender* appender)
  {
    mlog.debug << "Removing appender "
      << (appender == NULL ? "NULL" : appender->getName()) << mendl;

    PROTECT_WITH_SEMAPHORE(this->msgProcessingSem)
    {
      for (std::vector<Appender*>::iterator appenderIt =
        this->appenders.begin(); appenderIt != this->appenders.end();
        ++appenderIt)
      {
        if ((*appenderIt) == appender)
        {
          mlog.debug << "Done" << mendl;
          this->appenders.erase(appenderIt);
          return true;
        }
      }
      mlog.debug << "Removing failed" << mendl;
    }
    return false;
  }

  void LogOutput::processLogMsg(const std::list<LogItem>& msg)
  {
    if (Thread::getCurrentThread() != &LogOutputThread::getInstance())
    {
      mlog.warn << "Got request to process a log msg from "
        "a thread which is not LogOutputThread. This is not allowed"
        " - skipping" << mendl;
      return;
    }

    PROTECT_WITH_SEMAPHORE(this->msgProcessingSem)
    {
      for (std::vector<Appender*>::iterator appenderIt =
        this->appenders.begin(); appenderIt != this->appenders.end();
        ++appenderIt)
      {
        Appender* appender = *appenderIt;
        mlog.debug << "Giving msg to " << appender->getName() << mendl;
        appender->append(msg);
      }
    }
    mlog.debug << "Msg processed" << mendl;
  }

  LogOutput::~LogOutput()
  {
    for (std::vector<Appender*>::iterator appenderIt =
      this->appenders.begin(); appenderIt != this->appenders.end();
      ++appenderIt)
    {
      delete (*appenderIt);
    }
  }

  void LogOutput::startSimpleLogControllerThread()
  {
    SimpleLogControllerThread::getInstance().setRunning(true);
  }

  void LogOutput::stopSimpleLogControllerThread()
  {
    SimpleLogControllerThread::getInstance().setRunning(false);
  }

  void LogOutput::setInputLevel(const LogLevel* const level)
  {
    mlog.info << "Setting input level to " << level << mendl;
    LogControlItem control;
    control.header.controlType = CONTROL_ITEM_TYPE_SET_LEVEL;
    control.data.asLevelValue = level->value;
    mlog.info << "Publishing " << control << mendl;
    logControlTopic.publish(control);
  }

  const LogLevel* const LogOutput::getLevel(size_t levelValue)
  {
    if (this->levels.find(levelValue) != this->levels.end())
    {
      return this->levels[levelValue];
    }
    else
    {
      return NULL;
    }
  }

  void LogOutput::registerLevel(const LogLevel* const level)
  {
    if (level != NULL)
    {
      mlog.info << "Registering " << level << mendl;
      this->levels.insert(
        std::pair<size_t, const LogLevel* const>(level->value, level));
    }
  }

  MetaLogger LogOutput::mlog("LogOutput");

  static bool initLogOuput()
  {
    LogOutputThread::getInstance();
    SimpleLogControllerThread::getInstance();
    logOutputApp.getID();
    return true;
  }

  const bool linkerDangit_lookAtMe = initLogOuput();
} // namespace log
