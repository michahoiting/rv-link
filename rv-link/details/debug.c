/**
 *     Copyright (c) 2021, Micha Hoiting.
 *
 * \file  rv-link/details/debug.c
 * \brief Implementation of the debug logging.
  */

/* system library header file includes */
#include <stdio.h>
#include <stdarg.h>

/* own header file include */
#include <rv-link/details/debug.h>

void rvl_debug_log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vprintf(fmt, args);

    va_end(args);
}
