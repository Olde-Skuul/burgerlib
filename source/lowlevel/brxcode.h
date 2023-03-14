/***************************************

	Intrinsics and subroutines exclusive to the Xcode compilers

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRXCODE_H__
#define __BRXCODE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if defined(BURGER_DARWIN) && (defined(BURGER_GNUC) || defined(BURGER_CLANG))

extern "C" float sqrtf(float);
extern "C" double sqrt(double);
extern "C" float fabsf(float);
extern "C" double fabs(double);

#if defined(BURGER_PPC)

BURGER_INLINE float __sqrt(float fA)
{
	float fD;
	__asm__("fsqrts %0,%1" : "=f"(fD) : "f"(fA));
	return fD;
}

BURGER_INLINE double __sqrt(double dA)
{
	double dD;
	__asm__("fsqrt %0,%1" : "=f"(dD) : "f"(dA));
	return dD;
}

BURGER_INLINE float __fsel(float fA, float fB, float fC)
{
	float fD;
	__asm__("fsel %0,%1,%2,%3" : "=f"(fD) : "f"(fA), "f"(fB), "f"(fC));
	return fD;
}

BURGER_INLINE double __fsel(double dA, double dB, double dC)
{
	double dD;
	__asm__("fsel %0,%1,%2,%3" : "=f"(dD) : "f"(dA), "f"(dB), "f"(dC));
	return dD;
}

BURGER_INLINE double __fnabs(double dA)
{
	double dD;
	__asm__("fnabs %0, %1" : "=f"(dD) : "f"(dA));
	return dD;
}

BURGER_INLINE double __fctiw(double dA)
{
	double dD;
	__asm__("fctiw %0, %1" : "=f"(dD) : "f"(dA));
	return dD;
}

BURGER_INLINE double __fctiwz(double dA)
{
	double dD;
	__asm__("fctiwz %0, %1" : "=f"(dD) : "f"(dA));
	return dD;
}

BURGER_INLINE double __fctidz(double dA)
{
	double dD;
	__asm__("fctidz %0, %1" : "=f"(dD) : "f"(dA));
	return dD;
}

BURGER_INLINE double __fctid(double dA)
{
	double dD;
	__asm__("fctid %0, %1" : "=f"(dD) : "f"(dA));
	return dD;
}

BURGER_INLINE double __fcfid(double dA)
{
	double dD;
	__asm__("fcfid %0, %1" : "=f"(dD) : "f"(dA));
	return dD;
}

BURGER_INLINE uint32_t __cntlzw(uint32_t uInput)
{
	uint32_t uResult;
	__asm__("cntlzw %0,%1" : "=r"(uResult) : "r"(uInput));
	return uResult;
}

BURGER_INLINE uint64_t __cntlzd(uint64_t uInput)
{
	uint64_t uResult;
	__asm__("cntlzd %0,%1" : "=r"(uResult) : "r"(uInput));
	return uResult;
}

#endif

#endif
/* END */

#endif
