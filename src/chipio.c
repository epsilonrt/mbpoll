/* Copyright (c) 2015-2025 Pascal JEAN, All rights reserved. */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "mbpoll.h"
#include "chipio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
  #define strcasecmp _stricmp
  #define strncasecmp _strnicmp
  #define strcasestr strstr // Approximate, or implement proper strcasestr for Windows if needed
#else
  #include <strings.h>
#endif

#ifdef USE_CHIPIO
static const char sChipIoSlaveAddrStr[] = "chipio slave address";
static const char sChipIoIrqPinStr[] = "chipio irq pin";
#endif

bool bChipIoHandleOption(int opt, const char * optarg, xMbPollContext * ctx) {
#ifdef USE_CHIPIO
  if (ctx->xChip == NULL) {
    ctx->xChip = calloc(1, sizeof(xChipIoContext));
    if (!ctx->xChip) {
        vIoErrorExit("Memory allocation failed for chipio context");
    }
    ctx->xChip->iSlaveAddr = DEFAULT_CHIPIO_SLAVEADDR;
    ctx->xChip->iIrqPin = DEFAULT_CHIPIO_IRQPIN;
  }

  switch (opt) {
    case 'i':
      ctx->xChip->iSlaveAddr = iGetInt(sChipIoSlaveAddrStr, optarg, 0);
      vCheckIntRange(sChipIoSlaveAddrStr, ctx->xChip->iSlaveAddr,
                      CHIPIO_SLAVEADDR_MIN, CHIPIO_SLAVEADDR_MAX);
      ctx->bIsChipIo = true;
      return true;

    case 'n':
      ctx->xChip->iIrqPin = iGetInt(sChipIoIrqPinStr, optarg, 0);
      ctx->bIsChipIo = true;
      return true;
  }
#else
  (void)opt;
  (void)optarg;
  (void)ctx;
#endif
  return false;
}

bool bChipIoSetup(xMbPollContext * ctx) {
#ifdef USE_CHIPIO
  if ( (strcasestr (ctx->sDevice, "i2c") && ctx->bIsDefaultMode) ||
        ctx->bIsChipIo) {

    // Ensure context is allocated (e.g. if invoked by device name without -i/-n)
    if (ctx->xChip == NULL) {
      ctx->xChip = calloc(1, sizeof(xChipIoContext));
      if (!ctx->xChip) {
          vIoErrorExit("Memory allocation failed for chipio context");
      }
      ctx->xChip->iSlaveAddr = DEFAULT_CHIPIO_SLAVEADDR;
      ctx->xChip->iIrqPin = DEFAULT_CHIPIO_IRQPIN;
    }

    // Ouverture de la liaison i2c vers le chipio
    ctx->xChip->xChip = xChipIoOpen (ctx->sDevice, ctx->xChip->iSlaveAddr);
    if (ctx->xChip->xChip) {
      xDin xChipIrqPin = { .num = ctx->xChip->iIrqPin, .act = true,
                           .pull = ePullOff
                         };
      // Create virtual serial port
      ctx->xChip->xChipSerial = xChipIoSerialNew (ctx->xChip->xChip, &xChipIrqPin);
      if (ctx->xChip->xChipSerial) {

        // The virtual serial port will be used by libmodbus as a normal port
        ctx->sDevice = sChipIoSerialPortName (ctx->xChip->xChipSerial);
        if (iChipIoSerialSetAttr (ctx->xChip->xChipSerial, &ctx->xRtu) != 0) {

          vIoErrorExit ("Unable to set-up serial chipio port");
        }
      }
      else {
        iChipIoClose(ctx->xChip->xChip);
        ctx->xChip->xChip = NULL;
        vIoErrorExit ("serial chipio port failure");
      }
    }
    else {

      vIoErrorExit ("chipio not found");
    }

    ctx->eMode = eModeRtu;
    ctx->bIsChipIo = true;
    if (ctx->bIsVerbose) {
        printf("Set mode to RTU for chipio serial port\n");
    }
    return true;
  }
#else
  (void)ctx;
#endif
  return false;
}

void vChipIoClose(xMbPollContext * ctx) {
#ifdef USE_CHIPIO
  if (ctx->xChip) {
    if (ctx->xChip->xChipSerial) {
        vChipIoSerialDelete(ctx->xChip->xChipSerial);
        ctx->xChip->xChipSerial = NULL;
    }
    if (ctx->xChip->xChip) {
        iChipIoClose(ctx->xChip->xChip);
        ctx->xChip->xChip = NULL;
    }
    free(ctx->xChip);
    ctx->xChip = NULL;
  }
#endif
}

void vChipIoPrintUsageOptions(void) {
#ifdef USE_CHIPIO
  printf(
           "Options for ModBus RTU for ChipIo serial port : \n"
           "  -i #          I2c slave address (0x%02X-0x%02X, 0x%02X is default)\n"
           "  -n #          Irq pin number of GPIO (%d is default)\n",
           CHIPIO_SLAVEADDR_MIN,
           CHIPIO_SLAVEADDR_MAX,
           DEFAULT_CHIPIO_SLAVEADDR,
           DEFAULT_CHIPIO_IRQPIN
  );
#endif
}

const char * sChipIoGetAddStr(const xMbPollContext * ctx) {
#ifdef USE_CHIPIO
    if (ctx->bIsChipIo) {
      return " via ChipIo serial port";
    }
#else
    (void)ctx;
#endif
    return "";
}
