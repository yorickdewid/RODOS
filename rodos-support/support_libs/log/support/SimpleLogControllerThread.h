
#ifndef __SIMPLE_LOG_CONTROLLER_THREAD_H__
#define __SIMPLE_LOG_CONTROLLER_THREAD_H__

#include "thread.h"
#include "../src/Singleton.h"
#include "../src/meta/MetaLogger.h"

namespace LOG
{
  class SimpleLogControllerThread : public Thread,
    public Singleton<SimpleLogControllerThread>
  {
    public:

    SimpleLogControllerThread();
    virtual ~SimpleLogControllerThread();
    virtual void init();
    virtual void run();
    bool isRunning() const;
    void setRunning(bool activated);

    private:

    friend class Singleton<SimpleLogControllerThread>;

    static MetaLogger mlog;
    bool running;
  };
} // namespace log

#endif // __SIMPLE_LOG_CONTROLLER_THREAD_H__
