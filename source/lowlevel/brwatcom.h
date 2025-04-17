/***************************************

	Intrinsics and subroutines exclusive to the Open Watcom compiler

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRWATCOM_H__
#define __BRWATCOM_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if defined(BURGER_WATCOM) && !defined(DOXYGEN)

extern "C" {
namespace std {

// Declare these math functions to be intrinsics
#if defined(BURGER_X86)
__declspec(__watcall) extern double log(double);
__declspec(__watcall) extern double cos(double);
__declspec(__watcall) extern double sin(double);
__declspec(__watcall) extern double tan(double);
__declspec(__watcall) extern double sqrt(double);
__declspec(__watcall) extern double fabs(double);
__declspec(__watcall) extern double pow(double, double);
__declspec(__watcall) extern double atan2(double, double);
__declspec(__watcall) extern double fmod(double, double);
#pragma intrinsic(log, cos, sin, tan, sqrt, fabs, pow, atan2, fmod)

__declspec(__watcall) extern double acos(double);
__declspec(__watcall) extern double asin(double);
__declspec(__watcall) extern double atan(double);
__declspec(__watcall) extern double cosh(double);
__declspec(__watcall) extern double exp(double);
__declspec(__watcall) extern double log10(double);
__declspec(__watcall) extern double sinh(double);
__declspec(__watcall) extern double tanh(double);
#pragma intrinsic(acos, asin, atan, cosh, exp, log10, sinh, tanh)
#endif

__declspec(__watcall) extern int abs(int);
__declspec(__watcall) extern long int labs(long int);
#pragma intrinsic(abs, labs)

}

__declspec(__watcall) extern unsigned int _rotl(unsigned int, unsigned int);
__declspec(__watcall) extern unsigned int _rotr(unsigned int, unsigned int);
#pragma intrinsic(_rotl, _rotr)
}
#endif

/* END */

#endif
