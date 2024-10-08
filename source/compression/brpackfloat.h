/***************************************

    Floating point compression

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRPACKFLOAT_H__
#define __BRPACKFLOAT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
extern float BURGER_API Unpack16ToFloat(int16_t iInput);
extern int16_t BURGER_API PackFloatTo16(float fInput);
extern float BURGER_API Unpack16ToFloat(int16_t iInput,uint32_t uBaseExponent);
extern int16_t BURGER_API PackFloatTo16(float fInput,uint32_t uBaseExponent);
}
/* END */

#endif
