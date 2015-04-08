

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
* @file misc.cc
* @date 2008/04/22 17:08
* @author Lutz Dittrich, Sergio Montenegro
*
* Copyright 2008 DLR
*
* @brief small support functions
*
*/
#include "hw_specific.h"
#include "rodos.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

char xmallocBuf[XMALLOC_SIZE] = {0,0,0,0} ;

void* xmalloc(long len) {
   static long index = 0;

   if(len < 0) {
     return 0;
   }
   len = (len+7) & ~0x7; // round to be 32 bit align (4 bytes) TBD 64 Byte as needed by Thread::stack?

   if(len > XMALLOC_SIZE) {
      ERROR("xmalloc out of mem");
      return 0;
   }

   if(taskRunning) {
      ERROR("Xmalloc after system init completation");
   }
   if(index + len >= XMALLOC_SIZE)  {
      ERROR("xmalloc out of mem");
      return 0;
   }
   void *allocated =  &xmallocBuf[index];
   index += len;
   return allocated;
}



/******************************************************/

void nop( ... ) { }


/****************************************************/



#ifndef NO_RODOS_NAMESPACE
}
#endif


