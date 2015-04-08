#include "rodos.h"
#include "support_libs.h"
#include "../api/log.h"

#include "../support/LogOutput.h"
#include "../support/PatternLayout.h"
#include "../support/ConsoleAppender.h"
#include "MaskAppender.h"

class LogOutputConfEvent: public TimeEvent {
public:

    ~LogOutputConfEvent() {
        this->TimeEvent::~TimeEvent();
    }

    void init() {
        this->activateAt(1 * SECONDS);
    }

    void handle() {
        LOG::LogOutput& logOutput = LOG::LogOutput::getInstance();
        logOutput.addAppender( new MaskAppender(new LOG::PatternLayout("%m")) );
        logOutput.startSimpleLogControllerThread();
    }

} logOutputConfEvent;
