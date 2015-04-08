

/*********************************************************** Copyright
 **
 ** Copyright (c) 2008, German Aerospace Center (DLR)
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 **
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 **
 ** 3 Neither the name of the German Aerospace Center nor the names of
 **   its contributors may be used to endorse or promote products derived
 **   from this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 ****************************************************************************/


/**
 * @file initiator.h
 * @date 2008/05/01 16:15
 * @author Sergio Montenegro, Lutz Dittrich
 *
 * Copyright 2008 DLR
 *
 * Different events handlers
 *
 * @brief Just an interface class to allow an application to initialize before
 * scheduling.
 */

#ifndef __INITIATOR_H
#define __INITIATOR_H



#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
 * @class Initiator
 *
 * @brief Just an interface class to allow an application to initialize before
 * being scheduled.
 *
 * It provides an init method which shall be overwritten by the applications
 * init() method. The init() method will be invoked before the execution of any
 * thread is started.
 */

class Initiator : public ListElement {
    friend void initSystem();
protected:
    /** default list of all events */
    static List initiatorList;

    /**
     * Called by the iniator only once for each object in the system
     * at start up.
     *
     * Called before scheduling von threads beginns.
     */

    virtual void init(void) { }

public:
    Initiator(const char* name = "Initiator") : ListElement(initiatorList, name) { }
    virtual ~Initiator()  { }

};


#ifndef NO_RODOS_NAMESPACE
}
#endif


#endif
