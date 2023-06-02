/* Copyright (c) 2015-2023 Pascal JEAN, All rights reserved.
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
#define _GNU_SOURCE

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <getopt.h>
#include <inttypes.h>
#include <modbus.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/time.h>

#include <unistd.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif
#include "custom-rts.h"
#include "mbpoll-config.h"
#include "serial.h"
#include "version-git.h"

/* constants ================================================================ */
#define AUTHORS "Pascal JEAN"
#define WEBSITE "https://github.com/epsilonrt/mbpoll"

/* conditionals ============================================================= */
#if defined(__GNUC__) && __SIZEOF_FLOAT__ != 4 && !defined(__STDC_IEC_559__)
#error it seems that this platform does not conform to the IEEE-754 standard !
#define MBPOLL_FLOAT_DISABLE
#endif

/* macros =================================================================== */
#define BASENAME(f) (f)

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <libgen.h>
#undef BASENAME
#define BASENAME(f) basename(f)
#endif

#ifndef NDEBUG
#define PDEBUG(fmt, ...) \
    logString("%s:%d: %s(): " fmt, BASENAME(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define PDEBUG(...)                  \
    if (ctx.ePrint == ePrintVerbose) \
    logString(__VA_ARGS__)
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

/* types ==================================================================== */
typedef enum {
    eModeRtu,
    eModeTcp,
    eModeUnknown = -1,
} eModes;

typedef enum {
    eFuncCoil          = 0,
    eFuncDiscreteInput = 1,
    eFuncInputReg      = 3,
    eFuncHoldingReg    = 4,
    eFuncUnknown       = -1,
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

typedef enum {
    ePrintNothing,
    ePrintMinimal,
    ePrintNormal,
    ePrintVerbose,
    ePrintUnknown = -1,
} ePrintLevel;

/* macros =================================================================== */
#define SIZEOF_ILIST(list) (sizeof(list) / sizeof(int))
/*
 * Le pointeur sur les données est de type void *, les macros ci-dessous
 * permettent de caster l'accès aux données en fonction de leur format
 */
#define DUINT8(p, i)  ((uint8_t*)(p))[i]
#define DUINT16(p, i) ((uint16_t*)(p))[i]
#define DINT32(p, i)  ((int32_t*)(p))[i]
#define DFLOAT(p, i)  ((float*)(p))[i]

/* constants ================================================================ */
static char const* sModeList[]     = {"RTU", "TCP"};
static int const iModeList[]       = {eModeRtu, eModeTcp};
static char const* sParityList[]   = {"even", "odd", "none"};
static int const iParityList[]     = {SERIAL_PARITY_EVEN, SERIAL_PARITY_ODD, SERIAL_PARITY_NONE};
static char const* sDatabitsList[] = {"8", "7"};
static int const iDatabitsList[]   = {SERIAL_DATABIT_8, SERIAL_DATABIT_7};
static char const* sStopbitsList[] = {"1", "2"};
static int const iStopbitsList[]   = {SERIAL_STOPBIT_ONE, SERIAL_STOPBIT_TWO};
#ifdef MBPOLL_FLOAT_DISABLE
static char const* sFormatList[] = {"int16", "hex", "string", "int"};
static int const iFormatList[]   = {eFormatInt16, eFormatHex, eFormatString, eFormatInt};
#else
static char const* sFormatList[] = {"int16", "hex", "string", "int", "float"};
static int const iFormatList[]
    = {eFormatInt16, eFormatHex, eFormatString, eFormatInt, eFormatFloat};
#endif
static char const* sFunctionList[]
    = {"discrete output (coil)", "discrete input", "input register", "output (holding) register"};
static int const iFunctionList[] = {eFuncCoil, eFuncDiscreteInput, eFuncInputReg, eFuncHoldingReg};
static int const iPrintLevel[]
    = {ePrintNothing, ePrintMinimal, ePrintNormal, ePrintVerbose, ePrintUnknown};

static char const sModeStr[]         = "mode";
static char const sSlaveAddrStr[]    = "slave address";
static char const sRtuParityStr[]    = "rtu parity";
static char const sRtuStopbitsStr[]  = "rtu stop bits";
static char const sRtuDatabitsStr[]  = "rtu data bits";
static char const sRtuBaudrateStr[]  = "rtu baudrate";
static char const sTcpPortStr[]      = "tcp port";
static char const sTimeoutStr[]      = "timeout";
static char const sPollRateStr[]     = "poll rate";
static char const sFunctionStr[]     = "function";
static char const sFormatStr[]       = "format";
static char const sNumOfValuesStr[]  = "number of values";
static char const sStartRefStr[]     = "start reference";
static char const sDataStr[]         = "data";
static char const sUnknownStr[]      = "unknown";
static char const sIntStr[]          = "32-bit integer";
static char const sFloatStr[]        = "32-bit float";
static char const sWordStr[]         = "16-bit register";
static char const sLittleEndianStr[] = "(little endian)";
static char const sBigEndianStr[]    = "(big endian)";
static char const sPrintLevel[]      = "print level";
static char* progname;

#ifdef MBPOLL_GPIO_RTS
static char const sRtsPinStr[] = "RTS pin";
#endif

/* structures =============================================================== */
typedef struct xChipIoContext xChipIoContext;

typedef struct xMbPollContext {
    // Paramètres
    eModes eMode;
    eFunctions eFunction;
    eFormats eFormat;
    int* piSlaveAddr;
    int iSlaveCount;
    int* piStartRef;
    int iStartCount;
    int iCount;
    int iPollRate;
    double dTimeout;
    char* sTcpPort;
    char* sDevice;
    xSerialIos xRtu;
    int iRtuBaudrate;
    eSerialDataBits eRtuDatabits;
    eSerialStopBits eRtuStopbits;
    eSerialParity eRtuParity;
    bool bIsPolling;
    int iRtuMode;
    bool bIsWrite;
    bool bIsReportSlaveID;
    bool bIsDefaultMode;
    int iPduOffset;
    bool bWriteSingleAsMany;
    bool bIsChipIo;
    bool bIsBigEndian;
    ePrintLevel ePrint;
    bool bIsStream;
    FILE* xOutputStream;
    FILE* xTempStream;
#ifdef MBPOLL_GPIO_RTS
    int iRtsPin;
#endif

    // Variables de travail
    modbus_t* xBus;
    void* pvData;
    int iTxCount;
    int iRxCount;
    int iErrorCount;

    xChipIoContext* xChip; // TODO: séparer la partie chipio
} xMbPollContext;

/* private variables ======================================================== */

static xMbPollContext ctx = {
    // Paramètres
    .eMode       = DEFAULT_MODE,
    .eFunction   = DEFAULT_FUNCTION,
    .eFormat     = eFormatDec,
    .piSlaveAddr = NULL,
    .iSlaveCount = -1,
    .piStartRef  = NULL,
    .iStartCount = -1,
    .iCount      = DEFAULT_NUMOFVALUES,
    .iPollRate   = DEFAULT_POLLRATE,
    .dTimeout    = DEFAULT_TIMEOUT,
    .sTcpPort    = DEFAULT_TCP_PORT,
    .sDevice     = NULL,
    .xRtu = {
        .baud   = DEFAULT_RTU_BAUDRATE,
        .dbits  = DEFAULT_RTU_DATABITS,
        .sbits  = DEFAULT_RTU_STOPBITS,
        .parity = DEFAULT_RTU_PARITY,
        .flow   = SERIAL_FLOW_NONE,
       },
    .bIsPolling         = true,
    .bIsReportSlaveID   = false,
    .iRtuMode           = MODBUS_RTU_RTS_NONE,
    .bIsWrite           = true,
    .bIsDefaultMode     = true,
    .iPduOffset         = 1,
    .bWriteSingleAsMany = false,
    .bIsChipIo          = false,
    .bIsBigEndian       = false,
    .ePrint             = ePrintNormal,
    .bIsStream          = false,
    .xOutputStream      = NULL,
    .xTempStream        = NULL,
#ifdef MBPOLL_GPIO_RTS
    .iRtsPin = -1,
#endif

    // Variables de travail
    .xBus   = NULL,
    .pvData = NULL,
};

#ifdef USE_CHIPIO
// -----------------------------------------------------------------------------
#include <chipio/serial.h>
#include <sysio/rpi.h>

/* private variables ======================================================== */
// Paramètres
static int iChipIoSlaveAddr = DEFAULT_CHIPIO_SLAVEADDR;
static int iChipIoIrqPin    = DEFAULT_CHIPIO_IRQPIN;
// static bool  bIsChipIo = false;

// Variables de travail
static xChipIo* xChip;
static xChipIoSerial* xChipSerial;

/* constants ================================================================ */
static char const sChipIoSlaveAddrStr[] = "chipio slave address";
static char const sChipIoIrqPinStr[]    = "chipio irq pin";
// option -i et -n supplémentaires pour chipio
static char const* short_options = "O:m:a:r:c:t:1l:o:p:b:d:s:P:u0WRhVvwBqiS:n:";

#else /* USE_CHIPIO == 0 */
/* constants ================================================================ */
#ifdef MBPOLL_GPIO_RTS
static char const* short_options = "O:m:a:r:c:t:1l:o:p:b:d:s:P:u0WRS::F::hVvwBq";
#else
static char const* short_options = "O:m:a:r:c:t:1l:o:p:b:d:s:P:u0WRFhVvwBqS";
#endif
// -----------------------------------------------------------------------------
#endif /* USE_CHIPIO == 0 */

/* private functions ======================================================== */
void vAllocate(xMbPollContext* ctx);
int logString(char const* const format, ...);
int logError(char const* const format, ...);
int logChar(int const character);
void closeOutput();
void copyFromTempStream();
void streamData(char const* const, size_t size);
void vPrintReadValues(int iAddr, int iCount, xMbPollContext* ctx);
void vPrintConfig(xMbPollContext const* ctx);
void vPrintCommunicationSetup(xMbPollContext const* ctx);
void vReportSlaveID(xMbPollContext const* ctx);
void vHello(void);
void vVersion(void);
void vWarranty(void);
void vUsage(FILE* stream, int exit_msg);
void vFailureExit(bool bHelp, char const* format, ...);
#define vSyntaxErrorExit(fmt, ...) vFailureExit(true, fmt, ##__VA_ARGS__)
#define vIoErrorExit(fmt, ...)     vFailureExit(false, fmt, ##__VA_ARGS__)
void vCheckEnum(char const* sName, int iElmt, int const* iList, int iSize);
void vCheckIntRange(char const* sName, int i, int min, int max);
void vCheckDoubleRange(char const* sName, double d, double min, double max);
int iGetInt(char const* sName, char const* sNum, int iBase);
int* iGetIntList(char const* sName, char const* sList, int* iLen);
void vPrintIntList(int* iList, int iLen);
double dGetDouble(char const* sName, char const* sNum);
int iGetEnum(char const* sName, char* sElmt, char const** psStrList, int const* iList, int iSize);
char const* sEnumToStr(int iElmt, int const* iList, char const** psStrList, int iSize);
char const* sFunctionToStr(eFunctions eFunction);
char const* sModeToStr(eModes eMode);
void vSigIntHandler(int sig);
float fSwapFloat(float f);
int32_t lSwapLong(int32_t l);
void mb_delay(unsigned long d);

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// Portage des fonctions Microsoft
// -----------------------------------------------------------------------------
static char* _strlwr(char* str) {
    char* p = str;

    while (*p) {
        *p = tolower(*p);
        p++;
    }
    return str;
}
#else
// Portage des fonctions POSIX ou GNU

// -----------------------------------------------------------------------------
// posix
#define strcasecmp _stricmp

// -----------------------------------------------------------------------------
// posix
static char* basename(char* path) {
    static char fname[_MAX_FNAME];
    _splitpath(path, NULL, NULL, fname, NULL);

    return fname;
}

// -----------------------------------------------------------------------------
// posix
static char* strcasestr(char const* haystack, char const* needle) {
    size_t nlen = strlen(needle);
    size_t hlen = strlen(haystack) - nlen + 1;
    size_t i;

    for (i = 0; i < hlen; i++) {
        int j;
        for (j = 0; j < nlen; j++) {
            unsigned char c1 = haystack[i + j];
            unsigned char c2 = needle[j];
            if (toupper(c1) != toupper(c2)) {
                goto next;
            }
        }
        return (char*)haystack + i;
next:;
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// posix
static char* index(char const* s, int c) {
    while ((s) && (*s)) {
        if (c == *s) {
            return (char*)s;
        }
        s++;
    }
    return NULL;
}

#endif

/* main ===================================================================== */

int main(int argc, char** argv) {
    ctx.xTempStream   = tmpfile();
    ctx.xOutputStream = ctx.xTempStream;
    int iNextOption, iRet = 0;
    char* p;

    progname = argv[0];

    do {
        iNextOption = getopt(argc, argv, short_options);
        opterr      = 0;
        switch (iNextOption) {
            case 'v':
                if (ctx.ePrint != ePrintNormal) {
                    vSyntaxErrorExit(
                        "Illegal %s: Cannot use both Verbose and Quiet options.", sPrintLevel);
                }
                ctx.ePrint = ePrintVerbose;
                PDEBUG("debug enabled\n");
                break;

            case 'm':
                ctx.eMode
                    = iGetEnum(sModeStr, optarg, sModeList, iModeList, SIZEOF_ILIST(iModeList));
                ctx.bIsDefaultMode = false;
                break;

            case 'a':
                ctx.piSlaveAddr = iGetIntList(sSlaveAddrStr, optarg, &ctx.iSlaveCount);
                // Vérification dépend du mode
                break;

            case 'r': ctx.piStartRef = iGetIntList(sStartRefStr, optarg, &ctx.iStartCount); break;

            case 'c':
                ctx.iCount = iGetInt(sNumOfValuesStr, optarg, 0);
                vCheckIntRange(sNumOfValuesStr, ctx.iCount, NUMOFVALUES_MIN, NUMOFVALUES_MAX);
                ctx.bIsWrite = false;
                break;

            case 't':
                ctx.eFunction = iGetInt(sFunctionStr, optarg, 0);
                vCheckEnum(sFunctionStr, ctx.eFunction, iFunctionList, SIZEOF_ILIST(iFunctionList));
                p = index(optarg, ':');
                if (p) {
                    ctx.eFormat = iGetEnum(
                        sFormatStr, p + 1, sFormatList, iFormatList, SIZEOF_ILIST(iFormatList));
                }
                break;

            case 'u': ctx.bIsReportSlaveID = true; break;

            case '1': ctx.bIsPolling = false; break;

            case 'B': ctx.bIsBigEndian = true; break;

            case 'S': ctx.bIsStream = true; break;

            case 'O':
                ctx.xOutputStream = fopen(optarg, "w");
                if (ctx.xOutputStream == NULL) {
                    vSyntaxErrorExit("Could not open file '%s' in write mode.", optarg);
                }
                break;

            case 'R': ctx.iRtuMode = MODBUS_RTU_RTS_DOWN;
#ifdef MBPOLL_GPIO_RTS
                if (optarg) {
                    ctx.iRtsPin = iGetInt(sRtsPinStr, optarg, 10);
                }
#endif
                break;

            case 'F': ctx.iRtuMode = MODBUS_RTU_RTS_UP;
#ifdef MBPOLL_GPIO_RTS
                if (optarg) {
                    ctx.iRtsPin = iGetInt(sRtsPinStr, optarg, 10);
                }
#endif
                break;

            case '0': ctx.iPduOffset = 0; break;

            case 'W': ctx.bWriteSingleAsMany = true; break;

            case 'l':
                ctx.iPollRate = iGetInt(sPollRateStr, optarg, 0);
                if (ctx.iPollRate < 0) {
                    vSyntaxErrorExit("Illegal %s: %d", sPollRateStr, ctx.iPollRate);
                } else if (ctx.iPollRate < POLLRATE_MIN) {
                    logError(
                        "Warning: Small %s: %d ms, cannot guarantee it will be stable.\n",
                        sPollRateStr,
                        ctx.iPollRate);
                }
                break;

            case 'o':
                ctx.dTimeout = dGetDouble(sTimeoutStr, optarg);
                vCheckDoubleRange(sTimeoutStr, ctx.dTimeout, TIMEOUT_MIN, TIMEOUT_MAX);
                break;

            case 'q':
                if (ctx.ePrint == ePrintVerbose) {
                    vSyntaxErrorExit(
                        "Illegal %s: Cannot use both Verbose and Quiet options.", sPrintLevel);
                }
                if (ctx.ePrint > 0) {
                    ctx.ePrint--;
                }
                break;

                // TCP -----------------------------------------------------------------
            case 'p':
                ctx.sTcpPort = optarg;
                break;

                // RTU -----------------------------------------------------------------
            case 'b':
                ctx.xRtu.baud = iGetInt(sRtuBaudrateStr, optarg, 0);
                vCheckIntRange(sRtuBaudrateStr, ctx.xRtu.baud, RTU_BAUDRATE_MIN, RTU_BAUDRATE_MAX);
                break;
            case 'd':
                ctx.xRtu.dbits = iGetEnum(
                    sRtuDatabitsStr,
                    optarg,
                    sDatabitsList,
                    iDatabitsList,
                    SIZEOF_ILIST(iDatabitsList));
                break;
            case 's':
                ctx.xRtu.sbits = iGetEnum(
                    sRtuStopbitsStr,
                    optarg,
                    sStopbitsList,
                    iStopbitsList,
                    SIZEOF_ILIST(iStopbitsList));
                break;
            case 'P':
                ctx.xRtu.parity = iGetEnum(
                    sRtuParityStr, optarg, sParityList, iParityList, SIZEOF_ILIST(iParityList));
                break;

#ifdef USE_CHIPIO
                // -----------------------------------------------------------------------------
                // ChipIo --------------------------------------------------------------
            case 'i':
                iChipIoSlaveAddr = iGetInt(sChipIoSlaveAddrStr, optarg, 0);
                vCheckIntRange(
                    sChipIoSlaveAddrStr,
                    iChipIoSlaveAddr,
                    CHIPIO_SLAVEADDR_MIN,
                    CHIPIO_SLAVEADDR_MAX);
                ctx.bIsChipIo = true;
                break;

            case 'n':
                iChipIoIrqPin = iGetInt(sChipIoIrqPinStr, optarg, 0);
                ctx.bIsChipIo = true;
                break;
// -----------------------------------------------------------------------------
#endif /* USE_CHIPIO defined */

                // Misc. ---------------------------------------------------------------
            case 'h': vUsage(stdout, EXIT_SUCCESS); break;

            case 'V': vVersion(); break;

            case 'w': vWarranty(); break;

            case '?': /* An invalid option has been used */
                vSyntaxErrorExit("Unrecognized option or missing option parameter");
                break;
        }
    } while (iNextOption != -1);

    copyFromTempStream();

    if (ctx.iStartCount == -1) {
        ctx.piStartRef = malloc(sizeof(int));
        assert(ctx.piStartRef);
        ctx.piStartRef[0] = DEFAULT_STARTREF;
        ctx.iStartCount   = 1;
    }

    int i;
    if (ctx.iPduOffset) {
        for (i = 0; i < ctx.iStartCount; i++) {
            vCheckIntRange(sStartRefStr, ctx.piStartRef[i], STARTREF_MIN, STARTREF_MAX);
        }
    } else {
        for (i = 0; i < ctx.iStartCount; i++) {
            vCheckIntRange(sStartRefStr, ctx.piStartRef[i], STARTREF_MIN - 1, STARTREF_MAX - 1);
        }
    }

    // ignore iCount > 1 if start ref list contains more then one value
    if ((ctx.iStartCount > 1) && (ctx.iCount > 1)) {
        ctx.iCount = 1;
    }

    // Coils et Discrete inputs toujours en binaire
    if ((ctx.eFunction == eFuncCoil) || (ctx.eFunction == eFuncDiscreteInput)) {
        ctx.eFormat = eFormatBin;
    }

    // Lecture du port série ou de l'hôte
    if (optind == argc) {
        vSyntaxErrorExit("device or host parameter missing");
    }
    ctx.sDevice = argv[optind];

    if ((strcasestr(ctx.sDevice, "com") || strcasestr(ctx.sDevice, "tty")
         || strcasestr(ctx.sDevice, "ser"))
        && ctx.bIsDefaultMode) {
        // Mode par défaut si port série
        ctx.eMode = eModeRtu;
        PDEBUG("Set mode to RTU for serial port\n");
    }
#ifdef USE_CHIPIO
    // -----------------------------------------------------------------------------
    else if ((strcasestr(ctx.sDevice, "i2c") && ctx.bIsDefaultMode) || ctx.bIsChipIo) {

        // Ouverture de la liaison i2c vers le chipio
        xChip = xChipIoOpen(ctx.sDevice, iChipIoSlaveAddr);
        if (xChip) {
            xDin xChipIrqPin = {.num = iChipIoIrqPin, .act = true, .pull = ePullOff};
            // Création du port série virtuel
            xChipSerial = xChipIoSerialNew(xChip, &xChipIrqPin);
            if (xChipSerial) {
                // le port série virtuel sera utilisé par libmobus comme un port normal
                ctx.sDevice = sChipIoSerialPortName(xChipSerial);
                if (iChipIoSerialSetAttr(xChipSerial, &ctx.xRtu) != 0) {
                    vIoErrorExit("Unable to set-up serial chipio port");
                }
            } else {
                vIoErrorExit("serial chipio port failure");
            }
        } else {
            vIoErrorExit("chipio not found");
        }

        ctx.eMode     = eModeRtu;
        ctx.bIsChipIo = true;
        PDEBUG("Set mode to RTU for chipio serial port\n");
    }
// -----------------------------------------------------------------------------
#endif /* USE_CHIPIO defined */
    PDEBUG("Set device=%s\n", ctx.sDevice);

    if ((ctx.bIsReportSlaveID) && (ctx.eMode != eModeRtu)) {
        vSyntaxErrorExit("-u is available only in RTU mode");
    }

    if (!ctx.bIsReportSlaveID) {
        // Calcul du nombre de données à écrire
        int iNbToWrite = 0;
        if (ctx.bIsStream) {
            if (ctx.bIsWrite) {
                iNbToWrite = ctx.iStartCount;
                ctx.iCount = ctx.iStartCount;
            } else {
                setbuf(stdout, NULL);
            }
        } else {
            iNbToWrite = MAX(0, argc - optind - 1);
            if (iNbToWrite) {
                if (!ctx.bIsWrite) {
                    // option -c fournie pour une lecture avec des données à écrire !
                    vSyntaxErrorExit("-c parameter must not be specified for writing");
                }
                ctx.bIsPolling = false;
                ctx.iCount     = iNbToWrite;
                PDEBUG("%d write data have been found\n", iNbToWrite);
            } else {
                ctx.bIsWrite = false;
            }
        }

        // Allocation de la mémoire nécessaire
        vAllocate(&ctx);

        // Récupération sur la ligne de commande des données à écrire
        if (iNbToWrite) {
            int iValue, i = 0, arg;
            double dValue;

            for (arg = optind + 1; arg < argc; arg++, i++) {
                switch (ctx.eFunction) {
                    case eFuncDiscreteInput:
                    case eFuncInputReg:
                        vSyntaxErrorExit("Unable to write read-only element");
                        break;

                    case eFuncCoil:
                        // 1 octets contient 8 coils
                        iValue = iGetInt(sDataStr, argv[arg], 10);
                        vCheckIntRange(sDataStr, iValue, 0, 1);
                        DUINT8(ctx.pvData, i) = (uint8_t)iValue;
                        PDEBUG("Byte[%d]=%d\n", i, DUINT8(ctx.pvData, i));
                        break;
                        break;

                    case eFuncHoldingReg:
                        if (ctx.eFormat == eFormatInt) {
                            DINT32(ctx.pvData, i) = lSwapLong(iGetInt(sDataStr, argv[arg], 10));
                            PDEBUG("Int[%d]=%" PRId32 "\n", i, lSwapLong(DINT32(ctx.pvData, i)));
                        } else if (ctx.eFormat == eFormatFloat) {
                            dValue = dGetDouble(sDataStr, argv[arg]);
                            PDEBUG("%g,%g\n", FLT_MIN, FLT_MAX);
                            vCheckDoubleRange(sDataStr, dValue, -FLT_MAX, FLT_MAX);
                            DFLOAT(ctx.pvData, i) = fSwapFloat((float)dValue);
                            PDEBUG("Float[%d]=%g\n", i, fSwapFloat(DFLOAT(ctx.pvData, i)));
                        } else if (ctx.eFormat == eFormatString) {
                            vSyntaxErrorExit("You can use string format only for output");
                        } else if (ctx.eFormat == eFormatInt16) {
                            iValue = iGetInt(sDataStr, argv[arg], 0);
                            vCheckIntRange(sDataStr, iValue, INT16_MIN, INT16_MAX);
                            DUINT16(ctx.pvData, i) = (uint16_t)iValue;
                            PDEBUG("Word[%d]=0x%X\n", i, DUINT16(ctx.pvData, i));
                        } else {
                            iValue = iGetInt(sDataStr, argv[arg], 0);
                            vCheckIntRange(sDataStr, iValue, 0, UINT16_MAX);
                            DUINT16(ctx.pvData, i) = (uint16_t)iValue;
                            PDEBUG("Word[%d]=0x%X\n", i, DUINT16(ctx.pvData, i));
                        }
                        break;

                    default: // Impossible, la valeur a été vérifiée, évite un warning de gcc
                        break;
                }
            }
        }
    }

    if ((ctx.iSlaveCount > 1) && ((ctx.bIsStream) || (ctx.bIsWrite) || (ctx.bIsReportSlaveID))) {
        vSyntaxErrorExit("You can give a slave address list only for reading without streaming");
    }

    if ((ctx.iStartCount > 1) && (ctx.bIsWrite) && (!ctx.bIsStream)) {
        vSyntaxErrorExit("You can give a start ref list only for reading or streaming");
    }

    if (ctx.iSlaveCount == -1) {
        ctx.piSlaveAddr = malloc(sizeof(int));
        assert(ctx.piSlaveAddr);
        ctx.piSlaveAddr[0] = DEFAULT_SLAVEADDR;
        ctx.iSlaveCount    = 1;
    }

    // Fin de vérification des valeurs de paramètres et création des contextes
    switch (ctx.eMode) {
        case eModeRtu:
            for (i = 0; i < ctx.iSlaveCount; i++) {
                vCheckIntRange(sSlaveAddrStr, ctx.piSlaveAddr[i], RTU_SLAVEADDR_MIN, SLAVEADDR_MAX);
            }
            ctx.xBus = modbus_new_rtu(
                ctx.sDevice, ctx.xRtu.baud, ctx.xRtu.parity, ctx.xRtu.dbits, ctx.xRtu.sbits);
            break;

        case eModeTcp:
            for (i = 0; i < ctx.iSlaveCount; i++) {
                vCheckIntRange(sSlaveAddrStr, ctx.piSlaveAddr[i], TCP_SLAVEADDR_MIN, SLAVEADDR_MAX);
            }
            ctx.xBus = modbus_new_tcp_pi(ctx.sDevice, ctx.sTcpPort);
            break;

        default: break;
    }

    if (ctx.xBus == NULL) {
        vIoErrorExit("Unable to create the libmodbus context");
    }
    modbus_set_debug(ctx.xBus, (ctx.ePrint == ePrintVerbose));

    if (ctx.ePrint >= ePrintNormal) {
        vHello();
    }

    if ((ctx.iRtuMode != MODBUS_RTU_RTS_NONE) && (ctx.eMode == eModeRtu) && !ctx.bIsChipIo) {
#ifdef MBPOLL_GPIO_RTS
        if (ctx.iRtsPin >= 0) {
            double t = 11 / (double)ctx.xRtu.baud / 2 * 1e6; // delay 1/2 car

            if (init_custom_rts(ctx.iRtsPin, ctx.iRtuMode == MODBUS_RTU_RTS_UP) != 0) {
                vIoErrorExit("Unable to set GPIO RTS pin: %d", ctx.iRtsPin);
            }
            modbus_rtu_set_custom_rts(ctx.xBus, set_custom_rts);
            modbus_rtu_set_rts_delay(ctx.xBus, (int)t);
        }
#endif
        modbus_rtu_set_serial_mode(ctx.xBus, MODBUS_RTU_RS485);
        modbus_rtu_set_rts(ctx.xBus, ctx.iRtuMode);
    }

    // Connection au bus
    if (modbus_connect(ctx.xBus) == -1) {
        modbus_free(ctx.xBus);
        vIoErrorExit("Connection failed: %s", modbus_strerror(errno));
    }

    /*
     * évites que l'esclave prenne l'impulsion de 40µs créée par le driver à
     * l'ouverture du port comme un bit de start.
     */
    mb_delay(20);

    // Réglage du timeout de réponse
    uint32_t sec, usec;
#ifdef DEBUG

    modbus_get_byte_timeout(ctx.xBus, &sec, &usec);
    PDEBUG("Get byte timeout: %d s, %d us\n", sec, usec);
#endif
    sec  = (uint32_t)ctx.dTimeout;
    usec = (uint32_t)((ctx.dTimeout - sec) * 1E6);
    modbus_set_response_timeout(ctx.xBus, sec, usec);
    PDEBUG("Set response timeout to %" PRIu32 " sec, %" PRIu32 " us\n", sec, usec);

    // vSigIntHandler() intercepte le CTRL+C
    signal(SIGINT, vSigIntHandler);

    if (ctx.bIsReportSlaveID) {
        vReportSlaveID(&ctx);
    } else {
        int iNbReg, iStartReg;
        // Affichage complet de la configuration
        if (ctx.ePrint >= ePrintNormal) {
            vPrintConfig(&ctx);
        }

        // int32 et float utilisent 2 registres 16 bits
        iNbReg = ((ctx.eFormat == eFormatInt) || (ctx.eFormat == eFormatFloat)) ? ctx.iCount * 2
                                                                                : ctx.iCount;

        // Début de la boucle de scrutation
        do {
            if (ctx.bIsWrite) {
                // libmodbus utilise les adresses PDU !
                iStartReg = ctx.piStartRef[0] - ctx.iPduOffset;

                modbus_set_slave(ctx.xBus, ctx.piSlaveAddr[0]);
                ctx.iTxCount++;
                if (ctx.bIsStream) {
                    int readVals = fread(ctx.pvData, 1, iNbReg, stdin);
                    if (readVals != iNbReg) {
                        ctx.iErrorCount++;
                        logError(
                            "Write %s failed: %s\n",
                            sFunctionToStr(ctx.eFunction),
                            "Couldn't read from input stream.");
                        break;
                    }
                }
                // Ecriture ------------------------------------------------------------
                switch (ctx.eFunction) {
                    case eFuncCoil:
                        if (iNbReg == 1) {
                            // Ecriture d'un seul bit
                            iRet = modbus_write_bit(ctx.xBus, iStartReg, DUINT8(ctx.pvData, 0));
                        } else {
                            iRet = modbus_write_bits(ctx.xBus, iStartReg, iNbReg, ctx.pvData);
                        }
                        break;

                    case eFuncHoldingReg:
                        if (iNbReg == 1 && (!ctx.bWriteSingleAsMany)) {
                            // Ecriture d'un seul registre
                            iRet = modbus_write_register(
                                ctx.xBus, iStartReg, DUINT16(ctx.pvData, 0));
                        } else {
                            iRet = modbus_write_registers(ctx.xBus, iStartReg, iNbReg, ctx.pvData);
                        }
                        break;

                    default: // Impossible, la valeur a été vérifiée, évite un warning de gcc
                        break;
                }
                if (iRet == iNbReg) {
                    ctx.iRxCount++;
                    logString("Written %d references.\n", ctx.iCount);
                } else {
                    ctx.iErrorCount++;
                    logError(
                        "Write %s failed: %s\n",
                        sFunctionToStr(ctx.eFunction),
                        modbus_strerror(errno));
                }
                // Fin écriture --------------------------------------------------------
            } else {
                int i;

                // Lecture -------------------------------------------------------------
                for (i = 0; i < ctx.iSlaveCount; i++) {
                    modbus_set_slave(ctx.xBus, ctx.piSlaveAddr[i]);
                    ctx.iTxCount++;

                    logString("-- Polling slave %d...", ctx.piSlaveAddr[i]);
                    if (ctx.bIsPolling) {
                        logString(" Ctrl-C to stop)\n");
                    } else {
                        logChar('\n');
                    }

                    int j;
                    for (j = 0; j < ctx.iStartCount; j++) {
                        // libmodbus utilise les adresses PDU !
                        iStartReg = ctx.piStartRef[j] - ctx.iPduOffset;

                        switch (ctx.eFunction) {
                            case eFuncDiscreteInput:
                                iRet = modbus_read_input_bits(
                                    ctx.xBus, iStartReg, iNbReg, ctx.pvData);
                                break;

                            case eFuncCoil:
                                iRet = modbus_read_bits(ctx.xBus, iStartReg, iNbReg, ctx.pvData);
                                break;

                            case eFuncInputReg:
                                iRet = modbus_read_input_registers(
                                    ctx.xBus, iStartReg, iNbReg, ctx.pvData);
                                break;

                            case eFuncHoldingReg:
                                iRet = modbus_read_registers(
                                    ctx.xBus, iStartReg, iNbReg, ctx.pvData);
                                break;

                            default: // Impossible, la valeur a été vérifiée, évite un warning de
                                     // gcc
                                break;
                        }
                        if (iRet == iNbReg) {
                            ctx.iRxCount++;
                            if (ctx.bIsStream) {
                                streamData(ctx.pvData, ctx.iCount);
                            }
                            vPrintReadValues(ctx.piStartRef[j], ctx.iCount, &ctx);
                        } else {
                            ctx.iErrorCount++;
                            logError(
                                "Read %s failed: %s\n",
                                sFunctionToStr(ctx.eFunction),
                                modbus_strerror(errno));
                        }
                    }
                    if (ctx.bIsPolling) {
                        mb_delay(ctx.iPollRate);
                    }
                }
                // Fin lecture ---------------------------------------------------------
            }
        } while (ctx.bIsPolling);
    }

    vSigIntHandler(SIGTERM);
    return 0;
}

/* private functions ======================================================== */

// -----------------------------------------------------------------------------
int logString(char const* const format, ...) {
    if ((ctx.ePrint == ePrintNothing) || (ctx.xOutputStream == NULL)) {
        return 0;
    }
    va_list args;
    va_start(args, format);
    int result = vfprintf(ctx.xOutputStream, format, args);
    va_end(args);
    return result;
}

int logError(char const* const format, ...) {
    va_list args;
    va_list* args_pointer = &args;
    va_start(args, format);

    if (ctx.xOutputStream != NULL && ctx.xOutputStream != stdout) {
        va_list args_copy;
        va_copy(args_copy, args);
        fflush(ctx.xOutputStream);
        vfprintf(ctx.xOutputStream, format, args);
        va_end(args);
        fflush(ctx.xOutputStream);
        args_pointer = &args_copy;
    }

    int result = vfprintf(stderr, format, *args_pointer);
    va_end(*args_pointer);
    return result;
}

int logChar(int const character) {
    if ((ctx.ePrint == ePrintNothing) || (ctx.xOutputStream == NULL)) {
        return 0;
    }
    return putc(character, ctx.xOutputStream);
}

void closeOutput() {
    if (ctx.xOutputStream != NULL) {
        fflush(ctx.xOutputStream);
        if (ctx.xOutputStream != stdout) {
            fclose(ctx.xOutputStream);
        }
    }
}

void copyFromTempStream() {
    if (ctx.xTempStream == NULL) {
        return;
    }
    if (ctx.xOutputStream == ctx.xTempStream) {
        if (!ctx.bIsStream || ctx.bIsWrite) {
            ctx.xOutputStream = stdout;
        } else {
            ctx.xOutputStream = NULL;
            fclose(ctx.xTempStream);
            ctx.xTempStream = NULL;
            return;
        }
    }
    // Write anything that was written to the
    char buf[sizeof(long)];
    rewind(ctx.xTempStream);
    int readLength = 0;
    while ((readLength = fread(buf, sizeof(char), sizeof(long), ctx.xTempStream)) > 0) {
        if (readLength != 8) {
            buf[readLength] = 0;
        }
        fprintf(ctx.xOutputStream, "%s", buf);
    }
    fclose(ctx.xTempStream);
    ctx.xTempStream = NULL;
}

void streamData(char const* const data, size_t size) {
    if (ctx.bIsStream) {
        fwrite(data, 1, size, stdout);
        fflush(stdout);
    }
}

void vPrintReadValues(int iAddr, int iCount, xMbPollContext* ctx) {
    int i;
    for (i = 0; i < iCount; i++) {
        logString("[%d]: \t", iAddr);

        switch (ctx->eFormat) {
            case eFormatBin:
                logString("%c", (DUINT8(ctx->pvData, i) != FALSE) ? '1' : '0');
                iAddr++;
                break;

            case eFormatDec: {
                uint16_t v = DUINT16(ctx->pvData, i);
                if (v & 0x8000) {
                    logString("%u (%d)", v, (int)(int16_t)v);
                } else {
                    logString("%u", v);
                }
                iAddr++;

            } break;

            case eFormatInt16:
                logString("%d", (int)(int16_t)(DUINT16(ctx->pvData, i)));
                iAddr++;
                break;

            case eFormatHex:
                logString("0x%04X", DUINT16(ctx->pvData, i));
                iAddr++;
                break;

            case eFormatString:
                logString(
                    "%c%c",
                    (char)((int)(DUINT16(ctx->pvData, i) / 256)),
                    (char)(DUINT16(ctx->pvData, i) % 256));
                iAddr++;
                break;

            case eFormatInt:
                logString("%d", lSwapLong(DINT32(ctx->pvData, i)));
                iAddr += 2;
                break;

            case eFormatFloat:
                logString("%g", fSwapFloat(DFLOAT(ctx->pvData, i)));
                iAddr += 2;
                break;

            default: // Impossible normalement
                break;
        }
        logChar('\n');
    }
}

// -----------------------------------------------------------------------------
void vReportSlaveID(xMbPollContext const* ctx) {
    uint8_t ucReport[256];

    modbus_set_slave(ctx->xBus, ctx->piSlaveAddr[0]);
    // Affichage de la configuration
    logString("Protocol configuration: Modbus %s\n", sModeList[ctx->eMode]);
    logString("Slave configuration...: address = %d, report slave id\n", ctx->piSlaveAddr[0]);

    vPrintCommunicationSetup(ctx);

    int iRet = modbus_report_slave_id(ctx->xBus, 256, ucReport);

    if (iRet < 0) {
        logError("Report slave ID failed(%d): %s\n", iRet, modbus_strerror(errno));
    } else {
        if (iRet > 1) {
            int iLen = iRet - 2;

            logString(
                "Length: %d\n"
                "Id    : 0x%02X\n"
                "Status: %s\n",
                iRet,
                ucReport[0],
                (ucReport[1]) ? "On" : "Off");

            if (iLen > 0) {
                int i;
                logString("Data  : ");
                for (i = 2; i < (iLen + 2); i++) {
                    if (isprint(ucReport[i])) {
                        logChar(ucReport[i]);
                    } else {
                        logString("\\%02X", ucReport[i]);
                    }
                }
                logChar('\n');
            }
        } else {
            logError("no data available\n");
        }
    }
}

// -----------------------------------------------------------------------------
void vPrintCommunicationSetup(xMbPollContext const* ctx) {
    if (ctx->eMode == eModeRtu) {
#ifndef USE_CHIPIO
        // -----------------------------------------------------------------------------
        char const sAddStr[] = "";
#else  /* USE_CHIPIO defined */
        // -----------------------------------------------------------------------------
        char const* sAddStr;
        if (ctx->bIsChipIo) {
            sAddStr = " via ChipIo serial port";
        } else {
            sAddStr = "";
        }
// -----------------------------------------------------------------------------
#endif /* USE_CHIPIO defined */

        logString(
            "Communication.........: %s%s, %s\n"
            "                        t/o %.2f s, poll rate %d ms\n",
            ctx->sDevice,
            sAddStr,
            sSerialAttrToStr(&ctx->xRtu),
            ctx->dTimeout,
            ctx->iPollRate);
    } else {
        logString(
            "Communication.........: %s, port %s, t/o %.2f s, poll rate %d ms\n",
            ctx->sDevice,
            ctx->sTcpPort,
            ctx->dTimeout,
            ctx->iPollRate);
    }
}

// -----------------------------------------------------------------------------
void vPrintConfig(xMbPollContext const* ctx) {
    // Affichage de la configuration
    logString("Protocol configuration: Modbus %s\n", sModeList[ctx->eMode]);
    logString("Slave configuration...: address = ");
    vPrintIntList(ctx->piSlaveAddr, ctx->iSlaveCount);
    if (ctx->iStartCount > 1) {
        logString("\n                        start reference = ");
        vPrintIntList(ctx->piStartRef, ctx->iStartCount);
        logString("\n");
    } else {
        logString(
            "\n                        start reference = %d, count = %d\n",
            ctx->piStartRef[0],
            ctx->iCount);
    }
    vPrintCommunicationSetup(ctx);
    logString("Data type.............: ");
    switch (ctx->eFunction) {
        case eFuncDiscreteInput: logString("discrete input\n"); break;

        case eFuncCoil: logString("discrete output (coil)\n"); break;

        case eFuncInputReg:
            if (ctx->eFormat == eFormatInt) {
                logString("%s %s", sIntStr, ctx->bIsBigEndian ? sBigEndianStr : sLittleEndianStr);
            } else if (ctx->eFormat == eFormatFloat) {
                logString("%s %s", sFloatStr, ctx->bIsBigEndian ? sBigEndianStr : sLittleEndianStr);
            } else {
                logString("%s", sWordStr);
            }
            logString(", input register table\n");
            break;

        case eFuncHoldingReg:
            if (ctx->eFormat == eFormatInt) {
                logString("%s %s", sIntStr, ctx->bIsBigEndian ? sBigEndianStr : sLittleEndianStr);
            } else if (ctx->eFormat == eFormatFloat) {
                logString("%s %s", sFloatStr, ctx->bIsBigEndian ? sBigEndianStr : sLittleEndianStr);
            } else {
                logString("%s", sWordStr);
            }
            logString(", output (holding) register table\n");
            break;

        default: // Impossible, la valeur a été vérifiée, évite un warning de gcc
            break;
    }
    logChar('\n');
}

// -----------------------------------------------------------------------------
// Allocation de la mémoire pour les données à écrire ou à lire
void vAllocate(xMbPollContext* ctx) {
    size_t ulDataSize = ctx->iCount;
    switch (ctx->eFunction) {
        case eFuncCoil:
        case eFuncDiscreteInput:
            // 1 bit est stocké dans un octet
            break;

        case eFuncInputReg:
        case eFuncHoldingReg:
            if ((ctx->eFormat == eFormatInt) || (ctx->eFormat == eFormatFloat)) {
                // Registres 32-bits
                ulDataSize *= 4;
            } else {
                // Registres 16-bits
                ulDataSize *= 2;
            }
            break;

        default: // Impossible, la valeur a été vérifiée, évite un warning de gcc
            break;
    }
    ctx->pvData = calloc(1, ulDataSize);
    assert(ctx->pvData);
}

// -----------------------------------------------------------------------------
void vSigIntHandler(int sig) {
    if ((ctx.bIsPolling)) {
        logString(
            "--- %s %s statistics ---\n"
            "%d frames transmitted, %d received, %d errors, %.1f%% frame loss\n",
            ctx.sDevice,
            ctx.bIsWrite ? "write" : "read",
            ctx.iTxCount,
            ctx.iRxCount,
            ctx.iErrorCount,
            (double)(ctx.iTxCount - ctx.iRxCount) * 100.0 / (double)ctx.iTxCount);
    }

    free(ctx.pvData);
    free(ctx.piSlaveAddr);
    closeOutput();
    modbus_close(ctx.xBus);
    modbus_free(ctx.xBus);
#ifdef USE_CHIPIO
    // -----------------------------------------------------------------------------
    vChipIoSerialDelete(xChipSerial);
    iChipIoClose(xChip);
// -----------------------------------------------------------------------------
#endif /* USE_CHIPIO defined */
    if (sig == SIGINT) {
        logString("\neverything was closed.\nHave a nice day !\n");
    } else {
        logChar('\n');
    }
    fflush(stdout);
    exit(ctx.iErrorCount == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}

// -----------------------------------------------------------------------------
void vFailureExit(bool bHelp, char const* format, ...) {
    copyFromTempStream();
    va_list va;

    va_start(va, format);
    logError("%s: ", progname);
    logError(format, va);
    if (bHelp) {
        logError(" ! Try -h for help.\n");
    } else {
        logError(".\n");
    }
    va_end(va);
    fflush(stderr);
    free(ctx.pvData);
    free(ctx.piSlaveAddr);
    closeOutput();
    exit(EXIT_FAILURE);
}

// -----------------------------------------------------------------------------
void vVersion(void) {
    logString("%s\n", VERSION_SHORT);
    exit(EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
void vWarranty(void) {
    logString(
        "Copyright (c) 2015-2023 %s, All rights reserved.\n\n"

        " mbpoll is free software: you can redistribute it and/or modify\n"
        " it under the terms of the GNU General Public License as published by\n"
        " the Free Software Foundation, either version 3 of the License, or\n"
        " (at your option) any later version.\n\n"

        " mbpoll is distributed in the hope that it will be useful,\n"
        " but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        " MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        " GNU General Public License for more details.\n\n"

        " You should have received a copy of the GNU General Public License\n"
        " along with mbpoll. If not, see <http://www.gnu.org/licenses/>.\n",
        AUTHORS);
    exit(EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
void vHello(void) {
    logString("mbpoll %s - FieldTalk(tm) Modbus(R) Master Simulator\n", VERSION_SHORT);
    logString("Copyright (c) 2015-2023 %s, %s\n", AUTHORS, WEBSITE);
    logString("This program comes with ABSOLUTELY NO WARRANTY.\n");
    logString("This is free software, and you are welcome to redistribute it\n");
    logString("under certain conditions; type 'mbpoll -w' for details.\n\n");
}

// -----------------------------------------------------------------------------
void vUsage(FILE* stream, int exit_msg) {
    char* sMyName = basename(progname);
    fprintf(
        stream, "usage : %s [ options ] device|host [ writevalues... ] [ options ]\n\n", sMyName);

    fprintf(
        stream,
        "ModBus Master Simulator. It allows to read and write in ModBus slave registers\n"
        "                         connected by serial (RTU only) or TCP.\n\n"

        "Arguments :\n"
        "  device        Serial port when using ModBus RTU protocol\n"
        "                  COM1, COM2 ...              on Windows\n"
        "                  /dev/ttyS0, /dev/ttyS1 ...  on Linux\n"
        "                  /dev/ser1, /dev/ser2 ...    on QNX\n"
#ifdef USE_CHIPIO
        // -----------------------------------------------------------------------------
        "                I2c bus when using ModBus RTU via ChipIo serial port\n"
        "                  /dev/i2c-0, /dev/i2c-1 ...  on Linux\n"
// -----------------------------------------------------------------------------
#endif /* USE_CHIPIO defined */
        "  host          Host name or dotted IP address when using ModBus/TCP protocol\n"
        "  writevalues   List of values to be written.\n"
        //          01234567890123456789012345678901234567890123456789012345678901234567890123456789
        "                If none specified (default) %s reads data.\n"
        "                If negative numbers are provided, it will precede the list of\n"
        "                data to be written by two dashes ('--'). for example :\n"
        "                %s -t4:int /dev/ttyUSB0 -- 123 -1568 8974 -12\n",
        sMyName,
        sMyName);

    fprintf(
        stream,
        //          01234567890123456789012345678901234567890123456789012345678901234567890123456789
        "General options : \n"
        "  -m #          mode (rtu or tcp, %s is default)\n"
        "  -a #          Slave address (%d-%d for rtu, %d-%d for tcp, %d is default)\n"
        "                for reading, it is possible to give an address list\n"
        "                separated by commas or colons, for example :\n"
        "                -a 32,33,34,36:40 read [32,33,34,36,37,38,39,40]\n"
        "  -r #          Start reference (%d is default)\n"
        "                for reading, it is possible to give a reference list\n"
        "                separated by commas or colons\n"
        "  -c #          Number of values to read (%d-%d, %d is default)\n"
        "  -u            Read the description of the type, the current status, and other\n"
        "                information specific to a remote device (RTU only)\n"
        "  -t 0          Discrete output (coil) data type (binary 0 or 1)\n"
        "  -t 1          Discrete input data type (binary 0 or 1)\n"
        "  -t 3          16-bit input register data type\n"
        "  -t 3:int16    16-bit input register data type with signed int display\n"
        "  -t 3:hex      16-bit input register data type with hex display\n"
        "  -t 3:string   16-bit input register data type with string (char) display\n"
        "  -t 3:int      32-bit integer data type in input register table\n"
#ifndef MBPOLL_FLOAT_DISABLE
        "  -t 3:float    32-bit float data type in input register table\n"
#endif
        "  -t 4          16-bit output (holding) register data type (default)\n"
        "  -t 4:int16    16-bit output (holding) register data type with signed int display\n"
        "  -t 4:hex      16-bit output (holding) register data type with hex display\n"
        "  -t 4:string   16-bit output (holding) register data type with string (char) display\n"
        "  -t 4:int      32-bit integer data type in output (holding) register table\n"
#ifndef MBPOLL_FLOAT_DISABLE
        "  -t 4:float    32-bit float data type in output (holding) register table\n"
#endif
        "  -0            First reference is 0 (PDU addressing) instead 1\n"
        "  -W            Using function 10 for write a single register\n"
        "  -B            Big endian word order for 32-bit integer and float\n"
        "  -S            Stream input or output, in conjunction with -c it will read and create a\n"
        "                stream output (Use -O to write regular output somewhere else) with the\n"
        "                data from the device, otherwise it will read stdin as a stream input to\n"
        "                write to the device\n"
        "  -O #          Writes standard output to a file\n"
        "  -1            Poll only once only, otherwise every poll rate interval\n"
        "  -l #          Poll rate in ms, ( > %d could be unstable, %d is default)\n"
        "  -o #          Time-out in seconds (%.2f - %.2f, %.2f s is default)\n"
        "  -q            Quiet mode. Once: Minimum output only, Twice: Complete silence\n"
        "Options for ModBus / TCP : \n"
        "  -p #          TCP port number (%s is default)\n"
        "Options for ModBus RTU : \n"
        "  -b #          Baudrate (%d-%d, %d is default)\n"
        "  -d #          Databits (7 or 8, %s for RTU)\n"
        "  -s #          Stopbits (1 or 2, %s is default)\n"
        "  -P #          Parity (none, even, odd, %s is default)\n"
#ifdef MBPOLL_GPIO_RTS
        "  -R [#]        RS-485 mode (/RTS on (0) after sending)\n"
        "                 Optional parameter for the GPIO RTS pin number\n"
        "  -F [#]        RS-485 mode (/RTS on (0) when sending)\n"
        "                 Optional parameter for the GPIO RTS pin number\n"
#else
        "  -R            RS-485 mode (/RTS on (0) after sending)\n"
        "  -F            RS-485 mode (/RTS on (0) when sending)\n"
#endif
#ifdef USE_CHIPIO
        // -----------------------------------------------------------------------------
        "Options for ModBus RTU for ChipIo serial port : \n"
        "  -i #          I2c slave address (0x%02X-0x%02X, 0x%02X is default)\n"
        "  -n #          Irq pin number of GPIO (%d is default)\n"
// -----------------------------------------------------------------------------
#endif /* USE_CHIPIO defined */
        "\n"
        "  -h            Print this help summary page\n"
        "  -V            Print version and exit\n"
        "  -v            Verbose mode.  Causes %s to print debugging messages about\n"
        "                its progress.  This is helpful in debugging connection...\n",
        sModeToStr(DEFAULT_MODE),
        RTU_SLAVEADDR_MIN,
        SLAVEADDR_MAX,
        TCP_SLAVEADDR_MIN,
        SLAVEADDR_MAX,
        DEFAULT_SLAVEADDR,
        DEFAULT_STARTREF,
        NUMOFVALUES_MIN,
        NUMOFVALUES_MAX,
        DEFAULT_NUMOFVALUES,
        POLLRATE_MIN,
        DEFAULT_POLLRATE,
        TIMEOUT_MIN,
        TIMEOUT_MAX,
        DEFAULT_TIMEOUT,
        DEFAULT_TCP_PORT,
        RTU_BAUDRATE_MIN,
        RTU_BAUDRATE_MAX,
        DEFAULT_RTU_BAUDRATE,
        sSerialDataBitsToStr(DEFAULT_RTU_DATABITS),
        sSerialStopBitsToStr(DEFAULT_RTU_STOPBITS),
        sSerialParityToStr(DEFAULT_RTU_PARITY)
#ifdef USE_CHIPIO
        // -----------------------------------------------------------------------------
        ,
        CHIPIO_SLAVEADDR_MIN,
        CHIPIO_SLAVEADDR_MAX,
        DEFAULT_CHIPIO_SLAVEADDR,
        DEFAULT_CHIPIO_IRQPIN
// -----------------------------------------------------------------------------
#endif /* USE_CHIPIO defined */
        ,
        sMyName);
    exit(exit_msg);
}

// -----------------------------------------------------------------------------
void vCheckEnum(char const* sName, int iElmt, int const* iList, int iSize) {
    int i;
    for (i = 0; i < iSize; i++) {
        if (iElmt == iList[i]) {
            return;
        }
    }
    vSyntaxErrorExit("Illegal %s: %d", sName, iElmt);
}

// -----------------------------------------------------------------------------
void vCheckIntRange(char const* sName, int i, int min, int max) {
    if ((i < min) || (i > max)) {
        vSyntaxErrorExit("%s out of range (%d)", sName, i);
    }
}

// -----------------------------------------------------------------------------
void vCheckDoubleRange(char const* sName, double d, double min, double max) {
    if ((d < min) || (d > max)) {
        vSyntaxErrorExit("%s out of range (%g)", sName, d);
    }
}

// -----------------------------------------------------------------------------
int iGetEnum(char const* sName, char* sElmt, char const** psStrList, int const* iList, int iSize) {
    int i;
    for (i = 0; i < iSize; i++) {
        if (strcasecmp(sElmt, psStrList[i]) == 0) {
            PDEBUG("Set %s=%s\n", sName, _strlwr(sElmt));
            return iList[i];
        }
    }
    vSyntaxErrorExit("Illegal %s: %s", sName, sElmt);
    return -1;
}

// -----------------------------------------------------------------------------
char const* sEnumToStr(int iElmt, int const* iList, char const** psStrList, int iSize) {
    int i;

    for (i = 0; i < iSize;) {
        if (iElmt == iList[i]) {
            return psStrList[i];
        }
        i++;
    }
    return sUnknownStr;
}

// -----------------------------------------------------------------------------
char const* sModeToStr(eModes eMode) {
    return sEnumToStr(eMode, iModeList, sModeList, SIZEOF_ILIST(iModeList));
}

// -----------------------------------------------------------------------------
char const* sFunctionToStr(eFunctions eFunction) {
    return sEnumToStr(eFunction, iFunctionList, sFunctionList, SIZEOF_ILIST(iFunctionList));
}

// -----------------------------------------------------------------------------
void vPrintIntList(int* iList, int iLen) {
    int i;
    logChar('[');
    for (i = 0; i < iLen; i++) {
        logString("%d", iList[i]);
        if (i != (iLen - 1)) {
            logChar(',');
        } else {
            logChar(']');
        }
    }
}

// -----------------------------------------------------------------------------
int* iGetIntList(char const* name, char const* sList, int* iLen) {
    // 12,3,5:9,45

    int* iList = NULL;
    int i, iFirst = 0, iCount = 0;
    bool bIsLast  = false;
    char const* p = sList;
    char* endptr;

    PDEBUG("iGetIntList(%s)\n", sList);

    // Comptage et vérification de la liste des entiers
    while (*p) {
        i = strtol(p, &endptr, 0);
        if (endptr == p) {
            vSyntaxErrorExit("Illegal %s value: %s", name, p);
        }
        p = endptr;
        PDEBUG("Integer found: %d\n", i);

        if (*p == ':') {
            // i est le premier d'un plage first:last
            if (bIsLast) {
                // il ne peut pas y avoir 2 * ':' de suite !
                vSyntaxErrorExit("Illegal %s delimiter: '%c'", name, *p);
            }
            PDEBUG("Is First\n");
            iFirst  = i;
            bIsLast = true;
        } else if ((*p == ',') || (*p == 0)) {
            if (bIsLast) {
                int iRange, iLast;

                // i est dernier d'une plage first:last
                iLast  = MAX(iFirst, i);
                iFirst = MIN(iFirst, i);
                iRange = iLast - iFirst + 1;
                PDEBUG("Is Last, add %d items\n", iRange);
                iCount += iRange;
                bIsLast = false;
            } else {
                iCount++;
            }
        } else {
            vSyntaxErrorExit("Illegal %s delimiter: '%c'", name, *p);
        }

        if (*p) {
            p++; // On passe le délimiteur
        }
        PDEBUG("iCount=%d\n", iCount);
    }

    if (iCount > 0) {
        int iIndex = 0;

        // Allocation
        iList = calloc(iCount, sizeof(int));

        // Affectation
        p = sList;
        while (*p) {
            i = strtol(p, &endptr, 0);
            p = endptr;

            if (*p == ':') {
                // i est le premier d'un plage first:last
                iFirst  = i;
                bIsLast = true;
            } else if ((*p == ',') || (*p == 0)) {
                if (bIsLast) {
                    // i est dernier d'une plage first:last
                    int iLast = MAX(iFirst, i);
                    iFirst    = MIN(iFirst, i);

                    for (i = iFirst; i <= iLast; i++) {
                        iList[iIndex++] = i;
                    }
                    bIsLast = false;
                } else {
                    iList[iIndex++] = i;
                }
            }

            if (*p) {
                p++; // On passe le délimiteur
            }
        }
#ifdef DEBUG
        if (ctx.ePrint == ePrintVerbose) {
            vPrintIntList(iList, iCount);
            logChar('\n');
        }
#endif
    }
    *iLen = iCount;
    return iList;
}

// -----------------------------------------------------------------------------
int iGetInt(char const* name, char const* num, int base) {
    char* endptr;

    int i = strtol(num, &endptr, base);
    if (endptr == num) {
        vSyntaxErrorExit("Illegal %s value: %s", name, num);
    }

    PDEBUG("Set %s=%d\n", name, i);
    return i;
}

// -----------------------------------------------------------------------------
double dGetDouble(char const* name, char const* num) {
    char* endptr;

    double d = strtod(num, &endptr);
    if (endptr == num) {
        vSyntaxErrorExit("Illegal %s value: %s", name, num);
    }

    PDEBUG("Set %s=%g\n", name, d);
    return d;
}

// -----------------------------------------------------------------------------
float fSwapFloat(float f) {
    float ret = f;

    if (ctx.bIsBigEndian) {
        uint16_t* in  = (uint16_t*)&f;
        uint16_t* out = (uint16_t*)&ret;
        out[0]        = in[1];
        out[1]        = in[0];
    }
    return ret;
}

// -----------------------------------------------------------------------------
int32_t lSwapLong(int32_t l) {
    int32_t ret = l;

    if (ctx.bIsBigEndian) {
        uint16_t* in  = (uint16_t*)&l;
        uint16_t* out = (uint16_t*)&ret;
        out[0]        = in[1];
        out[1]        = in[0];
    }
    return ret;
}

// -----------------------------------------------------------------------------
void mb_delay(unsigned long d) {
    if (d) {
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
        if (d == -1) {
            sleep(-1);
        } else {
            struct timespec dt;

            dt.tv_nsec = (d % 1000UL) * 1000000UL;
            dt.tv_sec  = d / 1000UL;
            nanosleep(&dt, NULL);
        }
#else
        Sleep(d);
#endif
    }
}

/* ========================================================================== */
