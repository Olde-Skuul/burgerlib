/***************************************

    Handle endian swapping

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRENDIAN_H__
#define __BRENDIAN_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRWATCOM_H__
#include "brwatcom.h"
#endif

#ifndef __BRMETROWERKS_H__
#include "brmetrowerks.h"
#endif

#ifndef __BRVISUALSTUDIO_H__
#include "brvisualstudio.h"
#endif

#ifndef __BRINTELCOMPILER_H__
#include "brintelcompiler.h"
#endif

#ifndef __BRXCODE_H__
#include "brxcode.h"
#endif

/* BEGIN */

// Undocumented feature of the Metrowerks compiler.
// Turn on to alert the optimizer that results are static

#if (defined(BURGER_METROWERKS) && defined(BURGER_X86))
#pragma volatile_asm off
#endif

// Inline assembly prototypes, all are done as "C"

extern "C" {

/***************************************

    Symantec Cpp 68k

 ***************************************/

#if (defined(BURGER_APPLE_SC) && defined(BURGER_68K)) && !defined(DOXYGEN)
// rorw #8,d0
#pragma parameter __D0 _swapendian16(__D0)
uint16_t _swapendian16(uint16_t) = {0xE058};

// rorw #8,d0, swap d0, rorw #8,d0
#pragma parameter __D0 _swapendian32(__D0)
uint32_t _swapendian32(uint32_t) = {0xE058, 0x4840, 0xE058};

extern uint64_t _swapendian64(const uint64_t& rInput);

/***************************************

    Metrowerks 68k

***************************************/

#elif (defined(BURGER_METROWERKS) && defined(BURGER_68K)) && !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(uint16_t uInput)
{
    return __ror(uInput, 8);
}

// clang-format off
BURGER_INLINE uint32_t _swapendian32(uint32_t : __D0):
__D0 = {
    0xE058, // rorw #8,d0
    0x4840, // swap d0
    0xE058  // rorw #8,d0
};
// clang-format on

extern uint64_t _swapendian64(uint64_t uInput);

/***************************************

    GNU for 68K
    Favor built in intrinsics over inline assembly.

***************************************/

#elif (defined(BURGER_GNUC) && defined(BURGER_68K)) && !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput)
{
#if __has_builtin(__builtin_bswap16) || (BURGER_GNUC >= 40800)
    return __builtin_bswap16(uInput);
#else
    __asm__("rorw $8,%0" : : "=d"(uInput) : "0"(uInput) : "cc");
    return uInput;
#endif
}

BURGER_INLINE uint32_t _swapendian32(register uint32_t uInput)
{
#if __has_builtin(__builtin_bswap32) || (BURGER_GNUC >= 40300)
    return __builtin_bswap32(uInput);
#else
    __asm__(
        "rolw #8,%0\n"
        "swap %0\n"
        "rolw #8,%0"
        : "=d"(uInput)
        : "0"(uInput)
        : "cc");
    return uInput;
#endif
}

/***************************************

    Metrowerks PowerPC 32

***************************************/

#elif (defined(BURGER_METROWERKS) && defined(BURGER_PPC)) && !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput)
{
    int iResult = __rlwinm(static_cast<int>(uInput), 24, 24, 31);
    iResult = __rlwimi(iResult, static_cast<int>(uInput), 8, 16, 23);
    return static_cast<uint16_t>(iResult);
}

BURGER_INLINE uint32_t _swapendian32(register uint32_t uInput)
{
    int iResult = __rlwinm(static_cast<int>(uInput), 24, 0, 31);
    iResult = __rlwimi(iResult, static_cast<int>(uInput), 8, 8, 15);
    iResult = __rlwimi(iResult, static_cast<int>(uInput), 8, 24, 31);
    return static_cast<uint32_t>(iResult);
}

BURGER_INLINE uint64_t _swapendian64(register uint64_t uInput)
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

/***************************************

    SN Systems for PowerPC 32 (Playstation 3)

***************************************/

#elif (defined(BURGER_SNSYSTEMS) && defined(BURGER_PPC)) && !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput)
{
    int iResult = __rlwinm(static_cast<int>(uInput), 24, 24, 31);
    iResult = __rlwimi(iResult, static_cast<int>(uInput), 8, 16, 23);
    return static_cast<uint16_t>(iResult);
}

BURGER_INLINE uint32_t _swapendian32(register uint32_t uInput)
{
    int iResult = __rlwinm(static_cast<int>(uInput), 24, 0, 31);
    iResult = __rlwimi(iResult, static_cast<int>(uInput), 8, 8, 15);
    iResult = __rlwimi(iResult, static_cast<int>(uInput), 8, 24, 31);
    return static_cast<uint32_t>(iResult);
}

/***************************************

    GNU and clang for PowerPC 32
    Favor built in intrinsics over inline assembly.

***************************************/

#elif ( \
    (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)) && \
    !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(uint16_t uInput)
{
#if __has_builtin(__builtin_bswap16) || (BURGER_GNUC >= 40800)
    return __builtin_bswap16(uInput);
#else
    __asm__(
        "rlwinm %0,%1,8,16,23\n"
        "rlwimi %0,%1,24,24,31"
        : "=&r"(uInput)
        : "r"(uInput));
    return uInput;
#endif
}

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
#if __has_builtin(__builtin_bswap32) || (BURGER_GNUC >= 40300)
    return __builtin_bswap32(uInput);
#else
    __asm__(
        "rlwinm %0,%1,24,0,31\n"
        "rlwimi %0,%1,8,8,15\n"
        "rlwimi %0,%1,8,24,31"
        : "=&r"(uInput)
        : "r"(uInput));
    return uInput;
#endif
}

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput)
{
#if __has_builtin(__builtin_bswap64) || (BURGER_GNUC >= 40300)
    return __builtin_bswap64(uInput);
#else
#if defined(BURGER_POWERPC64)
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
#else
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
#endif
#endif
}

