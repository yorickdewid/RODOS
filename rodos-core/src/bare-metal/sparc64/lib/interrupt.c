/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Interrupt service routines to register and dispatch handlers.
 */

#include <asi.h>
#include <printk.h>
#include <oplib.h>
#include <trap.h>
#include <param.h>

struct {
  funcptr_t handler;
  const char *name;
} isr_table[MAX_IRQ_LEVEL];

/**
 * @brief Register an interrupt handler.
 *
 * @param pil Interrupt level to be serviced.
 * @param name Name of the Interrupt type.
 * @param handler Function pointer to the handler.
 */
void isr_register(int pil, const char *name, funcptr_t handler) {
  isr_table[pil-1].handler = handler;
  isr_table[pil-1].name = name;
}

const char *isr_getname(int pil) {
  return isr_table[pil-1].name;
}

/**
 * @brief IRQ handler, dispatches a registered handler.
 *
 * @param pil Processor interrupt level.
 */
void __attribute__ ((noinline)) irq_handler(int pil) {

  /* clear softint */
  if(pil == 14)
    clear_softint((1 << pil)|1); /* softint + TICK_INT */
  else
    clear_softint(1 << pil);

  /* call handler if present */
  if(!isr_table[pil-1].handler)
    printk("[%02d] unserviced interrupt\n", pil);
  else {
    /* debugging */
    /*printk("[%02d] dispatching %s interrupt handler\n",
        pil,
        isr_getname(pil));*/
    isr_table[pil-1].handler();
  }
}

/**
 * @brief Interrupt vector handler, TODO we just acknowledge for now.
 */
void vec_interrupt() {
  uint64_t intr_receive; /* IRQ vector receive status */
  uint64_t data0, data1, data2;

  asm volatile("ldxa [%1] %2, %0\n"
      : "=r" (intr_receive)
      : "r"  (0), "i" (ASI_INTR_RECEIVE));

  asm volatile("ldxa [%1] %2, %0\n"
      : "=r" (data0)
      : "r"  (0x40), "i" (ASI_INTR_R));

  asm volatile("ldxa [%1] %2, %0\n"
      : "=r" (data1)
      : "r"  (0x50), "i" (ASI_INTR_R));

  asm volatile("ldxa [%1] %2, %0\n"
      : "=r" (data2)
      : "r"  (0x60), "i" (ASI_INTR_R));

  /* TODO: call handler, handler register facility */

  asm volatile("stxa %0, [%1] %2\n"
      "membar #Sync"
      :
      : "r" (0), "r" (0),
      "i" (ASI_INTR_RECEIVE));
}

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
