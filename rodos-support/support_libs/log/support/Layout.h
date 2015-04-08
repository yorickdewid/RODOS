
#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#include <string>
#include <list>
#include "../src/LogItem.h"
#include "../src/meta/MetaLogger.h"

namespace LOG
{
  class Layout
  {
    public:

    Layout(const std::string& name);
    std::string layout(const std::list<LogItem>& msg);
    virtual std::string doLayout(const std::list<LogItem>& msg) = 0;
    const std::string& getName();
    virtual ~Layout();

    private:

    static MetaLogger mlog;
    std::string name;
  };
} // namespace log

#endif // __LAYOUT_H__
