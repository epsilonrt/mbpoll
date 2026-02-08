#ifndef MBPOLL_UTILS_H
#define MBPOLL_UTILS_H

#include <stdbool.h>
#include <stdint.h>

static inline int mb_min(int a, int b) {
    return (a < b) ? a : b;
}

static inline int mb_max(int a, int b) {
    return (a > b) ? a : b;
}

void vFailureExit (bool bHelp, const char *format, ...);

#define vSyntaxErrorExit(fmt,...) vFailureExit(true,fmt,##__VA_ARGS__)
#define vIoErrorExit(fmt,...) vFailureExit(false,fmt,##__VA_ARGS__)

int * iGetIntList (const char * name, const char * sList, int * iLen);
void vPrintIntList (int * iList, int iLen);

/**
 * @brief Swaps the two 16-bit words of a 32-bit integer if big endian flag is set.
 *
 * @param l The 32-bit integer to process.
 * @param bIsBigEndian If true, the two 16-bit halves are swapped.
 * @return The processed 32-bit integer.
 */
int32_t lSwapLong(int32_t l, bool bIsBigEndian);

/**
 * @brief Swaps the two 16-bit words of a 32-bit float if big endian flag is set.
 *
 * @param f The float to process.
 * @param bIsBigEndian If true, the two 16-bit halves are swapped.
 * @return The processed float.
 */
float fSwapFloat(float f, bool bIsBigEndian);

#endif /* MBPOLL_UTILS_H */
