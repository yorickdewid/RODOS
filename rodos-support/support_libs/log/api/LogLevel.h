
#ifndef __LOG_LEVEL_H__
#define __LOG_LEVEL_H__

#include <cstddef> // size_t

namespace LOG
{
  struct LogLevel
  {
    const char* name;
    const size_t value;

    inline LogLevel(const char* _name, size_t _value)
    : name(_name), value(_value)
    {
    }
  };

  extern const LogLevel* const debug;
  extern const LogLevel* const info;
  extern const LogLevel* const warn;
  extern const LogLevel* const error;
  extern const LogLevel* const off;
}

#endif // __LOG_LEVEL_H__
