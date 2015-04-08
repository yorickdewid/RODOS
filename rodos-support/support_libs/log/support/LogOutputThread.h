
#ifndef __LOG_OUTPUT_THREAD_H__
#define __LOG_OUTPUT_THREAD_H__

#include <map>
#include <list>
#include <string>
#include "thread.h"
#include "subscriber.h"
#include "fifo.h" // SyncFifo
#include "../src/Singleton.h"
#include "../src/LogItem.h"
#include "../src/meta/MetaLogger.h"

namespace LOG
{
  class LogOutputThread : public Thread, private Subscriber,
    public Singleton<LogOutputThread>
  {
    public:

    LogOutputThread();
    virtual ~LogOutputThread();
    virtual void init();
    virtual void run();

    private:

    static MetaLogger mlog;
    SyncFifo<LogItem, LOG_BUFFER_SIZE_ITEM_RECEIVE> receiveBuffer;
    std::map<std::string, std::map<std::string, std::list<LogItem> > > msgs;
  };
} // namespace log

#endif // __LOG_OUTPUT_THREAD_H__
