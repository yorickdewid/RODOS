#include "Appender.h"

namespace LOG
{
  Appender::Appender(const std::string& name, Layout* layout)
  {
    this->layout = layout;
    this->name = name;
  }

  void Appender::append(const std::list<LogItem>& msg)
  {
    mlog.debug << "Giving msg to " << this->layout->getName() << mendl;
    std::string msgAsString = this->layout->layout(msg);
    mlog.debug << "Appending msg" << mendl;
    this->doAppend(msgAsString);
  }

  const std::string& Appender::getName()
  {
    return this->name;
  }

  Appender::~Appender()
  {
    if (this->layout != NULL)
    {
      delete this->layout;
    }
  }

  MetaLogger Appender::mlog("Appender");

} // namespace log
