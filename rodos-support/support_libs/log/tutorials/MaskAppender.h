
#ifndef __MASK_APPENDER_H__
#define __MASK_APPENDER_H__

#include <string>
#include "../support/Appender.h"
#include "../support/Layout.h"
#include "../src/meta/MetaLogger.h"


class MaskAppender : public LOG::Appender
{
public:

    MaskAppender(LOG::Layout* layout);
    virtual ~MaskAppender();

protected:

    virtual void doAppend(const std::string& msg);

private:

    static LOG::MetaLogger mlog;
};


#endif // __MASK_APPENDER_H__
