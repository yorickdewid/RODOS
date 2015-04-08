/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Entry point, 'head' of the kernel.
 */

#include <spitfire.h>
#include <pstate.h>
#include <asi.h>
#include <lsu.h>
#include <param.h>

#include "params.h"

  /*
   * Globals.
   */
  .section    .data
  .align      8
  .register   %g2, #scratch
  .register   %g3, #scratch
  .register   %g6, #scratch
  .register   %g7, #scratch

  .global     prom_cif, prom_tba
prom_cif:     .xword 0                    ! entry point for PROM calls
prom_tba:     .xword 0                    ! backup of the PROM tba

  /*
   * Entry point, execution starts from here.
   */
  .section    KERNELSTART, "ax"
  .align      4
  .global     _boot
_boot:

  b           sparc64_boot_start
    flushw

 /**
  * SILO header:
  * The following header tells the bootloader that this is a very 'old'
  * version of linux that is unable to relocate itself. This ensures that
  * SILO loads the image at the virtual address 0x4000.
  */
  .align      8
  .global     sparc_ramdisk_image, sparc_ramdisk_size, sparc_ramdisk_image64
  .ascii      "HdrS"
  .word       SILO_VERSION_CODE           ! version code
  .half       0x0000                      ! HdrS version

  .half       0                           ! root flags
  .half       0                           ! root device
  .half       0                           ! ram flags
sparc_ramdisk_image:
  .word       0                           ! ramdisk image location 32-bit
sparc_ramdisk_size:
  .word       0                           ! ramdisk size

  /* ... newer versions of SILO. */
  .xword      0                           ! reboot command
  .xword      0                           ! bootstring info
sparc_ramdisk_image64:
  .xword      0                           ! ramdisk image location 64-bit


  .align      4
  .global     sparc64_boot_start
sparc64_boot_start:

  /* Make sure 32-bit address masking is disabled. */
  rdpr        %pstate, %g1
  andn        %g1, PSTATE_AM, %g1
  wrpr        %g1, %g0, %pstate

  /**
   * Remap the kernel to its VMA (LMA equals the SILO load address 0x4000).
   */
  set         VMA, %o0                    ! %o0 dest, virtual memory address
  set         LMA, %o1                    ! %o1 src, load memory address

  set         __size_text, %g1
  set         __size_data, %g2
  sub         %g1, %o0, %g1               ! subtract hardcoded link offset
  sub         %g2, %o0, %g2
  ldx         [%g1 + %o1], %g1            ! load the hardcoded size
  ldx         [%g2 + %o1], %g2
  add         %g1, %g2, %g1               ! sum of .text and .data size

  call        memcpy
    mov       %g1, %o2                    ! %o2 num of bytes

  /* We jump to a local label in order to adjust our program counter to the
   * new location. This instruction will trap and the PROM will map the
   * necessary pages for us :)
   */
  set         sun4u_init, %g1
  jmp         %g1
    nop

  /* Note: the symbol sun4u_init should be kept no matter what.
   * When generating a tftpboot image, TILO will be looking for it.
   */
