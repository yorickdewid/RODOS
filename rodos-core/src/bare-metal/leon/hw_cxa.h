/* -*- Mode: C++ -*- */

/*********************************************************** Copyright
 **
 ** Copyright (c) 2008-2010, German Aerospace Center (DLR)
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
* @file
* @date 2010/08/02
* @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
*
* Copyright 2010 DLR
*
* @brief Implementation of auxiliary definitions that gcc automatically calls.
*/

extern "C" {
  /**
   * The dso_handle that gcc generates references to. */
  extern void * __dso_handle;

  /**
   * atexit handler (for global dtors) */
  extern int __cxa_atexit(void (*)(void *), void *, void *);

  /**
   * Pure virtual handler: this causes a thread panic. */
  extern int __cxa_pure_virtual() __attribute__((__noreturn__));

  /**
   * GCCs thread-safe local static initialisation.
   *
   * This function will try to acquire a lock or a guard
   * and return 1 if it succeeded to get the lock or 0
   * if no initialisation is necessary, because it's
   * already done.  If the guard's lock is taken by
   * some other thread, this function will yield()
   * and try again.
   *
   * Caution:
   *
   *   You cannot share a local static initialisation in
   *   the kernel with one of the threads, because it may
   *   lead to a dead-lock, because the kernel cannot be
   *   preempted, and it cannot yield!  This occurs in
   *   the following situation:
   *
   *   * thread tries to access data, acquires guard.
   *   * thread is preempted while initialising guarded data
   *   * kernel tries to access same data, tries to acquire same
   *     guard, which is marked pending, so it waits,
   *     but no task-switch can happen for the thread to
   *     complete its initialisation
   *     => kernel dead-lock
   *
   * This situation will lead to a watchdog time-out.
   */
  extern int __cxa_guard_acquire (unsigned *guard);

  /**
   * Thread-safe static initialisation: mark successful initialisation.
   *
   * Next __cxa_guard_acquire() will simply return 0 without
   * the need to wait.
   */
  extern void __cxa_guard_release (unsigned *guard);

  /**
   * Thread-safe static initialisation: mark failed initialisation.
   *
   * Next __cxa_guard_acquire() will try to lock again. */
  extern void __cxa_guard_abort (unsigned *guard);
}
