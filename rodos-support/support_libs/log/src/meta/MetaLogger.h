
#ifndef __META_LOGGER_H__
#define __META_LOGGER_H__

#include "MetaLogInputter.h"

namespace LOG
{
  class MetaLogger
  {
    public:

    MetaLogInputter<MetaDebug> debug;
    MetaLogInputter<MetaInfo> info;
    MetaLogInputter<MetaWarn> warn;
    MetaLogInputter<MetaError> error;

    inline MetaLogger(const char* _name);
  };

  MetaLogger::MetaLogger(const char* _name)
  : debug(_name),
    info (_name),
    warn (_name),
    error(_name)
  {
  }
} // namespace log

#endif // __META_LOGGER_H__
