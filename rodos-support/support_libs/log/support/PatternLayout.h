
#ifndef __PATTERN_LAYOUT_H__
#define __PATTERN_LAYOUT_H__

#include <string>
#include <list>
#include "Layout.h"
#include "../src/LogItem.h"
#include "../src/meta/MetaLogger.h"

namespace LOG
{
  class PatternLayout : public Layout
  {
    public:

    PatternLayout(const std::string& pattern);
    virtual std::string doLayout(const std::list<LogItem>& msg);
    virtual ~PatternLayout();

    private:

    static MetaLogger mlog;
    std::string pattern;

    enum State { NORMAL, CONTROL };
  };
} // namespace log

#endif // __PATTERN_LAYOUT_H__
