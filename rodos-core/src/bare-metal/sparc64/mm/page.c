/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Hashed inverted page table (TODO).
 *
 * NOTE: work in progress, this is not usable at the moment...
 *
 * We use an inverted page table IPT approach in combination with hashing
 * where we only store entries for each physical page. A fixed-size table
 * contains as many entries as there are physical memory frames. For 128M of
 * memory and 8K frames this results in an HIPT with 16K entries
 * (128M/8K = 16K). The page table only contains valid mappings.
 */

#include <stddef.h>
#include <param.h>
#include <page.h>

pte_t page_table[16384]; // FIXME

void page_insert(uint32_t ctx, uint64_t vpn, uint64_t ppn, uint8_t flags) {
  pte_t *page;
  uint32_t index;

  index = page_hash_func(ctx+vpn, 14); /* FIXME: see above */
  page = &page_table[index];

  while(page->v != 0 && page->next != NULL) {
      /* FIXME: allocate */
      page = page->next; /* follow the chain */
  }

  page->ctx = ctx;
  page->va = vpn << PAGE_SHIFT;
  page->pa = ppn << PAGE_SHIFT;
  page->g = (flags & PAGE_G);
  page->w = (flags & PAGE_W);
  page->p = (flags & PAGE_P);
  page->c = (flags & PAGE_C);
  page->v = 1;
}

void page_remove(uint32_t ctx, uint64_t vpn) {
  pte_t *page;
  uint32_t index;

  index = page_hash_func(ctx+vpn, 14); /* FIXME: see above */
  page = &page_table[index];

  while((page->va >> PAGE_SHIFT) != vpn) {
    if(page->next) {
      page = page->next;
    } else {
      /* should not happen */
    }
  }
  /* found, set invalid */
  page->v = 0;
}


void page_lookup(uint32_t ctx, uint64_t vpn) {
  pte_t *page;
  uint32_t index;

  index = page_hash_func(ctx+vpn, 14); /* FIXME: see above */
  page = &page_table[index];

  while((page->va >> PAGE_SHIFT) != vpn) {
    if(page->next) {
      page = page->next;
    } else {
      /* trap: page fault */
    }
  }
}

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */

