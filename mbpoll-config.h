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
#ifndef _CONFIG_H_
#define _CONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif

/* constants ================================================================ */
#define RTU_SLAVEADDR_MIN 1
#define TCP_SLAVEADDR_MIN 0
#define SLAVEADDR_MAX     255
#define STARTREF_MIN      1
#define STARTREF_MAX      65536
#define NUMOFVALUES_MIN   1
#define NUMOFVALUES_MAX   125
#define POLLRATE_MIN      100
#define TIMEOUT_MIN       0.01
#define TIMEOUT_MAX       10.0
#define TCP_PORT_MIN      1
#define TCP_PORT_MAX      65535
#define RTU_BAUDRATE_MIN  1200
#define RTU_BAUDRATE_MAX  921600
#define CHIPIO_SLAVEADDR_MIN 0x03
#define CHIPIO_SLAVEADDR_MAX 0x77

/* default values =========================================================== */
#define DEFAULT_MODE          eModeTcp
#define DEFAULT_FUNCTION      eFuncHoldingReg
#define DEFAULT_SLAVEADDR     1
#define DEFAULT_STARTREF      1
#define DEFAULT_NUMOFVALUES   1
#define DEFAULT_POLLRATE      1000
#define DEFAULT_TIMEOUT       1.0
#define DEFAULT_TCP_PORT      "502"
#define DEFAULT_RTU_BAUDRATE  19200
#define DEFAULT_RTU_DATABITS  SERIAL_DATABIT_8
#define DEFAULT_RTU_STOPBITS  SERIAL_STOPBIT_ONE
#define DEFAULT_RTU_PARITY    SERIAL_PARITY_EVEN
#define DEFAULT_CHIPIO_SLAVEADDR  0x46
#define DEFAULT_CHIPIO_IRQPIN     GPIO_GEN6

/* ========================================================================== */
#ifdef __cplusplus
}
#endif
#endif /* _CONFIG_H_ defined */
