
#ifndef __DUMMY_LAYOUT_H__
#define __DUMMY_LAYOUT_H__

#include <string>
#include <list>
#include "Layout.h"
#include "../src/LogItem.h"

namespace LOG
{
  class DummyLayout : public Layout
  {
    public:

    DummyLayout();
    virtual std::string doLayout(const std::list<LogItem>& msg);
    virtual ~DummyLayout();
  };
} // namespace log

#endif // __DUMMY_LAYOUT_H__
