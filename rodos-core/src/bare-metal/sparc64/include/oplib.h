/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief The OpenBoot PROM client interface.
 *
 * This file contains routines concerning the Sun IEEE 1275 PROM low level
 * interface.
 */


#ifndef _SPARC64_OPLIB_H
#define _SPARC64_OPLIB_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Sun IEEE P1275 interface:
 *    - every cell is an 8-byte slot (uint64_t)
 *    - supports up to 16 input and 1 output argument
 *
 * Service data structure, used for interaction:
 *     Parameter                   used slots
 *    +---------------------------+------------+
 *    | Client interface service  | 1          |
 *    +---------------------------+------------+
 *    | Number of input arguments | 1          |
 *    +---------------------------+------------+
 *    | Number of return values   | 1          |
 *    +---------------------------+------------+
 *    | Inputs (arg1..argN)       | 0..16      |
 *    +---------------------------+------------+
 *    | Return                    | 0..1       |
 *    +---------------------------+------------+
 *
 * The array of parameters passed to OpenBoot PROM is 20 slots in length.
 */

#define PROM_ARGS_IN                      16
#define PROM_ARGS_OUT                     1
#define PROM_ARGS_MAX                     (PROM_ARGS_OUT+PROM_ARGS_IN)

#define PROM_MAXVERSLEN                   64

/** max boot-path and boot-arguments length */
#define PROM_MAXBARGLEN                   256

#define MAX_PHYS_BANKS                    32

#define MAX_CPUS                          16


typedef struct {
    int node;
    int mid;
    uint32_t clock; /* CPU clock-frequency */
} prom_cpuinfo;

/* These nodes and properties are staying constant after prom_init() .*/
extern uint32_t node_root;
extern uint32_t node_chosen;
extern uint32_t node_stdout;
extern uint32_t node_stdin;
extern uint32_t node_mmu;
extern uint32_t node_obp;
extern char prom_version[PROM_MAXVERSLEN];
extern char prom_bootpath[PROM_MAXBARGLEN];
extern char prom_bootargs[PROM_MAXBARGLEN];
extern char prom_subarch[16]; /* sun4u, sun4v, ... */
extern prom_cpuinfo prom_cpus[MAX_CPUS];

extern void prom_init(void);

/* cpu related */
extern void prom_init_cpu(void);

/* device tree related */
extern int prom_write(int handle, const char *str, int len);
extern int prom_finddevice(char *device);
extern int prom_getprop(int handle, char *prop, void *buf, int buflen);
extern long prom_getproplen(int handle, char *prop);
extern int prom_getpeer(int device);
extern int prom_getchild(int device);
extern int prom_getparent(int device);
extern int prom_setprop(int device, char *prop, const void *buf, int buflen);
extern void prom_exit(void);
extern void prom_halt(void);
extern void prom_poweroff(void);
extern int prom_callmethod(char *method, int handle, int nargs, ...);
extern int prom_packagetopath(int handle, const char *buf, int buflen);
extern int prom_settraptable(uint64_t tba);
extern uint32_t prom_gettime(void);
extern void prom_reboot(const char *boot_command);
extern uint32_t prom_finddevice_by_type(uint32_t node, char *name, int n);

#ifdef __SMP__
extern void prom_startcpu(int cpunode, uint64_t pc, uint64_t o0);
#endif

/* Provided through assembly entries. */
extern int prom_call(void *uint64_t);

/* other */
extern void clear_screen(void);

#ifdef __cplusplus
}
#endif

#endif /* _SPARC64_OPLIB_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
