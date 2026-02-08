#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "utils.h"

#ifdef DEBUG
#define PDEBUG(fmt,...) printf("utils.c:%d: %s(): " fmt, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define PDEBUG(...)
#endif

// -----------------------------------------------------------------------------
static int parse_next_element(const char **p, int *start, int *end, int *count, const char *name) {
  char *endptr;
  errno = 0;
  long val = strtol(*p, &endptr, 0);

  if (endptr == *p) {
    vSyntaxErrorExit("Illegal %s value: %s", name, *p);
  }

  // Check for overflow
  if (errno == ERANGE || val > INT_MAX || val < INT_MIN) {
    vSyntaxErrorExit("%s value out of range: %ld", name, val);
  }

  *start = (int)val;
  *p = endptr;

  if (**p == ':') {
    (*p)++; // Skip ':'
    errno = 0;
    val = strtol(*p, &endptr, 0);
    if (endptr == *p) {
      vSyntaxErrorExit("Illegal %s range end: %s", name, *p);
    }
    if (errno == ERANGE || val > INT_MAX || val < INT_MIN) {
      vSyntaxErrorExit("%s value out of range: %ld", name, val);
    }
    *end = (int)val;
    *p = endptr;

    int min = mb_min(*start, *end);
    int max = mb_max(*start, *end);
    *count += (max - min + 1);

    // Normalize start/end for caller convenience if needed,
    // but here we just need the count and the values.
    // Let's store the range as min:max
    *start = min;
    *end = max;
  } else {
    *end = *start;
    (*count)++;
  }

  if (**p == ',') {
    (*p)++;
  } else if (**p != '\0') {
    vSyntaxErrorExit("Illegal %s delimiter: '%c'", name, **p);
  }

  return 0;
}

/**
 * Parses a string list of integers (e.g. "1,2,5-10").
 *
 * @param name   The name of the parameter (used for error reporting).
 * @param sList  The string list to parse.
 * @param iLen   Output parameter returning the number of elements.
 *
 * Note: Be careful not to swap 'name' and 'sList'.
 */
int *
iGetIntList (const char * name, const char * sList, int * iLen) {
  // 12,3,5:9,45

  int * iList = NULL;
  int iCount = 0;
  const char * p = sList;

  PDEBUG ("iGetIntList(%s)\n", sList);

  // Pass 1: Count elements
  while (*p) {
    int start, end;
    parse_next_element(&p, &start, &end, &iCount, name);
    PDEBUG ("iCount=%d\n", iCount);
  }

  if (iCount > 0) {
    int iIndex = 0;

    // Allocation
    if ((size_t)iCount > SIZE_MAX / sizeof(int)) {
      vIoErrorExit ("Memory allocation size overflow for %s list", name);
    }
    iList = calloc (iCount, sizeof (int));
    if (iList == NULL) {
      vIoErrorExit ("Memory allocation failed for %s list", name);
    }

    // Pass 2: Fill list
    p = sList;
    while (*p) {
      int start, end, dummy_count = 0;
      parse_next_element(&p, &start, &end, &dummy_count, name);

      for (int i = start; i <= end; i++) {
        iList[iIndex++] = i;
      }
    }
#ifdef DEBUG
    // Removed dependency on ctx.bIsVerbose
    // if (ctx.bIsVerbose) {
    //   vPrintIntList (iList, iCount);
    //   putchar ('\n');
    // }
#endif
  }
  *iLen = iCount;
  return iList;
}

// -----------------------------------------------------------------------------
void
vPrintIntList (int * iList, int iLen) {
  int i;
  putchar ('[');
  for (i = 0; i < iLen; i++) {
    printf ("%d", iList[i]);
    if (i != (iLen - 1)) {
      putchar (',');
    }
    else {
      putchar (']');
    }
  }
}

// -----------------------------------------------------------------------------
int32_t lSwapLong(int32_t l, bool bIsBigEndian) {
    if (!bIsBigEndian) {
        return l;
    }
    int32_t ret;
    uint16_t tmp[2];
    uint16_t swapped[2];

    memcpy(tmp, &l, sizeof(l));

    swapped[0] = tmp[1];
    swapped[1] = tmp[0];

    memcpy(&ret, swapped, sizeof(ret));
    return ret;
}

// -----------------------------------------------------------------------------
float fSwapFloat(float f, bool bIsBigEndian) {
    if (!bIsBigEndian) {
        return f;
    }
    float ret;
    uint16_t tmp[2];
    uint16_t swapped[2];

    memcpy(tmp, &f, sizeof(f));

    swapped[0] = tmp[1];
    swapped[1] = tmp[0];

    memcpy(&ret, swapped, sizeof(ret));
    return ret;
}