/***************************************

    SN Systems for ARM 32

***************************************/

#elif (defined(BURGER_SNSYSTEMS) && defined(BURGER_ARM32)) && !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput)
{
    return static_cast<uint16_t>((__builtin_rev(uInput) >> 16));
}

BURGER_INLINE uint32_t _swapendian32(register uint32_t uInput)
{
    return __builtin_rev(uInput);
}

/***************************************

    GNU and clang for ARM 32
    Favor built in intrinsics over inline assembly.

***************************************/

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
    defined(BURGER_ARM32)) && \
    !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput)
{
#if __has_builtin(__builtin_bswap16) || (BURGER_GNUC >= 40800)
    return __builtin_bswap16(uInput);
#else
    uint16_t uResult;
    __asm__("rev16 %0,%1" : "=r"(uResult) : "r"(uInput));
    return uResult;
#endif
}

BURGER_INLINE uint32_t _swapendian32(register uint32_t uInput)
{
#if __has_builtin(__builtin_bswap32) || (BURGER_GNUC >= 40300)
    return __builtin_bswap32(uInput);
#else
    uint32_t uResult;
    __asm__("rev %0,%1" : "=r"(uResult) : "r"(uInput));
    return uResult;
#endif
}

/***************************************

    GNU and clang for ARM 64
    Favor built in intrinsics over inline assembly.

***************************************/

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
    defined(BURGER_ARM64)) && \
    !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput)
{
#if __has_builtin(__builtin_bswap16) || (BURGER_GNUC >= 40800)
    return __builtin_bswap16(uInput);
#else
    uint16_t uResult;
    __asm__("rev16 %w0,%w1" : "=r"(uResult) : "r"(uInput));
    return uResult;
#endif
}

BURGER_INLINE uint32_t _swapendian32(register uint32_t uInput)
{
#if __has_builtin(__builtin_bswap32) || (BURGER_GNUC >= 40300)
    return __builtin_bswap32(uInput);
#else
    uint32_t uResult;
    __asm__("rev %w0,%w1" : "=r"(uResult) : "r"(uInput));
    return uResult;
#endif
}

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput)
{
#if __has_builtin(__builtin_bswap64) || (BURGER_GNUC >= 40300)
    return __builtin_bswap64(uInput);
#else
    uint64_t uResult;
    __asm__("rev %x0,%x1" : "=r"(uResult) : "r"(uInput));
    return uResult;
#endif
}

/***************************************

    Metrowerks x86

***************************************/

#elif (defined(BURGER_METROWERKS) && defined(BURGER_X86)) && !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(uint16_t uInput)
{
    return __ror(uInput, 8);
}

BURGER_INLINE uint32_t _swapendian32(register uint32_t uInput)
{
    __asm bswap uInput;
    return uInput;
}

BURGER_INLINE uint64_t _swapendian64(register uint64_t uInput)
{
    register uint32_t uLo = static_cast<uint32_t>(uInput);
    register uint32_t uHi = static_cast<uint32_t>(uInput >> 32);
    __asm {
      mov eax,uHi
      mov edx,uLo
      bswap eax
      bswap edx
    }
}

/***************************************

    Watcom assembly to simulate Visual Studio intrinsics

***************************************/

#elif (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

// Simulate the Visual Studio intrinsics

extern uint16_t _swapendian16(uint16_t uInput);
extern uint32_t _swapendian32(uint32_t uInput);
extern uint64_t _swapendian64(uint64_t uInput);

#pragma aux _swapendian16 = \
    "ror ax,8" parm[ax] value[ax] modify exact[ax] nomemory;

#pragma aux _swapendian32 = \
    "bswap eax" parm[eax] value[eax] modify exact[eax] nomemory;

#pragma aux _swapendian64 = \
    "bswap eax" \
    "bswap edx" \
    "xchg eax,edx" parm[eax edx] value[eax edx] modify \
        exact[eax edx] nomemory;

/***************************************

    GNU and clang for i386
    Favor built in intrinsics over inline assembly.

***************************************/

#elif ( \
    (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_X86)) && \
    !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput)
{
#if __has_builtin(__builtin_bswap16) || (BURGER_GNUC >= 40800)
    return __builtin_bswap16(uInput);
#else
    __asm__("rorw $8,%0" : "+r"(uInput));
    return uInput;
#endif
}

BURGER_INLINE uint32_t _swapendian32(register uint32_t uInput)
{
#if __has_builtin(__builtin_bswap32) || (BURGER_GNUC >= 40300)
    return __builtin_bswap32(uInput);
#else
    __asm__("bswap %0" : "+r"(uInput));
    return uInput;
#endif
}

/***************************************

    GNU and clang for AMD64
    Favor built in intrinsics over inline assembly.

***************************************/

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
    defined(BURGER_AMD64)) && \
    !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput)
{
#if __has_builtin(__builtin_bswap16) || (BURGER_GNUC >= 40800)
    return __builtin_bswap16(uInput);
#else
    __asm__("rorw $8,%0" : "+r"(uInput));
    return uInput;
#endif
}

BURGER_INLINE uint32_t _swapendian32(register uint32_t uInput)
{
#if __has_builtin(__builtin_bswap32) || (BURGER_GNUC >= 40300)
    return __builtin_bswap32(uInput);
#else
    __asm__("bswap %0" : "+r"(uInput));
    return uInput;
#endif
}

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput)
{
#if __has_builtin(__builtin_bswap64) || (BURGER_GNUC >= 40300)
    return __builtin_bswap64(uInput);
#else
    __asm__("bswap %0" : "+r"(uInput));
    return uInput;
#endif
}

/***************************************

    Intel Compiler

***************************************/

#elif defined(BURGER_INTEL_COMPILER)

// _bswap instead of _byteswap_ulong, because... reasons

