/*
 * The purpose of this file is to have the includes
 * to support the basic types, basic libraries, and so on.
 * And also to create a basic sdk for this project.
 */
#ifndef __WORKSPACE_H__
#define __WORKSPACE_H__

#include <stdint.h>
#include <stdbool.h>

/* RODOS includes */
#include "uart/uart.h"
#include "flexcan/flexcan.h"
#include "global_timer/global_timer.h"
#include "MPCore/MPCore.h"


/* Core functionalities */
#include "ccm_pll.h"
#include "cortex_a9.h"
#include "gic.h"
#include "interrupt.h"
#include "mmu.h"
#include "cache_l2c310.h"


/* General */
#include "arm_cp_registers.h"
#include "asm_defines.h"
#include "gic_registers.h"
#include "io.h"
#include "registers.h"
#include "regs.h"
#include "soc_memory_map.h"


/* For drivers support */
#include "registers/regsuart.h"
#include "registers/regssrc.h"		// for core management (boot and reboot)
#include "registers/regsflexcan.h"

#endif
