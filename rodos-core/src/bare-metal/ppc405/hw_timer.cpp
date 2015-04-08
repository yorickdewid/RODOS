/**
 * @file hw_timer.cpp
 * @author Lutz Dittrich, Andre Bartke <dev@bartke.cc>,
 *         Tobias Mikschl <tobias.mikschl@uni-wuerzburg.de>
 *
 * @brief RODOS interfaces, time keeping and tick interrupts.
 */

#include <hw_specific.h>
#include <registers.h>
#include <rodos.h>

#include <stdint.h>

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

void hwInitTime() {}

uint64_t hwGetNanoseconds() {
  uint32_t tl, th;

  asm volatile (
    "mftbu 0\n"
    "mftbl 11\n"
    "mftbu 12\n"
    "cmpw 0, 12\n"
    "bne 0, $-16\n"
    "mr %0, 11\n"
    "mr %1, 12\n"
    : "=r" (tl), "=r" (th)
    : // no inputs
    : "0", "11", "12"
  );

  uint64_t t =
    ((uint64_t)th << (8 * sizeof(th)))
    | (uint64_t)tl;
#ifdef USE_DEV_BOARD_BIROS
	t = (t * 25) / 2;
#elif USE_DEV_BOARD_OLDSSMMU
  t = t * 10;
#endif

  return t;
}

int64_t Timer::microsecondsInterval = PARAM_TIMER_INTERVAL;

void Timer::init() {
  Timer::stop();

  /* for PIT interrupts the following bits must be set:
   * TCR[PIE] <= 1
   * MSR[EE]  <= 1
   */

  /* timer control register (TCR) */
  asm volatile (
    "mftcr  0\n"
    "or     0, 0, %0\n"
    "mttcr  0\n"
    : // no outputs
    : "r" (TCR_PIE|TCR_ARE)
    : "0"
  ); // enable PIE and ARE

  /* machine state register (MSR) */
  asm volatile (
    "mfmsr  0\n"
    "or     0, 0, %0\n"
    "mtmsr  0\n"
    : // no outputs
    : "r" (MSR_EE)
    : "0"
  ); // enable EE
}

void Timer::start() {
  Timer::stop();

  /* the programmable interval timer starts decrementing when it is loaded
   * with a non-zero value. A PIT event occurs when it is decremented while
   * containing the value 1.
   */
#ifdef USE_DEV_BOARD_BIROS
	  asm volatile (
    "mtpit  %0\n"
    : // no outputs
    : "r" ((unsigned int)Timer::microsecondsInterval * 80)
  ); // set PIT period
#elif USE_DEV_BOARD_OLDSSMMU
    asm volatile (
    "mtpit  %0\n"
    : // no outputs
    : "r" ((unsigned int)Timer::microsecondsInterval * 100)
  ); // set PIT period
#endif


}

void Timer::stop() {
  asm volatile (
    "li     0, 0\n"
    "mtpit  0\n"
    : // no outputs
    : // no inputs
    : "0"
  ); // stop the PIT

  /* remove pending interrupts */
  asm volatile (
    "lis    0, 0x0800\n"
    "mttsr  0\n"
    : // no outputs
    : // no inputs
    : "0"
  ); // clear PIS
}

void Timer::setInterval(const int64_t us) {
  Timer::microsecondsInterval = us;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif
