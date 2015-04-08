#include "rodos.h"
#include "LogOutputThread.h"

#include "debug.h" // PRINTF
#include "../src/logTopics.h"
#include "LogOutput.h"

namespace LOG
{
  LogOutputThread::LogOutputThread()
  : Thread("LogOutputThread", LOG_THREAD_PRIO_OUTPUT),
    Subscriber(logTopic, receiveBuffer, "LogOutputThread")
  {
  }

  LogOutputThread::~LogOutputThread()
  {
    this->Subscriber::~Subscriber();
    this->Thread::~Thread();
  }

  void LogOutputThread::init()
  {
    PRINTF("Receives and outputs log messages");
  }

  void LogOutputThread::run()
  {
    while (true)
    {
      LogItem item;

      // Receive item

      this->mlog.debug << "Reading receive buffer" << mendl;
      if (this->receiveBuffer.syncGet(item) == 0)
      {
        mlog.error << "Could not get log item. "
          "This should never happen! Suspending until someone resumes me."
          << mendl;
        AT(END_OF_TIME);
        continue;
      }
      mlog.debug << "Received " << item << mendl;
//      mlog.debug << item.data.asLong << mendl;

      // convert byte order after network transport

      item.header.itemType = longConvertHost2Net(item.header.itemType);

      switch((LogItemType)item.header.itemType)
      {
        case LOG_ITEM_TYPE_LONG:
          item.data.asLong = longConvertHost2Net(item.data.asLong);
          break;
        case LOG_ITEM_TYPE_POINTER:
          if (sizeof(void*)==4) {
          	item.data.asPointer = (void*)longConvertHost2Net((long)item.data.asPointer);
          } else { // 64bit architecures
  			item.data.asPointer = (void*)longlongConvertHost2Net((long long)item.data.asPointer);
          }
          break;
        case LOG_ITEM_TYPE_CHAR:
          //item.data.asChar = item.data.asChar;
          break;
        case LOG_ITEM_TYPE_TIME:
          item.data.asLongLong = longlongConvertHost2Net(item.data.asLongLong);
          break;
        case LOG_ITEM_TYPE_MSG_START:
  		  if (sizeof(size_t)==4) {
  			  item.data.asLevelValue = (size_t)longConvertHost2Net((long)item.data.asLevelValue);
  		  } else { // 64bit architecures
  			  item.data.asLevelValue = (size_t)longlongConvertHost2Net((long long)item.data.asLevelValue);
  		  }
          break;
        default:
          // We don't care about those
          break;
      }
      // Save it in a list specified by header

      std::string appName = item.header.appName;
      std::map<std::string, std::list<LogItem> >& appMsgs = this->msgs[appName];

      std::string threadName = item.header.threadName;
      std::list<LogItem>& msg = appMsgs[threadName];

      msg.push_back(item);

      // If that msg is complete, give it to LogOutput

      if (item.header.itemType == (int)LOG_ITEM_TYPE_MSG_END)
      {
        mlog.debug << "Got complete log msg: " << msg << mendl;

        mlog.debug << "Giving log msg to LogOutput" << mendl;
        LogOutput::getInstance().processLogMsg(msg);

        mlog.debug << "Clearing log msg" << mendl;
        msg.clear();
      }
    }
  }

  MetaLogger LogOutputThread::mlog("LogOutputThread");
} // namespace log
