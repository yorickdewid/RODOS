
#ifndef __APPENDER_H__
#define __APPENDER_H__

#include <string>
#include <list>
#include "../src/LogItem.h"
#include "Layout.h"
#include "../src/meta/MetaLogger.h"

namespace LOG
{
  class Appender
  {
    public:

    Appender(const std::string& name, Layout* layout);
    virtual void append(const std::list<LogItem>& msg);
    const std::string& getName();
    virtual ~Appender();

    protected:

    virtual void doAppend(const std::string& msg) = 0;

    private:

    static MetaLogger mlog;
    Layout* layout;
    std::string name;
  };
} // namespace log

#endif // __APPENDER_H__
