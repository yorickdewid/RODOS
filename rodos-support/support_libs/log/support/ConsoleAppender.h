
#ifndef __CONSOLE_APPENDER_H__
#define __CONSOLE_APPENDER_H__

#include <string>
#include "Appender.h"
#include "Layout.h"
#include "../src/meta/MetaLogger.h"

namespace LOG
{
  class ConsoleAppender : public Appender
  {
    public:

    ConsoleAppender(Layout* layout);
    virtual ~ConsoleAppender();

    protected:

    virtual void doAppend(const std::string& msg);

    private:

    static MetaLogger mlog;
  };
} // namespace log

#endif // __CONSOLE_APPENDER_H__
