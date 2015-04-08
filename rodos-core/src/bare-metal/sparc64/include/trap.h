/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Trap handler routines.
 */

#ifndef _SPARC64_TRAPS_H
#define _SPARC64_TRAPS_H

#include <pstate.h>
#include <types.h>

/** This needs to be rounded up to an even value for the 16-byte stack frame
 * alignment according to the V9 specs. */
#define TRAP_NITEMS            6

/** The stack frame that is allocated during trap handling. */
#define TRAP_FRAME_SZ          (STACK_FRAME_SZ + (8 * TRAP_NITEMS))

/** Offset for trap handler %o1 = %sp + STACK_BIAS + TREGS_OFFSET */
#define TREGS_OFFSET            TRAP_FRAME_SZ-(8* 3)

/** Note, start here at 8*1 not zero! */
#define TRAP_TSTATE            (8* 1)
#define TRAP_TPC               (8* 2)
#define TRAP_TNPC              (8* 3)
#define TRAP_Y                 (8* 4)

#ifndef __ASM__

#ifdef __cplusplus
extern "C" {
#endif

/** This struct defines how registers are stored on the stack during a trap.
 * As for TREGS_OFFSET we substract 3 slots so according to the list above
 * the first register will be TNPC, then TPC and TSTATE.
 */
typedef struct {
  long tnpc;
  long tpc;
  long tstate;
} tregs_t;

extern void debug_tl0(tregs_t *regs);

extern void bad_trap(int level, tregs_t *regs);

extern void irq_handler(int pil);
extern const char *isr_getname(int pil);
extern void isr_register(int pil, const char *name, funcptr_t handler);
extern void vec_interrupt(void);

extern void trap_instruction_access_exception(void);
extern void trap_instruction_access_error(tregs_t *regs);
extern void trap_illegal_instruction(void);
extern void trap_privileged_opcode(void);
extern void trap_fp_disabled(void);
extern void trap_data_accesss_exception(void);
extern void trap_mem_address_not_aligned(void);
extern void trap_instruction_access_error_tl1(void);
extern void trap_illegal_instruction_tl1(void);
extern void trap_data_accesss_exception_tl1(void);
extern void trap_mem_address_not_aligned_tl1(void);

typedef union {
  struct {
    uint64_t      : 61;
    unsigned fef  : 1;
    unsigned du   : 1;
    unsigned dl   : 1;
  } __attribute__ ((packed));
  uint64_t value;
} fprs_t;

static inline void enable_interrupts(void) {
  asm volatile(
      ".register %%g2, #scratch\n"
      "rdpr %%pstate, %%g2\n"
      "or   %%g2, %0, %%g2\n"
      "wrpr %%g0, %%g2, %%pstate\n"
      :
      : "i" (PSTATE_IE));
}

static inline void disable_interrupts(void) {
  asm volatile(
      ".register %%g2, #scratch\n"
      "rdpr %%pstate, %%g2\n"
      "andn %%g2, %0, %%g2\n"
      "wrpr %%g0, %%g2, %%pstate\n"
      :
      : "i" (PSTATE_IE));
}

static inline void clear_softint(unsigned long bits) {
  asm volatile("wr %0, 0x0, %%clear_softint\n"
      :
      : "r" (bits));
}

static inline void set_softint(unsigned long bits) {
  asm volatile("wr %0, 0x0, %%set_softint\n"
      :
      : "r" (bits));
}

static inline unsigned long get_softint(void) {
  unsigned long retval;

  asm volatile("rd %%softint, %0\n"
      : "=r" (retval));

  return retval;
}

#ifdef __cplusplus
}
#endif

#endif /* __ASM__ */

#endif /* _SPARC64_TRAPS_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
