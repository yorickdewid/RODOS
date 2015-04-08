/*
 * @file MPCore.h
 * @date 2013/07/24 12:00
 * @author Tiago Duarte
 * @description: Multiple Core framework for handling a Multiple-Core environment.
 */

#ifndef __MPCORE_H__
#define __MPCORE_H__

#include "workspace.h"

typedef void (*function_to_run) (void);

#if defined(__cplusplus)
extern "C" {
#endif

void start_core1(function_to_run func);
void start_core2(function_to_run func);
void start_core3(function_to_run func);

int start_rodos_core1();
int start_rodos_core2();
int start_rodos_core3();

void reset_core1();
void reset_core2();
void reset_core3();

void disable_core1();
void disable_core2();
void disable_core3();

#if defined(__cplusplus)
}
#endif

#endif
