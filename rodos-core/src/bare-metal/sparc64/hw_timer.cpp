/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief RODOS interfaces, time keeping and tick interrupts.
 */

#include <rodos.h>
#include <hw_timer.h>
#include <hw_specific.h>
#include <context.h>
#include <params.h>
#include <oplib.h>
#include <trap.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

extern long long timeToTryAgainToSchedule;
extern bool isSchedulingEnabled;

/** Global tick counter represents ticks since startup. */
uint64_t tick_counter;
/** Tick interval represents the number of ticks between timer interrupts */
uint64_t tick_interval;
/** Multiplier that represents the nanoseconds per tick on this machine. */
uint64_t tick_nspt;

/**
 * @brief Compute the number of nanoseconds since startup.
 *
 * @return Nanoseconds since system startup.
 */
long long unsigned int hwGetNanoseconds() {
  return (tick_counter*tick_nspt);
}

/* abslute time (from host) stuff like pseudorandom number generators **/
long long unsigned int hwGetAbsoluteNanoseconds() {
  return hwGetNanoseconds(); /* + realtime; */
}

/**
 * @brief Tick interrupt handler.
 */
void __attribute__ ((noinline)) tick_handler() {
  uint64_t ticks;

  // if you don't need time events, comment this out to improve performance
  long long timeNow = NOW();
  TimeEvent::propagate(timeNow);

  /* reset tick compare registers */
  asm volatile("wr %%g0, %0, %%tick_cmpr\n"
      :
      : "r" (tick_interval));

  /* read tick register and reset it directly afterwards */
  asm volatile(
      "rd %%tick, %0\n"
      "wrpr %%g0, %1, %%tick\n"
      : "=r" (ticks)
      : "i" (0));

  /* update global tick counter */
  tick_counter+=ticks;

  /* initiates a preemptive context switch only if
   *    -we get here from usermode
   *    -scheduling is enabled
   *    -timeToTryAgainToSchedule time is reached
   */
  if(is_supervisor() || !isSchedulingEnabled || (NOW() < timeToTryAgainToSchedule))
    return;

  __asmSaveContext();
}
/** Nothing to do here, Timer::init() will do everything necessary. */
void hwInitTime() {}

/** Some initial value, this gets overwritten pretty quickly after boot. */
long long Timer::microsecondsInterval = 1;

/**
 * @brief Initialize the tick counter and calculate the timer interval.
 */
void Timer::init() {
  /* startup equals zero ticks */
  tick_counter = 0;

  /* basically this is just
   *    (1000000000LL / clock-frequency)
   * but in case of frequencies like 168MHz we need the resulting number to
   * be rounded up. So this becomes
   *    ((1000000000LL*10 / clock-frequency) + 5) / 10
   */
  tick_nspt = ((1000000000LL*10/prom_cpus[0].clock)+5)/10;

  /* register an interrupt trap handler */
  isr_register(14, "tick", tick_handler);

  /* update interval according to settings and clock-frequency */
  setInterval(PARAM_TIMER_INTERVAL);
 
  /* debugging 
  printk("Timer: Timer Interval     =%dus\n"
         "       Ticks per Invertal =%d\n"
         "       Frequency          =%dHz\n"
         "       Nanosec per Tick   ~%dns\n",
      PARAM_TIMER_INTERVAL,
      tick_interval,
      prom_cpus[0].clock / tick_interval,
      tick_nspt);*/

  /* Start the timer:
   * This will trigger a level 14 tick interrupt when the interval is reached.
   * A software interrupt is generated when the counter field of the tick
   * register reaches the value of the tick_cmpr register.
   */
  asm volatile("wr %%g0, %0, %%tick_cmpr\n"
      :
      : "r" (tick_interval));

  asm volatile("wrpr %%g0, %0, %%tick\n"
      :
      : "i" (0));
}

/* Used to disable/enable timer interrupts during context switching. We take
 * care of that ourselves so there is no need to do anything here.
 */
void Timer::start() {}
void Timer::stop() {}

/**
 * Initially RODOS calls Timer::setInverval(PARAM_TIMER_INTERVAL).
 */
void Timer::setInterval(const long long us) {
   /* ticks per interval */
  tick_interval = us * prom_cpus[0].clock / 1000000;

  /* lets also update the member variable */
  microsecondsInterval = tick_interval;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
