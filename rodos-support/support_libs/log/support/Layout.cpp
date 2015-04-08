#include "Layout.h"

namespace LOG
{
  Layout::Layout(const std::string& name)
  {
    this->name = name;
  }

  std::string Layout::layout(const std::list<LogItem>& msg)
  {
    mlog.debug << "Laying out msg" << mendl;
    return this->doLayout(msg);
  }

  const std::string& Layout::getName()
  {
    return this->name;
  }

  Layout::~Layout()
  {
  }

  MetaLogger Layout::mlog("Layout");
} // namespace log
