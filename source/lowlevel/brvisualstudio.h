/***************************************

	Intrinsics and subroutines exclusive to the Microsoft Visual Studio
	compilers

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRVISUALSTUDIO_H__
#define __BRVISUALSTUDIO_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

//
// Note: _MSC_VER values for determining specific version.
//
// _MSC_VER == 1930 (Visual Studio 2022)
// _MSC_VER == 1920 (Visual Studio 2019)
// _MSC_VER == 1910 (Visual Studio 2017)
// _MSC_VER == 1900 (Visual Studio 2015)
// _MSC_VER == 1800 (Visual Studio 2013)
// _MSC_VER == 1700 (Visual Studio 2012)
// _MSC_VER == 1600 (Visual Studio 2010)
// _MSC_VER == 1500 (Visual Studio 2008)
// _MSC_VER == 1400 (Visual Studio 2005)
// _MSC_VER == 1310 (Visual Studio 2003)
// _MSC_VER == 1300 (Visual Studio 2002)
// _MSC_VER == 1200 (Visual Studio 6)
// _MSC_VER == 1100 (Visual studio 5)
//

/* BEGIN */
#if defined(BURGER_MSVC) && !defined(DOXYGEN)

extern "C" {

// Ensure these are intrinsics
extern double __cdecl fabs(double);
extern double __cdecl sqrt(double);
#pragma intrinsic(fabs, sqrt)

#if defined(BURGER_AMD64) || defined(BURGER_ARM)
extern float __cdecl sqrtf(float);
#pragma intrinsic(sqrtf)
#endif

#if defined(BURGER_ARM)
extern float __cdecl fabsf(float);
#pragma intrinsic(fabsf)
#endif

extern unsigned int __cdecl _rotl(unsigned int, int);
extern unsigned __int64 __cdecl _rotl64(unsigned __int64, int);
extern unsigned int __cdecl _rotr(unsigned int, int);
extern unsigned __int64 __cdecl _rotr64(unsigned __int64, int);
#pragma intrinsic(_rotl, _rotl64, _rotr, _rotr64)

#if defined(BURGER_INTEL)

// Visual Studio 2005 or higher has __cpuid()
#if (BURGER_MSVC >= 140000000)
extern void __cpuid(int[4], int);
#pragma intrinsic(__cpuid)
#else

// For Visual Studio 2003 or earlier, use this instead
BURGER_INLINE void __cpuid(int a[4], int b)
{
	// clang-format off
    BURGER_ASM {
    // Get the pointer to the destination buffer
    mov esi,a
    mov eax,b   // Command byte
    cpuid       // Invoke CPUID
    // Store the result in the same order as Visual C
    mov[esi],eax
    mov[esi + 4],ebx
    mov[esi + 8],ecx
    mov[esi + 12],edx
    }
	// clang-format on
}

#endif

// Visual studio 2008 or higher has __cpuidex()
#if (BURGER_MSVC >= 150000000)
extern void __cpuidex(int[4], int, int);
#pragma intrinsic(__cpuidex)
#else

#if defined(BURGER_X86)
// For Visual Studio 2005 or earlier, use this instead
BURGER_INLINE void __cpuidex(int a[4], int b, int c)
{
	// clang-format off
    BURGER_ASM {
    // Get the pointer to the destination buffer
    mov esi,a
    mov eax,b	// Command byte
    mov ecx,c	// Get the sub command
    cpuid		// Invoke CPUID
    // Store the result in the same order as Visual C
    mov[esi],eax
    mov[esi + 4],ebx
    mov[esi + 8],ecx
    mov[esi + 12],edx
    }
	// clang-format on
}
#endif
#endif

#endif

// Not available on Xbox 360
#if !defined(BURGER_XBOX360)
unsigned char _BitScanForward(unsigned long* Index, unsigned long Mask);
unsigned char _BitScanReverse(unsigned long* Index, unsigned long Mask);
#pragma intrinsic(_BitScanForward, _BitScanReverse)
#endif

// Visual Studio before 2012 doesn't __cdecl these
#if defined(BURGER_X86) && (BURGER_MSVC >= 170000000)
long __cdecl _InterlockedExchange(long volatile*, long);
long __cdecl _InterlockedExchangeAdd(long volatile*, long);
long __cdecl _InterlockedCompareExchange(long volatile*, long, long);
#else
long _InterlockedExchange(long volatile*, long);
long _InterlockedExchangeAdd(long volatile*, long);
long _InterlockedCompareExchange(long volatile*, long, long);
#endif
#pragma intrinsic(_InterlockedExchange, _InterlockedExchangeAdd, \
	_InterlockedCompareExchange)

long __cdecl _InterlockedIncrement(long volatile*);
long __cdecl _InterlockedDecrement(long volatile*);
#pragma intrinsic(_InterlockedIncrement, _InterlockedDecrement)

#if defined(BURGER_64BITCPU)
__int64 _InterlockedExchange64(__int64 volatile*, __int64);
__int64 _InterlockedIncrement64(__int64 volatile*);
__int64 _InterlockedDecrement64(__int64 volatile*);
__int64 _InterlockedExchangeAdd64(__int64 volatile*, __int64);
__int64 _InterlockedCompareExchange64(__int64 volatile*, __int64, __int64);
#pragma intrinsic(_InterlockedExchange64, _InterlockedIncrement64, \
	_InterlockedDecrement64, _InterlockedExchangeAdd64, \
	_InterlockedCompareExchange64)

