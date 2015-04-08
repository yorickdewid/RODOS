/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief A few trap handler routines.
 */

#include <pstate.h>
#include <printk.h>
#include <oplib.h>
#include <trap.h>
#include <asm.h>

/**
 * @brief Print trap registers for TL = 0.
 * A call to this function only makes sense after etrap* has run. So
 * basically this function should not be called outside of these trap
 * handlers.
 *
 * @param regs pointer to stack address.
 */
void debug_tl0(tregs_t *regs) {
  printk("tstate = 0x%x\n",regs->tstate);
  printk(
      " .gl     = %x\n"
      " .ccr    = %x\n"
      " .asi    = %x\n"
      " .pil    = %x\n"
      " .pstate = %x\n"
      " .cwp    = %x\n",
    (regs->tstate&TSTATE_GL) >> TSTATE_SHIFT_GL,
    (regs->tstate&TSTATE_CCR) >> TSTATE_SHIFT_CCR,
    (regs->tstate&TSTATE_ASI) >> TSTATE_SHIFT_ASI,
    (regs->tstate&TSTATE_PIL) >> TSTATE_SHIFT_PIL,
    (regs->tstate&TSTATE_PSTATE) >> TSTATE_SHIFT_PSTATE,
    (regs->tstate&TSTATE_CWP) >> TSTATE_SHIFT_CWP
  );
  printk("\n");
  printk("tpc  = 0x%x\n",regs->tpc);
  printk("tnpc = 0x%x\n",regs->tnpc);
}

/**
 * @brief Bad trap handler, results in kernel panic.
 *
 * @param level trap index in trap table.
 * @param regs this structure represents stack contents.
 */
void bad_trap(int level, tregs_t *regs) {
  uint64_t tl;

  asm volatile("rdpr %%tl, %0\n"
           : "=r" (tl));

  debug_tl0(regs);

  if(level < 0x100)
    panic("\n\nKernel panic: bad hw trap 0x%x at TL = %d", level, tl);

  level -= 0x100;
  panic("\n\nKernel panic: bad sw trap %d at TL = %d", level, tl);
}

void trap_instruction_access_exception() {
  panic("\n\nKernel panic: Instruction access exception");
}

void trap_instruction_access_error(tregs_t *regs) {
  debug_tl0(regs);
  panic("\n\nKernel panic: Instruction access error");
}

void trap_illegal_instruction(void) {
  panic("\n\nKernel panic: Illegal instruction");
}

void trap_privileged_opcode() {
  panic("\n\nTrap: Privileged opcode");
}

/**
 * @brief Enable the FPU and retry the trapped instruction.
 */
void trap_fp_disabled() {
  fprs_t fprs;
  fprs.fef = 1;
  asm volatile("wr %%g0, %0, %%fprs\n"
      :
      : "r" (fprs.value));
}

void trap_data_accesss_exception() {
  panic("\n\nKernel panic: Data access exception");
}

void trap_mem_address_not_aligned() {
  panic("\n\nKernel panic: Memory address not aligned");
}

void trap_instruction_access_error_tl1() {
  panic("\n\nKernel panic: Instruction access error");
}

void trap_illegal_instruction_tl1() {
  panic("\n\nKernel panic: Illegal instruction");
}

void trap_data_accesss_exception_tl1() {
  panic("\n\nKernel panic: Data access exception");
}

void trap_mem_address_not_aligned_tl1() {
  panic("\n\nKernel panic: Memory address not aligned");
}

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