BURGER_INLINE uint16_t _swapendian16(uint16_t uInput)
{
    return static_cast<uint16_t>(_bswap(uInput) >> 16);
}

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
    return _bswap(uInput);
}

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput)
{
    return _bswap64(uInput);
}

/***************************************

    Visual Studio

***************************************/

#elif defined(BURGER_MSVC) && !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(uint16_t uInput)
{
    return _byteswap_ushort(uInput);
}

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
    return _byteswap_ulong(uInput);
}

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput)
{
    return _byteswap_uint64(uInput);
}

#endif
}

/***************************************

    Swap the endian

    While there are intrinsics for byte swapping, have
    actually seen the code output?

    No?

    Don't. You'll wail in terror.

***************************************/

namespace Burger {

class SwapEndian {
public:
    /***************************************

        Swap 16 bits

    ***************************************/

    static BURGER_INLINE uint16_t Load(uint16_t uInput)
    {
#if defined(BURGER_MSVC) || defined(BURGER_INTEL_COMPILER) || \
    ((defined(BURGER_GNUC) || defined(BURGER_CLANG) || \
         defined(BURGER_SNSYSTEMS) || defined(BURGER_METROWERKS)) && \
        defined(BURGER_PPC)) || \
    ((defined(BURGER_METROWERKS) || defined(BURGER_APPLE_SC) || \
         defined(BURGER_GNUC)) && \
        defined(BURGER_68K)) || \
    ((defined(BURGER_GNUC) || defined(BURGER_CLANG) || \
         defined(BURGER_SNSYSTEMS)) && \
        defined(BURGER_ARM32)) || \
    ((defined(BURGER_METROWERKS) || defined(BURGER_WATCOM) || \
         defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
        defined(BURGER_X86)) || \
    ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_ARM64))

        return _swapendian16(uInput);
#else
        return ((uInput & 0xFF) << 8) | ((uInput >> 8) & 0xFF);
#endif
    }

    /***************************************

        Swap 32 bits

    ***************************************/

#if (defined(BURGER_METROWERKS) && defined(BURGER_ARM32)) && !defined(DOXYGEN)
    static uint32_t BURGER_API Load(uint32_t uInput);

#else

    static BURGER_INLINE uint32_t Load(register uint32_t uInput)
    {
#if defined(BURGER_MSVC) || defined(BURGER_INTEL_COMPILER) || \
    ((defined(BURGER_GNUC) || defined(BURGER_CLANG) || \
         defined(BURGER_SNSYSTEMS) || defined(BURGER_METROWERKS)) && \
        defined(BURGER_PPC)) || \
    ((defined(BURGER_METROWERKS) || defined(BURGER_APPLE_SC) || \
         defined(BURGER_GNUC)) && \
        defined(BURGER_68K)) || \
    ((defined(BURGER_GNUC) || defined(BURGER_CLANG) || \
         defined(BURGER_SNSYSTEMS)) && \
        defined(BURGER_ARM32)) || \
    ((defined(BURGER_METROWERKS) || defined(BURGER_WATCOM) || \
         defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
        defined(BURGER_X86)) || \
    ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_ARM64))

        return _swapendian32(uInput);

#elif defined(BURGER_ARM_COMPILER)
        return __builtin_bswap32(uInput);

#else
        uint32_t uTemp = (uInput >> 24U) & 0xFFU;
        uTemp |= (uInput >> 8U) & 0xFF00U;
        uTemp |= (uInput << 8U) & 0xFF0000U;
        uTemp |= uInput << 24U;
        return uTemp;
#endif
    }
#endif

    /***************************************

        Swap 64 bits

        Not all compilers handle 64 bit endian swap as easily as 16/32 bit

    ***************************************/
#if defined(BURGER_HAS_64_BIT_SUPPORT)
    static BURGER_INLINE uint64_t Load(uint64_t uInput)
    {

        // _byteswap_uint64() is broken on Visual Studio 2003 with optimizations
        // on
#if (BURGER_MSVC >= 140000000) || \
    ((defined(BURGER_GNUC) || defined(BURGER_CLANG) || \
         defined(BURGER_METROWERKS)) && \
        defined(BURGER_PPC)) || \
    ((defined(BURGER_METROWERKS) || defined(BURGER_APPLE_SC)) && \
        defined(BURGER_68K)) || \
    ((defined(BURGER_METROWERKS) || defined(BURGER_WATCOM)) && \
        defined(BURGER_X86)) || \
    ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_ARM64))

        return _swapendian64(uInput);

#elif defined(BURGER_ARM_COMPILER)
        return __builtin_bswap64(uInput);

#else
        // Swap the endian on each piece
        uint32_t uLo = Load(static_cast<uint32_t>(uInput));
        uint32_t uHi = Load(static_cast<uint32_t>(uInput >> 32U));
        // Reconstruct the uint64_t with lo and hi swapped
        return (static_cast<uint64_t>(uLo) << 32) + uHi;
#endif
    }
#endif

    /***************************************

        Swap 32 bit float

        Intel 64 bit sucks less. Swapping floats doesn't hurt as much

    ***************************************/

#if (defined(BURGER_PS4) || \
    (defined(BURGER_GNUC) && !defined(__SNC__)) && defined(BURGER_AMD64)) && \
    !defined(DOXYGEN)
    static BURGER_INLINE float Load(float fInput)
    {
        uint32_t uTemp;
#if defined(BURGER_PS4)
        // VMX code
        __asm__(
            "vmovd %1,%0\n"
            "bswap %0\n"
            "vmovd %0,%1"
            : "=r"(uTemp), "+x"(fInput));
#else
        // SSE2 code
        __asm__(
            "movd %1,%0\n"
            "bswap %0\n"
            "movd %0,%1"
            : "=r"(uTemp), "+x"(fInput));
#endif
        return fInput;
    }
