

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
* @file application.h
* @date 2008/06/04 11:46
* @author Sergio Montenegro, Lutz Dittrich
*
* Copyright 2008 DLR
*
* @brief collection of information about an application (header)
*
*/

#pragma once

#include "listelement.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
* @class Application
* @brief collection of information about an application.
*
* Registers applications names/versions to be printed and
* to be searched for (discovery).
* Each application gets a name which will be printed at system start up.
* Each application has an optional applicationID number which can be used by
* other applications to find it and to determine if it was loaded/linked or
* not.
*/
class Application: public ListElement {
    friend void initSystem();

private:
    /** List of all registered applications.
    */
    static List applicationList;

    /// free usable to identify and find loaded modules by number/id
    int32_t applicationId;

public:
    /** Constructor; Creates and registeres an application with given name
      * and id.
      * @param[in] name The name of the application.
      * @param[in] id The application id.
      */
    Application(const char* name, const int32_t id=1000);

    /** Destructor; Should never be called.
      */
    virtual ~Application();

    int32_t getID() const { return applicationId; }

    /** Prints all registered applications and corresponding IDs.
        (do not call before main())
    */
    static void printApplications();

    /** If the list contains an application with the given id then this
    *   functions returns a pointer to this application. If there is more
    *   than one application with that id, this function returns a pointer
    *   to the first one it encounters. In any other case
    *   this methode returns null.  (do not call before main())
    *   @param[in] id The id to search for.
    *   @return pointer to application with given id or null if not found.
    */
    static Application* findApplication(const int32_t id);

    /// to signal activate / deactivate applications
    bool isActivated;

    virtual void init(void) { } ///< calles by rodos::main at system start

    virtual void activate(); ///< user implemented to activate the application
    virtual void deactivate(); ///< user implemented to deactivate the application

    /** To send requests to applications, The application may answer.
      * @return number of bytes written in answerMsg, <0 -> error, -1 -> not implemented
      */
    virtual int32_t request(void* requestMsg, int len, void* answerMsg, int maxLen);

};


#ifndef NO_RODOS_NAMESPACE
}
#endif

