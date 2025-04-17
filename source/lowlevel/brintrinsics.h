/***************************************

	Compiler intrinsics.

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRINTRINSICS_H__
#define __BRINTRINSICS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */

// Undocumented feature of the Metrowerks compiler.
// Turn on to alert the optimizer that results are static

#if (defined(BURGER_METROWERKS) && defined(BURGER_X86))
#pragma volatile_asm off
#endif

// None of this is to be documented
#if !defined(DOXYGEN)

// Compiler built ins (__builtin__*)

#if defined(BURGER_GNUC) || defined(BURGER_CLANG) || defined(BURGER_SNSYSTEMS)
extern float __builtin_fabsf(float);
extern double __builtin_fabs(double val);
extern float __builtin_sqrtf(float);
extern double __builtin_sqrt(double);
#endif

// Of cource SN Systems would define it differently
#if (defined(BURGER_CLANG) || defined(BURGER_GNUC)) || \
	(defined(BURGER_SNSYSTEMS) && defined(BURGER_PPC))
extern int __builtin_clz(unsigned int);
#elif defined(BURGER_SNSYSTEMS)
extern uint32_t __builtin_clz(uint32_t);
#endif

#if defined(BURGER_GNUC) || defined(BURGER_CLANG) || \
	(defined(BURGER_SNSYSTEMS) && !defined(BURGER_PPC))
extern uint32_t __builtin_rol(uint32_t, const uint32_t);
extern uint32_t __builtin_ror(uint32_t, const uint32_t);
#endif

// Inline assembly prototypes, all are done as "C"

