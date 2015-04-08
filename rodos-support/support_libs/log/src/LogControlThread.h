
#ifndef __LOG_CONTROL_THREAD_H__
#define __LOG_CONTROL_THREAD_H__

#include "rodos.h"
#include "subscriber.h"
#include "Singleton.h"
#include "meta/MetaLogger.h"
#include "LogControlItem.h"
#include "LogItem.h"
#include "PreLogItem.h"
#include "logTopics.h"

namespace LOG
{
  class LogControlThread : public Thread, private Subscriber,
    public Singleton<LogControlThread>
  {
    public:

    inline LogControlThread();
    inline virtual ~LogControlThread();
    inline virtual void init();
    virtual void run();

    private:

    static MetaLogger mlog;
    SyncFifo<LogControlItem, LOG_BUFFER_SIZE_CONTROL_ITEM_RECEIVE>
      receiveBuffer;

    inline void mlogDebugStuff();
  }; // class LogInputControlThread


  LogControlThread::LogControlThread()
  : Thread("LogControlThread", LOG_THREAD_PRIO_CONTROL),
    Subscriber(logControlTopic, receiveBuffer, "LogControlThread")
  {
  }

  LogControlThread::~LogControlThread()
  {
    this->Subscriber::~Subscriber();
    this->Thread::~Thread();
  }

  void LogControlThread::init()
  {
    PRINTF("Receives and realizes log controls");
  }

#define MACRO_TO_STR(X) (#X)

  void LogControlThread::mlogDebugStuff()
  {
    this->mlog.debug << "Here comes debug stuff:" << mendl;
    this->mlog.debug << "LOGGING" << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_BUFFER_SIZE_CONTROL_ITEM_RECEIVE)
      << "=" << LOG_BUFFER_SIZE_CONTROL_ITEM_RECEIVE << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_BUFFER_SIZE_INPUT)
      << "=" << LOG_BUFFER_SIZE_INPUT << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_ITEM_MAX_APP_NAME_LENGTH)
      << "=" << LOG_ITEM_MAX_APP_NAME_LENGTH << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_ITEM_MAX_C_STRING_LENGTH)
      << "=" << LOG_ITEM_MAX_C_STRING_LENGTH << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_ITEM_MAX_THREAD_NAME_LENGTH)
      << "=" << LOG_ITEM_MAX_THREAD_NAME_LENGTH << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_LEVEL_DEBUG_VALUE)
      << "=" << LOG_LEVEL_DEBUG_VALUE << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_LEVEL_INFO_VALUE)
      << "=" << LOG_LEVEL_INFO_VALUE << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_LEVEL_WARN_VALUE)
      << "=" << LOG_LEVEL_WARN_VALUE << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_LEVEL_ERROR_VALUE)
      << "=" << LOG_LEVEL_ERROR_VALUE << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_LEVEL_OFF_VALUE)
      << "=" << LOG_LEVEL_OFF_VALUE << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_META_LEVEL_VALUE)
      << "=" << LOG_META_LEVEL_VALUE << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_THREAD_PRIO_INPUT)
      << "=" << LOG_THREAD_PRIO_INPUT << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_THREAD_PRIO_CONTROL)
      << "=" << LOG_THREAD_PRIO_CONTROL << mendl;
#ifdef LOG_OUTPUT
    this->mlog.debug << "LOG_OUTPUT" << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_THREAD_PRIO_OUTPUT)
      << "=" << LOG_THREAD_PRIO_OUTPUT << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_THREAD_PRIO_SIMPLE_CONTROLLER)
      << "=" << LOG_THREAD_PRIO_SIMPLE_CONTROLLER << mendl;
    this->mlog.debug << MACRO_TO_STR(LOG_BUFFER_SIZE_ITEM_RECEIVE)
      << "=" << LOG_BUFFER_SIZE_ITEM_RECEIVE << mendl;
#endif // #ifdef LOGG_OUTPUT
    this->mlog.debug << "sizeof(char)=" << sizeof(char) << mendl;
    this->mlog.debug << "sizeof(short)=" << sizeof(short) << mendl;
    this->mlog.debug << "sizeof(int)=" << sizeof(int) << mendl;
    this->mlog.debug << "sizeof(long)=" << sizeof(long) << mendl;
    this->mlog.debug << "sizeof(long long)=" << sizeof(long long) << mendl;
    this->mlog.debug << "sizeof(PreLogItem)=" << sizeof(PreLogItem) << mendl;
    this->mlog.debug << "sizeof(LogItem)=" << sizeof(LogItem) << mendl;
    this->mlog.debug << "sizeof(LogControlItem)=" << sizeof(LogControlItem)
      << mendl;
  }
} // namespace log

#endif // __LOG_CONTROL_THREAD_H__
