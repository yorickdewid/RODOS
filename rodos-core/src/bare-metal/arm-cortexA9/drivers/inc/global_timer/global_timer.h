#ifndef __GLOBAL_TIMER_H__
#define __GLOBAL_TIMER_H__

#include "workspace.h"

#define LOWER_COUNTER_REG	0
#define HIGHER_COUNTER_REG	1

#if defined(__cplusplus)
extern "C" {
#endif

void global_timer_start();
void global_timer_stop();

void global_timer_set_comparator(long long value);

long long global_timer_get_comp();
long long global_timer_get_value();

void gbl_tmr_enable();
void gbl_tmr_disable();

void gbl_tmr_disable_comp();
void gbl_tmr_enable_comp();

void gbl_tmr_disable_irq();
void gbl_tmr_enable_irq();

void gbl_tmr_disable_auto_inc();
void gbl_tmr_enable_auto_inc();

void gbl_tmr_clear_event();


#if defined(__cplusplus)
}
#endif

#endif