#else
    static float BURGER_API Load(float fInput);
#endif

    /***************************************

        Swap 64 bit double

        Intel 64 bit sucks less. Swapping floats doesn't hurt as much

    ***************************************/

#if (defined(BURGER_PS4) || \
    (defined(BURGER_GNUC) && !defined(__SNC__)) && defined(BURGER_AMD64)) && \
    !defined(DOXYGEN)
    static BURGER_INLINE double Load(double dInput)
    {
        uint64_t uTemp;
#if defined(BURGER_PS4)
        // VMX code
        __asm__(
            "vmovq %1,%0\n"
            "bswap %0\n"
            "vmovq %0,%1"
            : "=r"(uTemp), "+x"(dInput));
#else
        // SSE2 code
        __asm__(
            "movd %1,%0\n"
            "bswap %0\n"
            "movd %0,%1"
            : "=r"(uTemp), "+x"(dInput));
#endif
        return dInput;
    }
#else
    static double BURGER_API Load(double dInput);
#endif

    /***************************************

        Load and swap a 16 bit integer

    ***************************************/

    static BURGER_INLINE uint16_t Load(const uint16_t* pInput)
    {
#if (defined(BURGER_METROWERKS) && (defined(BURGER_X86) || defined(BURGER_68K)))
        return __ror(pInput[0], 8);

#elif defined(BURGER_METROWERKS) && defined(BURGER_PPC)
        return static_cast<uint16_t>(__lhbrx(const_cast<uint16_t*>(pInput), 0));

#elif (defined(BURGER_GNUC) && !defined(__SNC__)) && defined(BURGER_PPC)
        uint16_t uTemp;
        __asm__ __volatile__("lhbrx %0,0,%1"
                             : "=r"(uTemp)
                             : "r"(pInput), "m"(*pInput));
        return uTemp;

#elif defined(BURGER_XBOX360)
        return __loadshortbytereverse(0, pInput);

#elif defined(BURGER_PS3)
        return static_cast<uint16_t>(__lhbrx(const_cast<uint16_t*>(pInput)));

#elif defined(BURGER_PS4)
        uint16_t uTemp;
        __asm__("movbew %1,%0" : "=r"(uTemp) : "m"(*pInput));
        return uTemp;

#elif defined(BURGER_INTEL_COMPILER)
        return _swapendian16(pInput[0]);

#else
        return Load(pInput[0]);

#endif
    }

    /***************************************

        Load and swap a 32 bit integer

    ***************************************/

    static BURGER_INLINE uint32_t Load(const uint32_t* pInput)
    {
#if (defined(BURGER_METROWERKS) && defined(BURGER_PPC))
        return __lwbrx(const_cast<uint32_t*>(pInput), 0);

#elif (defined(BURGER_GNUC) && !defined(__SNC__)) && defined(BURGER_PPC)
        uint32_t uTemp;
        __asm__ __volatile__("lwbrx %0,0,%1"
                             : "=r"(uTemp)
                             : "r"(pInput), "m"(*pInput));
        return uTemp;

#elif defined(BURGER_XBOX360)
        return __loadwordbytereverse(0, pInput);

#elif defined(BURGER_PS3)
        return static_cast<uint32_t>(__lwbrx(const_cast<uint32_t*>(pInput)));

#elif defined(BURGER_PS4)
        uint32_t uTemp;
        __asm__ __volatile__("movbel %1,%0" : "=r"(uTemp) : "m"(*pInput));
        return uTemp;

#elif defined(BURGER_INTEL_COMPILER)
        return _swapendian32(pInput[0]);

#elif ((defined(BURGER_METROWERKS) && defined(BURGER_68K)) || \
    defined(BURGER_MSVC))
        return _swapendian32(pInput[0]);

#else
        return Load(pInput[0]);
#endif
    }

    /***************************************

        Load and swap a 64 bit integer

    ***************************************/
#if defined(BURGER_HAS_64_BIT_SUPPORT)
    static BURGER_INLINE uint64_t Load(const uint64_t* pInput)
    {
        // PowerMac G5 doesn't have the ldbrx instruction
#if (defined(BURGER_GNUC) && !defined(__SNC__)) && \
    defined(BURGER_POWERPC64) && !defined(BURGER_MACOSX)
        uint64_t uTemp;
        __asm__ __volatile__("ldbrx %0,0,%1"
                             : "=r"(uTemp)
                             : "r"(pInput), "m"(*pInput));
        return uTemp;

#elif (defined(BURGER_GNUC) && !defined(__SNC__)) && defined(BURGER_POWERPC)
        uint64_t uTemp;
        __asm__ __volatile__("lwbrx %L0,0,%1"
                             : "=r"(uTemp)
                             : "r"(pInput), "m"(*pInput));
        __asm__ __volatile__(
            "lwbrx %0,0,%1"
            : "=r"(uTemp)
            : "r"((char*)pInput + 4), "m"(*pInput), "0"(uTemp));
        return uTemp;

#elif defined(BURGER_METROWERKS) && defined(BURGER_PPC)

        uint32_t uTempHi = __lwbrx(const_cast<uint64_t*>(pInput), 0);
        uint32_t uTempLo = __lwbrx(const_cast<uint64_t*>(pInput), 4);
        return (static_cast<uint64_t>(uTempLo) << 32) | uTempHi;

#elif defined(BURGER_XBOX360)
        return __loaddoublewordbytereverse(0, pInput);

#elif defined(BURGER_PS3)
        return __ldbrx(const_cast<uint64_t*>(pInput));

#elif defined(BURGER_PS4)
        uint64_t uTemp;
        __asm__ __volatile__("movbeq %1,%0" : "=r"(uTemp) : "m"(*pInput));
        return uTemp;

#else
        return Load(pInput[0]);
#endif
    }
