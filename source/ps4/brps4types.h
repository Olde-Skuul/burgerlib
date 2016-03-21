/***************************************

	Typedefs specific to the Playstation 4

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRPS4TYPES_H__
#define __BRPS4TYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if defined(BURGER_PS4) && !defined(DOXYGEN)
struct pthread_mutex;
extern "C" {
extern float fabsf(float);
extern double fabs(double);
extern float sqrtf(float);
extern double sqrt(double);
}
float __builtin_fabsf(float);
double __builtin_fabs(double val);
float __builtin_sqrtf(float val);
double __builtin_sqrt(double val);
#endif
/* END */

#endif
