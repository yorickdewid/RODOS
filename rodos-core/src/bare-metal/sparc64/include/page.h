/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Page Table (hashed inverted page table) (TODO).
 *
 * Memory is divided into pages with size PAGE_SIZE. A page table is a
 * collection of page table entries that map a virtual page number to a page
 * table entry. A page fault occurs if no virtual to physical mapping exists.
 * On SPARC this will result in an MMU trap. On UltraSPARC we use a
 * translation table which is the functional equivalent of a page table. It
 * should be noted that we use one translation table entry (TTE) for each
 * page.
 */

#ifndef _SPARC64_PAGE_H
#define _SPARC64_PAGE_H

#include <stdint.h>

#define PAGE_G        (1 << 0)
#define PAGE_W        (1 << 1)
#define PAGE_P        (1 << 2)
#define PAGE_C        (1 << 3)
#define PAGE_V        (1 << 4)

/** page table entry
 *    - context
 *
 *    - virt page number
 *    - phys page number
 *
 *    - global
 *    - writeable
 *    - privileged (supervisor required)
 *    - cacheable
 *    - valid
 *
 *    - TODO: dirty / execute ?
 *
 *    - chain element for collisions
 */
typedef struct pte {
  /* context as in process ID */
  uint16_t ctx;

  /* mapping */
  uint64_t va; /* page number+offset = 44 bit vaddress */
  uint64_t pa; /* page number+offset = 41 bit paddress */
  /* PAGE_SIZE=2^offset; offset bits are zero */

  /* flags */
  uint8_t g : 1;
  uint8_t w : 1;
  uint8_t p : 1;
  uint8_t c : 1;
  uint8_t v : 1;
  struct pte *next;
} __attribute__ ((packed)) pte_t;

/** hashed inverted page table indexed by hash */
typedef struct {
  pte_t *pages[16384]; /* FIXME: static for 128M, change to dynamic allocation */
  uint64_t  entry_cnt;
} page_table_t;

/**
 * hash function TODO...
 */
static const uint32_t s = 2654435769;

// /** Hash function: Multiplication method */ ? TODO
static inline uint32_t page_hash_func(uint64_t k, uint32_t i) {
  uint64_t t = s * k & 0x00000000FFFFFFFF; /* s*k%w */
  return t >> (32 - i); /* m=2^i */
}

extern void paging_init(void);
extern void page_insert(uint32_t ctx, uint64_t vpn, uint64_t ppn, uint8_t flags);
extern void page_remove(uint32_t ctx, uint64_t vpn);
extern void page_lookup(uint32_t ctx, uint64_t vpn);

#endif /* _SPARC64_PAGE_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
