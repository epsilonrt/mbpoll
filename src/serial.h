/* Copyright © 2015-2025 Pascal JEAN, All rights reserved.
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
#ifndef MBPOLL_SERIAL_H
#define MBPOLL_SERIAL_H

/**
 * @enum eSerialDataBits
 * @brief Number of data bits
 */
typedef enum {
  SERIAL_DATABIT_5 = 5,
  SERIAL_DATABIT_6 = 6,
  SERIAL_DATABIT_7 = 7,
  SERIAL_DATABIT_8 = 8,
  SERIAL_DATABIT_UNKNOWN = -1
} eSerialDataBits;

/**
 * @enum eSerialParity
 * @brief Parity
 */
typedef enum {
  SERIAL_PARITY_NONE = 'N',
  SERIAL_PARITY_EVEN = 'E',
  SERIAL_PARITY_ODD  = 'O',
  SERIAL_PARITY_SPACE = 'S',
  SERIAL_PARITY_MARK = 'M',
  SERIAL_PARITY_UNKNOWN = -1
} eSerialParity;

/**
 * @enum eSerialStopBits
 * @brief Number of stop bits
 */
typedef enum {
  SERIAL_STOPBIT_ONE = 1,
  SERIAL_STOPBIT_TWO = 2,
  SERIAL_STOPBIT_ONEHALF = 3,
  SERIAL_STOPBIT_UNKNOWN
} eSerialStopBits;

/**
 * @enum eSerialFlow
 * @brief Flow control type
 *
 * @warning RS485 modes are not handled by termios (Linux-specific)
 */
typedef enum {

  SERIAL_FLOW_NONE = ' ', /**< No flow control */
  SERIAL_FLOW_RTSCTS = 'H', /**< Hardware flow control RTS/CTS */
  SERIAL_FLOW_XONXOFF = 'S', /**< Software flow control XON/XOFF */
  SERIAL_FLOW_RS485_RTS_AFTER_SEND = 'R', /**< RS485 half-duplex, RTS logic 0 after transmission */
  SERIAL_FLOW_RS485_RTS_ON_SEND = 'r', /**< RS485 half-duplex, RTS logic 0 during transmission */
  SERIAL_FLOW_UNKNOWN = -1
} eSerialFlow;

/* structures =============================================================== */
/**
 * Serial port configuration
 */
typedef struct xSerialIos {
  long baud; /**< Baud rate, negative on error */
  eSerialDataBits dbits; /**< Data bits */
  eSerialParity parity; /**< Parity */
  eSerialStopBits sbits;/**< Stop bits */
  eSerialFlow flow;/**< Flow control */
  int flag; /**< Reserved for future use */
} xSerialIos;

/* internal public functions ================================================ */

/**
 * Returns a string describing the configuration from parameters
 *
 * Format is BBBBBB-DPSF where:
 * - BBBBBB Baudrate
 * - D Data bits (5, 6, 7, 8)
 * - P Parity (N, E, O)
 * - S Stop (1, 2)
 * - F Flow (H, S, R, r)
 *
 * @return string representation of configuration, NULL on error
 */
const char * sSerialAttrToStr (const xSerialIos * xIos);

/**
 * Returns a string describing the port configuration
 *
 * Format is BBBBBB-DPSF where:
 * - BBBBBB Baudrate
 * - D Data bits (5, 6, 7, 8)
 * - P Parity (N, E, O)
 * - S Stop (1, 2)
 * - F Flow (H, S, R, r)
 *
 * @param fd file descriptor of the port
 * @return string representation of configuration, NULL on error
 */
const char * sSerialAttrStr (int fd);

/**
 * Returns string corresponding to a flow control value
 */
const char * sSerialFlowToStr (eSerialFlow eFlow);

/**
 * Returns string corresponding to a parity value
 */
const char * sSerialParityToStr (eSerialParity eParity);

/**
 * Returns string corresponding to a data bits value
 */
const char * sSerialDataBitsToStr (eSerialDataBits eDataBits);

/**
 * Returns string corresponding to a stop bits value
 */
const char * sSerialStopBitsToStr (eSerialStopBits eStopBits);

/* ========================================================================== */
#endif /* MBPOLL_SERIAL_H */
