/***************************************

    Adler 16 hash manager

    Implemented following the documentation found in
    http://en.wikipedia.org/wiki/Adler-32
    and return a 16 bit version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRADLER16_H__
#define __BRADLER16_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
extern uint_t BURGER_API CalcAdler16(const void *pInput,uintptr_t uInputLength,uint_t uAdler16=1);
}
/* END */

#endif