sun4u_init:
  /* The OpenFirmware entry point arrives in %o4. */
  set         prom_cif, %g1
  stx         %o4, [%g1]                  ! store OpenFirmware entry point

  /* The I/D MMUs and on-chip caches should be already enabled, this only
   * makes sure that they really are.
   */
  mov         (LSU_CONTROL_IC|LSU_CONTROL_DC|LSU_CONTROL_IM|LSU_CONTROL_DM), %g1
  stxa        %g1, [%g0] ASI_LSU_CONTROL
  membar      #Sync                       ! make it visible

  /* Setup pstate to a known state.
   * Privileged mode, enable floating points and interrupts
   */
  wrpr        %g0, (PSTATE_PRIV|PSTATE_PEF|PSTATE_IE), %pstate
  wr          %g0, %g0, %fprs
  wrpr        %g0, %g0, %tl               ! set traplevel 0
  wrpr        %g0, %g0, %pil              ! set priority interrupt level 0
  wrpr        %g0, %g0, %wstate           ! use spill/fill traps 0

  /* Reset window control registers, extract NWINDOWS-1 from %ver */
  rdpr        %ver, %l0                   ! get VERSION register
  and         %l0, VERS_MAXWIN, %g2
  sub         %g2, 1, %g1
  wrpr        %g2, %g0, %cleanwin         ! NWINDOWS-1
  wrpr        %g1, %g0, %cansave          ! NWINDOWS-2
  wrpr        %g0, %g0, %canrestore       ! 0
  wrpr        %g0, %g0, %otherwin         ! 0

  /* Disable the timer for now. */
  setx        (1 << 63), %g2, %g1
  wr          %g1, 0, %tick_cmpr

  /**
   * MMU setup, lock the kernel as a 4M page in the tlb.
   * As we make use of ctx 0 for kernel mode and ctx 1 for user mode but have
   * not memory management in place yet, we just lock these entries statically
   * for both contexts.
   */

  rdpr        %pstate, %g3
  andn        %g3, PSTATE_IE, %g2         ! disable interrupts
  wrpr        %g2, %pstate

  /* DMMU */

  /* g1 should point to the start of physical memory that will end up in g7.
   * NOTE: It was pointed out somewhere that in *some* cases this isn't 0x0.
   * For now we just assume 0x0.
   */
  clr         %g1

  /* Get the base of physical memory
   *    - cut bits higher than MMU_PHYS_BITS
   *    - cut bits from the page offset
   */
  sllx        %g1, (64 - MMU_PHYS_BITS), %g1
  srlx        %g1, (64 - MMU_PHYS_BITS) + PAGE_SHIFT, %g7

  /* set asi to dmmu register space */
  wr          %g0, ASI_DMMU, %asi

  /* dtlb write tag */
  set         VMA, %g1
  stxa        %g1, [TLB_TAG_ACCESS] %asi  ! ctx 0, so nothing else to do

  /* dtlb insert kernel mapping; flags: L CP CV E P W G (see tlb.h) */
  set         VMA, %g1
  set         0b1110110, %g2              ! flags: L CP CV P W
  add         %g1, %g2, %g1
  add         %g1, %g7, %g1               ! add memory base offset
  mov         3, %g2                      ! SIZE = PAGE_SIZE << 3*THIS = 4M
  sllx        %g2, 61, %g2                ! size bits at 60
  add         %g1, %g2, %g1
  mov         1, %g2
  sllx        %g2, 63, %g2                ! valid bit at 63
  add         %g1, %g2, %g1
  stxa        %g1, [%g0] ASI_DTLB_DATA_IN
  membar      #Sync

  /* dtlb write tag */
  set         VMA, %g1
  add         %g1, 1, %g1                 ! ctx 1
  stxa        %g1, [TLB_TAG_ACCESS] %asi

  /* dtlb insert kernel mapping; flags: L CP CV E P W G (see tlb.h) */
  set         VMA, %g1
  set         0b1110011, %g2              ! flags: L CP CV W G
  add         %g1, %g2, %g1
  add         %g1, %g7, %g1               ! add memory base offset
  mov         3, %g2                      ! SIZE = PAGE_SIZE << 3*THIS = 4M
  sllx        %g2, 61, %g2                ! size bits at 60
  add         %g1, %g2, %g1
  mov         1, %g2
  sllx        %g2, 63, %g2                ! valid bit at 63
  add         %g1, %g2, %g1
  stxa        %g1, [%g0] ASI_DTLB_DATA_IN
  membar      #Sync

  /* IMMU
   * It is noteworthy that the FLUSH instruction is required to maintain
   * coherency. SPARCv9 provides the FLUSH instruction to synchronize
   * instruction and data memory after the code space has been modiﬁed.
   */

  /* set asi to immu register space */
  wr          %g0, ASI_IMMU, %asi

