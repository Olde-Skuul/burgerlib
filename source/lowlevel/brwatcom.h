/***************************************

	Intrinsics and subroutines exclusive to the Open Watcom compiler

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
#if defined(BURGER_WATCOM) || defined(DOXYGEN)

#if !defined(DOXYGEN)

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

// Modern intrinsics
extern long _InterlockedExchange(long volatile*, long);
#pragma aux _InterlockedExchange parm[edx][eax] = \
		"lock xchg eax,[edx]" value[eax] modify exact[eax];

extern long _InterlockedIncrement(long volatile*);
#pragma aux _InterlockedIncrement parm[edx] = \
		"mov eax,1" \
		"lock xadd [edx],eax" \
		"inc eax" value[eax] modify exact[eax];

extern long _InterlockedDecrement(long volatile*);
#pragma aux _InterlockedDecrement parm[edx] = \
		"or eax,0FFFFFFFFH" \
		"lock xadd [edx],eax" \
		"dec eax" value[eax] modify exact[eax];

extern long _InterlockedExchangeAdd(long volatile*, long);
#pragma aux _InterlockedExchangeAdd parm[edx][eax] = \
		"lock xadd [edx],eax" value[eax] modify exact[eax];

extern long _InterlockedCompareExchange(long volatile*, long, long);
#pragma aux _InterlockedCompareExchange parm[edx][ecx][eax] = \
		"lock cmpxchg [edx],ecx" value[eax] modify exact[eax];

extern int32_t BurgerIntMathMul32GetUpper32(int32_t iInputA, int32_t iInputB);
#pragma aux BurgerIntMathMul32GetUpper32 = \
	"imul edx" parm[eax][edx] value[edx] modify exact[eax edx] nomemory;

extern int32_t BurgerIntMathMul32x32To64Div32(
	int32_t iInputMulA, int32_t iInputMulB, int32_t iInputDiv);
#pragma aux BurgerIntMathMul32x32To64Div32 = \
	"imul edx" \
	"idiv ebx" parm[eax][edx][ebx] value[eax] modify exact[eax edx];

extern Fixed32 BurgerFixedMathMultiply(Fixed32 fInputMulA, Fixed32 fInputMulB);
#pragma aux BurgerFixedMathMultiply = \
	"imul edx" \
	"shrd eax,edx,16" parm[eax][edx] value[eax] modify \
	exact[eax edx] nomemory;

extern Fixed32 BurgerFixedMathDivide(
	Fixed32 fInputNumerator, Fixed32 fInputDenominator);
#pragma aux BurgerFixedMathDivide = \
	"mov edx,eax" \
	"shl eax,16" \
	"sar edx,16" \
	"idiv ebx" parm[eax][ebx] value[eax] modify exact[eax edx] nomemory;

extern Fixed32 BurgerFixedMathReciprocal(Fixed32 fInput);
#pragma aux BurgerFixedMathReciprocal = \
	"cmp ecx,-1" \
	"mov eax,080000000H" \
	"je Done" \
	"dec eax" \
	"cmp ecx,2" \
	"jb Done" \
	"xor eax,eax" \
	"mov edx,1" \
	"idiv ecx" \
	"Done:" parm[ecx] value[eax] modify exact[eax edx] nomemory;

// Intrinsics from Visual Studio
extern void __watcall __cpuid(int a[4], int b);
extern void __watcall __cpuidex(int a[4], int b, int c);

// Copy of _BitScanForward() from MSVC
// https://learn.microsoft.com/en-us/cpp/intrinsics/bitscanforward-bitscanforward64?view=msvc-170
extern unsigned char _BitScanForward(unsigned long* Index, unsigned long Mask);
#pragma aux _BitScanForward = \
	"bsf eax,eax" \
	"mov dword ptr [edx],eax" \
	"setne al" parm[eax][ecx] value[eax] modify exact[eax];

// Copy of _BitScanReverse() from MSVC
// https://learn.microsoft.com/en-us/cpp/intrinsics/bitscanreverse-bitscanreverse64?view=msvc-170
extern unsigned char _BitScanReverse(unsigned long* Index, unsigned long Mask);
#pragma aux _BitScanReverse = \
	"bsr eax,eax" \
	"mov dword ptr [edx],eax" \
	"setne al" parm[eax][ecx] value[eax] modify exact[eax];
}
#endif
#endif

/* END */

#endif
