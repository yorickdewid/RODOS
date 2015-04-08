


/**
 * @file rodos-apis.h
 * @date 2008/06/06 15:57
 * @author Sergio Montenero
 *
 * Copyright 2008 DLR
 *
 */

#pragma once

#define RODOS_VERSION 119
#define RODOS_VERSION_TEXT "RODOS-119"

#include <stdint.h>

#include "hw_predefines.h"
// #include "macros_bits_bytes.h" due to conflicts with imported drivers, you habe to incllude it by yourself

#include "string_pico.h"
#include "debug.h"

#include "hostinfos.h"  /* architecture dependent */
#include "params.h"     /* architecture dependent */
#include "hw_stdlib.h"  /* architecture dependent: for printf, memcpy */
#include "hw_thread_defs.h"   /* architecture dependent defines, etg atomartiy */

#include "misc.h"
#include "checksumes.h"
#include "bytesex.h"

#include "listelement.h"

#include "initiator.h"

#include "timemodel.h"
#include "application.h"
#include "reserved_application_ids.h"
#include "thread.h"

#include "putter.h"
#include "fifo.h"
#include "commbuffer.h"

#include "semaphore.h"
#include "timeevent.h"
#include "timepoints.h"

#include "topic.h"
#include "reserved_topic_ids.h"
#include "subscriber.h"

#include "udp.h"

#include "yprintf.h"
#include "hw_postdefines.h"

#include "gateway.h"
