
#ifndef __PRE_LOG_ITEM_H__
#define __PRE_LOG_ITEM_H__

#include "application.h"
#include "thread.h"
#include "LogItem.h"
#include "../api/LogLevel.h"

#ifndef NO_RODOS_NAMESPACE
using namespace RODOS;
#endif

namespace LOG
{
  struct PreLogItemHeader
  {
    const Application* app;
    const Thread* thread;
    //LogItemType itemType;
    int itemType; // because of network transport

    inline PreLogItemHeader()
    : itemType((int)LOG_ITEM_TYPE_EMPTY)
    {
    }

    inline PreLogItemHeader(const Application* _app,
      const Thread* _thread, int _itemType)
    : app(_app), thread(_thread), itemType(_itemType)
    {
    }
  };

  union PreLogItemData
  {
    const char* asCString;
    long asLong;
    void* asPointer;
    const LogLevel* asLevel;
    char asChar;
    long long asLongLong;
  };

  struct PreLogItem
  {
    PreLogItemHeader header;
    PreLogItemData data;

    inline PreLogItem()
    {
    }

    inline PreLogItem(const Application* app,
      const Thread* thread, LogItemType itemType)
    : header(app, thread, itemType)
    {
    }
  };
}

#endif // __PRE_LOG_ITEM_H__
