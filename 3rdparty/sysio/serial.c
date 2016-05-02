/*
 * sysio/serial.c
 * @brief Liaison série (Implémentation, partie portable)
 * Copyright (c) 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <sysio/serial.h>
#include <sysio/compat.h>

/* public variables ========================================================= */
const char sUnknown[] = "Unknown" ;

/* private variables ======================================================== */
static const char sNone[] = "none";

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
const char *
sSerialAttrToStr (const xSerialIos * xIos) {
  // BBBBBBBB-DPSF
  static char str[16];
  snprintf (str, 16, "%10ld-%1d%c%1d%c",
            xIos->baud,
            xIos->dbits,
            xIos->parity,
            xIos->sbits,
            xIos->flow);
  return str;
}

// -----------------------------------------------------------------------------
const char *
sSerialFlowToStr (eSerialFlow eFlow) {

  switch (eFlow) {

    case SERIAL_FLOW_NONE:
      return sNone;
    case SERIAL_FLOW_RTSCTS:
      return "hardware";
    case SERIAL_FLOW_XONXOFF:
      return "software";
    case SERIAL_FLOW_RS485_RTS_AFTER_SEND:
      return "rs485 rts after send";
    case SERIAL_FLOW_RS485_RTS_ON_SEND:
      return "rs485 rts on send";
    default:
      break;
  }
  return sUnknown;
}

// -----------------------------------------------------------------------------
const char *
sSerialParityToStr (eSerialParity eParity) {

  switch (eParity) {

    case SERIAL_PARITY_NONE:
      return sNone;
    case SERIAL_PARITY_EVEN:
      return "even";
    case SERIAL_PARITY_ODD:
      return "odd";
    case SERIAL_PARITY_SPACE:
      return "space";
    case SERIAL_PARITY_MARK:
      return "mark";
    default:
      break;
  }
  return sUnknown;
}

// -----------------------------------------------------------------------------
const char *
sSerialDataBitsToStr (eSerialDataBits eDataBits) {

  switch (eDataBits) {

    case SERIAL_DATABIT_5:
      return "5";
    case SERIAL_DATABIT_6:
      return "6";
    case SERIAL_DATABIT_7:
      return "7";
    case SERIAL_DATABIT_8:
      return "8";
    default:
      break;
  }
  return sUnknown;
}

// -----------------------------------------------------------------------------
const char *
sSerialStopBitsToStr (eSerialStopBits eStopBits) {

  switch (eStopBits) {

    case SERIAL_STOPBIT_ONE:
      return "1";
    case SERIAL_STOPBIT_TWO:
      return "2";
    case SERIAL_STOPBIT_ONEHALF:
      return "1.5";
    default:
      break;
  }
  return sUnknown;
}

/* ========================================================================== */
