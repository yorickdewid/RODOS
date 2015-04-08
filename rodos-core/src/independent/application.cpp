

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
* @file application.cc
* @date 2008/06/04 11:41
* @author Sergio Montenegro, Lutz Dittrich
*
* Copyright 2008 DLR
*
* \brief Collection of information about an application (header)
*
*/

#include "rodos-apis.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/* list of applications registerd */
List Application::applicationList = 0;

/**
* Constructor
*/
Application::Application(const char* name_, const int32_t id) :
                         ListElement(applicationList, name_) {
  applicationId = id;
  isActivated = true;
}


/**
* Destructor
*/
Application::~Application() {
  ERROR("Application deleted");
}

/** more code to come from user */
void Application::activate() {
	isActivated = true;
}

/** more code to come from user */
void Application::deactivate() {
	isActivated = false;
}

int32_t Application::request(void* requestMsg, int len, void* answerMsg, int maxlen) {
	return -1;
}



/**
* Prints all applications names and ids
*/
void Application::printApplications() {
  ITERATE_LIST(Application, applicationList) {
    PRINTF("      %5d -> '%s'\n", (int)iter->applicationId, iter->getName());
  }
}


/**
* returns a reference to the associated application, if not found 0
*/
Application* Application::findApplication(const int32_t id) {
  ITERATE_LIST(Application, applicationList) {
    if (iter->applicationId == id) {
      return iter;
    }
  }
  return 0;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif


