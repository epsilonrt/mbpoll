/*
 * sysio/win32/delay.c
 * @brief fonctions de temporisation
 * Copyright (c) 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sysio/delay.h>
#ifdef SYSIO_OS_UNIX
#include <unistd.h>
#include <sys/select.h>
#include <time.h>
#include <errno.h>
#define Sleep(x) sleep(x)
#define INFINITE -1
#else
#include <windows.h>
#endif

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
// Must be thread friendly
int
delay_ms (long lMs) {

#ifdef _WIN32
  if (lMs == -1) {

    Sleep (INFINITE);
  }
  else {
    Sleep (lMs);
  }
#else
  if (lMs == -1) {

    sleep (-1);
  }
  else {
    struct timespec request, remaining;
    request.tv_nsec = (lMs % 1000UL) * 1000000UL;
    request.tv_sec  = lMs / 1000UL;

    request.tv_sec = lMs / 1000;
    while (nanosleep (&request, &remaining) == -1 && errno == EINTR) {
      request = remaining;
    }
  }
#endif
  return 0;
}

// -----------------------------------------------------------------------------
// Must be thread friendly
int
delay_us (long lUs) {

  if (lUs) {

    if (lUs == -1) {

      Sleep (INFINITE);
    }
    else {
      struct timeval tv;
      tv.tv_sec = lUs / 1000000UL;
      tv.tv_usec = lUs % 1000000UL;
      return select (0, NULL, NULL, NULL, &tv);
    }
  }
  return 0;
}

/* ========================================================================== */
