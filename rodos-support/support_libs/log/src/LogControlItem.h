
#ifndef __LOG_CONTROL_ITEM_H__
#define __LOG_CONTROL_ITEM_H__

#include <cstddef> // size_t

namespace LOG
{
  enum LogControlItemType
  {
    CONTROL_ITEM_TYPE_EMPTY = 0,
    CONTROL_ITEM_TYPE_SET_LEVEL = 1
  };

  struct LogControlItemHeader
  {
    LogControlItemType controlType;

    inline LogControlItemHeader()
    : controlType(CONTROL_ITEM_TYPE_EMPTY)
    {
    }
  };

  union LogControlItemData
  {
    size_t asLevelValue;
  };

  struct LogControlItem
  {
    LogControlItemHeader header;
    LogControlItemData data;
  };
}

#endif // __LOG_CONTROL_ITEM_H__
