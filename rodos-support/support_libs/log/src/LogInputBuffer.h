
#ifndef __LOG_INPUT_BUFFER_H__
#define __LOG_INPUT_BUFFER_H__

#include "rodos.h"
#include "PreLogItem.h"
#include "meta/MetaLogger.h"
#include "hw_thread_defs.h" // PRIORITY_CEILING

namespace LOG
{
  class LogInputBuffer :
    public SyncFifo<PreLogItem, LOG_BUFFER_SIZE_INPUT>
  {
    public:

    inline LogInputBuffer();
    virtual inline ~LogInputBuffer();

    // These functions should not be inline. However, when I defined them
    // in .cpp as normal, linker said: multiple definitions. Maybe he
    // doesn't like it when the overriden function in super class is
    // inline, but the overriding function in sub class isn't.
    virtual inline bool put(const PreLogItem& item);
    virtual inline int syncGet(PreLogItem& item, const TTime timeout = 0);

    private:

    static MetaLogger mlog;

    // We don't provide this
    virtual inline bool syncPut(PreLogItem& item, const TTime timeout = 0);
    // We don't provide this. However, syncGet uses it, so it has to work
    virtual inline bool get(PreLogItem& item);
  };
  
  LogInputBuffer::LogInputBuffer()
  {
  }

  LogInputBuffer::~LogInputBuffer()
  {
  }

  bool LogInputBuffer::put(const PreLogItem& item)
  {
    bool puttingSuccessful = false;

    mlog.debug << "Putting " << item << " asynch." << mendl;

    PRIORITY_CEILING
    {
      puttingSuccessful =
        this->SyncFifo<PreLogItem, LOG_BUFFER_SIZE_INPUT>::put(item);
    }

    if (!puttingSuccessful)
    {
      mlog.warn << "Buffer full; " << item << " lost" << mendl;
    }

    PRIORITY_CEILING
    {
      if(this->suspendedReader)
      {
        mlog.debug << "Resuming " << this->suspendedReader << mendl;
        this->suspendedReader->resume();
      }
    }
    return puttingSuccessful;
  }

  int LogInputBuffer::syncGet(PreLogItem& item, const TTime timeout)
  {
    mlog.debug << "Getting log item synch., maybe I have to wait"
      << mendl;
    int ret =
      this->SyncFifo<PreLogItem, LOG_BUFFER_SIZE_INPUT>::syncGet(item);
    if (ret == 0)
    {
      mlog.error << "Could not get pre log item. "
        "This should never happen!" << mendl;
    }
    else
    {
      mlog.debug << "Got " << item << mendl;
    }
    return ret;
  }


  // We don't provide this
  bool LogInputBuffer::syncPut(PreLogItem& item, const TTime timeout)
  {
    return false;
  }

  // We don't provide this. However, syncGet uses it, so it has to work
  bool LogInputBuffer::get(PreLogItem& item)
  {
    return this->SyncFifo<PreLogItem, LOG_BUFFER_SIZE_INPUT>::get(item);
  }

  extern LogInputBuffer logInputBuffer;
} // namespace log

#endif // __LOG_INPUT_BUFFER_H__
