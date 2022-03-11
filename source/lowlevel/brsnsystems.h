/***************************************

	Intrinsics and subroutines exclusive to the SN Systems compilers

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSNSYSTEMS_H__
#define __BRSNSYSTEMS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */

#if defined(BURGER_SNSYSTEMS) && !defined(DOXYGEN)

extern float __builtin_fabsf(float);
extern double __builtin_fabs(double);
extern float __builtin_sqrtf(float);
extern double __builtin_sqrt(double);
extern uint32_t __builtin_rol(uint32_t, const uint32_t);
extern uint32_t __builtin_ror(uint32_t, const uint32_t);
extern int __builtin_clz(int);

#endif
/* END */

#endif
