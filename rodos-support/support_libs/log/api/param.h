/*
 * param.h
 *
 *  Created on: May 31, 2013
 *      Author: user
 */

#ifndef _LOG_PARAM_H_
#define _LOG_PARAM_H_

// set default values for Logging Framework

#ifndef LOG_APP_ID
#define LOG_APP_ID								424298
#endif

#ifndef LOG_THREAD_PRIO_CONTROL
#define LOG_THREAD_PRIO_CONTROL                 50
#endif

#ifndef LOG_THREAD_PRIO_INPUT
#define LOG_THREAD_PRIO_INPUT                   100
#endif

#ifndef LOG_ITEM_MAX_C_STRING_LENGTH
#define LOG_ITEM_MAX_C_STRING_LENGTH            16
#endif

#ifndef LOG_ITEM_MAX_APP_NAME_LENGTH
#define LOG_ITEM_MAX_APP_NAME_LENGTH            16
#endif

#ifndef LOG_ITEM_MAX_THREAD_NAME_LENGTH
#define LOG_ITEM_MAX_THREAD_NAME_LENGTH         16
#endif

#ifndef LOG_BUFFER_SIZE_INPUT
#define LOG_BUFFER_SIZE_INPUT                   100
#endif

#ifndef LOG_BUFFER_SIZE_CONTROL_ITEM_RECEIVE
#define LOG_BUFFER_SIZE_CONTROL_ITEM_RECEIVE    20
#endif

#ifndef LOG_LEVEL_DEBUG_VALUE
#define LOG_LEVEL_DEBUG_VALUE                   10
#endif

#ifndef LOG_LEVEL_INFO_VALUE
#define LOG_LEVEL_INFO_VALUE                    20
#endif

#ifndef LOG_LEVEL_WARN_VALUE
#define LOG_LEVEL_WARN_VALUE                    30
#endif

#ifndef LOG_LEVEL_ERROR_VALUE
#define LOG_LEVEL_ERROR_VALUE                   40
#endif

#ifndef LOG_LEVEL_OFF_VALUE
#define LOG_LEVEL_OFF_VALUE                     50
#endif

#ifndef LOG_META_LEVEL_VALUE
#define LOG_META_LEVEL_VALUE                    50 // set to off
#endif

#ifndef LOG_BUFFER_SIZE_ITEM_RECEIVE
#define LOG_BUFFER_SIZE_ITEM_RECEIVE            100
#endif

#ifndef LOG_APP_ID_OUTPUT
#define LOG_APP_ID_OUTPUT                       424299
#endif

#ifndef LOG_THREAD_PRIO_SIMPLE_CONTROLLER
#define LOG_THREAD_PRIO_SIMPLE_CONTROLLER       50
#endif

#ifndef LOG_THREAD_PRIO_OUTPUT
#define LOG_THREAD_PRIO_OUTPUT                  100
#endif

#endif /* _LOG_PARAM_H_ */
