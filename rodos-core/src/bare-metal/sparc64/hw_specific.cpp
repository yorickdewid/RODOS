/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief RODOS intefaces, hardware specifics and context initialization.
 */

#include <string_pico.h>
#include <hw_specific.h>
#include <rodos.h>
#include <context.h>
#include <params.h>
#include <printk.h>
#include <oplib.h>
#include <trap.h>
#include <hw_timer.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

long volatile taskRunning= 0;

void hwInit() {
  // now that all global constructors have run, it is safe to enable interrupts
  enable_interrupts();
}

/**
 * @brief Create a new context in a given stack.
 * The target to call is always 'threadStartupWrapper' with the 'object'
 * pointer as an argument.
 *
 * @param stack stack top pointer
 * @param object object pointer for the callback function
 *
 * @return new context
 */
long* hwInitContext(long* stack, void* object) {
  context_t *ctx;

  /*
   * FIXME: Normally the stack size is DEFAULT_STACKSIZE. There is however
   * a tutorial called threadchecker where the size differs. If we want to be
   * able to create different stack sizes we would pass the size as an
   * additional argument to this function (in a perfect solution). On SPARC
   * the stack grows in the negative direction so the current implementation
   * is somehow flawed since we always add a constant. -AndreB
   */
  uintptr_t tos = (uintptr_t) stack + DEFAULT_STACKSIZE;
  tos = ALIGN_FLOOR(tos, STACK_FRAME_ALIGNMENT);

  /* stack is now aligned, we have to subtract a stack frame which is the
   * context_t stuct. Additionally we substact the stack bias from %sp and
   * %fp in order to comply with the SPARC ABI.
   */
  ctx = (context_t *) (tos - ALIGN_CEIL(sizeof(*ctx), 8));
  /* zero out the current frame */
  memset(ctx, 0, sizeof(*ctx));

  ctx->id = ctx_number++;
  ctx->sp = (uintptr_t)(ctx) - STACK_BIAS - STACK_FRAME_SZ;
  ctx->o0 = (uintptr_t) object;
  ctx->pc = (uintptr_t) threadStartupWrapper;

  /* debugging  */
  /*printk(
      "Debug: Thread %04d (ctx=0x%x)\n"
      "  stack=0x%x-0x%x sp=0x%x pc=0x%x\n",
      ctx->id, ctx,
      tos-DEFAULT_STACKSIZE, tos, ctx->sp, ctx->pc);*/

  return ((long*) ctx);
}

extern "C" void __cxa_finalize(void *);
void hwResetAndReboot() {
  __cxa_finalize(0);
  prom_reboot("reset-all");
}

/** stub on bare-metal */
void sp_partition_yield() {}
/** stub on most bare-metal targets */
void startIdleThread() {}

// watchdog stubs
static long long timeOfLastTriggerWatchdog = 0;
void hwInitWatchdog(long intervalMilliseconds) {}
void hwTriggerWatchdog() { timeOfLastTriggerWatchdog = NOW(); }

#ifndef NO_RODOS_NAMESPACE
}
#endif

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
