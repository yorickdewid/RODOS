/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Translation lookaside buffer (TODO).
 */

#include <param.h>
#include <printk.h>
#include <tlb.h>

void load_pte_into_dtlb(pte_t *t) {
  tte_tag tag;
  tte_data data;

  tag.g   = t->g;
  tag.ctx = t->ctx; /* as in process ID */
  /* 31 bit page number, not 44 bit address */
  tag.va  = t->va >> PAGE_SHIFT;
  write_dtlb_tag(tag.value);

  data.v      = 1;
  data.size   = 0; /* 8K */
  data.nfo    = 0; /* unused */
  data.ie     = 0; /* unused */
  data.soft2  = 0; /* unused */
  data.diag   = 0; /* unused */
  /* 28 bit page number, not 41 bit address */
  data.pa     = t->pa >> PAGE_SHIFT;
  data.soft   = 0; /* unused */
  data.l      = 0; /* unlocked */
  data.cp     = t->c;
  data.cv     = t->c;
  data.e      = 0; /* unused */
  data.p      = t->p;
  data.w      = t->w;
  data.g      = t->g;
  write_dtlb_data_in(data.value);
}


void load_pte_into_itlb(pte_t *t) {
  tte_tag tag;
  tte_data data;

  tag.g   = t->g;
  tag.ctx = t->ctx; /* as in process ID */
  tag.va  = t->va >> PAGE_SHIFT;
  write_itlb_tag(tag.value);

  data.v      = 1;
  data.size   = 0; /* 8K */
  data.nfo    = 0; /* unused */
  data.ie     = 0; /* unused */
  data.soft2  = 0; /* unused */
  data.diag   = 0; /* unused */
  data.pa     = t->pa >> PAGE_SHIFT;
  data.soft   = 0; /* unused */
  data.l      = 0; /* unlocked */
  data.cp     = t->c;
  data.cv     = 0;
  data.e      = 0; /* unused */
  data.p      = t->p;
  data.w      = t->w;
  data.g      = t->g;
  write_itlb_data_in(data.value);
}

void invalidate_dtlb_all(void) {
  tte_data data;
  int i;

  for(i = 0; i<MAX_DTLB_ENTRIES; i++) {
    data.value = read_dtlb_data(i);
    /* only invalidate if unlocked */
    if(!data.l) {
      data.v = 0;
      write_dtlb_data_access(i, data.value);
    }
  }
}

void invalidate_itlb_all(void) {
  tte_data data;
  int i;

  for(i = 0; i<MAX_ITLB_ENTRIES; i++) {
    data.value = read_itlb_data(i);
    /* only invalidate if unlocked */
    if(!data.l) {
      data.v = 0;
      write_itlb_data_access(i, data.value);
    }
  }
}


/* The following functions are for debugging purposes. */

void debug_print_sfsr() {
  tlb_sfsr sfsr;
  tlb_sfar sfar;

  sfsr.value = read_dtlb_sfsr();
  sfar.value = read_dtlb_sfsr();

  printk("\n  ==== SFSR ====\n");
  printk(" ASI=%02x FT=%x E=%d CT=%d PR=%d W=%d OW=%d FV=%d\n",
    sfsr.asi,
    sfsr.ft,
    sfsr.e,
    sfsr.ct,
    sfsr.pr,
    sfsr.w,
    sfsr.ow,
    sfsr.fv);

  printk("\n  ==== SFAR ====\n");
  printk(" FaultAddress=0x%x\n", sfar.value);
}

void debug_print_dtlb() {
  tlb_t dtlb;
  int i;

  for(i = 0; i<MAX_DTLB_ENTRIES; i++) {
    dtlb.tag[i].value = read_dtlb_tag(i);
    dtlb.data[i].value = read_dtlb_data(i);
  }

  printk("\n  ==== DMMU ====\n");
  printk("  # CTXT  ------- VA ------- V SIZE ------- PA ------- L CP CV E P W G\n");
  for(i = 0; i<MAX_DTLB_ENTRIES; i++) {
    if(dtlb.data[i].v)
      printk("%3d %04d  0x%016x %d %4d 0x%016x %d %d  %d  %d %d %d %d\n",
        i,
        dtlb.tag[i].ctx,
        (uint64_t) dtlb.tag[i].va << PAGE_SHIFT,
        dtlb.data[i].v,
        8 << 3*dtlb.data[i].size,
        (uint64_t) dtlb.data[i].pa << PAGE_SHIFT,
        dtlb.data[i].l,
        dtlb.data[i].cp,
        dtlb.data[i].cv,
        dtlb.data[i].e,
        dtlb.data[i].p,
        dtlb.data[i].w,
        dtlb.data[i].g);
  }
}

void debug_print_itlb() {
  tlb_t itlb;
  int i;

  for(i = 0; i<MAX_ITLB_ENTRIES; i++) {
    itlb.tag[i].value = read_itlb_tag(i);
    itlb.data[i].value = read_itlb_data(i);
  }

  printk("\n  ==== IMMU ====\n");
  printk("  # CTXT  ------- VA ------- V SIZE ------- PA ------- L CP CV E P W G\n");
  for(i = 0; i<MAX_ITLB_ENTRIES; i++) {
    if(itlb.data[i].v)
      printk("%3d %04d  0x%016x %d %4d 0x%016x %d %d  %d  %d %d %d %d\n",
        i,
        itlb.tag[i].ctx,
        (uint64_t) itlb.tag[i].va << PAGE_SHIFT,
        itlb.data[i].v,
        8 << 3*itlb.data[i].size, /* 8^size */
        (uint64_t) itlb.data[i].pa << PAGE_SHIFT,
        itlb.data[i].l,
        itlb.data[i].cp,
        itlb.data[i].cv,
        itlb.data[i].e,
        itlb.data[i].p,
        itlb.data[i].w,
        itlb.data[i].g);
  }
}

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */

