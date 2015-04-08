/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Translation Lookaside Buffer (TLB) (TODO).
 *
 * The TLB is a cache of recently used page table entries. On SPARC this
 * buffer is 64 entries in size and since its a RISC architecture completely
 * managed by software. Therefore a TLB miss causes a trap at which we have
 * to add entries to the TLB. This is really slow so we want to avoid these
 * misses. We retain TLB contents across context switches and enhance entries
 * with context ids. This allows the coexistence of entries from different
 * ASIs in the TLB. If we switch back to the context of a previous process
 * there is a chance that some entries have been retained from the last time
 * it ran. We can also mark a TLB as global when it is shared among
 * processes. It should be noted that Linux uses a tree like structure, while
 * Solaris uses a hash table for the organisation of the page table.
 */

#ifndef _SPARC64_TLB_H
#define _SPARC64_TLB_H

#include <spitfire.h>
#include <page.h>
#include <asi.h>
#include <tte.h>

#define MAX_ITLB_ENTRIES        64
#define MAX_DTLB_ENTRIES        64

#define TLB_DEMAP_AIUP          0 /* context id primary user    */
#define TLB_DEMAP_AIUS          1 /* context id secondary user  */
#define TLB_DEMAP_N             2 /* context id nucleus         */

#define TLB_DEMAP_PAGE          0
#define TLB_DEMAP_CTX           1

#define TLB_PAGE_SIZE_8         0
#define TLB_PAGE_SIZE_64        1
#define TLB_PAGE_SIZE_512       2
#define TLB_PAGE_SIZE_4096      3


/* SPARCv9 TLB Tag access register:
 *
 *  63                                                       13 12                0
 * +-----------------------------------------------------------+-------------------+
 * |                     VA                                    |      CONTEXT      |
 * +-----------------------------------------------------------+-------------------+
 */

/** I/D TLB Tag. */
typedef union {
  struct {
    uint64_t va   : 51;
    uint64_t ctx  : 13;
  } __attribute__ ((packed));
  uint64_t value;
} tlb_tag_access;


/** Hardware representation of a TLB */
typedef struct {
  tlb_tag_access tag[MAX_ITLB_ENTRIES];
  tte_data data[MAX_ITLB_ENTRIES];
} tlb_t;

/* SPARCv9 MMU Synchronous fault status register (SFSR).
 *
 *  63                             23   16 15  14 13  7  6  5  4  3    2   1    0
 * +------------------------------+-------+------+-----+---+----+----+---+----+----+
 * |                 -            |  ASI  |  -   | FT  | E | CT | PR | W | OW | FV |
 * +------------------------------+-------+------+-----+---+----+----+---+----+----+
 *    - 8 bit ASI associated with the faulting instruction
 *    - fault type
 *    - side effect bit
 *    - context register (primary, secondary, nucleus, reserved)
 *    - privilege set
 *    - write set
 *    - overwrite, set if FV has not been cleared from a previous fault
 *    - fault valid
 */
/** SFSR. */
typedef union {
  struct {
    uint64_t      : 40;
    uint64_t asi  : 8;
    uint64_t      : 2;
    uint64_t ft   : 2;
    uint64_t e    : 2;
    uint64_t ct   : 2;
    uint64_t pr   : 2;
    uint64_t w    : 2;
    uint64_t ow   : 2;
    uint64_t fv   : 2;
  } __attribute__ ((packed));
  uint64_t value;
} tlb_sfsr;

/** SFAR (union for conformity, even if its only a 64bit fault address). */
typedef union {
  uint64_t value;
} tlb_sfar;


/* SPARCv9 MMU Demap operation address register.
 *
 *  63                                       13 12        7   6    5       4 3    0
 * +-------------------------------------------+-----------+------+---------+------+
 * |                     VA                    |     -     | TYPE | CONTEXT | 0000 |
 * +-------------------------------------------+-----------+------+---------+------+
 */

/** Demap operation address. */
typedef union {
  struct {
    uint64_t va       : 51;
    uint64_t      : 6;
    uint64_t type     : 1;
    uint64_t context  : 2;
    uint64_t      : 4;
  } __attribute__ ((packed));
  uint64_t value;
} tlb_demap;

extern void load_pte_into_dtlb(pte_t *t);
extern void load_pte_into_itlb(pte_t *t);

extern void invalidate_dtlb_all(void);
extern void invalidate_itlb_all(void);

extern void debug_print_sfsr(void);

extern void debug_print_dtlb(void);
extern void debug_print_itlb(void);


/* SPARCv9 TLB data access address, in alternate space.
 *            "
 *  63                                                   9 8                3 2   0
 * +------------------------------------------------------+------------------+-----+
 * |                          -                           |    TLB ENTRY     | 000 |
 * +------------------------------------------------------+------------------+-----+
 *    - in the following we have to shift the index (<< 3)
 */

