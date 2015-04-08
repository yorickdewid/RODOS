/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Translation Storage Buffer (TSB) (TODO).
 */

#include <stddef.h>
#include <tsb.h>
#include <printk.h>

tsb_entry_t *dtsb;
tsb_entry_t *itsb;

/**
 * @brief Initialize the TSB.
 * This links the global pointer to the secured space in the kernel.
 */
void init_tsb() {
  tsb_reg tsbreg;

  dtsb = (tsb_entry_t*) &tsb;
  itsb = (tsb_entry_t*) (&tsb + MAX_DTSB_ENTRIES * sizeof(tsb_entry_t));

  /* The tsb addr is at least PAGE aligned, so bits 13:0 are zero anyway. */
  tsbreg.base = ((uint64_t) &tsb) >> PAGE_SHIFT;
  tsbreg.split = 0;
  tsbreg.size = 6; /* 512*2^6 = 32k dTSB */
  write_dtsb_reg(tsbreg.value);

  tsbreg.base = ((uint64_t) (&tsb + MAX_DTSB_ENTRIES * sizeof(tsb_entry_t))) >> PAGE_SHIFT;
  tsbreg.split = 0;
  tsbreg.size = 6; /* 512*2^6 = 32k iTSB */
  write_itsb_reg(tsbreg.value);
}

/**
 * @brief Invalidate some TSB entries.
 *
 * @param page First page.
 * @param num Number of pages.
 */
void invalidate_tsb(uint64_t page, uint32_t num) {
  uint64_t page0;
  uint64_t i;

  page0 = ((page >> PAGE_SHIFT) & TSB_OFFSET_MASK) << 3;
  if(num > MAX_DTSB_ENTRIES)
    num = MAX_DTSB_ENTRIES;
  else
    num = num;

  /* since we never map anything at page zero in ctx zero, we choose 0 to be
   * an invalid tag entry.
   */
  for(i = 0; i<num; i++) {
    dtsb[page0+i].tag.value = 0;
    itsb[page0+i].tag.value = 0;
  }
}

/**
 * @brief Invalidate the whole TSB.
 */
void invalidate_tsb_all() {
  uint64_t i;

  for(i = 0; i<MAX_DTSB_ENTRIES; i++) {
    dtsb[(i & TSB_OFFSET_MASK) << 3].tag.value = 0;
    itsb[(i & TSB_OFFSET_MASK) << 3].tag.value = 0;
  }
}

/**
 * @brief Load a PTE into the dTSB.
 *
 * @param t Page table entry.
 */
void load_pte_into_dtsb(pte_t *t) {
  tsb_entry_t *tsb;
  uint32_t offset;

  /** TSB Lookup:
   * TTE PA = TSB base address + [VA<27:13>,000]
   *    - shift 13 r (PAGE_SHIFT)
   *    - and SIZE_MASK
   *    - shift 3 l
   */
  offset = ((t->va >> PAGE_SHIFT) & TSB_OFFSET_MASK) << 3;
  tsb = &dtsb[offset];

  /* Forces all stores after the MEMBAR to wait until all stores before the
   * MEMBAR have reached global visibility. "memory" prevents the compiler
   * from keeping data cached in registers across the asm statement.
   */
  tsb->tag.lock = 1;
  asm volatile ("membar #StoreStore\n" ::: "memory");

  tsb->tag.ctx = t->ctx; /* as in process ID */
  /* 41 most significant bits */
  tsb->tag.va  = (uint64_t) t->va >> TTE_VA_SHIFT;

  tsb->data.v      = 1;
  tsb->data.size   = 0; /* 8K */
  tsb->data.nfo    = 0; /* unused */
  tsb->data.ie     = 0; /* unused */
  tsb->data.soft2  = 0; /* unused */
  tsb->data.diag   = 0; /* unused */
  /* 28 bit page number, not 41 bit address */
  tsb->data.pa     = t->pa >> PAGE_SHIFT;
  tsb->data.soft   = 0; /* unused */
  tsb->data.l      = 0; /* unlocked */
  tsb->data.cp     = t->c;
  tsb->data.cv     = t->c;
  tsb->data.e      = 0; /* unused */
  tsb->data.p      = t->p;
  tsb->data.w      = t->w;
  tsb->data.g      = t->g;

  /* Unlock when everything reached global visibility */
  asm volatile ("membar #StoreStore\n" ::: "memory");
  tsb->tag.lock = 0;
}

/**
 * @brief Load a PTE into the iTSB.
 *
 * @param t Page table entry.
 */
void load_pte_into_itsb(pte_t *t) {
  tsb_entry_t *tsb;
  uint32_t offset;

  offset = ((t->va >> PAGE_SHIFT) & TSB_OFFSET_MASK) << 3;
  tsb = &itsb[offset];

  /* Again, lock before we do anything else. */
  tsb->tag.lock = 1;
  asm volatile ("membar #StoreStore\n" ::: "memory");

  tsb->tag.ctx = t->ctx; /* as in process ID */
  tsb->tag.va  = (uint64_t) t->va >> TTE_VA_SHIFT;

  tsb->data.v      = 1;
  tsb->data.size   = 0; /* 8K */
  tsb->data.nfo    = 0; /* unused */
  tsb->data.ie     = 0; /* unused */
  tsb->data.soft2  = 0; /* unused */
  tsb->data.diag   = 0; /* unused */
  tsb->data.pa     = t->pa >> PAGE_SHIFT;
  tsb->data.soft   = 0; /* unused */
  tsb->data.l      = 0; /* unlocked */
  tsb->data.cp     = t->c;
  tsb->data.cv     = t->c;
  tsb->data.e      = 0; /* unused */
  tsb->data.p      = t->p;
  tsb->data.w      = t->w;
  tsb->data.g      = t->g;

  /* Unlock when everything reached global visibility */
  asm volatile ("membar #StoreStore\n" ::: "memory");
  tsb->tag.lock = 0;
}

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
