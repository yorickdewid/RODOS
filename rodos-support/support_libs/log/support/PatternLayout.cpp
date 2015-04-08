#include "PatternLayout.h"

#include <sstream> // ostringstream
#include <map>
#include "LogOutput.h"

namespace LOG
{
  PatternLayout::PatternLayout(const std::string& pattern)
  : Layout("PatternLayout")
  {
    this->pattern = pattern;
    mlog.debug << "Created with pattern \""
      << this->pattern << "\"" << mendl;
  }

  std::string PatternLayout::doLayout(const std::list<LogItem>& msg)
  {
    std::ostringstream msgAsString;

    State state = NORMAL;

    mlog.debug << "Going through pattern" << mendl;
    for (std::string::iterator patternCharIt =
      this->pattern.begin(); patternCharIt != this->pattern.end();
      ++patternCharIt)
    {
      char patternChar = *patternCharIt;
      mlog.debug << "Pattern char: \"" << patternChar << "\"" << mendl;
      switch (state)
      {
        case NORMAL:
        {
          if (patternChar == '%')
          {
            state = CONTROL;
          }
          else
          {
            msgAsString << patternChar;
          }
          break;
        }
        case CONTROL:
        {
          switch (patternChar)
          {
            case '%':
            {
              msgAsString << '%';
              break;
            }
            case 'l':
            {
              if (msg.front().header.itemType == (int)LOG_ITEM_TYPE_MSG_START)
              {
                size_t levelValue = msg.front().data.asLevelValue;
                const LogLevel* const level =
                  LogOutput::getInstance().getLevel(levelValue);
                if (level != NULL)
                {
                  msgAsString << level->name;
                }
                else
                {
                  mlog.warn << "Got unknown level value: " << levelValue << mendl;
                  msgAsString << "(unknown:" << levelValue << ")";
                }
              }
              else
              {
                mlog.warn << "First log message item is no msg start item "
                  "-- cannot determine log level value." << mendl;
                msgAsString << "(unknown)";
              }
              break;
            }
            case 'a':
            {
              std::string appName = msg.front().header.appName;
              msgAsString << appName;
              break;
            }
            case 't':
            {
              std::string threadName = msg.front().header.threadName;
              msgAsString << threadName;
              break;
            }
            case 'm':
            {
              for (std::list<LogItem>::const_iterator itemIt = msg.begin();
                itemIt != msg.end();
                ++itemIt)
              {
                LogItem item = *itemIt;
                switch ((LogItemType)item.header.itemType)
                {
                  case LOG_ITEM_TYPE_C_STRING:
                  {
                    msgAsString << item.data.asCString;
                    break;
                  }
                  case LOG_ITEM_TYPE_CHAR:
                  {
                    msgAsString << item.data.asChar;
                    break;
                  }
                  case LOG_ITEM_TYPE_LONG:
                  {
                    msgAsString << item.data.asLong;
                    break;
                  }
                  case LOG_ITEM_TYPE_POINTER:
                  {
                    msgAsString << item.data.asPointer;
                    break;
                  }
                  default:
                  {
                    // Unknown or uninteresting log item type
                    mlog.debug << "Unknown or uninteresting log item "
                      "type: " << item.header.itemType << mendl;
                    break;
                  }
                }
              }
              break;
            }
            default:
            {
              // Unknown control char
              mlog.warn << "Unknown control char: '" << patternChar << "'"
                << mendl;
              break;
            }
          }
          state = NORMAL;
          break;
        }
        default:
        {
          // Unknown state
          mlog.error << "Unknown state: " << state << " - this should "
            "not happen!" << mendl;
          break;
        }
      }
      mlog.debug << "Intermediate result: \"" << msgAsString.str()
        << "\"" << mendl;
    }

    std::string msgAsStringResult = msgAsString.str();
    mlog.debug << "Result: \"" << msgAsStringResult << "\"" << mendl;

    return msgAsStringResult;
  }

  PatternLayout::~PatternLayout()
  {
    this->Layout::~Layout();
  }

  MetaLogger PatternLayout::mlog("PatternLayout");
} // namespace log
