
#ifndef __LOG_INPUT_THREAD_H__
#define __LOG_INPUT_THREAD_H__

#include <cstddef> // size_t
#include "thread.h"
#include "debug.h" // PRINTF
#include "string_pico.h" // strncpy
#include "Singleton.h"
#include "LogItem.h"
#include "PreLogItem.h"
#include "meta/MetaLogger.h"
#include "../api/param.h"

#include "bytesex.h"

namespace LOG
{
  class LogInputThread : public Thread, public Singleton<LogInputThread>
  {
    public:

    inline LogInputThread();
    inline virtual void init();
    virtual void run();

    private:

    static MetaLogger mlog;
    bool madeAppOrThreadNameLengthWarning;

    // Hard choice, but I think inline makes sense here although the
    // functions do a little more than, for example, just return a value
    // or something
    inline void convertNonCStringData(LogItem& logItem,
      const PreLogItem& preLogItem);
    inline bool copyCString(char* target, const char* source,
      size_t targetLength);
    inline void convertHeader(LogItem& logItem,
      const PreLogItem& preLogItem);

    size_t i;
    LogItem items[100];

  }; // class LogInputThread

  LogInputThread::LogInputThread()
  : Thread("LogInputThread", LOG_THREAD_PRIO_INPUT),
    madeAppOrThreadNameLengthWarning(false), i(0)
  {
  }

  void LogInputThread::init()
  {
    PRINTF("Collects and publishes log messages");
  }

  void LogInputThread::convertNonCStringData(LogItem& logItem,
    const PreLogItem& preLogItem)
  {// TODO: simplify
    switch((LogItemType)preLogItem.header.itemType)
    {
      case LOG_ITEM_TYPE_LONG:
      {
        logItem.data.asLong = longConvertHost2Net(preLogItem.data.asLong);
        break;
      }
      case LOG_ITEM_TYPE_POINTER:
      {
        if (sizeof(void*)==4) {
        	logItem.data.asPointer = (void*)longConvertHost2Net((long)preLogItem.data.asPointer);
		} else { // 64bit architecures
			logItem.data.asPointer = (void*)longlongConvertHost2Net((long long)preLogItem.data.asPointer);
		}
    	break;
      }
      case LOG_ITEM_TYPE_CHAR:
      {
        logItem.data.asChar = preLogItem.data.asChar;
        break;
      }
      case LOG_ITEM_TYPE_TIME:
      {
        logItem.data.asLongLong = longlongConvertHost2Net(preLogItem.data.asLongLong);
        break;
      }
      case LOG_ITEM_TYPE_MSG_START:
      {
		  if (sizeof(size_t)==4) {
			  logItem.data.asLevelValue = (size_t)longConvertHost2Net((long)preLogItem.data.asLevel->value);
		  } else { // 64bit architecures
			  logItem.data.asLevelValue = (size_t)longlongConvertHost2Net((long long)preLogItem.data.asLevel->value);
		  }
        break;
      }
      default:
      {
        // We don't care about those
        break;
      }
    }
  }

  bool LogInputThread::copyCString(char* target, const char* source,
    size_t maxTargetLength)
  {
    size_t sourceLength = strlen(source);

    // If there's not enough space in target, we can't copy the entire source
    bool sourceTooLong = sourceLength > maxTargetLength;

    // Copy. We want to copy null terminator, too
    strncpy(target, source, maxTargetLength + 1);

    if (sourceTooLong)
    {
      // In this case, the last character that strncpy copied was not the
      // null terminator, but some other character. So let's set null.

      size_t lastTargetIndex = maxTargetLength;
        // Normally maxTargetLength-1, but length doesn't include null
        // terminator. So, technically it's (maxTargetLength+1)-1
      target[lastTargetIndex] = '\0';
    }

    return sourceTooLong;
  }

  void LogInputThread::convertHeader(LogItem& logItem,
    const PreLogItem& preLogItem)
  {
    bool appNameTooLong = this->copyCString(logItem.header.appName,
      preLogItem.header.app->getName(),
      LOG_ITEM_MAX_APP_NAME_LENGTH);
//    mlog.debug << "App name: \"" << logItem.header.appName << "\"" << mendl;
    bool threadNameTooLong = this->copyCString(logItem.header.threadName,
      preLogItem.header.thread->getName(),
      LOG_ITEM_MAX_THREAD_NAME_LENGTH);
//    mlog.debug << "Thread name: \"" << logItem.header.threadName << "\"" << mendl;
    if(!this->madeAppOrThreadNameLengthWarning &&
      (appNameTooLong || threadNameTooLong))
    {
      mlog.warn << (appNameTooLong ? "App" : "Thread") << " name \""
        << (appNameTooLong ?
        preLogItem.header.app->getName() :
        preLogItem.header.thread->getName())
        << "\" is too long and will be truncated. This sort of warning "
        "will only appear once." << mendl;
      this->madeAppOrThreadNameLengthWarning = true;
    }
    logItem.header.itemType = longConvertHost2Net(preLogItem.header.itemType);
  }
} // namespace log

#endif // __LOG_INPUT_THREAD_H__