extern "C" {

#if defined(BURGER_CLANG) || defined(BURGER_GNUC)
#if __GNUC_PREREQ(2, 8) && \
	!(defined(BURGER_DARWIN) || defined(BURGER_PS4) || defined(BURGER_SWITCH))
extern float fabsf(float) throw();
extern double fabs(double) throw();
extern float sqrtf(float) throw();
extern double sqrt(double) throw();
#else
extern float fabsf(float);
extern double fabs(double);
extern float sqrtf(float);
extern double sqrt(double);
#endif
#endif

// Supported by all versions of Visual Studio 7-2022
#if defined(BURGER_MSVC)

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

extern unsigned short __cdecl _byteswap_ushort(unsigned short);
extern unsigned long __cdecl _byteswap_ulong(unsigned long);
extern unsigned __int64 __cdecl _byteswap_uint64(unsigned __int64);
#pragma intrinsic(_byteswap_ushort, _byteswap_ulong, _byteswap_uint64)

// Supported by Visual Studio 2015 or higher
#if (BURGER_MSVC >= 190000000) && defined(BURGER_INTEL)
extern unsigned short __cdecl _load_be_u16(void const*);
extern unsigned int __cdecl _load_be_u32(void const*);
extern unsigned __int64 __cdecl _load_be_u64(void const*);
extern void __cdecl _store_be_u16(void*, unsigned short);
extern void __cdecl _store_be_u32(void*, unsigned int);
extern void __cdecl _store_be_u64(void*, unsigned __int64);
#endif

// Visual Studio 2008 and earlier doesn't have this intrinsic
#if defined(BURGER_INTEL) && (BURGER_MSVC < 160000000)
extern "C" uint64_t BURGER_API _xgetbv(unsigned int uInput);
#endif

#elif defined(BURGER_WATCOM) && defined(BURGER_X86)

extern float sqrtf(float);
#pragma aux sqrtf = "fsqrt" parm[8087] value[8087] modify[8087] nomemory;

// Intrinsics from Visual Studio
extern void __watcall __cpuid(int a[4], int b);
extern void __watcall __cpuidex(int a[4], int b, int c);

extern uint64_t _xgetbv(unsigned int uInput);
#pragma aux _xgetbv = "db 0x0F, 0x01, 0xD0" parm[ecx] value[eax edx] modify \
	exact[eax edx] nomemory;

extern unsigned int _mm_getcsr(void);
#pragma aux _mm_getcsr = \
	"push eax" \
	"stmxcsr [esp]" \
	"pop eax" value[eax] modify exact[eax] nomemory;

extern void _mm_setcsr(unsigned int);
#pragma aux _mm_setcsr = \
	"push eax" \
	"ldmxcsr [esp]" \
	"pop eax" parm[eax] modify exact[eax] nomemory;

// Modern intrinsics, lifted from Visual Studio
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

#elif defined(BURGER_METROWERKS) && defined(BURGER_X86)

// Hidden intrinsics 68K and x86
// C_Compilers_Reference_3.2,pdf page 51
// __rol(uint16_t uInput,uint8_t uShift);
// __rol(uint32_t uInput,uint8_t uShift);
// __ror(uint16_t uInput,uint8_t uShift);
// __ror(uint32_t uInput,uint8_t uShift);

BURGER_INLINE uint64_t _xgetbv(register uint_t xcr) BURGER_NOEXCEPT
{
	asm {
		mov ecx, xcr
		db 0x0F, 0x01, 0xD0
	}
}

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

BURGER_INLINE uint32_t _BitScanForward(
	register unsigned long* Index, register unsigned long Mask)
{
	// clang-format off
	__asm {
		mov		eax, Mask
		mov		ebx, Index
		bsf		eax, eax
		mov		dword ptr[ebx],eax
		setne	al
	}
	// clang-format on
}

BURGER_INLINE uint32_t _BitScanReverse(
	register unsigned long* Index, register unsigned long Mask)
{
	// clang-format off
	__asm {
		mov		eax, Mask
		mov		ebx, Index
		bsr		eax, eax
		mov		dword ptr[ebx],eax
		setne	al
	}
	// clang-format on
}

extern void __fastcall __cpuid(int a[4], int b);
extern void __fastcall __cpuidex(int a[4], int b, int c);

#elif defined(BURGER_METROWERKS) && defined(BURGER_68K)

extern double __fabs(double x);
extern void* __alloca(unsigned x);

#elif defined(BURGER_METROWERKS) && defined(BURGER_PPC)

// Most PowerPC CPUs don't have fsqrt or fsqrts in hardware.
// Use Burgerlib's square_root() which is optimized PPC code

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

#elif (defined(BURGER_SNSYSTEMS) && defined(BURGER_ARM32))
extern uint32_t __builtin_rev(uint32_t);

#elif (defined(BURGER_SNSYSTEMS) && defined(BURGER_PPC))
extern float sqrtf(float);
extern double sqrt(double);

#elif defined(BURGER_GHS) && defined(BURGER_PPC)
int __RLWINM(int, int, int, int);
int __RLWNM(int, int, int, int);
int __RLWIMI(int, int, int, int, int);
#define __rlwinm __RLWINM
#define __rlwnm __RLWNM
#define __rlwimi __RLWIMI

#elif defined(BURGER_INTEL_COMPILER)
extern double __cdecl fabs(double);
extern double __cdecl sqrt(double);
extern float __cdecl sqrtf(float);
extern int __cdecl _bswap(int);
extern __int64 __cdecl _bswap64(__int64);

#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)

// clang-format off
// Must be macros due to the fact that the input is also an output
#define __rlwimi(rA, rS, cnt, mb, me) ({ \
	__asm__("rlwimi %0,%2,%3,%4,%5" \
		: "=r"(rA) \
		: "0"(rA), "r"(rS), "n"(cnt), "n"(mb), "n"(me)); \
	rA; })
#define __rlwinm(rS, cnt, mb, me) ({ \
	int rA; \
	__asm__("rlwinm %0,%1,%2,%3,%4" \
		: "=r"(rA) \
		: "r"(rS), "n"(cnt), "n"(mb), "n"(me)); \
	rA; })

#define __stfiwx(dA, iBase, iIndex) \
	__asm__("stfiwx %0, %1, %2" : : "f"(dA), "b%"(iIndex), "r"(iBase) \
		: "memory")

BURGER_INLINE int __lhbrx(void* pInput, int iOffset)
{
	uint16_t uResult;
	__asm__ volatile("lhbrx %0,%1,%2"
		: "=r"(uResult)
		: "b%"(iOffset), "r"(pInput)
		: "memory");
	return static_cast<int>(uResult);
}

BURGER_INLINE int __lwbrx(void* pInput, int iOffset)
{
	uint32_t uResult;
	__asm__ volatile("lwbrx %0,%1,%2"
		: "=r"(uResult)
		: "b%"(iOffset), "r"(pInput)
		: "memory");
	return static_cast<int>(uResult);
}

