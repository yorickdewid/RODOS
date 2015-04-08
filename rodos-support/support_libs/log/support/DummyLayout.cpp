#include "DummyLayout.h"

namespace LOG
{
  DummyLayout::DummyLayout()
  : Layout("DummyLayout")
  {
  }

  std::string DummyLayout::doLayout(const std::list<LogItem>& msg)
  {
    return "";
  }

  DummyLayout::~DummyLayout()
  {
    this->Layout::~Layout();
  }
} // namespace log
