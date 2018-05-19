/**
 * Copyright (c) 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _MBPOLL_GPIO_RTS_H_
#define _MBPOLL_GPIO_RTS_H_

#include <modbus/modbus.h>

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC int  init_gpio_rts (int num, int act);
EXTERNC void set_gpio_rts (modbus_t *ctx, int on);

#endif /* _MBPOLL_GPIO_RTS_H_ defined */
