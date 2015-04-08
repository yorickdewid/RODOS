/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Translation Storage Buffer (TSB) (TODO).
 *
 * The TSB is a software cache of TTEs that resides in physical memory. It is
 * represented in an array structure and contains 2048 entries.
 */

#ifndef _SPARC64_TSB_H
#define _SPARC64_TSB_H

#include <param.h>

/** Reserved TSB space in param.h divided by 16 (8-byte tag + 8-byte data) */
#define TSB_NENTRIES            TSB_SIZE_BYTES / 16
#define MAX_DTSB_ENTRIES        TSB_NENTRIES / 2
#define MAX_ITSB_ENTRIES        TSB_NENTRIES / 2
#define TSB_OFFSET_MASK         ((TSB_NENTRIES / 2) - 1)

#include <spitfire.h>
#include <asi.h>
#include <page.h>
#include <tte.h>

/* SPARCv9 TSB register format.
 *
 *  63                                       13   12    11             3 2        0
 * +-------------------------------------------+-------+----------------+----------+
 * |                  TSB_BASE                 | SPLIT |       -        | TSB_SIZE |
 * +-------------------------------------------+-------+----------------+----------+
 *    - base virtual address of the translation storage buffer
 *    - split 0="common tsb"; 1="two separate tsbs"
 *    - size, number of entries = 512 * 2^TSB_SIZE
 */

typedef union {
  struct {
    uint64_t base   : 51;
    uint64_t split  : 1;
    uint64_t        : 9;
    uint64_t size   : 3;
  } __attribute__ ((packed));
  uint64_t value;
} tsb_reg;


/* SPARCv9 TSB Tag target register (derived from TTE Tag).
 *
 *  63 61 60           48 47   42 41                                             0
 * +-----+---------------+-------+-------------------------------------------------+
 * | 000 |    CONTEXT    |   -   |                        VA                       |
 * +-----+---------------+-------+-------------------------------------------------+
 *    - context associated with the missing virtual address
 *    - va, the most significant bits of the missing virtual address
 *    - lock bit 47, we need to choose a lock bit from a of the reserved block
 */

typedef union {
  struct {
    uint64_t      : 3;
    uint64_t ctx  : 13;
    uint64_t lock : 1;
    uint64_t      : 5;
    uint64_t va   : 42;
  } __attribute__ ((packed));
  uint64_t value;
} tsb_tag;

/** TSB entry */
typedef struct {
  tsb_tag tag;
  tte_data data;
} tsb_entry_t;

extern void *tsb;
extern void init_tsb(void);

extern void load_pte_into_dtsb(pte_t *t);
extern void load_pte_into_itsb(pte_t *t);

extern void invalidate_tsb(uint64_t page, uint32_t num);
extern void invalidate_tsb_all(void);

static inline uint64_t read_dtsb_reg(void) {
  uint64_t reg;

  asm volatile ("ldxa [%1] %2, %0\n"
      : "=r" (reg)
      : "r"  (TSB_REG), "i" (ASI_DMMU));
  return reg;
}

static inline uint64_t read_itsb_reg(void) {
  uint64_t reg;

  asm volatile ("ldxa [%1] %2, %0\n"
      : "=r" (reg)
      : "r"  (TSB_REG), "i" (ASI_IMMU));
  return reg;
}

static inline void write_dtsb_reg(uint64_t reg) {
  asm volatile("stxa %0, [%1] %2\n"
      "membar #Sync"
      :
      : "r" (reg), "r" (TSB_REG),
      "i" (ASI_DMMU));
}

static inline void write_itsb_reg(uint64_t reg) {
  asm volatile("stxa %0, [%1] %2\n"
      "membar #Sync"
      :
      : "r" (reg), "r" (TSB_REG),
      "i" (ASI_IMMU));
}

#endif /* _SPARC64_TSB_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
