/***************************************

    Typedefs specific to the Playstation 3

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRPS3TYPES_H__
#define __BRPS3TYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if defined(BURGER_PS3) && !defined(DOXYGEN)

extern "C" {
extern float sqrtf(float);
extern double sqrt(double);
}

float __builtin_fabsf(float);
double __builtin_fabs(double val);

#endif

/* END */

#endif
