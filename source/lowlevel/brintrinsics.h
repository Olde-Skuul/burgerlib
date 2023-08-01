/***************************************

	Compiler intrinsics.

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

// Inline assembly prototypes, all are done as "C"

extern "C" {

// Compiler prototypes

#if !defined(DOXYGEN)
// Supported by all versions of Visual Studio 7-2022
#if defined(BURGER_MSVC)
extern unsigned short __cdecl _byteswap_ushort(unsigned short);
extern unsigned long __cdecl _byteswap_ulong(unsigned long);
extern unsigned __int64 __cdecl _byteswap_uint64(unsigned __int64);
#pragma intrinsic(_byteswap_ushort, _byteswap_ulong, _byteswap_uint64)

#if defined(BURGER_XBOXONE)
extern unsigned short __cdecl _load_be_u16(void const*);
extern unsigned int __cdecl _load_be_u32(void const*);
extern unsigned __int64 __cdecl _load_be_u64(void const*);
extern void __cdecl _store_be_u16(void *, unsigned short);
extern void __cdecl _store_be_u32(void *, unsigned int);
extern void __cdecl _store_be_u64(void *, unsigned __int64);
#endif

// Visual Studio 2008 and earlier doesn't have this intrinsic
#if defined(BURGER_INTEL) && (BURGER_MSVC < 160000000)
extern "C" uint64_t BURGER_API _xgetbv(unsigned int uInput);
#endif

#elif defined(BURGER_WATCOM) && defined(BURGER_X86)
extern uint64_t _xgetbv(unsigned int uInput);
#pragma aux _xgetbv = "db 0x0F, 0x01, 0xD0" parm[ecx] value[eax edx] modify \
	exact[eax edx] nomemory;

#elif defined(BURGER_METROWERKS) && defined(BURGER_X86)
BURGER_INLINE uint64_t _xgetbv(register uint_t xcr) BURGER_NOEXCEPT
{
	asm {
		mov ecx, xcr
		db 0x0F, 0x01, 0xD0
	}
}

#elif (defined(BURGER_SNSYSTEMS) && defined(BURGER_ARM32))
extern uint32_t __builtin_rev(uint32_t);

#elif defined(BURGER_GHS) && defined(BURGER_PPC)
int __RLWINM(int, int, int, int);
int __RLWNM(int, int, int, int);
int __RLWIMI(int, int, int, int, int);
#define __rlwinm __RLWINM
#define __rlwnm __RLWNM
#define __rlwimi __RLWIMI

#elif defined(BURGER_INTEL_COMPILER)
extern int __cdecl _bswap(int);
extern __int64 __cdecl _bswap64(__int64);

#elif defined(BURGER_PS3)
#include <ppu_intrinsics.h>

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
#endif
}

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
