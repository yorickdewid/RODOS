
#ifndef __META_LOG_INPUTTER_H__
#define __META_LOG_INPUTTER_H__

#include <cstddef> // size_t
#include "application.h"
#include "thread.h"
#include "debug.h" // PRINTF
#include "../../api/Endl.h"
#include "../../api/LogLevel.h"
#include "../LogItem.h"
#include "../PreLogItem.h"
#include "../LogControlItem.h"
#include "../consoleMsgOutputSemaphore.h"
#include "MetaEndl.h"
#include "MetaLogLevel.h"

#ifdef LOG_OUTPUT
#include <list>
#include <string>
#endif

namespace LOG
{
  template<class MetaLogLevel>
  class MetaLogInputter
  {
    public:

    inline MetaLogInputter(const char* _loggerName);
    MetaLogInputter& operator<<(const MetaEndl& endl);
    MetaLogInputter& operator<<(const Endl& endl);
    MetaLogInputter& operator<<(const LogLevel* level);
    MetaLogInputter& operator<<(const LogLevel& level);
    MetaLogInputter& operator<<(const Thread* thread);
    MetaLogInputter& operator<<(const Application* app);
    MetaLogInputter& operator<<(size_t val);
    MetaLogInputter& operator<<(int val);
    MetaLogInputter& operator<<(char val);
    MetaLogInputter& operator<<(const char* val);
    MetaLogInputter& operator<<(long val);
    MetaLogInputter& operator<<(void* val);
    MetaLogInputter& operator<<(const PreLogItem& item);
    MetaLogInputter& operator<<(const LogControlItem& item);
    MetaLogInputter& operator<<(const LogItem& item);

#ifdef LOG_OUTPUT
    MetaLogInputter& operator<<(const std::list<LogItem>& val);
    MetaLogInputter& operator<<(const std::string& val);
#endif // #ifdef LOG_OUTPUT

    protected:

    bool prepare();

    private:

    bool beginNewMsg;
    const char* loggerName;
    const char* levelName;
  };

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>::MetaLogInputter(const char* _loggerName)
  : beginNewMsg(true), loggerName(_loggerName), levelName(MetaLogLevel::name)
  {
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    const MetaEndl& endl)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("\n");
    consoleMsgOutputSemaphore.leave();
    this->beginNewMsg = true;
    return *this;
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    const Endl& endl)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("endl");
    return *this;
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    const LogLevel* level)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("LogLevel[name=\"%s\", value=%u]", level->name, level->value);
    return *this;
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    const LogLevel& level)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("LogLevel[name=\"%s\", value=%u]", level.name, level.value);
    return *this;
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    const Thread* thread)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("Thread[name=\"%s\"]", thread->getName());
    return *this;
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    const Application* app)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("Application[name=\"%s\", id=%ld]", app->getName(), app->getID());
    return *this;
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    size_t val)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("%u", val);
    return *this;
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    int val)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("%d", val);
    return *this;
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    char val)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("%c", val);
    return *this;
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    const char* val)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("%s", val);
    return *this;
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    long val)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("%ld", val);
    return *this;
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    void* val)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("0x%x", reinterpret_cast<unsigned int>(val));
    return *this;
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    const PreLogItem& item)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("PreLogItem[app=");
    *this << item.header.app;
    PRINTF(", thread=");
    *this << item.header.thread;
    PRINTF(", itemType=");
    switch (item.header.itemType)
    {
      case LOG_ITEM_TYPE_EMPTY:
      {
        PRINTF("empty");
        break;
      }
      case LOG_ITEM_TYPE_LONG:
      {
        PRINTF("long, data=%ld", item.data.asLong);
        break;
      }
      case LOG_ITEM_TYPE_C_STRING:
      {
        PRINTF("cStr, data=\"%s\"", item.data.asCString);
        break;
      }
      case LOG_ITEM_TYPE_POINTER:
      {
        PRINTF("pointer, data=%x",
          reinterpret_cast<unsigned int>(item.data.asPointer));
        break;
      }
      case LOG_ITEM_TYPE_MSG_START:
      {
        PRINTF("msgStart, data=");
        *this << item.data.asLevel;
        break;
      }
      case LOG_ITEM_TYPE_CHAR:
      {
        PRINTF("%c", item.data.asChar);
        break;
      }
      case LOG_ITEM_TYPE_MSG_END:
      {
        PRINTF("msgEnd");
        break;
      }
      case LOG_ITEM_TYPE_TIME:
      {
        TTime time = item.data.asLongLong;
        TTime minutes = time/MINUTES;
        TTime seconds = time/SECONDS;
        TTime milliseconds = time/MILLISECONDS;
        seconds -= minutes*60;
        milliseconds -= seconds*1000;
        PRINTF("time: %lldm, %llds, %lldms", minutes,
            seconds, milliseconds);
        break;
      }
      default:
      {
        PRINTF("(unknown:%d)", item.header.itemType);
        break;
      }
    }
    PRINTF("]");
    return *this;
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    const LogControlItem& item)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("LogControlItem[controlType=");
    switch (item.header.controlType)
    {
      case CONTROL_ITEM_TYPE_EMPTY:
      {
        PRINTF("empty");
        break;
      }
      case CONTROL_ITEM_TYPE_SET_LEVEL:
      {
        PRINTF("setLevel, data=%u", item.data.asLevelValue);
        break;
      }
      default:
      {
        PRINTF("(%d)", item.header.controlType);
        break;
      }
    }
    PRINTF("]");
    return *this;
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    const LogItem& item)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("LogItem[app=\"%s\", thread=\"%s\", type=",
      item.header.appName, item.header.threadName);
    switch (item.header.itemType)
    {
      case LOG_ITEM_TYPE_EMPTY:
      {
        PRINTF("empty");
        break;
      }
      case LOG_ITEM_TYPE_LONG:
      {
        PRINTF("long, data=%ld", item.data.asLong);
        break;
      }
      case LOG_ITEM_TYPE_C_STRING:
      {
        PRINTF("cStr, data=\"%s\"", item.data.asCString);
        break;
      }
      case LOG_ITEM_TYPE_CHAR:
      {
        PRINTF("char, data=\"%c\"", item.data.asChar);
        break;
      }
      case LOG_ITEM_TYPE_POINTER:
      {
        PRINTF("pointer, data=%x",
          reinterpret_cast<unsigned int>(item.data.asPointer));
        break;
      }
      case LOG_ITEM_TYPE_MSG_START:
      {
        PRINTF("msgStart, data=%u", item.data.asLevelValue);
        break;
      }
      case LOG_ITEM_TYPE_MSG_END:
      {
        PRINTF("msgEnd");
        break;
      }
      case LOG_ITEM_TYPE_TIME:
      {
        TTime time = item.data.asLongLong;
                TTime minutes = time/MINUTES;
                TTime seconds = time/SECONDS;
                TTime milliseconds = time/MILLISECONDS;
                seconds -= minutes*60;
                milliseconds -= seconds*1000;
                PRINTF("time: %lldm, %llds, %lldms", minutes,
                    seconds, milliseconds);
        break;
      }
      default:
      {
        PRINTF("(unknown:%d)", item.header.itemType);
        break;
      }
    }
    PRINTF("]");
    return *this;
  }

