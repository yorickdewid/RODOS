
#ifndef __LOG_OUTPUT_H__
#define __LOG_OUTPUT_H__

#include <vector>
#include <map>
#include <cstddef> // size_t
#include "semaphore.h"
#include "../src/Singleton.h"
#include "Appender.h"
#include "../src/LogItem.h"
#include "../src/meta/MetaLogger.h"
#include "../api/LogLevel.h"

namespace LOG
{
  class LogOutput : public Singleton<LogOutput>
  {
    public:

    ~LogOutput();
    const std::vector<Appender*>& getAppenders() const;
    void addAppender(Appender* appender);
    bool removeAppender(Appender* appender);
    void processLogMsg(const std::list<LogItem>& msg);
    void startSimpleLogControllerThread();
    void stopSimpleLogControllerThread();
    void setInputLevel(const LogLevel* const level);
    const LogLevel* const getLevel(size_t levelValue);
    void registerLevel(const LogLevel* const level);

    private:

    friend class Singleton<LogOutput>;

    static MetaLogger mlog;
    std::vector<Appender*> appenders;
    Semaphore msgProcessingSem;
    std::map<size_t, const LogLevel* const> levels;

    LogOutput();
  };

} // namespace log

#endif // __LOG_OUTPUT_H__
