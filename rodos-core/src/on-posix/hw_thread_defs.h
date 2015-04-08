

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
* @file hw_defs.h
* @date 2008/04/22 16:54
* @author sergio Montenegro
*
* Copyright 2008 DLR
*
*/

#ifndef __HW_THREAD_DEFS_H__
#define __HW_THREAD_DEFS_H__

namespace RODOS {

/***** Warning, this posix implemenation of priority_ceiling
 * does not work allways, 
 * until now I did not find a possiblity to implement
 * atomarity in posix
 */
extern void globalAtomarLock();
extern void globalAtomarUnlock();

/**
* Priority ceiling is used to avoid thread switches in a critical region.
* This is equivalent to atomarity at thread level.
* This has no effect to interrupts and is no interrupt atomarity
* use eg.
* PRIORITY_CEILEING { a = true; b = false; suspend(); }
* Warning:
* If you suspend (or wait) inside of PRIORITY_CEILING, please only as last operation in
* the block.
*/


#define PRIORITY_CEILING for(bool _onlyOnce = true, globalAtomarLock(); _onlyOnce;  _onlyOnce = false, globalAtomarUnlock())


}
#endif /* __HW_SPECIFIC_H__ */
