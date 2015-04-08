
#include "rodos.h"
#include "LogControlThread.h"

#include "../api/Logger.h"
#include "LogItem.h"

namespace LOG
{
  void LogControlThread::run()
  {
    this->mlogDebugStuff();

    while (true)
    {
      LogControlItem controlItem;
      mlog.debug << "Reading/waiting for next control item" << mendl;
      this->receiveBuffer.syncGet(controlItem);
      mlog.info << "Got " << controlItem << mendl;

      switch (controlItem.header.controlType)
      {
        case CONTROL_ITEM_TYPE_SET_LEVEL:
        {
          size_t levelValue = controlItem.data.asLevelValue;
          Logger::setLevelValue(levelValue);
          break;
        }
        default:
        {
          mlog.warn << "Don't know what to do with " << controlItem << mendl;
          break;
        }
      }
    }
  }

  MetaLogger LogControlThread::mlog("LogControlThread");

} // namespace log