BURGER_INLINE void __sthbrx(uint16_t uValue, void* pInput, int iOffset)
{
	__asm__("sthbrx %0,%1,%2"
		:
		: "r"(uValue), "b%"(iOffset), "r"(pInput)
		: "memory");
}

BURGER_INLINE void __stwbrx(uint32_t uValue, void* pInput, int iOffset)
{
	__asm__("stwbrx %0,%1,%2"
		:
		: "r"(uValue), "b%"(iOffset), "r"(pInput)
		: "memory");
}
// clang-format on

#elif (defined(BURGER_CLANG) || defined(BURGER_GNUC)) && defined(BURGER_INTEL)

#if defined(BURGER_CLANG) && defined(__XSAVE__)
#define _xgetbv(T) __builtin_ia32_xgetbv((uint64_t)(T))
#elif !__has_builtin(_xgetbv)
extern uint64_t _xgetbv(uint_t xcr) BURGER_NOEXCEPT;
#endif

#endif
}
#endif

#if (defined(BURGER_METROWERKS) && defined(BURGER_X86))
#pragma volatile_asm reset
#endif

/***************************************

	Memory barriers

	They are done as macros

***************************************/

#if defined(BURGER_WATCOM)
extern void BURGER_MEMORYBARRIER(void);
#pragma aux BURGER_MEMORYBARRIER = "" parm[] modify exact[];

#elif defined(BURGER_MSVC)
#if (BURGER_MSVC >= 140000000)
extern "C" void _ReadBarrier(void);
#pragma intrinsic(_ReadBarrier)
#endif
extern "C" void _WriteBarrier(void);
extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_WriteBarrier, _ReadWriteBarrier)
#define BURGER_MEMORYBARRIER() _ReadWriteBarrier()

#elif defined(BURGER_GNUC) || defined(BURGER_CLANG) || defined(BURGER_WIIU) || \
	defined(DOXYGEN)
#define BURGER_MEMORYBARRIER() __asm__ __volatile__("" : : : "memory")

#else
#define BURGER_MEMORYBARRIER() (void)0
#endif

/***************************************

	Threading instructions

***************************************/

// clang-format off
#if defined(BURGER_WATCOM)
extern void BURGER_PAUSEOPCODE(void);
#pragma aux BURGER_PAUSEOPCODE = ".686p" ".xmm2" "pause"

#elif (defined(BURGER_MSVC) || defined(BURGER_METROWERKS)) && defined(BURGER_INTEL)
#define BURGER_PAUSEOPCODE() _mm_pause()

#elif (defined(BURGER_CLANG) || defined(BURGER_GNUC)) && defined(BURGER_INTEL)
#define BURGER_PAUSEOPCODE() __asm__ __volatile__("pause")

#elif defined(BURGER_MSVC) && defined(BURGER_ARM)
extern "C" void __yield(void);
#pragma intrinsic(__yield)
#define BURGER_PAUSEOPCODE() __yield()

#elif defined(BURGER_CLANG) && defined(BURGER_ARM)
#define BURGER_PAUSEOPCODE() __builtin_arm_yield()

#elif defined(BURGER_GNUC) && defined(BURGER_ARM)
#define BURGER_PAUSEOPCODE() __asm__ __volatile__("yield" ::: "memory")

#elif defined(BURGER_METROWERKS) && defined(BURGER_PPC)
#define BURGER_PAUSEOPCODE() asm volatile { or r27, r27, r27 }

#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)
#define BURGER_PAUSEOPCODE() __asm__ __volatile__("or r27,r27,r27")

#elif defined(BURGER_XBOX360)
#define BURGER_PAUSEOPCODE() YieldProcessor()

#elif defined(BURGER_SNSYSTEMS) && defined(BURGER_PPC)
#define BURGER_PAUSEOPCODE() __asm__ volatile("or 27, 27, 27;")

#elif defined(BURGER_RISCV)
#define BURGER_PAUSEOPCODE() \
	__asm__ __volatile__(".insn i 0x0F, 0, x0, x0, 0x010")
	
#else
#define BURGER_PAUSEOPCODE() (void)0
#endif
// clang-format on

/* END */

#endif
