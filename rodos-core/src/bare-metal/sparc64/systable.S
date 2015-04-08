/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief System call table.
 * In the system_call handler, we subtract 1 from the syscall number. This
 * allows to start the syscall table below with the first system call without
 * special care for the 0th entry.
 * The enumeration of the syscalls needs to be kept in sync with syscall.h.
 */

  .align      64
  .global     sparc64_systable
sparc64_systable:
  .xword      0                 /* 001 */
  .xword      0                 /* 002 */
  .xword      0                 /* 003 */
  .xword      sys_write         /* 004 */
  .xword      sys_yield         /* 005 */
  .xword      0                 /* 006 */
  .xword      0                 /* 007 */
  .xword      0                 /* 008 */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
