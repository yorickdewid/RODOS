#include "ConsoleAppender.h"

#include <iostream> // cout, endl
#include "../src/consoleMsgOutputSemaphore.h"

namespace LOG
{
  ConsoleAppender::ConsoleAppender(Layout* layout)
  : Appender("ConsoleAppender", layout)
  {
  }

  ConsoleAppender::~ConsoleAppender()
  {
    this->Appender::~Appender();
  }

  void ConsoleAppender::doAppend(const std::string& msg)
  {
    consoleMsgOutputSemaphore.enter();
    mlog.debug << "Printing msg to console (stdout)" << mendl;
    std::cout << msg << std::endl;
    consoleMsgOutputSemaphore.leave();
  }

  MetaLogger ConsoleAppender::mlog("ConsoleAppender");

} // namespace log
