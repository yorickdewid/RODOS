

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
 * @file timepoints.h
 * @date 2008/06/12 9:51
 * @author Sergio Montenegro, Lutz Dittrich
 *
 * Copyright 2008 DLR
 *
 * @brief %Time tagged text (header)
 *
 */

#pragma once

#include "timemodel.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
 * @class TimePoints
 * @brief %Time tagged text
 *
 * TimePoints registers timepoints associated with a free text comment.
 * This is usefull to trace applications and system behavior.
 * @warning this is not thread safe !!!
*/

class TimePoints {
public:
  static const int POINTS = 200; ///< number of points
  int64_t  time[POINTS];
  const char*  text[POINTS];
  int32_t index;  ///< how many entries are written (add counter)

  TimePoints(void);
  void clear(void); ///< Clear all points
  void add(const char *name);
  void printList(void); ///< prints and restart
};


#ifndef NO_RODOS_NAMESPACE
}
#endif

