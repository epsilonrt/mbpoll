/**
 * @file sysio/delay.h
 * @brief Temporisation
 *
 * Copyright © 2014 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */

#ifndef _SYSIO_COMPAT_H_
#define _SYSIO_COMPAT_H_

#if defined(_MSC_VER)

#if _MSC_VER < 1900
#include <stdio.h>
#include <stdarg.h>

#define snprintf c99_snprintf
#define vsnprintf c99_vsnprintf

__inline int c99_vsnprintf (char *outBuf, size_t size, const char *format, va_list ap) {
  int count = -1;

  if (size != 0) {
    count = _vsnprintf_s (outBuf, size, _TRUNCATE, format, ap);
  }
  if (count == -1) {
    count = _vscprintf (format, ap);
  }

  return count;
}

__inline int c99_snprintf (char *outBuf, size_t size, const char *format, ...) {
  int count;
  va_list ap;

  va_start (ap, format);
  count = c99_vsnprintf (outBuf, size, format, ap);
  va_end (ap);

  return count;
}


#endif

#include <string.h>

// -----------------------------------------------------------------------------
__inline int 
strcasecmp(const char *s1, const char *s2) {
  return _stricmp (s1, s2);
}

#endif
#endif /* _SYSIO_COMPAT_H_ defined */
