
#ifndef __LOG_ITEM_H__
#define __LOG_ITEM_H__

#include <cstddef> // size_t
#include "../api/param.h"

namespace LOG
{
  enum LogItemType
  {
    LOG_ITEM_TYPE_EMPTY = 0,
    LOG_ITEM_TYPE_MSG_START = 100, // we need both, start and end
    LOG_ITEM_TYPE_MSG_END = 2,
    LOG_ITEM_TYPE_C_STRING = 3,
    LOG_ITEM_TYPE_CHAR = 4,
    LOG_ITEM_TYPE_LONG = 6,
    LOG_ITEM_TYPE_POINTER = 8,
    LOG_ITEM_TYPE_TIME = 9
  };

  struct LogItemHeader
  {
    char appName[LOG_ITEM_MAX_APP_NAME_LENGTH + 1];
    char threadName[LOG_ITEM_MAX_THREAD_NAME_LENGTH + 1];
    //LogItemType itemType;
    int itemType; // because of network transport

    LogItemHeader()
    : itemType((int)LOG_ITEM_TYPE_EMPTY)
    {
    }
  };

  union LogItemData
  {
    char asCString[LOG_ITEM_MAX_C_STRING_LENGTH + 1];
    long asLong;
    void* asPointer;
    char asChar;
    size_t asLevelValue;
    long long asLongLong;
  };

  struct LogItem
  {
    LogItemHeader header;
    LogItemData data;
  };
} // namespace log

#endif // __LOG_ITEM_H__
