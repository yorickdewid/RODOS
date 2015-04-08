#include "MaskAppender.h"
#include "../src/consoleMsgOutputSemaphore.h"
#include "mask.h"
#include "yprintf.h"

using namespace LOG;

MaskAppender::MaskAppender(Layout* layout)
: Appender("MaskAppender", layout)
{
    INIT_MASK(); // Init console mask
}

MaskAppender::~MaskAppender()
{
    this->Appender::~Appender();
}

void MaskAppender::doAppend(const std::string& msg)
{
    // Split the message in text and value with ':' as the split position
    int pos = msg.find(":",0);
    std::string msg_text = msg.substr(0, (pos-1) );
    std::string msg_value = msg.substr( (pos+1), msg.length());

    /**
     * Distinguish between the different messages and print them to the
     * right position
     */
    if(msg_text.find("Time", 0) != std::string::npos)
    {
        consoleMsgOutputSemaphore.enter();
        mlog.debug << "Printing msg to mask" << mendl;
        PRINTF(TIME "%s", msg_value.c_str());
        consoleMsgOutputSemaphore.leave();
    }
    else if(msg_text.find("Voltage", 0) != std::string::npos)
    {
        consoleMsgOutputSemaphore.enter();
        mlog.debug << "Printing msg to mask" << mendl;
        PRINTF(BATV "%s", msg_value.c_str());
        consoleMsgOutputSemaphore.leave();
    }
    else if(msg_text.find("Current", 0) != std::string::npos)
    {
        consoleMsgOutputSemaphore.enter();
        mlog.debug << "Printing msg to mask" << mendl;
        PRINTF(BATC "%s", msg_value.c_str());
        consoleMsgOutputSemaphore.leave();
    }
    else
    {
        mlog.error << "Unknown message type" << mendl;
        xprintf("Error: Unknown message type");
    }
    PRINTF(START); // Set cursor position to (1/1)
}

MetaLogger MaskAppender::mlog("MaskAppender");