1:
  set         1b, %g6

  /* itlb write tag ctx 1 */
  set         VMA, %g1
  add         %g1, 1, %g1                 ! ctx 1
  stxa        %g1, [TLB_TAG_ACCESS] %asi

  /* itlb insert ctx 1 mapping */
  set         VMA, %g1
  set         0b1110001, %g2              ! flags: L CP CV G
  add         %g1, %g2, %g1
  add         %g1, %g7, %g1               ! add memory base offset
  mov         3, %g2                      ! SIZE = PAGE_SIZE << 3*THIS = 4M
  sllx        %g2, 61, %g2                ! size bits at 60
  add         %g1, %g2, %g1
  mov         1, %g2
  sllx        %g2, 63, %g2                ! valid bit at 63
  add         %g1, %g2, %g1
  stxa        %g1, [%g0] ASI_ITLB_DATA_IN
  membar      #Sync
  flush       %g6

  /* switch to ctx 1 */
  mov         1, %g1
  mov         PRIMARY_CONTEXT, %g2
  stxa        %g1, [%g2] ASI_DMMU
  membar      #Sync
  flush       %g6

  /* itlb write tag ctx 0 */
  set         VMA, %g1
  stxa        %g1, [TLB_TAG_ACCESS] %asi  ! ctx 0, so nothing else to do
  flush       %g6

  /* itlb insert ctx 0 locked mapping */
  set         VMA, %g1
  set         0b1110100, %g2              ! flags: L CP CV P
  add         %g1, %g2, %g1
  add         %g1, %g7, %g1               ! add memory base offset
  mov         3, %g2                      ! SIZE = PAGE_SIZE << 3*THIS = 4M
  sllx        %g2, 61, %g2                ! size bits at 60
  add         %g1, %g2, %g1
  mov         1, %g2
  sllx        %g2, 63, %g2                ! valid bit at 63
  add         %g1, %g2, %g1
  stxa        %g1, [%g0] ASI_ITLB_DATA_IN
  flush       %g6

  /* MMU set contexts: primary: 0, secondary: 0 */
  mov         PRIMARY_CONTEXT, %g1
  stxa        %g0, [%g1] ASI_DMMU
  membar      #Sync
  mov         SECONDARY_CONTEXT, %g1
  mov         %g0, %g2
  stxa        %g2, [%g1] ASI_DMMU
  membar      #Sync

  wrpr        %g3, %pstate

  /* We need to setup a stack before we can jump into C code. */
  set         boot_stack_top -STACK_BIAS, %g1
  add         %g1, -STACK_FRAME_SZ, %sp

  /* Make sure BSS is clear. */
  sethi       %hi(__bss_start), %o0
  or          %o0, %lo(__bss_start), %o0
  sethi       %hi(__bss_end), %o2
  or          %o2, %lo(__bss_end), %o2
  sub         %o2, %o0, %o2
  call        memset
    mov       %g0, %o1

  /* Medium/Anywhere code model:
   * As for the Medium/Anywhere code model, the MMU includes a special
   * register %g4 that contains the base address of the data segment.
   * This is automatically added to the relative address for each access.
   * Despite what code model is used, we set %g4 to be compatible. For now we
   * just assume zero.
   */
  call        prom_init                   ! init OpenBoot PROM
    clr       %g4                         ! data segment base address

  call        bootstrap
    mov       %l0, %o0                    ! %ver for compatibility check

  /* Switch to our trap table. */
  call        setup_traptable
    nop

  /* Finally dispatch into C startup code... */
  call        _start
    nop

  /* not reached */
1:
  b           1b
    nop

  /* MMU: Translation storage buffer. TODO
   * The v9 specs require the tsb to be size aligned.

1:
  .skip       XXX

  .global     tsb
tsb:
  .skip       TSB_SIZE_BYTES
   */

  /*
   * Reserve space for boot stack.
   */
  .section    .data
  .align      STACK_FRAME_ALIGNMENT
boot_stack:
  .skip       STACK_BOOT_SIZE
boot_stack_top:
  .global     boot_stack_top

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