// Not available on Xbox 360
#if !defined(BURGER_XBOX360)
unsigned char _BitScanForward64(unsigned long* Index, unsigned __int64 Mask);
unsigned char _BitScanReverse64(unsigned long* Index, unsigned __int64 Mask);
#pragma intrinsic(_BitScanForward64, _BitScanReverse64)
#endif

#endif

// Visual studio 2005 and earlier don't have these SSE type conversions
#if defined(BURGER_AMD64) && (BURGER_MSVC < 150000000)
BURGER_INLINE float _mm_cvtss_f32(__m128 vInput)
{
	return vInput.m128_f32[0];
}

BURGER_INLINE double _mm_cvtsd_f64(__m128d vInput)
{
	return vInput.m128d_f64[0];
}

BURGER_INLINE __m128 _mm_castpd_ps(__m128d vInput)
{
	union {
		__m128d a;
		__m128 b;
	} x;
	x.a = vInput;
	return x.b;
}

BURGER_INLINE __m128i _mm_castpd_si128(__m128d vInput)
{
	union {
		__m128d a;
		__m128i b;
	} x;
	x.a = vInput;
	return x.b;
}

BURGER_INLINE __m128d _mm_castps_pd(__m128 vInput)
{
	union {
		__m128 a;
		__m128d b;
	} x;
	x.a = vInput;
	return x.b;
}

BURGER_INLINE __m128i _mm_castps_si128(__m128 vInput)
{
	union {
		__m128 a;
		__m128i b;
	} x;
	x.a = vInput;
	return x.b;
}

BURGER_INLINE __m128 _mm_castsi128_ps(__m128i vInput)
{
	union {
		__m128i a;
		__m128 b;
	} x;
	x.a = vInput;
	return x.b;
}

BURGER_INLINE __m128d _mm_castsi128_pd(__m128i vInput)
{
	union {
		__m128i a;
		__m128d b;
	} x;
	x.a = vInput;
	return x.b;
}

#endif
}
#endif

/***************************************

	__cpuid() and __cpuidex() intrinsics for other compilers

***************************************/

#if defined(BURGER_INTEL) && (defined(BURGER_CLANG) || defined(BURGER_GNUC))

#if defined(BURGER_X86) && defined(__PIC__)

// If PIC is enabled, EBX / RBX is the pointer to the local data
// so it must be preserved

BURGER_INLINE void __cpuid(int a[4], int b)
{
	__asm__ __volatile__(
		"pushl	%%ebx\n"
		"cpuid\n"
		"movl	%%ebx,%1\n"
		"popl	%%ebx"
		: "=a"((a)[0]), "=r"((a)[1]), "=c"((a)[2]), "=d"((a)[3])
		: "0"(b));
}

BURGER_INLINE void __cpuidex(int a[4], int b, int c)
{
	__asm__ __volatile__(
		"pushl	%%ebx\n"
		"cpuid\n"
		"movl	%%ebx,%1\n"
		"popl	%%ebx"
		: "=a"((a)[0]), "=r"((a)[1]), "=c"((a)[2]), "=d"((a)[3])
		: "0"(b), "2"(c));
}

#elif defined(BURGER_AMD64) && defined(__PIC__)

extern "C" {
BURGER_INLINE void __cpuid(int a[4], int b)
{
	__asm__ __volatile__(
		"pushq	%%rbx\n"
		"cpuid\n"
		"movl	%%ebx,%1\n"
		"popq	%%rbx"
		: "=a"((a)[0]), "=r"((a)[1]), "=c"((a)[2]), "=d"((a)[3])
		: "0"(b));
}

BURGER_INLINE void __cpuidex(int a[4], int b, int c)
{
	__asm__ __volatile__(
		"pushq	%%rbx\n"
		"cpuid\n"
		"movl	%%ebx,%1\n"
		"popq	%%rbx"
		: "=a"((a)[0]), "=r"((a)[1]), "=c"((a)[2]), "=d"((a)[3])
		: "0"(b), "2"(c));
}
}

#else

BURGER_INLINE void __cpuid(int a[4], int b)
{
	__asm__ __volatile__(
		"cpuid"
		: "=a"((a)[0]), "=b"((a)[1]), "=c"((a)[2]), "=d"((a)[3])
		: "0"(b));
}

BURGER_INLINE void __cpuidex(int a[4], int b, int c)
{
	__asm__ __volatile__(
		"cpuid"
		: "=a"((a)[0]), "=b"((a)[1]), "=c"((a)[2]), "=d"((a)[3])
		: "0"(b), "2"(c));
}

#endif

// If not an intrinsic, implement _BitScanForward() and _BitScanReverse()
#if !__has_builtin(_BitScanForward)
BURGER_INLINE uint8_t _BitScanForward(unsigned long* Index, unsigned long Mask)
{
	uint8_t bZero;
	__asm__(
		"bsf %2, %0 \n \
		setne %1"
		: "=r"(*Index), "=q"(bZero)
		: "mr"(Mask));
	return bZero;
}
#endif

#if !__has_builtin(_BitScanReverse)
BURGER_INLINE uint8_t _BitScanReverse(unsigned long* Index, unsigned long Mask)
{
	uint8_t bZero;
	__asm__(
		"bsr %2, %0 \n \
		setne %1"
		: "=r"(*Index), "=q"(bZero)
		: "mr"(Mask));
	return bZero;
}
#endif

#elif defined(BURGER_PPC) && defined(BURGER_MSVC)

#define __cntlzw(x) _CountLeadingZeros(x)
#define __cntlzd(x) _CountLeadingZeros64(x)

#endif

/* END */

#endif
