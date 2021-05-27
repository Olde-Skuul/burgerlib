/***************************************

    UnPackBytes clone from the Apple IIgs

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRUNPACKBYTES_H__
#define __BRUNPACKBYTES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
extern uintptr_t BURGER_API UnpackBytes(const uint8_t *pInput,uintptr_t uInputLength,uint8_t **ppOutput,uintptr_t *pOutputLength);
}
/* END */

#endif
