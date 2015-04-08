/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Assembly routines.
 */

#include <pstate.h>
#include <param.h>
#include <asi.h>

  .section    .text
  .align      4
  .register   %g2, #scratch

/*
 * Call OpenFirmware entry point.
 */
  .global     prom_call
  /* int prom_call(void *args); */
prom_call:
  save        %sp, -STACK_FRAME_SZ, %sp
  mov         %i0, %o0                    ! copy argument
  set         prom_cif, %l0
  ldx         [%l0], %l0                  ! the prom entry point
  rdpr        %pstate, %l1
  jmpl        %l0, %o7
    wrpr      %g0, PSTATE_PRIV, %pstate
  clr         %g4                         ! make sure %g4 is untouched
  wrpr        %l1, %g0, %pstate           ! restore previous pstate
  ret
    restore


/*
 * Setup the kernel trap table.
 */
  .global     setup_traptable
  /* void setup_traptable(void); */
setup_traptable:
  save        %sp, -STACK_WINDOW_SZ, %sp
  rdpr        %tba, %l0
  sethi       %hi(prom_tba), %l1
  or          %l1, %lo(prom_tba), %l1
  stx         %l0, [%l1]                  ! save PROM %tba address
  rdpr        %pstate, %l1
  wrpr        %g0, (PSTATE_PRIV|PSTATE_AG), %pstate
  /* now switch to kernel trap table */
  sethi       %hi(sparc64_ttable_tl0), %g5
  wrpr        %g5, %lo(sparc64_ttable_tl0), %tba
  /* point the PROM to our trap table
   * TODO: fix this
  call        prom_settraptable
    mov       %g5, %o0
  wrpr        %g0, %g0, %wstate
   */
  or          %l1, PSTATE_IE, %l1
  wrpr        %g0, %l1, %pstate
  ret
    restore


/*
 * System call handler that vectors into the syscall table.
 * When we get here, the syscall number is present in %g1.
 */
  .global     system_call
system_call:
  sub         %g1, 1, %g1                 ! entry 0 is syscall 1
  sllx        %g1, 3, %g1                 ! systbl[SYSCALL_NO * 8]
  sethi       %hi(sparc64_systable), %g2
  or          %g2, %lo(sparc64_systable), %g2
  ldx         [%g2 + %g1], %g1
  jmpl        %g1, %g0
    nop


/*
 * call printk with the given arguments, then freeze.
 */
  .global     panic
  /* void panic(const char *fmt, ...); */
panic:
  call printk
    nop
1:
  b           1b
    nop


/* Functions can be 'leaf optimized' so that they operate in the callers
 * registers. Therefore we can use the out registers freely, save and restore
 * instructions do not need to be executed and it is impossible to fill/spill
 * registers windows. This requires the usage of retl - return from leaf
 * routine.
 */

/*
 * memcpy copies num bytes from src to dest.
 */
  .global     _memcpy
  /* void *memcpy(void *dest, const void *src, size_t num); */
_memcpy:
  brz,pn      %o2, 2f
    mov       %g0, %g1
  ldub        [%o1+%g1], %g2
1:
  stb         %g2, [%o0+%g1]              ! [dest+offset] <= %g2
  add         %g1, 1, %g1                 ! increment til %g2 == %o2
  cmp         %g1, %o2
  bne,a,pt    %xcc, 1b                    ! note the annulled delay slot
    ldub      [%o1+%g1], %g2              ! %g2 <= [src+offset]
  add         %o0, %g1, %o0
2:
  retl                                    ! return from leaf function
    nop


/*
 * memset sets num bytes starting from ptr to value.
 * This is here instead of the non ansi function __bzero to zero a block of
 * bytes in the early boot stage.
 */
  .global     _memset
  /* void *memset (void *ptr, int value, size_t num); */
_memset:
  brz,pn      %o2, 2f
   nop
1:
  add         %o0, %o2, %g1
  add         %o2, -1, %o2
  brnz,pt     %o2, 1b
    stb       %o1, [%g1-1]                ! byte-by-byte
2:
  retl                                    ! return from leaf function
    nop


/*
 * memcpy copies num bytes from src to dest.
 * loads are from userspace (secondary ctx id).
 */
  .global     memcpy_from_user
  /* void *memcpy_from_user(void *dest, const void *src, size_t num); */
memcpy_from_user:
  brz,pn      %o2, 2f
    mov       %g0, %g1
  lduba       [%o1+%g1] ASI_AIUS, %g2
1:
  stb         %g2, [%o0+%g1]
  add         %g1, 1, %g1
  cmp         %g1, %o2
  bne,a,pt    %xcc, 1b
    lduba     [%o1+%g1] ASI_AIUS, %g2     ! annulled delay slot
  add         %o0, %g1, %o0
2:
  retl                                    ! return from leaf function
    nop


/*
 * memcpy copies num bytes from src to dest.
 * stores are to userspace (secondary ctx id).
 */
  .global     memcpy_to_user
  /* void *memcpy_to_user(void *dest, const void *src, size_t num); */
memcpy_to_user:
  brz,pn      %o2, 2f
    mov       %g0, %g1
  ldub        [%o1+%g1], %g2
1:
  stba        %g2, [%o0+%g1] ASI_AIUS
  add         %g1, 1, %g1
  cmp         %g1, %o2
  bne,a,pt    %xcc, 1b
    ldub      [%o1+%g1], %g2              ! annulled delay slot
  add         %o0, %g1, %o0
2:
  retl                                    ! return from leaf function
    nop


/*
 * memcpy copies num bytes from src to dest.
 * loads and stores operate in userspace (secondary ctx id).
 */
  .global     memcpy_in_user
  /* void *memcpy_in_user(void *dest, const void *src, size_t num); */
memcpy_in_user:
  brz,pn      %o2, 2f
    mov       %g0, %g1
  lduba       [%o1+%g1] ASI_AIUS, %g2
1:
  stba        %g2, [%o0+%g1] ASI_AIUS
  add         %g1, 1, %g1
  cmp         %g1, %o2
  bne,a,pt    %xcc, 1b
    lduba     [%o1+%g1] ASI_AIUS, %g2     ! annulled delay slot
  add         %o0, %g1, %o0
2:
  retl                                    ! return from leaf function
    nop


/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
