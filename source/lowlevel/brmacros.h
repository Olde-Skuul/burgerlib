/***************************************

	Macros, just macros

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMACROS_H__
#define __BRMACROS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */

// Double precision
/** Pi as a double */
#define BURGER_PI (3.14159265358979323846264338328)

/** sqrt(2.0) as a double */
#define BURGER_SQRT2 (1.41421356237309504880168872421)

/** ln(2.0) as a double */
#define BURGER_LN2 (0.69314718055994530941723212146)

/** ln(10.0) as a double */
#define BURGER_LN10 (2.3025850929940456840179914547)

// Half precision (16 bit float)

/** Smallest value for a 16 bit float */
#define BURGER_HALF_MIN static_cast<float16_t>(0x0001U)

/** Smallest normalized value for a 16 bit float */
#define BURGER_HALF_NRM_MIN static_cast<float16_t>(0x0400U)

/** Largest value stored in a 16 bit float */
#define BURGER_HALF_MAX static_cast<float16_t>(0x7BFFU)

/** Smallest positive difference between 16 bit float values */
#define BURGER_HALF_EPSILON static_cast<float16_t>(0x1400U)

/** 16 bit float positive infinity */
#define BURGER_HALF_POS_INF static_cast<float16_t>(0x7C00U)

/** 16 bit float negative infinity */
#define BURGER_HALF_NEG_INF static_cast<float16_t>(0xFC00U)

/** 16 bit float QNAN */
#define BURGER_HALF_QNAN static_cast<float16_t>(0x7FFFU)

/** 16 bit float SNAN */
#define BURGER_HALF_SNAN static_cast<float16_t>(0x7DFFU)

/* END */

#endif
