#ifndef _LED_H_
#define _LED_H_

/*
 * Copyright (c) 2011 Fraunhofer FIRST, Berlin
 * All rights reserved.
 *
 * @author Bernd Oestmann
 */

/*
 *	LED at PC.12 (Olimex Eval Boards)
 */
#ifdef __cplusplus
extern "C" {
#endif

void led_init();

void led(int on);

#ifdef __cplusplus
}
#endif

#endif // _LED_H_
