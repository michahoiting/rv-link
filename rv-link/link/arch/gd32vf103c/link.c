/**
 *     Copyright (c) 2021, Micha Hoiting.
 *
 * \file  rv-link/link/arch/gd32vf103c/link.c
 * \brief Platform specific implementation of generic link related functions.
  */

/* own header file include */
#include <rv-link/link/link.h>

/* system library header file includes */
#include <stddef.h>
#include <stdio.h>

const char* rvl_link_get_unique_dev_id(char *str, size_t len)
{
    snprintf(str, len, "SN%08X", (unsigned int) (
            *(uint32_t*) 0x1FFFF7E8 ^
            *(uint32_t*) 0x1FFFF7EC ^
            *(uint32_t*) 0x1FFFF7F0));
    return str;
}
