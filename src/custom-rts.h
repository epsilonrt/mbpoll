/* Copyright © 2015-2018 Pascal JEAN, All rights reserved.
 *
 * mbpoll is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mbpoll is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with mbpoll.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _MBPOLL_GPIO_RTS_H_
#define _MBPOLL_GPIO_RTS_H_

#include <modbus.h>

int  init_custom_rts (int num, int act);
void set_custom_rts (modbus_t *ctx, int on);

#endif /* _MBPOLL_GPIO_RTS_H_ defined */
