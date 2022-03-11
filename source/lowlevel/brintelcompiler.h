/***************************************

	Intrinsics and subroutines exclusive to the Intel compiler.

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRINTELCOMPILER_H__
#define __BRINTELCOMPILER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if defined(BURGER_INTEL_COMPILER) && !defined(DOXYGEN)

extern "C" {
extern double __cdecl fabs(double);
extern double __cdecl sqrt(double);
extern float __cdecl sqrtf(float);
}

#endif

/* END */

#endif
