
#include "rodos.h"
#include "LogInputThread.h"

#include "LogInputBuffer.h"
#include "logTopics.h"

namespace LOG
{
  void LogInputThread::run()
  {
    PreLogItem preLogItem;
    while(true)
    {
      mlog.debug << "Reading pre log item buffer" << mendl;
      if (logInputBuffer.syncGet(preLogItem) == 0)
      {
        mlog.error << "Could not get log item. "
          "This should never happen! Suspending until someone resumes me."
          << mendl;
        AT(END_OF_TIME);
        continue;
      }

      if (preLogItem.header.itemType == (int)LOG_ITEM_TYPE_C_STRING)
      {
        size_t firstIndex = 0;
        const size_t lastIndex = strlen(preLogItem.data.asCString);
        do
        {
          LogItem logItem;

          this->convertHeader(logItem, preLogItem);
          this->copyCString(logItem.data.asCString,
            preLogItem.data.asCString + firstIndex,
            LOG_ITEM_MAX_C_STRING_LENGTH);

          firstIndex += LOG_ITEM_MAX_C_STRING_LENGTH;

          mlog.debug << "Publishing " << logItem << mendl;
//          this->items[i] = logItem;
//          logTopic.publish(this->items[i]);
          logTopic.publish(logItem);
//          ++i;
          AT(NOW()+10*MILLISECONDS);
//          AT(NOW()+100*MILLISECONDS);
//          AT(NOW()+250*MILLISECONDS);
//          AT(NOW()+500*MILLISECONDS);
//          AT(NOW()+1*SECONDS);
//          AT(NOW()+2*SECONDS);
//          AT(NOW()+4*SECONDS);
//          AT(NOW()+10*SECONDS);
        }
        while(firstIndex <= lastIndex);
      }
      else
      {
        LogItem logItem;

        this->convertNonCStringData(logItem, preLogItem);
        this->convertHeader(logItem, preLogItem); // itemType byte order is changed here --> don't reorder these two method calls!

        mlog.debug << "Publishing " << logItem << mendl;
//        this->items[i] = logItem;
//        logTopic.publish(this->items[i]);
        logTopic.publish(logItem);
//        ++i;
        AT(NOW()+10*MILLISECONDS);
//        AT(NOW()+100*MILLISECONDS);
//        AT(NOW()+250*MILLISECONDS);
//        AT(NOW()+500*MILLISECONDS);
//        AT(NOW()+1*SECONDS);
//        AT(NOW()+2*SECONDS);
//        AT(NOW()+4*SECONDS);
//        AT(NOW()+10*SECONDS);
//        logItem.data.asLong = 999999;
      }
    }
  }

  MetaLogger LogInputThread::mlog("LogInputThread");
} // namespace log