#endif

    /***************************************

        Swap 32 bit double

        Intel 64 bit sucks less. Swapping floats doesn't hurt as much

    ***************************************/

#if (defined(BURGER_PS4) || \
    (defined(BURGER_GNUC) && !defined(__SNC__)) && defined(BURGER_AMD64)) && \
    !defined(DOXYGEN)
    static BURGER_INLINE float Load(const float* pInput)
    {
        float fTemp;
        uint32_t uTemp;
#if defined(BURGER_PS4)
        // VMX code
        __asm__(
            "movbe %2,%0\n"
            "vmovd %0,%1"
            : "=r"(uTemp), "=x"(fTemp)
            : "m"(*pInput));
#else
        // SSE2 code
        __asm__(
            "movl %2,%0\n"
            "bswap %0\n"
            "movd %0,%1"
            : "=r"(uTemp), "=x"(fTemp)
            : "m"(*pInput));
#endif
        return fTemp;
    }

#else
    static float BURGER_API Load(const float* pInput);
#endif

#if (defined(BURGER_PS4) || \
    (defined(BURGER_GNUC) && !defined(__SNC__)) && defined(BURGER_AMD64)) && \
    !defined(DOXYGEN)
    static BURGER_INLINE double Load(const double* pInput)
    {
        double dTemp;
        uint64_t uTemp;
#if defined(BURGER_PS4)
        // VMX code
        __asm__(
            "movbe %2,%0\n"
            "vmovq %0,%1"
            : "=r"(uTemp), "=x"(dTemp)
            : "m"(*pInput));
#else
        // SSE2 code
        __asm__(
            "movq %2,%0\n"
            "bswap %0\n"
            "movd %0,%1"
            : "=r"(uTemp), "=x"(dTemp)
            : "m"(*pInput));
#endif
        return dTemp;
    }

#else
    static double BURGER_API Load(const double* pInput);
#endif

    /***************************************

        Metrowerks X86

    ***************************************/

#if (defined(BURGER_METROWERKS) && defined(BURGER_X86)) && !defined(DOXYGEN)

    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput)
    {
        pOutput[0] = __ror(uInput, 8);
    }

    static BURGER_INLINE void Store(uint32_t* pOutput, register uint32_t uInput)
    {
        __asm bswap uInput;
        pOutput[0] = uInput;
    }

    /***************************************

        Metrowerks 680x0

    ***************************************/

#elif (defined(BURGER_METROWERKS) && defined(BURGER_68K)) && !defined(DOXYGEN)

    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput)
    {
        pOutput[0] = __ror(uInput, 8);
    }

    static BURGER_INLINE void Store(uint32_t* pOutput, register uint32_t uInput)
    {
        pOutput[0] = _swapendian32(uInput);
    }

    /***************************************

        Metrowerks PowerPC

    ***************************************/

#elif (defined(BURGER_METROWERKS) && defined(BURGER_PPC)) && !defined(DOXYGEN)

    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput)
    {
        __sthbrx(uInput, pOutput, 0);
    }

    static BURGER_INLINE void Store(uint32_t* pOutput, uint32_t uInput)
    {
        __stwbrx(uInput, pOutput, 0);
    }

    /***************************************

        Xbox 360

    ***************************************/

#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)

    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput)
    {
        __storeshortbytereverse(uInput, 0, pOutput);
    }

    static BURGER_INLINE void Store(uint32_t* pOutput, uint32_t uInput)
    {
        __storewordbytereverse(uInput, 0, pOutput);
    }

    /***************************************

        Sony PS3

    ***************************************/

#elif defined(BURGER_PS3) && !defined(DOXYGEN)

    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput)
    {
        __sthbrx(pOutput, uInput);
    }

    static BURGER_INLINE void Store(uint32_t* pOutput, uint32_t uInput)
    {
        __stwbrx(pOutput, uInput);
    }

    /***************************************

        Intel Compiler (X86/X64)

    ***************************************/

#elif defined(BURGER_INTEL_COMPILER) && !defined(DOXYGEN)

    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput)
    {
        pOutput[0] = _swapendian16(uInput);
    }

    static BURGER_INLINE void Store(uint32_t* pOutput, uint32_t uInput)
    {
        pOutput[0] = _swapendian32(uInput);
    }

/***************************************

    Watcom, Visual C and GNUC

***************************************/

// Byteswap intrinsics were defined above
#elif ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
    defined(BURGER_MSVC) || (defined(BURGER_GNUC) && !defined(__SNC__))) && \
    !defined(DOXYGEN)

    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput)
    {
        pOutput[0] = Load(uInput);
    }

    static BURGER_INLINE void Store(uint32_t* pOutput, uint32_t uInput)
    {
        pOutput[0] = Load(uInput);
    }

#else

    /***************************************

        Platform neutral

    ***************************************/

    static void BURGER_API Store(uint16_t* pOutput, uint16_t uInput);
    static void BURGER_API Store(uint32_t* pOutput, uint32_t uInput);

#endif

#if defined(BURGER_XBOX360) && !defined(DOXYGEN)

    static BURGER_INLINE void Store(uint64_t* pOutput, uint64_t uInput)
    {
        return __storedoublewordbytereverse(uInput, 0, pOutput);
    }

#elif defined(BURGER_PS3) && !defined(DOXYGEN)

    static BURGER_INLINE void Store(uint64_t* pOutput, uint64_t uInput)
    {
        __stdbrx(pOutput, uInput);
    }

#elif ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
    (defined(BURGER_INTEL) && defined(BURGER_MSVC)) || \
    ((defined(BURGER_GNUC) && !defined(__SNC__)) && \
        (defined(BURGER_AMD64) || defined(BURGER_ARM64) || \
            defined(BURGER_PPC)))) && \
    !defined(DOXYGEN)

    static BURGER_INLINE void Store(uint64_t* pOutput, uint64_t uInput)
    {
        pOutput[0] = Load(uInput);
    }

