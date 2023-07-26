/***************************************

	Intrinsics and subroutines exclusive to the Freescale and Metrowerks
	compilers

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMETROWERKS_H__
#define __BRMETROWERKS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

// Hidden intrinsics 68K and x86
// C_Compilers_Reference_3.2,pdf page 51
// __rol(uint16_t uInput,uint8_t uShift);
// __rol(uint32_t uInput,uint8_t uShift);
// __ror(uint16_t uInput,uint8_t uShift);
// __ror(uint32_t uInput,uint8_t uShift);

/* BEGIN */
#if defined(BURGER_METROWERKS) && !defined(DOXYGEN)

extern "C" {

#if defined(BURGER_X86)

extern float __builtin_fabsf(float) __attribute__((nothrow))
__attribute__((const));
extern double __builtin_fabs(double) __attribute__((nothrow))
__attribute__((const));
extern double __builtin_sqrt(double) __attribute__((nothrow))
__attribute__((const));
extern unsigned int __builtin___count_leading_zero32(unsigned int)
	__attribute__((nothrow)) __attribute__((const));
extern unsigned int __builtin___count_trailing_zero32(unsigned int)
	__attribute__((nothrow)) __attribute__((const));
extern unsigned int __builtin___count_leading_zero64(unsigned long long)
	__attribute__((nothrow)) __attribute__((const));
extern unsigned int __builtin___count_trailing_zero64(unsigned long long)
	__attribute__((nothrow)) __attribute__((const));
// extern unsigned int __builtin___count_bits32(unsigned long long)
// __attribute__((nothrow)) __attribute__((const)); extern unsigned int
// __builtin___count_bits64(unsigned long long) __attribute__((nothrow))
// __attribute__((const));

BURGER_INLINE long _InterlockedExchange(
	register long volatile* pOutput, register long lValue)
{
	__asm lock xchg lValue, [pOutput];
	return lValue;
}

BURGER_INLINE long _InterlockedIncrement(register long volatile* pOutput)
{
	register long lTemp = 1;
	__asm lock xadd[pOutput], lTemp;
	return lTemp + 1;
}

BURGER_INLINE long _InterlockedDecrement(register long volatile* pOutput)
{
	register long lTemp = -1;
	__asm lock xadd[pOutput], lTemp;
	return lTemp - 1;
}

BURGER_INLINE long _InterlockedExchangeAdd(
	register long volatile* pOutput, register long lValue)
{
	__asm lock xadd[pOutput], lValue;
	return lValue;
}

BURGER_INLINE long _InterlockedCompareExchange(register long volatile* pOutput,
	register long lAfter, register long lBefore)
{
	__asm {
		mov eax,lBefore
		lock cmpxchg [pOutput],lAfter
		mov lBefore,eax
	}
	return lBefore;
}

extern void __cdecl __cpuid(int a[4], int b);
extern void __cdecl __cpuidex(int a[4], int b, int c);

BURGER_INLINE uint32_t _BitScanForward(
	register unsigned long* Index, register unsigned long Mask)
{
	// clang-format off
    BURGER_ASM {
    mov eax, Mask
    mov ebx, Index
    bsf eax, eax
    mov dword ptr[ebx],eax
    setne al
    }
	// clang-format on
}

BURGER_INLINE uint32_t _BitScanReverse(
	register unsigned long* Index, register unsigned long Mask)
{
	// clang-format off
    BURGER_ASM {
    mov eax, Mask
    mov ebx, Index
    bsr eax, eax
    mov dword ptr[ebx],eax
    setne al
    }
	// clang-format on
}

#elif defined(BURGER_68K)

// muls.l d1,d1:d0
#pragma parameter __D1 BurgerIntMathMul32GetUpper32(__D0, __D1)
int32_t BurgerIntMathMul32GetUpper32(int32_t iInputMulA, int32_t iInputMulB) = {
	0x4c01, 0xc01};

// muls.l d1,d1:d0
// divs.l d2,d1:d0
#pragma parameter __D0 BurgerIntMathMul32x32To64Div32(__D0, __D1, __D2)
int32_t BurgerIntMathMul32x32To64Div32(int32_t iInputMulA, int32_t iInputMulB,
	int32_t iInputDiv) = {0x4c01, 0xc01, 0x4c42, 0xc01};

extern double __fabs(double x);
extern void* __alloca(unsigned x);

#elif defined(BURGER_PPC)

// Most PowerPC CPUs don't have fsqrt or fsqrts in hardware.
// Use Burgerlib's Sqrt() which is optimized PPC code

extern double sqrt(double);

BURGER_INLINE float sqrtf(float fInput)
{
	return static_cast<float>(sqrt(fInput));
}

#if __has_intrinsic(__builtin___rotate_left32)
extern unsigned int __builtin___rotate_left32(unsigned int, int)
	__attribute__((nothrow)) __attribute__((const));
#endif

#if __has_intrinsic(__builtin___rotate_right32)
extern unsigned int __builtin___rotate_right32(unsigned int, int)
	__attribute__((nothrow)) __attribute__((const));
#endif

#endif
}
#endif
/* END */

#endif
