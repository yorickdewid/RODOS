

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
* @file listelement.cc
* @date 2008/04/26 17:12
* @author Sergio Montenegro, Lutz Dittrich
*
* Copyright 2008 DLR
*
* Listelement: basic object for operating system objects
*
* @brief linked list management
*/

#include "rodos.h"
#include "hw_specific.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

int32_t ListElement::listElementCounter = 0;
/*
* create an element and add to the beginning of the list (unsorted list)
*/
ListElement::ListElement(List& list, const char* name, void* myOwner) {
    owner = myOwner;
    this->name = name;
    listElementID = listElementCounter++;
    append(list);
}

void ListElement::append(List& list) {
    next = list;
    list = this;
    if(taskRunning) {
        ERROR("Listelement::Constructor after sys initialisation");
        PRINTF("List '%s' was created not static\n", name);
    }
}

/*
*  destructor
*/
ListElement::~ListElement() {
    ERROR("ListElement deleted");
}



#ifndef NO_RODOS_NAMESPACE
}
#endif