#else
    static void BURGER_API Store(uint64_t* pOutput, uint64_t uInput);

#endif

    static void BURGER_API Store(float* pOutput, float fInput);
    static void BURGER_API Store(double* pOutput, double dInput);

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

    static BURGER_INLINE uint16_t LoadAny(const uint16_t* pInput)
    {
        return _swapendian16(pInput[0]);
    }

    static BURGER_INLINE uint32_t LoadAny(const uint32_t* pInput)
    {
        return _swapendian32(pInput[0]);
    }

    static BURGER_INLINE uint64_t LoadAny(const uint64_t* pInput)
    {
        return _swapendian64(pInput[0]);
    }

#else

    static uint16_t BURGER_API LoadAny(const uint16_t* pInput);
    static uint32_t BURGER_API LoadAny(const uint32_t* pInput);
    static uint64_t BURGER_API LoadAny(const uint64_t* pInput);

#endif

    static float BURGER_API LoadAny(const float* pInput);
    static double BURGER_API LoadAny(const double* pInput);

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

    static BURGER_INLINE void StoreAny(uint16_t* pOutput, uint16_t uInput)
    {
        pOutput[0] = _swapendian16(uInput);
    }

    static BURGER_INLINE void StoreAny(uint32_t* pOutput, uint32_t uInput)
    {
        pOutput[0] = _swapendian32(uInput);
    }

    static BURGER_INLINE void StoreAny(uint64_t* pOutput, uint64_t uInput)
    {
        pOutput[0] = _swapendian64(uInput);
    }
#else
    static void BURGER_API StoreAny(uint16_t* pOutput, uint16_t uInput);
    static void BURGER_API StoreAny(uint32_t* pOutput, uint32_t uInput);
    static void BURGER_API StoreAny(uint64_t* pOutput, uint64_t uInput);
#endif

    static void BURGER_API StoreAny(float* pOutput, float fInput);
    static void BURGER_API StoreAny(double* pOutput, double dInput);

    static BURGER_INLINE void Fixup(uint16_t* pInput)
    {
        pInput[0] = Load(pInput);
    }

    static BURGER_INLINE void Fixup(uint32_t* pInput)
    {
        pInput[0] = Load(pInput);
    }

#if (defined(BURGER_XBOX360) || defined(BURGER_PS3) || \
    defined(BURGER_AMD64) || \
    ((defined(BURGER_MACOS) || defined(BURGER_IOS)) && \
        (defined(BURGER_AMD64) || defined(BURGER_ARM64)))) && \
    !defined(DOXYGEN)

    static BURGER_INLINE void Fixup(uint64_t* pInput)
    {
        pInput[0] = Load(pInput);
    }

#elif defined(BURGER_PPC) && defined(BURGER_METROWERKS)

    static BURGER_INLINE void Fixup(uint64_t* pInput)
    {
        uint32_t uLow = __lwbrx(pInput, 0);
        uint32_t uHigh = __lwbrx(pInput, 4);
        reinterpret_cast<uint32_t*>(pInput)[0] = uHigh;
        reinterpret_cast<uint32_t*>(pInput)[1] = uLow;
    }

#else

    static void BURGER_API Fixup(uint64_t* pInput);

#endif

    static void BURGER_API FixupAny(uint16_t* pInput);
    static void BURGER_API FixupAny(uint32_t* pInput);
    static void BURGER_API FixupAny(uint64_t* pInput);

    static BURGER_INLINE Int16 Load(Int16 iInput)
    {
        return static_cast<Int16>(Load(static_cast<uint16_t>(iInput)));
    }
    static BURGER_INLINE Int32 Load(Int32 iInput)
    {
        return static_cast<Int32>(Load(static_cast<uint32_t>(iInput)));
    }
#if defined(BURGER_HAS_64_BIT_SUPPORT)
    static BURGER_INLINE Int64 Load(Int64 iInput)
    {
        return static_cast<Int64>(Load(static_cast<uint64_t>(iInput)));
    }
#endif

    static BURGER_INLINE Int16 Load(const Int16* pInput)
    {
        return static_cast<Int16>(
            Load(reinterpret_cast<const uint16_t*>(pInput)));
    }
    static BURGER_INLINE Int32 Load(const Int32* pInput)
    {
        return static_cast<Int32>(
            Load(reinterpret_cast<const uint32_t*>(pInput)));
    }
#if defined(BURGER_HAS_64_BIT_SUPPORT)
    static BURGER_INLINE Int64 Load(const Int64* pInput)
    {
        return static_cast<Int64>(
            Load(reinterpret_cast<const uint64_t*>(pInput)));
    }
#endif
    static BURGER_INLINE void Store(Int16* pOutput, Int16 iInput)
    {
        Store(reinterpret_cast<uint16_t*>(pOutput),
            static_cast<uint16_t>(iInput));
    }
    static BURGER_INLINE void Store(Int32* pOutput, Int32 iInput)
    {
        Store(reinterpret_cast<uint32_t*>(pOutput),
            static_cast<uint32_t>(iInput));
    }
#if defined(BURGER_HAS_64_BIT_SUPPORT)
    static BURGER_INLINE void Store(Int64* pOutput, Int64 iInput)
    {
        Store(reinterpret_cast<uint64_t*>(pOutput),
            static_cast<uint64_t>(iInput));
    }
#endif
    static BURGER_INLINE Int16 LoadAny(const Int16* pInput)
    {
        return static_cast<Int16>(
            LoadAny(reinterpret_cast<const uint16_t*>(pInput)));
    }
    static BURGER_INLINE Int32 LoadAny(const Int32* pInput)
    {
        return static_cast<Int32>(
            LoadAny(reinterpret_cast<const uint32_t*>(pInput)));
    }
