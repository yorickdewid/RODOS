/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief System calls.
 * The system call facility is done in a similar fashion linux does it.
 */

#ifndef _SPARC64_SYSCALL_H
#define _SPARC64_SYSCALL_H

/**
 * @defgroup hwSyscalls Number assignment for system calls.
 *
 * Note that the enumeration must begin with 1. Usertraps start with an
 * offset of 0x100 in the trap table. Zero is used for the syscall handler.
 */
/**@{*/

/* userlevel code looks similar to this:
 * mov        arg0, %o0
 * mov        arg1, %o1
 * mov        arg2, %o2
 * mov        <system call number>, %g1
 * ta         SYS_syscall
 */

/* System call numbers */
#define SYS_syscall             0
#define SYS_write               4
#define SYS_yield               5 /**< manual task switch, Thread::yield */
//#define SYS_exit                1
//#define SYS_wait4               7
//#define SYS_brk                 17
//#define SYS_mmap                71

/**@}*/

#ifndef __ASM__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint64_t _syscall6(uint8_t, uint64_t,uint64_t,uint64_t,
          uint64_t,uint64_t,uint64_t);

#ifdef __cplusplus
}
#endif

#define _syscall0(number) \
        _syscall6(number, 0, 0, 0, 0, 0, 0)

#define _syscall1(number,o0) \
        _syscall6(number,o0, 0, 0, 0 ,0, 0)

#define _syscall2(number,o0,o1) \
        _syscall6(number,o0,o1, 0, 0, 0, 0)

#define _syscall3(number,o0,o1,o2) \
        _syscall6(number,o0,o1,o2, 0, 0, 0)

#define _syscall4(number,o0,o1,o2,o3) \
        _syscall6(number,o0,o1,o2,o3, 0, 0)

#define _syscall5(number,o0,o1,o2,o3,o4) \
        _syscall6(number,o0,o1,o2,o3,o4, 0)

#define write(o0,o1,o2)         _syscall3(SYS_write,(uint64_t)o0,(uint64_t)o1,(uint64_t)o2)
#define yield()                 _syscall0(SYS_yield)

extern void sys_write(int fd, const char *buf, size_t nbytes);
extern void sys_yield();

#endif /* __ASM__ */

#endif /* _SPARC64_SYSCALL_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
