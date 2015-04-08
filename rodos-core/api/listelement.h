

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
* @file listelement.h
* @date 2008/04/28 16:13
* @author Sergio Montenegro, Lutz Dittrich
*
* Copyright 2008 DLR
*
* @brief linked list management (header)
*
*/

#pragma once

#include <stdint.h>

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/**
* @def ITERATE_LIST(__type,__list)
* @param[in] __type Base type of list nodes.
* @param[in] __list The list.
* a handy macro: for_each_do
*/
#define ITERATE_LIST(__type,__list) \
                                for (__type* iter = (__type*)__list; iter!=0;\
                                            iter = (__type*)(iter->getNext()))


class ListElement;

/** A list is just the pointer to the first element */
typedef ListElement* List;


/**
* @class ListElement
* @brief node of a single linked list
*
* A node of a single linked list.
* @warning Not thread safe,
* only for static elements which will never be deleted, eg. never on stacks
*/
class ListElement {

private:
  /** pointer to next element */
  ListElement* next;
  static int32_t listElementCounter;

public:

  /** name of the element */
  const char* name;
  int32_t listElementID; ///< set from listElementCounter
  void* owner; ///< free for user, rodos does not use it

  /** Creates an element and adds it to the beginning of the list.
  * @param[in,out] list The list the element shall be added to.
  * @param[in] name Clear text name.
  */
  ListElement(List &list, const char* name = "ListElement", void* myOwner = 0);

  /** destructor */
  ~ListElement();

  void append(List& list);

  /**  Get next element in list.
  *  @return pointer to next element in the list
  */
  ListElement* getNext() const {
  	return next;
  }

  /**
  * Returns the name of the object as string.
  * @return pointer to name of the object
  */
  const char* getName() const {
    return name;
  }

};


#ifndef NO_RODOS_NAMESPACE
}
#endif