#if defined(BURGER_HAS_64_BIT_SUPPORT)
    static BURGER_INLINE Int64 LoadAny(const Int64* pInput)
    {
        return static_cast<Int64>(
            LoadAny(reinterpret_cast<const uint64_t*>(pInput)));
    }
#endif
    static BURGER_INLINE void StoreAny(Int16* pOutput, Int16 iInput)
    {
        StoreAny(reinterpret_cast<uint16_t*>(pOutput),
            static_cast<uint16_t>(iInput));
    }
    static BURGER_INLINE void StoreAny(Int32* pOutput, Int32 iInput)
    {
        StoreAny(reinterpret_cast<uint32_t*>(pOutput),
            static_cast<uint32_t>(iInput));
    }
#if defined(BURGER_HAS_64_BIT_SUPPORT)
    static BURGER_INLINE void StoreAny(Int64* pOutput, Int64 iInput)
    {
        StoreAny(reinterpret_cast<uint64_t*>(pOutput),
            static_cast<uint64_t>(iInput));
    }
#endif
    static BURGER_INLINE void Fixup(Int16* pInput)
    {
        Fixup(reinterpret_cast<uint16_t*>(pInput));
    }
    static BURGER_INLINE void Fixup(Int32* pInput)
    {
        Fixup(reinterpret_cast<uint32_t*>(pInput));
    }
    static BURGER_INLINE void Fixup(Int64* pInput)
    {
        Fixup(reinterpret_cast<uint64_t*>(pInput));
    }
    static BURGER_INLINE void Fixup(float* pInput)
    {
        Fixup(static_cast<uint32_t*>(static_cast<void*>(pInput)));
    }
    static BURGER_INLINE void Fixup(double* pInput)
    {
        Fixup(static_cast<uint64_t*>(static_cast<void*>(pInput)));
    }
    static BURGER_INLINE void FixupAny(Int16* pInput)
    {
        FixupAny(reinterpret_cast<uint16_t*>(pInput));
    }
    static BURGER_INLINE void FixupAny(Int32* pInput)
    {
        FixupAny(reinterpret_cast<uint32_t*>(pInput));
    }
    static BURGER_INLINE void FixupAny(Int64* pInput)
    {
        FixupAny(reinterpret_cast<uint64_t*>(pInput));
    }
    static BURGER_INLINE void FixupAny(float* pInput)
    {
        FixupAny(static_cast<uint32_t*>(static_cast<void*>(pInput)));
    }
    static BURGER_INLINE void FixupAny(double* pInput)
    {
        FixupAny(static_cast<uint64_t*>(static_cast<void*>(pInput)));
    }
};

class NativeEndian {

public:
    static BURGER_INLINE uint16_t Load(uint16_t uInput)
    {
        return uInput;
    }
    static BURGER_INLINE uint32_t Load(uint32_t uInput)
    {
        return uInput;
    }
    static BURGER_INLINE uint64_t Load(uint64_t uInput)
    {
        return uInput;
    }
    static BURGER_INLINE float Load(float fInput)
    {
        return fInput;
    }
    static BURGER_INLINE double Load(double dInput)
    {
        return dInput;
    }

    static BURGER_INLINE uint16_t Load(const uint16_t* pInput)
    {
        return pInput[0];
    }
    static BURGER_INLINE uint32_t Load(const uint32_t* pInput)
    {
        return pInput[0];
    }
    static BURGER_INLINE uint64_t Load(const uint64_t* pInput)
    {
        return pInput[0];
    }
    static BURGER_INLINE float Load(const float* pInput)
    {
        return pInput[0];
    }
    static BURGER_INLINE double Load(const double* pInput)
    {
        return pInput[0];
    }

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)
    static BURGER_INLINE uint16_t LoadAny(const uint16_t* pInput)
    {
        return pInput[0];
    }

    static BURGER_INLINE uint32_t LoadAny(const uint32_t* pInput)
    {
        return pInput[0];
    }

    static BURGER_INLINE uint64_t LoadAny(const uint64_t* pInput)
    {
        return pInput[0];
    }
#else
    static uint16_t BURGER_API LoadAny(const uint16_t* pInput);
    static uint32_t BURGER_API LoadAny(const uint32_t* pInput);
    static uint64_t BURGER_API LoadAny(const uint64_t* pInput);
#endif

    static float BURGER_API LoadAny(const float* pInput);
    static double BURGER_API LoadAny(const double* pInput);

    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput)
    {
        pOutput[0] = uInput;
    }
    static BURGER_INLINE void Store(uint32_t* pOutput, uint32_t uInput)
    {
        pOutput[0] = uInput;
    }
    static BURGER_INLINE void Store(uint64_t* pOutput, uint64_t uInput)
    {
        pOutput[0] = uInput;
    }
    static BURGER_INLINE void Store(float* pOutput, float fInput)
    {
        pOutput[0] = fInput;
    }
    static BURGER_INLINE void Store(double* pOutput, double dInput)
    {
        pOutput[0] = dInput;
    }

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)
    static void BURGER_API StoreAny(uint16_t* pOutput, uint16_t uInput)
    {
        pOutput[0] = uInput;
    }

    static void BURGER_API StoreAny(uint32_t* pOutput, uint32_t uInput)
    {
        pOutput[0] = uInput;
    }

    static void BURGER_API StoreAny(uint64_t* pOutput, uint64_t uInput)
    {
        pOutput[0] = uInput;
    }

#else
    static void BURGER_API StoreAny(uint16_t* pOutput, uint16_t uInput);
    static void BURGER_API StoreAny(uint32_t* pOutput, uint32_t uInput);
    static void BURGER_API StoreAny(uint64_t* pOutput, uint64_t uInput);
