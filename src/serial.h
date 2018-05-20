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
#ifndef _MBPOLL_SERIAL_H_
#define _MBPOLL_SERIAL_H_

/**
 * @enum eSerialDataBits
 * @brief Nombre de bits de données
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
 * @brief Parité
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
 * @brief Nombre de bits de stop
 */
typedef enum {
  SERIAL_STOPBIT_ONE = 1,
  SERIAL_STOPBIT_TWO = 2,
  SERIAL_STOPBIT_ONEHALF = 3,
  SERIAL_STOPBIT_UNKNOWN
} eSerialStopBits;

/**
 * @enum eSerialFlow
 * @brief Type de contrôle de flux
 * 
 * @warning Les modes RS485 ne sont pas gérés par termios (spécifiques à linux)
 */
typedef enum {

  SERIAL_FLOW_NONE = ' ', /**< Pas de contrôle de flux */
  SERIAL_FLOW_RTSCTS = 'H', /**< Contrôle de flux matériel RTS/CTS */
  SERIAL_FLOW_XONXOFF = 'S', /**< Contrôle de flux logiciel XON/XOFF */
  SERIAL_FLOW_RS485_RTS_AFTER_SEND = 'R', /**< RS485 half-duplex, RTS au niveau logique 0 après transmission */
  SERIAL_FLOW_RS485_RTS_ON_SEND = 'r', /**< RS485 half-duplex, RTS au niveau logique 0 pendant transmission */
  SERIAL_FLOW_UNKNOWN = -1
} eSerialFlow;

/* structures =============================================================== */
/**
 * Configuration d'un port série
 */
typedef struct xSerialIos {
  long baud; /**< Vitesse de transmission, négative si erreur */
  eSerialDataBits dbits; /**< Bits de données */
  eSerialParity parity; /**< Parité */
  eSerialStopBits sbits;/**< Bits de stop */
  eSerialFlow flow;/**< Contrôle de flux */
  int flag; /**< Réservé pour un usage futur */
} xSerialIos;

/* internal public functions ================================================ */

/**
 * Chaîne de caractère décrivant la configuration correspondant aux paramètres
 *
 * Le format est BBBBBB-DPSF avec :
 * - BBBBBB Baudrate
 * - D Data bits (5, 6, 7 ,8)
 * - P Parité (N, E, O)
 * - S Stop (1, 2)
 * - F Flow (H, S, R, r)
 * .
 * @return la représentation de la configuration sous forme de string, NULL si
 * erreur.
 */
const char * sSerialAttrToStr (const xSerialIos * xIos);

/**
 *  Lecture du type de contrôle de flux en cours sous forme "lisible"
 *
 * @param fd le descripteur de fichier du port
 */
const char * sSerialGetFlowStr (int fd);

/**
 * Chaîne de caractère décrivant la configuration du port
 *
 * Le format est BBBBBB-DPSF avec :
 * - BBBBBB Baudrate
 * - D Data bits (5, 6, 7 ,8)
 * - P Parité (N, E, O)
 * - S Stop (1, 2)
 * - F Flow (H, S, R, r)
 * .
 * @param fd le descripteur de fichier du port
 * @return la représentation de la configuration sous forme de string, NULL si
 * erreur.
 */
const char * sSerialAttrStr (int fd);

/**
 *  Chaîne de caractère correspondant à une valeur de contrôle de flux
 */
const char * sSerialFlowToStr (eSerialFlow eFlow);

/**
 *  Chaîne de caractère correspondant à une valeur de parité
 */
const char * sSerialParityToStr (eSerialParity eParity);

/**
 *  Chaîne de caractère correspondant à une valeur de bits de données
 */
const char * sSerialDataBitsToStr (eSerialDataBits eDataBits);

/**
 *  Chaîne de caractère correspondant à une valeur de bits de stop
 */
const char * sSerialStopBitsToStr (eSerialStopBits eStopBits);

/* ========================================================================== */
#endif /* _MBPOLL_SERIAL_H_ */
