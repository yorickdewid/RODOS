#include "rodos.h"

#include "LogInputter.h"
#include "LogInputBuffer.h"

namespace LOG
{
  LogInputter::LogInputter(const Application* const _app,
    const Thread* const _thread, const LogLevel* level)
  : app(_app), thread(_thread), activated(true)
  {
    mlog.debug << "Being created for " << this->app
      << ", " << this->thread << " and " << level << mendl;

    PreLogItem preLogItem(this->app, this->thread,
          LOG_ITEM_TYPE_MSG_START);
    preLogItem.data.asLevel = level;
    logInputBuffer.put(preLogItem);

    preLogItem.header.itemType = (int)LOG_ITEM_TYPE_TIME;
    preLogItem.data.asLongLong = NOW();
    logInputBuffer.put(preLogItem);
  }


  LogInputter& LogInputter::operator<<(const char* cStringInput)
  {
    mlog.debug << "Got C string: \"" << cStringInput << "\"" << mendl;
    if (this->activated)
    {
      PreLogItem logInputItem(this->app, this->thread,
        LOG_ITEM_TYPE_C_STRING);
      logInputItem.data.asCString = cStringInput;
      logInputBuffer.put(logInputItem);
    }
    return *this;
  }

  LogInputter& LogInputter::operator<<(const Endl& endlInput)
  {
    mlog.debug << "Got endl" << mendl;
    if (this->activated)
    {
      PreLogItem logInputItem(this->app, this->thread,
        LOG_ITEM_TYPE_MSG_END);
      logInputBuffer.put(logInputItem);
    }
    return *this;
  }

  LogInputter& LogInputter::operator<<(short shortInput)
  {
    mlog.debug << "Got short input: " << shortInput << mendl;
    if (this->activated)
    {
      PreLogItem logInputItem(this->app, this->thread,
        LOG_ITEM_TYPE_LONG);
      logInputItem.data.asLong = shortInput;
      logInputBuffer.put(logInputItem);
    }
    return *this;
  }

  LogInputter& LogInputter::operator<<(int intInput)
  {
    mlog.debug << "Got int input: " << intInput << mendl;
    if (this->activated)
    {
      PreLogItem logInputItem(this->app, this->thread,
        LOG_ITEM_TYPE_LONG);
      logInputItem.data.asLong = intInput;
      logInputBuffer.put(logInputItem);
//      logInputItem.data.asLong = 1;
//      logInputBuffer.put(logInputItem);
//      logInputItem.data.asLong = 2;
//      logInputBuffer.put(logInputItem);
//      logInputItem.data.asLong = 3;
//      logInputBuffer.put(logInputItem);
    }
    return *this;
  }

  LogInputter& LogInputter::operator<<(long longInput)
  {
    mlog.debug << "Got long input: " << longInput << mendl;
    if (this->activated)
    {
      PreLogItem logInputItem(this->app, this->thread,
        LOG_ITEM_TYPE_LONG);
      logInputItem.data.asLong = longInput;
      logInputBuffer.put(logInputItem);
    }
    return *this;
  }

  LogInputter& LogInputter::operator<<(char charInput)
  {
    mlog.debug << "Got char input: \"" << charInput << "\"" << mendl;
    if (this->activated)
    {
      PreLogItem logInputItem(this->app, this->thread,
        LOG_ITEM_TYPE_CHAR);
      logInputItem.data.asChar = charInput;
      logInputBuffer.put(logInputItem);
    }
    return *this;
  }

  LogInputter& LogInputter::operator<<(void* pointerInput)
  {
    mlog.debug << "Got pointer input: " << pointerInput << mendl;
    if (this->activated)
    {
      PreLogItem logInputItem(this->app, this->thread,
        LOG_ITEM_TYPE_POINTER);
      logInputItem.data.asPointer = pointerInput;
      logInputBuffer.put(logInputItem);
    }
    return *this;
  }

  MetaLogger LogInputter::mlog("LogInputter");
} // namespace log
