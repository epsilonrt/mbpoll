/* Copyright (c) 2015-2025 Pascal JEAN, All rights reserved.
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

#ifndef _MBPOLL_H_
#define _MBPOLL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <modbus.h>
#include <stdbool.h>
#include <stdint.h>
#include "serial.h"
#include "mbpoll-config.h"

// Forward declaration
struct xChipIoContext;

/* types ==================================================================== */
typedef enum {
  eModeRtu,
  eModeTcp,
  eModeUnknown = -1,
} eModes;

typedef enum {
  eFuncCoil = 0,
  eFuncDiscreteInput = 1,
  eFuncInputReg = 3,
  eFuncHoldingReg = 4,
  eFuncUnknown = -1,
} eFunctions;

typedef enum {
  eFormatDec,
  eFormatInt16,
  eFormatHex,
  eFormatString,
  eFormatInt,
  eFormatFloat,
  eFormatBin,
  eFormatUnknown = -1,
} eFormats;

/* structures =============================================================== */
typedef struct xMbPollContext {

  // Parameters
  eModes eMode;
  eFunctions eFunction;
  eFormats eFormat;
  int * piSlaveAddr;
  int iSlaveCount;
  int * piStartRef;
  int iStartCount;
  int iCount;
  int iPollRate;
  double dTimeout;
  char * sTcpPort;
  char * sDevice;
  xSerialIos xRtu;
  int iRtuBaudrate;
  eSerialDataBits eRtuDatabits;
  eSerialStopBits eRtuStopbits;
  eSerialParity eRtuParity;
  bool bIsVerbose;
  bool bIsPolling;
  int  iRtuMode;
  bool bIsWrite;
  bool bIsReportSlaveID;
  bool bIsDefaultMode;
  int iPduOffset;
  bool bWriteSingleAsMany;
  bool bIsChipIo;
  bool bIsBigEndian;
  bool bIsQuiet;
  bool bPrintHex;
  bool bEnableMaxSlaveQuirk;
  bool bEnableReplyToBroadcastQuirk;
#ifdef MBPOLL_GPIO_RTS
  int iRtsPin;
#endif

  // Working variables
  modbus_t * xBus;
  void * pvData;
  int iTxCount;
  int iRxCount;
  int iErrorCount;

  struct xChipIoContext * xChip;
} xMbPollContext;

/* helper functions ========================================================= */
void vCheckIntRange (const char * sName, int i, int min, int max);
int iGetInt (const char * sName, const char * sNum, int iBase);
#include "utils.h"

#ifdef __cplusplus
}
#endif

#endif /* _MBPOLL_H_ */
