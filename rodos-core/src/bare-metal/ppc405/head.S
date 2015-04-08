/**
 * @file head.S
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Entry point, 'head' of the kernel.
 */

/* Kernel entry point after an hw reset. */
  .section    KERNELSTART, "ax"
  .align      2
  .globl      _boot
_boot:
  b       continue_boot

  
/* When compiling for an emulator, KERNELSTART is located at 0x00000000,
 * since a small 'bios' file takes care of the jump. In all other cases
 * the kernel entry point KERNELSTART is located at 0xfffffffc and
 * therefore we need an absolute jump to to startup routine.
 */
  .global     continue_boot
continue_boot:
  /* setup a stack */
  lis     1, _stack@h
  ori     1, 1, _stack@l

  /* setup EVPR (we only need the high 16-bits) */
  lis     0, KERNELSTART@h
  mtevpr  0

  /* dispatch into C startup code */
  b       _start


/* Exception and Interrupt Vectors:
 * Add jumps to handler routines here, not the handler routines
 * themselves. If unimplemented, an endless loop at that very
 * address can be easily spotted during debugging.
 */

  /* Instruction Storage Exception (0x0400) */
  . = 0x0400
_instructionStorageException: // unimpl: endless loop (delete line when implemented)
  b       _instructionStorageException

  /* External Interrupt (0x0500) */
  . = 0x0500
/*_externalInterrupt: */// unimpl: endless loop (delete line when implemented)
  b       _externalInterrupt 

  /* Alignment Exception (0x0600) */
  . = 0x0600
_alignmentException: // unimpl: endless loop (delete line when implemented)
  b       _alignmentException

  /* Program Exception (0x0700) */
  . = 0x0700
_programException: // unimpl: endless loop (delete line when implemented)
  b       _programException

  /* System Call (0x0c00) */
  . = 0x0c00
_syscall: // unimpl: endless loop (delete line when implemented)
  b       _syscall

  /* Programmable-Interval Timer Interrupt (0x1000) */
  . = 0x1000
  b       _pitInterrupt

  /* Fixed-Interval Timer Interrupt (0x1010) */
  . = 0x1010
_fitInterrupt: // unimpl: endless loop (delete line when implemented)
  b       _fitInterrupt

  /* Watchdog-Timer Interrupt (0x1020) */
  . = 0x1020
_wdtException: // unimpl: endless loop (delete line when implemented)
  b       _wdtException
  
/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
