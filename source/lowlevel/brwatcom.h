/***************************************

    Intrinsics and subroutines exclusive to the Open Watcom compiler

    Copyright (c) 1995-2019 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
// Disable 'sizeof' operand contains compiler generated information
#pragma disable_message(549)
// Disable assuming unary 'operator &' not overloaded for type
#pragma disable_message(666)

extern "C" {
namespace std {

// Found in math.h
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
//__declspec(__watcall) extern div_t div(int,int);
__declspec(__watcall) extern long int labs(long int);
#pragma intrinsic(abs, /* div, */ labs)

}

__declspec(__watcall) extern unsigned int _rotl(unsigned int, unsigned int);
__declspec(__watcall) extern unsigned int _rotr(unsigned int, unsigned int);
#pragma intrinsic(_rotl, _rotr)

extern const float g_fBurgerIntMathNearesttable[2];
extern const float g_fBurgerMath65536;

extern void WatcomAssertNothing(void);
#pragma aux WatcomAssertNothing = modify exact[] nomemory;

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

extern Int32 BurgerIntMathMul32GetUpper32(Int32 iInputA, Int32 iInputB);
#pragma aux BurgerIntMathMul32GetUpper32 = \
    "imul edx" parm[eax][edx] value[edx] modify exact[eax edx] nomemory;

extern Int32 BurgerIntMathMul32x32To64Div32(
    Int32 iInputMulA, Int32 iInputMulB, Int32 iInputDiv);
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

extern Fixed32 BurgerFixedMathFromFloatFloor(float fInput);
#pragma aux BurgerFixedMathFromFloatFloor = \
    "sub esp,4" \
    "fmul dword ptr [g_fBurgerMath65536]" \
    "fadd dword ptr [g_fBurgerIntMathNearesttable]" \
    "fistp dword ptr [esp]" \
    "pop eax" parm[8087] value[eax] modify exact[eax] nomemory;

extern Fixed32 BurgerFixedMathFromFloat(float fInput);
#pragma aux BurgerFixedMathFromFloat = \
    "sub esp,4" \
    "fst dword ptr [esp]" \
    "fmul dword ptr [g_fBurgerMath65536]" \
    "mov eax,[esp]" \
    "shr eax,31" \
    "fadd dword ptr [g_fBurgerIntMathNearesttable+eax*4]" \
    "fistp dword ptr [esp]" \
    "pop eax" parm[8087] value[eax] modify exact[eax] nomemory;

extern Fixed32 BurgerFixedMathFromFloatCeil(float fInput);
#pragma aux BurgerFixedMathFromFloatCeil = \
    "sub esp,4" \
    "fmul dword ptr [g_fBurgerMath65536]" \
    "fadd dword ptr [g_fBurgerIntMathNearesttable+4]" \
    "fistp dword ptr [esp]" \
    "pop eax" parm[8087] value[eax] modify exact[eax] nomemory;

extern Fixed32 BurgerFixedMathFromFloatNearest(float fInput);
#pragma aux BurgerFixedMathFromFloatNearest = \
    "sub esp,4" \
    "fmul dword ptr [g_fBurgerMath65536]" \
    "fistp dword ptr [esp]" \
    "pop eax" parm[8087] value[eax] modify exact[eax] nomemory;

extern void BurgerFixedMathFromFloatFloor2(Fixed32* pOutput, float fInput);
#pragma aux BurgerFixedMathFromFloatFloor2 = \
    "fmul dword ptr [g_fBurgerMath65536]" \
    "fadd dword ptr [g_fBurgerIntMathNearesttable]" \
    "fistp dword ptr [eax]" parm[eax][8087] modify exact[];

extern void BurgerFixedMathFromFloat2(Fixed32* pOutput, float fInput);
#pragma aux BurgerFixedMathFromFloat2 = \
    "sub esp,4" \
    "fst dword ptr [esp]" \
    "fmul dword ptr [g_fBurgerMath65536]" \
    "pop ecx" \
    "shr ecx,31" \
    "fadd dword ptr [g_fBurgerIntMathNearesttable+ecx*4]" \
    "fistp dword ptr [eax]" parm[eax][8087] modify[ecx];

extern void BurgerFixedMathFromFloatCeil2(Fixed32* pOutput, float fInput);
#pragma aux BurgerFixedMathFromFloatCeil2 = \
    "fmul dword ptr [g_fBurgerMath65536]" \
    "fadd dword ptr [g_fBurgerIntMathNearesttable+4]" \
    "fistp dword ptr [eax]" parm[eax][8087] modify exact[];

extern void BurgerFixedMathFromFloatNearest2(Fixed32* pOutput, float fInput);
#pragma aux BurgerFixedMathFromFloatNearest2 = \
    "fmul dword ptr [g_fBurgerMath65536]" \
    "fistp dword ptr [eax]" parm[eax][8087] modify exact[];

extern void __cpuid(int a[4], int b);
#pragma aux __cpuid = \
    "cpuid" \
    "mov [esi+0],eax" \
    "mov [esi+4],ebx" \
    "mov [esi+8],ecx" \
    "mov [esi+12],edx" parm[esi][eax] modify[ebx ecx edx];

extern void __cpuidex(int a[4], int b, int c);
#pragma aux __cpuidex = \
    "cpuid" \
    "mov [esi+0],eax" \
    "mov [esi+4],ebx" \
    "mov [esi+8],ecx" \
    "mov [esi+12],edx" parm[esi][eax][ecx] modify[ebx ecx edx];

extern unsigned char _BitScanForward(unsigned long* Index, unsigned long Mask);

#pragma aux _BitScanForward = \
    "bsf eax,eax" \
    "mov dword ptr [edx],eax" \
    "setne al" parm[eax][ecx] value[eax] modify exact[eax];

extern unsigned char _BitScanReverse(unsigned long* Index, unsigned long Mask);

#pragma aux _BitScanReverse = \
    "bsr eax,eax" \
    "mov dword ptr [edx],eax" \
    "setne al" parm[eax][ecx] value[eax] modify exact[eax];
}

#endif

/* END */

#endif