#endif

    static void BURGER_API StoreAny(float* pOutput, float fInput);
    static void BURGER_API StoreAny(double* pOutput, double dInput);

    static BURGER_INLINE void Fixup(uint16_t*) {}
    static BURGER_INLINE void Fixup(uint32_t*) {}
    static BURGER_INLINE void Fixup(uint64_t*) {}

    static BURGER_INLINE void FixupAny(uint16_t*) {}
    static BURGER_INLINE void FixupAny(uint32_t*) {}
    static BURGER_INLINE void FixupAny(uint64_t*) {}

    static BURGER_INLINE Int16 Load(Int16 iInput)
    {
        return iInput;
    }
    static BURGER_INLINE Int32 Load(Int32 iInput)
    {
        return iInput;
    }
    static BURGER_INLINE Int64 Load(Int64 iInput)
    {
        return iInput;
    }

    static BURGER_INLINE Int16 Load(const Int16* pInput)
    {
        return pInput[0];
    }
    static BURGER_INLINE Int32 Load(const Int32* pInput)
    {
        return pInput[0];
    }
#if defined(BURGER_HAS_64_BIT_SUPPORT)
    static BURGER_INLINE Int64 Load(const Int64* pInput)
    {
        return pInput[0];
    }
#endif
    static BURGER_INLINE void Store(Int16* pOutput, Int16 iInput)
    {
        pOutput[0] = iInput;
    }
    static BURGER_INLINE void Store(Int32* pOutput, Int32 iInput)
    {
        pOutput[0] = iInput;
    }
    static BURGER_INLINE void Store(Int64* pOutput, Int64 iInput)
    {
        pOutput[0] = iInput;
    }

    static BURGER_INLINE Int16 LoadAny(const Int16* pInput)
    {
        return static_cast<Int16>(
            LoadAny(reinterpret_cast<const uint16_t*>(pInput)));
    }
    static BURGER_INLINE Int32 LoadAny(const Int32* pInput)
    {
        return static_cast<Int32>(
            LoadAny(reinterpret_cast<const uint32_t*>(pInput)));
    }
#if defined(BURGER_HAS_64_BIT_SUPPORT)
    static BURGER_INLINE Int64 LoadAny(const Int64* pInput)
    {
        return static_cast<Int64>(
            LoadAny(reinterpret_cast<const uint64_t*>(pInput)));
    }
#endif
    static BURGER_INLINE void StoreAny(Int16* pOutput, Int16 iInput)
    {
        StoreAny(reinterpret_cast<uint16_t*>(pOutput),
            static_cast<uint16_t>(iInput));
    }
    static BURGER_INLINE void StoreAny(Int32* pOutput, Int32 iInput)
    {
        StoreAny(reinterpret_cast<uint32_t*>(pOutput),
            static_cast<uint32_t>(iInput));
    }
#if defined(BURGER_HAS_64_BIT_SUPPORT)
    static BURGER_INLINE void StoreAny(Int64* pOutput, Int64 iInput)
    {
        StoreAny(reinterpret_cast<uint64_t*>(pOutput),
            static_cast<uint64_t>(iInput));
    }
#endif

    static BURGER_INLINE void Fixup(Int16*) {}
    static BURGER_INLINE void Fixup(Int32*) {}
    static BURGER_INLINE void Fixup(Int64*) {}
    static BURGER_INLINE void Fixup(float*) {}
    static BURGER_INLINE void Fixup(double*) {}

    static BURGER_INLINE void FixupAny(Int16*) {}
    static BURGER_INLINE void FixupAny(Int32*) {}
    static BURGER_INLINE void FixupAny(Int64*) {}
    static BURGER_INLINE void FixupAny(float*) {}
    static BURGER_INLINE void FixupAny(double*) {}
};

#if (defined(BURGER_METROWERKS) && defined(BURGER_X86))
#pragma volatile_asm reset
#endif

// Which endian to swap, which to not.

#if defined(BURGER_LITTLEENDIAN)

class LittleEndian: public NativeEndian {
};
class BigEndian: public SwapEndian {
};
#define BURGER_BIGENDIAN16(x) (((x & 0xFFU) << 8U) + ((x >> 8U) & 0xFFU))
#define BURGER_BIGENDIAN32(x) \
    (((x & 0xFFU) << 24U) + ((x << 8U) & 0xFF0000U) + ((x >> 8U) & 0xFF00U) + \
        ((x >> 24U) & 0xFFU))
#define BURGER_LITTLEENDIAN16(x) (x)
#define BURGER_LITTLEENDIAN32(x) (x)
#else

class LittleEndian: public SwapEndian {
};
class BigEndian: public NativeEndian {
};
#define BURGER_LITTLEENDIAN16(x) (((x & 0xFFU) << 8U) + ((x >> 8U) & 0xFFU))
#define BURGER_LITTLEENDIAN32(x) \
    (((x & 0xFFU) << 24U) + ((x << 8U) & 0xFF0000U) + ((x >> 8U) & 0xFF00U) + \
        ((x >> 24U) & 0xFFU))
#define BURGER_BIGENDIAN16(x) (x)
#define BURGER_BIGENDIAN32(x) (x)

#endif

extern void BURGER_API ConvertEndian(uint16_t* pInput, uintptr_t uCount);
extern void BURGER_API ConvertEndian(
    uint16_t* pOutput, const uint16_t* pInput, uintptr_t uCount);
extern void BURGER_API ConvertEndian(uint32_t* pInput, uintptr_t uCount);
extern void BURGER_API ConvertEndian(
    uint32_t* pOutput, const uint32_t* pInput, uintptr_t uCount);
extern void BURGER_API ConvertEndian(uint64_t* pInput, uintptr_t uCount);
extern void BURGER_API ConvertEndian(
    uint64_t* pOutput, const uint64_t* pInput, uintptr_t uCount);
extern void BURGER_API SwapCharsToBytes(void* pInput, uintptr_t uLength);
extern void BURGER_API SwapCharsToBytes(
    void* pOutput, const void* pInput, uintptr_t uLength);

}

/* END */

#endif