#ifdef LOG_OUTPUT

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    const std::list<LogItem>& val)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("LogItems[");
    for (std::list<LogItem>::const_iterator itemIt = val.begin();
      itemIt != val.end();
      ++itemIt)
    {
      const LogItem& item = *itemIt;
      *this << item;
      if (itemIt != --val.end()) // end() is behind last element
      {
        PRINTF(", ");
      }
    }
    PRINTF("]");

    return *this;
  }

  template<class MetaLogLevel>
  MetaLogInputter<MetaLogLevel>& MetaLogInputter<MetaLogLevel>::operator<<(
    const std::string& val)
  {
    if (!this->prepare())
    {
      return *this;
    }
    PRINTF("%s", val.c_str());
    return *this;
  }

#endif // #ifdef LOG_OUTPUT

  template<class MetaLogLevel>
  bool MetaLogInputter<MetaLogLevel>::prepare()
  {
    if (this->beginNewMsg)
    {
      consoleMsgOutputSemaphore.enter();
      const char* threadName = Thread::getCurrentThread()->getName();
      TTime time = NOW();
                      TTime minutes = time/MINUTES;
                      TTime seconds = time/SECONDS;
                      TTime milliseconds = time/MILLISECONDS;
//                      TTime microseconds = time/MICROSECONDS;
//                      TTime nanoseconds = time/NANOSECONDS;
                      seconds %= 60;
                      milliseconds %= 1000;
//                      microseconds %= 1000;
//                      nanoseconds %= 1000;
      PRINTF("%lldm, %llds, %lldms [%s] %s@%s: ", minutes,
          seconds, milliseconds,
          this->levelName, threadName, this->loggerName);
      this->beginNewMsg = false;
    }
    return true;
  }
} // namespace log

#undef LOG_META_LOG_INPUTTER_DISABLE
#define LOG_META_LOG_INPUTTER_DISABLE(level)       \
namespace LOG                                      \
{                                                  \
  template<>                                       \
  class MetaLogInputter<level>                     \
  {                                                \
    public:                                        \
                                                   \
    MetaLogInputter(const char* _name)             \
    {                                              \
    }                                              \
                                                   \
    template<class AnyClass>                       \
    MetaLogInputter& operator<<(AnyClass anyInput) \
    {                                              \
      return *this;                                \
    }                                              \
  };                                               \
} // namespace log

#if LOG_META_LEVEL_VALUE > LOG_LEVEL_DEBUG_VALUE
LOG_META_LOG_INPUTTER_DISABLE(MetaDebug)
#endif

#if LOG_META_LEVEL_VALUE > LOG_LEVEL_INFO_VALUE
LOG_META_LOG_INPUTTER_DISABLE(MetaInfo)
#endif

#if LOG_META_LEVEL_VALUE > LOG_LEVEL_WARN_VALUE
LOG_META_LOG_INPUTTER_DISABLE(MetaWarn)
#endif

#if LOG_META_LEVEL_VALUE > LOG_LEVEL_ERROR_VALUE
LOG_META_LOG_INPUTTER_DISABLE(MetaError)
#endif

#endif // __META_LOG_INPUTTER_H__