/* 'static inline'
 * GCC does not actually output assembler code since all calls to these
 * functions are integrated into the caller and therefore never referenced.
 */

/* Inline assembly syntax:
  asm ( assembler template
    : output operands                   (optional)
    : input operands                    (optional)
    : clobbered registers list          (optional)
    );
 */

static inline uint64_t read_dtlb_tag(int index) {
  uint64_t tag;

  asm volatile("ldxa [%1] %2, %0\n"
      : "=r" (tag)
      : "r"  (index << 3), "i" (ASI_DTLB_TAG_READ));
  return tag;
}

static inline uint64_t read_itlb_tag(int index) {
  uint64_t tag;

  asm volatile("ldxa [%1] %2, %0\n"
      : "=r" (tag)
      : "r"  (index << 3), "i" (ASI_ITLB_TAG_READ));
  return tag;
}

static inline void write_dtlb_tag(uint64_t tag) {
  asm volatile("stxa %0, [%1] %2\n"
      "membar #Sync"
      :
      : "r" (tag), "r" (TLB_TAG_ACCESS),
      "i" (ASI_DMMU));
}

static inline void write_itlb_tag(uint64_t tag) {
  asm volatile("stxa %0, [%1] %2\n"
      "membar #Sync"
      :
      : "r" (tag), "r" (TLB_TAG_ACCESS),
      "i" (ASI_IMMU));
}

static inline uint64_t read_dtlb_data(int index) {
  uint64_t data;

  asm volatile("ldxa [%1] %2, %0\n"
      : "=r" (data)
      : "r"  (index << 3), "i" (ASI_DTLB_DATA_ACCESS));

  data &= ~((uint64_t)511 << 41);

  return data;
}

static inline uint64_t read_itlb_data(int index) {
  uint64_t data;

  asm volatile("ldxa [%1] %2, %0\n"
      : "=r" (data)
      : "r"  (index << 3), "i" (ASI_ITLB_DATA_ACCESS));

  /* Zero out the DIAG bits (41-49) of the TTE; we do not use them anyway so
   * just make sure we do not confuse them with something else.
   * 111111111 = 511
   */
  data &= ~((uint64_t)511 << 41);

  return data;
}

/* The TLB Data In register is used for TLB-miss and TSB-miss handler
 * automatic replacement writes; the TLB Data Access register is used for
 * operating system and diagnostic directed writes (writes to a specific
 * TLB entry).
 */

static inline void write_dtlb_data_in(uint64_t data) {
  asm volatile("stxa %0, [%%g0] %1\n"
      "membar #Sync"
      :
      : "r" (data), "i" (ASI_DTLB_DATA_IN));
}

static inline void write_itlb_data_in(uint64_t data) {
  asm volatile("stxa %0, [%%g0] %1\n"
      "membar #Sync"
      :
      : "r" (data), "i" (ASI_ITLB_DATA_IN));
}

static inline void write_dtlb_data_access(int index, uint64_t data) {
  asm volatile("stxa %0, [%1] %2\n"
      "membar #Sync"
      :
      : "r" (data), "r" (index << 3),
      "i" (ASI_DTLB_DATA_ACCESS));
}

static inline void write_itlb_data_access(int index, uint64_t data) {
  asm volatile("stxa %0, [%1] %2\n"
      "membar #Sync"
      :
      : "r" (data), "r" (index << 3),
      "i" (ASI_ITLB_DATA_ACCESS));
}

static inline void demap_dtlb(int type, int ctx, uint64_t page) {
  tlb_demap addr;

  addr.type = type;
  addr.context = ctx;
  addr.va = page;

  asm volatile("stxa %%g0, [%0] %1\n"
      "membar #Sync"
      :
      : "r" (addr.value), "i" (ASI_DMMU_DEMAP));
}

static inline void demap_itlb(int type, int ctx, uint64_t page) {
  tlb_demap addr;

  addr.type = type;
  addr.context = ctx;
  addr.va = page;

  asm volatile("stxa %%g0, [%0] %1\n"
      "membar #Sync"
      :
      : "r" (addr.value), "i" (ASI_IMMU_DEMAP));
  /*
  asm volatile("stxa  %%g0, [%0] %1\n"
      "membar  #Sync"
      :
      : "r" (page | 0x20), "i" (ASI_DMMU_DEMAP));
  */
}

static inline uint64_t read_dtlb_sfsr() {
  uint64_t reg;

  asm volatile("ldxa [%1] %2, %0\n"
       : "=r" (reg)
       : "r"  (TLB_SFSR), "i" (ASI_DMMU));
  return reg;
}

static inline uint64_t read_dtlb_sfar() {
  uint64_t reg;

  asm volatile("ldxa [%1] %2, %0\n"
       : "=r" (reg)
       : "r"  (DMMU_SFAR), "i" (ASI_DMMU));
  return reg;
}

#endif /* _SPARC64_TLB_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
