#ifndef PLATFORM_LK_H
#define PLATFORM_LK_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define platform_printf printf
#define platform_flush() fflush(stdout)
#define platform_printerror printf
#define platform_strlen strlen
#define platform_malloc malloc
#define platform_free free
#define platform_sprintf sprintf
#define platform_strcpy strcpy
#define platform_memcpy memcpy

#endif
/***   End Of File   ***/
