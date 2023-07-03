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

#endif
#endif

/***************************************

	uint16_t _swapendian16(uint16_t uInput)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

extern uint16_t _swapendian16(uint16_t uInput);
#pragma aux _swapendian16 = \
	"ror ax,8" parm[ax] value[ax] modify exact[ax] nomemory;

#elif (defined(BURGER_APPLE_SC) && defined(BURGER_68K)) && !defined(DOXYGEN)

// rorw #8,d0
#pragma parameter __D0 _swapendian16(__D0)
uint16_t _swapendian16(uint16_t) = {0xE058};

#elif (defined(BURGER_METROWERKS) && \
	(defined(BURGER_68K) || defined(BURGER_X86))) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(uint16_t uInput) BURGER_NOEXCEPT
{
	return __ror(uInput, 8);
}

#elif (__has_builtin(__builtin_bswap16) || (BURGER_GNUC >= 40800)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(uint16_t uInput) BURGER_NOEXCEPT
{
	return __builtin_bswap16(uInput);
}

#elif ((defined(BURGER_METROWERKS) || defined(BURGER_MRC) || \
		   defined(BURGER_SNSYSTEMS) || defined(BURGER_GHS)) && \
	defined(BURGER_PPC)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput) BURGER_NOEXCEPT
{
	register int iResult = __rlwinm(static_cast<int>(uInput), 24, 24, 31);
	iResult = __rlwimi(iResult, static_cast<int>(uInput), 8, 16, 23);
	return static_cast<uint16_t>(iResult);
}

#elif ( \
	(defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_68K)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput) BURGER_NOEXCEPT
{
	__asm__("rorw $8,%0" : : "=d"(uInput) : "0"(uInput) : "cc");
	return uInput;
}

#elif ( \
	(defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(uint16_t uInput) BURGER_NOEXCEPT
{
	__asm__(
		"rlwinm %0,%1,8,16,23\n"
		"rlwimi %0,%1,24,24,31"
		: "=&r"(uInput)
		: "r"(uInput));
	return uInput;
}

#elif (defined(BURGER_SNSYSTEMS) && defined(BURGER_ARM32)) && !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput) BURGER_NOEXCEPT
{
	return static_cast<uint16_t>((__builtin_rev(uInput) >> 16));
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_ARM32)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput) BURGER_NOEXCEPT
{
	uint16_t uResult;
	__asm__("rev16 %0,%1" : "=r"(uResult) : "r"(uInput));
	return uResult;
}
#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_ARM64)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput) BURGER_NOEXCEPT
{
	uint16_t uResult;
	__asm__("rev16 %w0,%w1" : "=r"(uResult) : "r"(uInput));
	return uResult;
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_INTEL)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput) BURGER_NOEXCEPT
{
	__asm__("rorw $8,%0" : "+r"(uInput));
	return uInput;
}

#elif defined(BURGER_INTEL_COMPILER) && !defined(DOXYGEN)

// _bswap instead of _byteswap_ulong, because... reasons

BURGER_INLINE uint16_t _swapendian16(uint16_t uInput) BURGER_NOEXCEPT
{
	return static_cast<uint16_t>(_bswap(uInput) >> 16);
}

#elif defined(BURGER_MSVC) && !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(uint16_t uInput) BURGER_NOEXCEPT
{
	return _byteswap_ushort(uInput);
}

#else

#warning _swapendian16
BURGER_INLINE uint16_t _swapendian16(uint16_t uInput) BURGER_NOEXCEPT
{
	return ((uInput & 0xFF) << 8) | ((uInput >> 8) & 0xFF);
}

#endif

/***************************************

	32 bit endian swap

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

extern uint32_t _swapendian32(uint32_t uInput);
#pragma aux _swapendian32 = \
	"bswap eax" parm[eax] value[eax] modify exact[eax] nomemory;

#elif (defined(BURGER_APPLE_SC) && defined(BURGER_68K)) && !defined(DOXYGEN)

// rorw #8,d0, swap d0, rorw #8,d0
#pragma parameter __D0 _swapendian32(__D0)
uint32_t _swapendian32(uint32_t) = {0xE058, 0x4840, 0xE058};

#elif (defined(BURGER_METROWERKS) && defined(BURGER_68K)) && !defined(DOXYGEN)

// clang-format off
BURGER_INLINE uint32_t _swapendian32(uint32_t : __D0) :
    __D0 = {
    0xE058, // rorw #8,d0
    0x4840, // swap d0
    0xE058  // rorw #8,d0
};
// clang-format on

#elif (defined(BURGER_METROWERKS) && defined(BURGER_X86)) && !defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(register uint32_t uInput) BURGER_NOEXCEPT
{
	__asm bswap uInput;
	return uInput;
}

#elif (__has_builtin(__builtin_bswap32) || (BURGER_GNUC >= 40300) || \
	defined(BURGER_ARM_COMPILER)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput) BURGER_NOEXCEPT
{
	return __builtin_bswap32(uInput);
}

#elif ((defined(BURGER_METROWERKS) || defined(BURGER_MRC) || \
		   defined(BURGER_SNSYSTEMS) || defined(BURGER_GHS)) && \
	defined(BURGER_PPC)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput) BURGER_NOEXCEPT
{
	register int iResult = __rlwinm(static_cast<int>(uInput), 24, 0, 31);
	iResult = __rlwimi(iResult, static_cast<int>(uInput), 8, 8, 15);
	iResult = __rlwimi(iResult, static_cast<int>(uInput), 8, 24, 31);
	return static_cast<uint32_t>(iResult);
}

#elif ( \
	(defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_68K)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	__asm__(
		"rolw #8,%0\n"
		"swap %0\n"
		"rolw #8,%0"
		: "=d"(uInput)
		: "0"(uInput)
		: "cc");
	return uInput;
}

#elif ( \
	(defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	__asm__(
		"rlwinm %0,%1,24,0,31\n"
		"rlwimi %0,%1,8,8,15\n"
		"rlwimi %0,%1,8,24,31"
		: "=&r"(uInput)
		: "r"(uInput));
	return uInput;
}

#elif (defined(BURGER_SNSYSTEMS) && defined(BURGER_ARM32)) && !defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	return __builtin_rev(uInput);
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_ARM32)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	uint32_t uResult;
	__asm__("rev %0,%1" : "=r"(uResult) : "r"(uInput));
	return uResult;
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_ARM64)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	uint32_t uResult;
	__asm__("rev %w0,%w1" : "=r"(uResult) : "r"(uInput));
	return uResult;
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_INTEL)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	__asm__("bswap %0" : "+r"(uInput));
	return uInput;
}

#elif defined(BURGER_INTEL_COMPILER)

// _bswap instead of _byteswap_ulong, because... reasons

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	return _bswap(uInput);
}

#elif defined(BURGER_MSVC) && !defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	return _byteswap_ulong(uInput);
}

#else

#warning _swapendian32
BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	return ((uInput & 0xFF000000) >> 24) | ((uInput & 0x00FF0000) >> 8) |
		((uInput & 0x0000FF00) << 8) | ((uInput & 0x000000FF) << 24);
}

#endif

/***************************************

	64 bit endian swap

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

extern uint64_t _swapendian64(uint64_t uInput);
#pragma aux _swapendian64 = \
	"bswap eax" \
	"bswap edx" \
	"xchg eax,edx" parm[eax edx] value[eax edx] modify \
	exact[eax edx] nomemory;

#elif (defined(BURGER_APPLE_SC) && defined(BURGER_68K)) && !defined(DOXYGEN)

extern uint64_t _swapendian64(const uint64_t& rInput);

#elif ((defined(BURGER_METROWERKS) && defined(BURGER_68K)) || \
	(defined(BURGER_GHS) && defined(BURGER_PPC)) || \
	(defined(BURGER_SNSYSTEMS) && \
		(defined(BURGER_PPC) || defined(BURGER_ARM32)))) && \
	!defined(DOXYGEN)

extern uint64_t _swapendian64(uint64_t uInput);

#elif (defined(BURGER_METROWERKS) && defined(BURGER_X86)) && !defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(register uint64_t uInput)
{
	register uint32_t uLo = static_cast<uint32_t>(uInput);
	register uint32_t uHi = static_cast<uint32_t>(uInput >> 32);
	__asm {
        mov eax, uHi
        mov edx, uLo
        bswap eax
        bswap edx
	}
}

#elif (__has_builtin(__builtin_bswap64) || (BURGER_GNUC >= 40300) || \
	defined(BURGER_ARM_COMPILER)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	return __builtin_bswap64(uInput);
}

#elif (defined(BURGER_METROWERKS) && defined(BURGER_PPC)) && !defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(register uint64_t uInput) BURGER_NOEXCEPT
{
	// Perform a 64 bit endian swap using register halves
	register uint64_t uTemp;

	// clang-format off
	asm{
		// ROR 24 bits to swap first and last bytes
		rlwinm uTemp@hiword,uInput@loword,24, 0, 31
		rlwinm uTemp@loword,uInput@hiword,24, 0, 31

		// Perform insertions to get the middle two bytes
		rlwimi uTemp@hiword,uInput@loword,8, 8, 15
		rlwimi uTemp@loword,uInput@hiword,8, 8, 15
		rlwimi uTemp@hiword,uInput@loword,8, 24, 31
		rlwimi uTemp@loword,uInput@hiword,8, 24, 31
	}
	// clang-format on

	return uTemp;
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_POWERPC64)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	uint64_t uInputHi = uInput >> 32;
	uint64_t uTemp;
	uint64_t uResult;
	__asm__(
		"rlwinm %0,%2,24,0,31\n"
		"rlwinm %1,%3,24,0,31\n"
		"rlwimi %0,%2,8,8,15\n"
		"rlwimi %1,%3,8,8,15\n"
		"rlwimi %0,%2,8,24,31\n"
		"rlwimi %1,%3,8,24,31\n"
		: "=&r"(uResult), "=&r"(uTemp)
		: "r"(uInput), "r"(uInputHi));
	return (uResult << 32) | uTemp;
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_POWERPC)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	__asm__(
		"rlwinm %L0,%1,24,0,31\n"
		"rlwinm %0,%L1,24,0,31\n"
		"rlwimi %L0,%1,8,8,15\n"
		"rlwimi %0,%L1,8,8,15\n"
		"rlwimi %L0,%1,8,24,31\n"
		"rlwimi %0,%L1,8,24,31"
		: "=&r"(uInput)
		: "r"(uInput));
	return uInput;
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_ARM64)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	uint64_t uResult;
	__asm__("rev %x0,%x1" : "=r"(uResult) : "r"(uInput));
	return uResult;
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_AMD64)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	__asm__("bswap %0" : "+r"(uInput));
	return uInput;
}

#elif defined(BURGER_INTEL_COMPILER) && !defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	return _bswap64(uInput);
}

// Note: This function will fail on Visual Studio 2003, so test for 2005 or
// higher.
#elif (BURGER_MSVC >= 140000000) && !defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	return _byteswap_uint64(uInput);
}

#elif (defined(BURGER_MSVC) && defined(BURGER_X86)) && !defined(DOXYGEN)

// Visual Studio 2003 has a bug that will cause bad code generation
// if using the _byteswap_uint64(). The code below is a workaround

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	return _swapendian32(static_cast<uint32_t>(uInput >> 32)) |
		(static_cast<uint64_t>(_swapendian32(static_cast<uint32_t>(uInput)))
			<< 32);
}

#else

#warning _swapendian64

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput)
{
	return _swapendian32(static_cast<uint32_t>(uInput >> 32)) |
		(static_cast<uint64_t>(_swapendian32(static_cast<uint32_t>(uInput)))
			<< 32);
}

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

#elif  (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)
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
