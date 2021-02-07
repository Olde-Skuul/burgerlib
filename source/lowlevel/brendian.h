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

#ifndef __BRINTRINSICS_H__
#include "brintrinsics.h"
#endif

#ifndef __BRALGORITHM_H__
#include "bralgorithm.h"
#endif

/* BEGIN */

// Undocumented feature of the Metrowerks compiler.
// Turn on to alert the optimizer that results are static

#if (defined(BURGER_METROWERKS) && defined(BURGER_X86))
#pragma volatile_asm off
#endif

/***************************************

    Swap the endian

    While there are intrinsics for byte swapping, have
    actually seen the code output?

    No?

    Don't. You'll wail in terror.

***************************************/

namespace Burger {

struct SwapEndian {

    // clang-format off
    static BURGER_INLINE BURGER_CONSTEXPR char Load(char iInput) BURGER_NOEXCEPT { return iInput; }
    static BURGER_INLINE BURGER_CONSTEXPR uint8_t Load(uint8_t uInput) BURGER_NOEXCEPT { return uInput; }
    static BURGER_INLINE BURGER_CONSTEXPR int8_t Load(int8_t iInput) BURGER_NOEXCEPT { return iInput; }
    static BURGER_INLINE uint16_t Load(uint16_t uInput) BURGER_NOEXCEPT { return _swapendian16(uInput); }
#if (defined(BURGER_METROWERKS) && defined(BURGER_ARM32)) && !defined(DOXYGEN)
    static uint32_t BURGER_API Load(uint32_t uInput) BURGER_NOEXCEPT;
#else
    static BURGER_INLINE uint32_t Load(uint32_t uInput) BURGER_NOEXCEPT { return _swapendian32(uInput); }
#endif
    static BURGER_INLINE uint64_t Load(uint64_t uInput) BURGER_NOEXCEPT { return _swapendian64(uInput); }

#if (defined(BURGER_PS4) || \
    ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
        defined(BURGER_AMD64))) && \
    !defined(DOXYGEN)
    static BURGER_INLINE float Load(float fInput) BURGER_NOEXCEPT
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
    static float BURGER_API Load(float fInput) BURGER_NOEXCEPT;
#endif

#if (defined(BURGER_PS4) || \
    (defined(BURGER_GNUC) && !defined(__SNC__)) && defined(BURGER_AMD64)) && \
    !defined(DOXYGEN)
    static BURGER_INLINE double Load(double dInput) BURGER_NOEXCEPT
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
    static double BURGER_API Load(double dInput) BURGER_NOEXCEPT;
#endif
    static BURGER_INLINE int16_t Load(int16_t iInput) BURGER_NOEXCEPT {
        return static_cast<int16_t>(Load(static_cast<uint16_t>(iInput))); }
    static BURGER_INLINE int32_t Load(int32_t iInput) BURGER_NOEXCEPT {
        return static_cast<int32_t>(Load(static_cast<uint32_t>(iInput))); }
    static BURGER_INLINE int64_t Load(int64_t iInput) BURGER_NOEXCEPT {
        return static_cast<int64_t>(Load(static_cast<uint64_t>(iInput))); }

    static BURGER_INLINE BURGER_CONSTEXPR char Load(const char* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR uint8_t Load(const uint8_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR int8_t Load(const int8_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE uint16_t Load(const uint16_t* pInput) BURGER_NOEXCEPT
    {
#if defined(BURGER_METROWERKS) && defined(BURGER_PPC)
        return static_cast<uint16_t>(__lhbrx(const_cast<uint16_t*>(pInput), 0));

#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)
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

#else
        return _swapendian16(pInput[0]);

#endif
    }

    static BURGER_INLINE uint32_t Load(const uint32_t* pInput) BURGER_NOEXCEPT
    {
#if (defined(BURGER_METROWERKS) && defined(BURGER_PPC))
        return __lwbrx(const_cast<uint32_t*>(pInput), 0);

#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)
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
#else
        return _swapendian32(pInput[0]);
#endif
    }

    static BURGER_INLINE uint64_t Load(const uint64_t* pInput) BURGER_NOEXCEPT
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
        return _swapendian64(pInput[0]);
#endif
    }

#if (defined(BURGER_PS4) || \
    (defined(BURGER_GNUC) && !defined(__SNC__)) && defined(BURGER_AMD64)) && \
    !defined(DOXYGEN)
    static BURGER_INLINE float Load(const float* pInput) BURGER_NOEXCEPT
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
    static float BURGER_API Load(const float* pInput) BURGER_NOEXCEPT;
#endif

#if (defined(BURGER_PS4) || \
    (defined(BURGER_GNUC) && !defined(__SNC__)) && defined(BURGER_AMD64)) && \
    !defined(DOXYGEN)
    static BURGER_INLINE double Load(const double* pInput) BURGER_NOEXCEPT
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
    static double BURGER_API Load(const double* pInput) BURGER_NOEXCEPT;
#endif

    static BURGER_INLINE void Store(char* pOutput, char iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE void Store(uint8_t* pOutput, uint8_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void Store(int8_t* pOutput, int8_t iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
#if (defined(BURGER_METROWERKS) && defined(BURGER_X86)) && !defined(DOXYGEN)
    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT { pOutput[0] = __ror(uInput, 8); }
    static BURGER_INLINE void Store(uint32_t* pOutput, register uint32_t uInput) BURGER_NOEXCEPT {
        __asm bswap uInput;
        pOutput[0] = uInput;
    }

#elif (defined(BURGER_METROWERKS) && defined(BURGER_68K)) && !defined(DOXYGEN)

    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT { pOutput[0] = __ror(uInput, 8); }
    static BURGER_INLINE void Store(uint32_t* pOutput, register uint32_t uInput) BURGER_NOEXCEPT { pOutput[0] = _swapendian32(uInput); }

#elif (defined(BURGER_METROWERKS) && defined(BURGER_PPC)) && !defined(DOXYGEN)
    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT { __sthbrx(uInput, pOutput, 0); }
    static BURGER_INLINE void Store(uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT { __stwbrx(uInput, pOutput, 0); }
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT { __storeshortbytereverse(uInput, 0, pOutput); }
    static BURGER_INLINE void Store(uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT { __storewordbytereverse(uInput, 0, pOutput); }
#elif defined(BURGER_PS3) && !defined(DOXYGEN)
    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT { __sthbrx(pOutput, uInput); }
    static BURGER_INLINE void Store(uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT { __stwbrx(pOutput, uInput); }
#elif defined(BURGER_INTEL_COMPILER) && !defined(DOXYGEN)
    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT { pOutput[0] = _swapendian16(uInput); }
    static BURGER_INLINE void Store(uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT { pOutput[0] = _swapendian32(uInput); }

// Byteswap intrinsics were defined above
#elif ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
    defined(BURGER_MSVC) || (defined(BURGER_GNUC) && !defined(__SNC__))) && \
    !defined(DOXYGEN)

    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT { pOutput[0] = Load(uInput); }
    static BURGER_INLINE void Store(uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT { pOutput[0] = Load(uInput); }
#else
    static void BURGER_API Store(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT;
    static void BURGER_API Store(uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT;
#endif

#if defined(BURGER_XBOX360) && !defined(DOXYGEN)
    static BURGER_INLINE void Store(uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT { return __storedoublewordbytereverse(uInput, 0, pOutput); }
#elif defined(BURGER_PS3) && !defined(DOXYGEN)
    static BURGER_INLINE void Store(uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT { __stdbrx(pOutput, uInput); }
#elif ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
    (defined(BURGER_INTEL) && defined(BURGER_MSVC)) || \
    ((defined(BURGER_GNUC) && !defined(__SNC__)) && \
        (defined(BURGER_AMD64) || defined(BURGER_ARM64) || \
            defined(BURGER_PPC)))) && \
    !defined(DOXYGEN)

    static BURGER_INLINE void Store(uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT { pOutput[0] = Load(uInput); }
#else
    static void BURGER_API Store(uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT;
#endif
    static void BURGER_API Store(float* pOutput, float fInput) BURGER_NOEXCEPT;
    static void BURGER_API Store(double* pOutput, double dInput) BURGER_NOEXCEPT;


    static BURGER_INLINE BURGER_CONSTEXPR char LoadAny(const char* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR uint8_t LoadAny(const uint8_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR int8_t LoadAny(const int8_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

    static BURGER_INLINE uint16_t LoadAny(const uint16_t* pInput) BURGER_NOEXCEPT { return _swapendian16(pInput[0]); }
    static BURGER_INLINE uint32_t LoadAny(const uint32_t* pInput) BURGER_NOEXCEPT { return _swapendian32(pInput[0]); }
    static BURGER_INLINE uint64_t LoadAny(const uint64_t* pInput) BURGER_NOEXCEPT { return _swapendian64(pInput[0]); }
#else
    static uint16_t BURGER_API LoadAny(const uint16_t* pInput) BURGER_NOEXCEPT;
    static uint32_t BURGER_API LoadAny(const uint32_t* pInput) BURGER_NOEXCEPT;
    static uint64_t BURGER_API LoadAny(const uint64_t* pInput) BURGER_NOEXCEPT;
#endif
    static float BURGER_API LoadAny(const float* pInput) BURGER_NOEXCEPT;
    static double BURGER_API LoadAny(const double* pInput) BURGER_NOEXCEPT;

    static BURGER_INLINE void StoreAny(char* pOutput, char iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE void StoreAny(uint8_t* pOutput, uint8_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void StoreAny(int8_t* pOutput, int8_t iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

    static BURGER_INLINE void StoreAny(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT { pOutput[0] = _swapendian16(uInput); }
    static BURGER_INLINE void StoreAny(uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT { pOutput[0] = _swapendian32(uInput); }
    static BURGER_INLINE void StoreAny(uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT { pOutput[0] = _swapendian64(uInput); }
#else
    static void BURGER_API StoreAny(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT;
    static void BURGER_API StoreAny(uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT;
    static void BURGER_API StoreAny(uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT;
#endif

    static void BURGER_API StoreAny(float* pOutput, float fInput) BURGER_NOEXCEPT;
    static void BURGER_API StoreAny(double* pOutput, double dInput) BURGER_NOEXCEPT;

    static BURGER_INLINE void Fixup(char*) BURGER_NOEXCEPT {}
    static BURGER_INLINE void Fixup(uint8_t*) BURGER_NOEXCEPT {}
    static BURGER_INLINE void Fixup(int8_t*) BURGER_NOEXCEPT {}
    static BURGER_INLINE void Fixup(uint16_t* pInput) BURGER_NOEXCEPT { pInput[0] = Load(pInput); }
    static BURGER_INLINE void Fixup(uint32_t* pInput) BURGER_NOEXCEPT { pInput[0] = Load(pInput); }
#if defined(BURGER_64BITCPU) && !defined(DOXYGEN)
    static BURGER_INLINE void Fixup(uint64_t* pInput) BURGER_NOEXCEPT { pInput[0] = Load(pInput); }
#elif defined(BURGER_PPC) && defined(BURGER_METROWERKS)
    static BURGER_INLINE void Fixup(uint64_t* pInput) BURGER_NOEXCEPT
    {
        uint32_t uLow = __lwbrx(pInput, 0);
        uint32_t uHigh = __lwbrx(pInput, 4);
        reinterpret_cast<uint32_t*>(pInput)[0] = uHigh;
        reinterpret_cast<uint32_t*>(pInput)[1] = uLow;
    }
#else
    static void BURGER_API Fixup(uint64_t* pInput) BURGER_NOEXCEPT;
#endif
    static BURGER_INLINE void FixupAny(char*) BURGER_NOEXCEPT {}
    static BURGER_INLINE void FixupAny(uint8_t*) BURGER_NOEXCEPT {}
    static BURGER_INLINE void FixupAny(int8_t*) BURGER_NOEXCEPT {}
    static void BURGER_API FixupAny(uint16_t* pInput) BURGER_NOEXCEPT;
    static void BURGER_API FixupAny(uint32_t* pInput) BURGER_NOEXCEPT;
    static void BURGER_API FixupAny(uint64_t* pInput) BURGER_NOEXCEPT;


    static BURGER_INLINE int16_t Load(const int16_t* pInput) BURGER_NOEXCEPT {
        return static_cast<int16_t>(Load(reinterpret_cast<const uint16_t*>(pInput))); }
    static BURGER_INLINE int32_t Load(const int32_t* pInput) BURGER_NOEXCEPT {
        return static_cast<int32_t>(Load(reinterpret_cast<const uint32_t*>(pInput))); }
    static BURGER_INLINE int64_t Load(const int64_t* pInput) BURGER_NOEXCEPT {
        return static_cast<int64_t>(Load(reinterpret_cast<const uint64_t*>(pInput))); }

    static BURGER_INLINE void Store(int16_t* pOutput, int16_t iInput) BURGER_NOEXCEPT {
        Store(reinterpret_cast<uint16_t*>(pOutput), static_cast<uint16_t>(iInput)); }
    static BURGER_INLINE void Store(int32_t* pOutput, int32_t iInput) BURGER_NOEXCEPT {
        Store(reinterpret_cast<uint32_t*>(pOutput), static_cast<uint32_t>(iInput)); }
    static BURGER_INLINE void Store(int64_t* pOutput, int64_t iInput) BURGER_NOEXCEPT {
        Store(reinterpret_cast<uint64_t*>(pOutput), static_cast<uint64_t>(iInput)); }

    static BURGER_INLINE int16_t LoadAny(const int16_t* pInput) BURGER_NOEXCEPT {
        return static_cast<int16_t>(LoadAny(reinterpret_cast<const uint16_t*>(pInput))); }
    static BURGER_INLINE int32_t LoadAny(const int32_t* pInput) BURGER_NOEXCEPT {
        return static_cast<int32_t>(LoadAny(reinterpret_cast<const uint32_t*>(pInput))); }
    static BURGER_INLINE int64_t LoadAny(const int64_t* pInput) BURGER_NOEXCEPT {
        return static_cast<int64_t>(LoadAny(reinterpret_cast<const uint64_t*>(pInput))); }

    static BURGER_INLINE void StoreAny(int16_t* pOutput, int16_t iInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint16_t*>(pOutput), static_cast<uint16_t>(iInput)); }
    static BURGER_INLINE void StoreAny(int32_t* pOutput, int32_t iInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint32_t*>(pOutput), static_cast<uint32_t>(iInput)); }
    static BURGER_INLINE void StoreAny(int64_t* pOutput, int64_t iInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint64_t*>(pOutput), static_cast<uint64_t>(iInput)); }

    static BURGER_INLINE void Fixup(int16_t* pInput) BURGER_NOEXCEPT { Fixup(reinterpret_cast<uint16_t*>(pInput)); }
    static BURGER_INLINE void Fixup(int32_t* pInput) BURGER_NOEXCEPT { Fixup(reinterpret_cast<uint32_t*>(pInput)); }
    static BURGER_INLINE void Fixup(int64_t* pInput) BURGER_NOEXCEPT { Fixup(reinterpret_cast<uint64_t*>(pInput)); }
    static BURGER_INLINE void Fixup(float* pInput) BURGER_NOEXCEPT { Fixup(static_cast<uint32_t*>(static_cast<void*>(pInput))); }
    static BURGER_INLINE void Fixup(double* pInput) BURGER_NOEXCEPT { Fixup(static_cast<uint64_t*>(static_cast<void*>(pInput))); }

    static BURGER_INLINE void FixupAny(int16_t* pInput) BURGER_NOEXCEPT { FixupAny(reinterpret_cast<uint16_t*>(pInput)); }
    static BURGER_INLINE void FixupAny(int32_t* pInput) BURGER_NOEXCEPT { FixupAny(reinterpret_cast<uint32_t*>(pInput)); }
    static BURGER_INLINE void FixupAny(int64_t* pInput) BURGER_NOEXCEPT { FixupAny(reinterpret_cast<uint64_t*>(pInput)); }
    static BURGER_INLINE void FixupAny(float* pInput) BURGER_NOEXCEPT { FixupAny(static_cast<uint32_t*>(static_cast<void*>(pInput))); }
    static BURGER_INLINE void FixupAny(double* pInput) BURGER_NOEXCEPT { FixupAny(static_cast<uint64_t*>(static_cast<void*>(pInput))); }

#if defined(BURGER_HAS_WCHAR_T) || defined(DOXYGEN)
    static BURGER_INLINE wchar_t Load(wchar_t uInput) BURGER_NOEXCEPT { return Load(static_cast<uint16_t>(uInput)); }
    static BURGER_INLINE wchar_t Load(const wchar_t* pInput) BURGER_NOEXCEPT { return Load(reinterpret_cast<const uint16_t*>(pInput)); }
    static BURGER_INLINE void Store(wchar_t* pOutput, wchar_t uInput) BURGER_NOEXCEPT { Store(reinterpret_cast<uint16_t *>(pOutput), static_cast<uint16_t>(uInput)); }
    static BURGER_INLINE wchar_t LoadAny(const wchar_t* pInput) BURGER_NOEXCEPT {
        return static_cast<wchar_t>(LoadAny(reinterpret_cast<const uint16_t*>(pInput))); }
    static BURGER_INLINE void StoreAny(wchar_t* pOutput, wchar_t uInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint16_t*>(pOutput), uInput); }
    static BURGER_INLINE void Fixup(wchar_t*pInput) BURGER_NOEXCEPT { Fixup(reinterpret_cast<uint16_t *>(pInput)); }
    static BURGER_INLINE void FixupAny(wchar_t*pInput) BURGER_NOEXCEPT { FixupAny(reinterpret_cast<uint16_t *>(pInput)); }
#endif

#if defined(BURGER_INT_NOT_IN_STDINT) || defined(DOXYGEN)
    static BURGER_INLINE unsigned int Load(unsigned int uInput) BURGER_NOEXCEPT {
        return static_cast<unsigned int>(Load(static_cast<uint2uint_t>(uInput))); }
    static BURGER_INLINE signed int Load(signed int iInput) BURGER_NOEXCEPT {
        return static_cast<signed int>(Load(static_cast<uint2uint_t>(iInput))); }
    static BURGER_INLINE unsigned int Load(const unsigned int* pInput) BURGER_NOEXCEPT {
        return static_cast<unsigned int>(Load(reinterpret_cast<const uint2uint_t *>(pInput))); }
    static BURGER_INLINE signed int Load(const signed int* pInput) BURGER_NOEXCEPT {
        return static_cast<signed int>(Load(reinterpret_cast<const uint2uint_t *>(pInput))); }
    static BURGER_INLINE void Store(unsigned int* pOutput, unsigned int uInput) BURGER_NOEXCEPT {
        Store(reinterpret_cast<uint2uint_t *>(pOutput), static_cast<uint2uint_t>(uInput)); }
    static BURGER_INLINE void Store(signed int* pOutput, signed int iInput) BURGER_NOEXCEPT {
        Store(reinterpret_cast<uint2uint_t *>(pOutput), static_cast<uint2uint_t>(iInput)); }
    static BURGER_INLINE unsigned int LoadAny(const unsigned int* pInput) BURGER_NOEXCEPT {
        return static_cast<unsigned int>(LoadAny(reinterpret_cast<const uint2uint_t*>(pInput)));}
    static BURGER_INLINE signed int LoadAny(const signed int* pInput) BURGER_NOEXCEPT {
        return static_cast<signed int>(LoadAny(reinterpret_cast<const uint2uint_t*>(pInput))); }
    static BURGER_INLINE void StoreAny(unsigned int* pOutput, unsigned int uInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint2uint_t*>(pOutput), static_cast<uint2uint_t>(uInput)); }
    static BURGER_INLINE void StoreAny(signed int* pOutput, signed int iInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint2uint_t*>(pOutput), static_cast<uint2uint_t>(iInput)); }
    static BURGER_INLINE void Fixup(unsigned int*pInput) BURGER_NOEXCEPT {
        Fixup(reinterpret_cast<uint2uint_t *>(pInput)); }
    static BURGER_INLINE void Fixup(signed int*pInput) BURGER_NOEXCEPT {
        Fixup(reinterpret_cast<uint2uint_t *>(pInput)); }
    static BURGER_INLINE void FixupAny(unsigned int*pInput) BURGER_NOEXCEPT {
        FixupAny(reinterpret_cast<uint2uint_t *>(pInput)); }
    static BURGER_INLINE void FixupAny(signed int*pInput) BURGER_NOEXCEPT {
        FixupAny(reinterpret_cast<uint2uint_t *>(pInput)); }
#endif

#if defined(BURGER_LONG_NOT_IN_STDINT) || defined(DOXYGEN)
    static BURGER_INLINE unsigned long Load(unsigned long uInput) BURGER_NOEXCEPT {
        return static_cast<unsigned long>(Load(static_cast<ulong2uint_t>(uInput))); }
    static BURGER_INLINE signed long Load(signed long iInput) BURGER_NOEXCEPT {
        return static_cast<signed long>(Load(static_cast<ulong2uint_t>(iInput))); }
    static BURGER_INLINE unsigned long Load(const unsigned long* pInput) BURGER_NOEXCEPT {
        return static_cast<unsigned long>(Load(reinterpret_cast<const ulong2uint_t *>(pInput))); }
    static BURGER_INLINE signed long Load(const signed long* pInput) BURGER_NOEXCEPT {
        return static_cast<signed long>(Load(reinterpret_cast<const ulong2uint_t *>(pInput))); }
    static BURGER_INLINE void Store(unsigned long* pOutput, unsigned long uInput) BURGER_NOEXCEPT {
        Store(reinterpret_cast<ulong2uint_t *>(pOutput), static_cast<ulong2uint_t>(uInput)); }
    static BURGER_INLINE void Store(signed long* pOutput, signed long iInput) BURGER_NOEXCEPT {
        Store(reinterpret_cast<ulong2uint_t *>(pOutput), static_cast<ulong2uint_t>(iInput)); }
    static BURGER_INLINE unsigned long LoadAny(const unsigned long* pInput) BURGER_NOEXCEPT {
        return static_cast<unsigned long>(LoadAny(reinterpret_cast<const ulong2uint_t*>(pInput))); }
    static BURGER_INLINE signed long LoadAny(const signed long* pInput) BURGER_NOEXCEPT {
        return static_cast<signed long>(LoadAny(reinterpret_cast<const ulong2uint_t*>(pInput))); }
    static BURGER_INLINE void StoreAny(unsigned long* pOutput, unsigned long uInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<ulong2uint_t*>(pOutput), static_cast<ulong2uint_t>(uInput)); }
    static BURGER_INLINE void StoreAny(signed long* pOutput, signed long iInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<ulong2uint_t*>(pOutput), static_cast<ulong2uint_t>(iInput)); }
    static BURGER_INLINE void Fixup(unsigned long*pInput) BURGER_NOEXCEPT {
        Fixup(reinterpret_cast<ulong2uint_t *>(pInput)); }
    static BURGER_INLINE void Fixup(signed long*pInput) BURGER_NOEXCEPT {
        Fixup(reinterpret_cast<ulong2uint_t *>(pInput)); }
    static BURGER_INLINE void FixupAny(unsigned long*pInput) BURGER_NOEXCEPT {
        FixupAny(reinterpret_cast<ulong2uint_t *>(pInput)); }
    static BURGER_INLINE void FixupAny(signed long*pInput) BURGER_NOEXCEPT {
        FixupAny(reinterpret_cast<ulong2uint_t *>(pInput)); }
#endif
    // clang-format on
};

/***************************************

    Don't swap endian

***************************************/

struct NativeEndian {

    // clang-format off
    static BURGER_INLINE BURGER_CONSTEXPR char Load(char iInput) BURGER_NOEXCEPT { return iInput; }
    static BURGER_INLINE BURGER_CONSTEXPR uint8_t Load(uint8_t uInput) BURGER_NOEXCEPT { return uInput; }
    static BURGER_INLINE BURGER_CONSTEXPR int8_t Load(int8_t iInput) BURGER_NOEXCEPT { return iInput; }
    static BURGER_INLINE BURGER_CONSTEXPR uint16_t Load(uint16_t uInput) BURGER_NOEXCEPT { return uInput; }
    static BURGER_INLINE BURGER_CONSTEXPR int16_t Load(int16_t iInput) BURGER_NOEXCEPT { return iInput; }
    static BURGER_INLINE BURGER_CONSTEXPR uint32_t Load(uint32_t uInput) BURGER_NOEXCEPT { return uInput; }
    static BURGER_INLINE BURGER_CONSTEXPR int32_t Load(int32_t iInput) BURGER_NOEXCEPT { return iInput; }
    static BURGER_INLINE BURGER_CONSTEXPR uint64_t Load(uint64_t uInput) BURGER_NOEXCEPT { return uInput; }
    static BURGER_INLINE BURGER_CONSTEXPR int64_t Load(int64_t iInput) BURGER_NOEXCEPT { return iInput; }
    static BURGER_INLINE BURGER_CONSTEXPR float Load(float fInput) BURGER_NOEXCEPT { return fInput; }
    static BURGER_INLINE BURGER_CONSTEXPR double Load(double dInput) BURGER_NOEXCEPT { return dInput; }

    static BURGER_INLINE BURGER_CONSTEXPR char Load(const char* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR uint8_t Load(const uint8_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR int8_t Load(const int8_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR uint16_t Load(const uint16_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR int16_t Load(const int16_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR uint32_t Load(const uint32_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR int32_t Load(const int32_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR uint64_t Load(const uint64_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR int64_t Load(const int64_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR float Load(const float* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR double Load(const double* pInput) BURGER_NOEXCEPT { return pInput[0]; }

    static BURGER_INLINE void Store(char* pOutput, char iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE void Store(uint8_t* pOutput, uint8_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void Store(int8_t* pOutput, int8_t iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void Store(int16_t* pOutput, int16_t iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE void Store(uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void Store(int32_t* pOutput, int32_t iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE void Store(int64_t* pOutput, int64_t iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE void Store(uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void Store(float* pOutput, float fInput) BURGER_NOEXCEPT { pOutput[0] = fInput; }
    static BURGER_INLINE void Store(double* pOutput, double dInput) BURGER_NOEXCEPT { pOutput[0] = dInput; }

    static BURGER_INLINE BURGER_CONSTEXPR char LoadAny(const char* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR uint8_t LoadAny(const uint8_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR int8_t LoadAny(const int8_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
#if (defined(BURGER_INTEL) || defined(BURGER_ARM64)) && !defined(DOXYGEN)
    static BURGER_INLINE BURGER_CONSTEXPR uint16_t LoadAny(const uint16_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR uint32_t LoadAny(const uint32_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR uint64_t LoadAny(const uint64_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR float LoadAny(const float* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR double LoadAny(const double* pInput) BURGER_NOEXCEPT { return pInput[0]; }
#else
    static uint16_t BURGER_API LoadAny(const uint16_t* pInput) BURGER_NOEXCEPT;
    static uint32_t BURGER_API LoadAny(const uint32_t* pInput) BURGER_NOEXCEPT;
    static uint64_t BURGER_API LoadAny(const uint64_t* pInput) BURGER_NOEXCEPT;
    static float BURGER_API LoadAny(const float* pInput) BURGER_NOEXCEPT;
    static double BURGER_API LoadAny(const double* pInput) BURGER_NOEXCEPT;
#endif
    static BURGER_INLINE int16_t LoadAny(const int16_t* pInput) BURGER_NOEXCEPT {
        return static_cast<int16_t>(LoadAny(reinterpret_cast<const uint16_t*>(pInput)));}
    static BURGER_INLINE int32_t LoadAny(const int32_t* pInput) BURGER_NOEXCEPT {
        return static_cast<int32_t>(LoadAny(reinterpret_cast<const uint32_t*>(pInput))); }
    static BURGER_INLINE int64_t LoadAny(const int64_t* pInput) BURGER_NOEXCEPT {
        return static_cast<int64_t>(LoadAny(reinterpret_cast<const uint64_t*>(pInput))); }

    static BURGER_INLINE void StoreAny(char* pOutput, char iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE void StoreAny(uint8_t* pOutput, uint8_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void StoreAny(int8_t* pOutput, int8_t iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
#if (defined(BURGER_INTEL) || defined(BURGER_ARM64)) && !defined(DOXYGEN)
    static void BURGER_API StoreAny(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static void BURGER_API StoreAny(uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static void BURGER_API StoreAny(uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static void BURGER_API StoreAny(float* pOutput, float fInput) BURGER_NOEXCEPT { pOutput[0] = fInput; }
    static void BURGER_API StoreAny(double* pOutput, double dInput) BURGER_NOEXCEPT { pOutput[0] = dInput; }
#else
    static void BURGER_API StoreAny(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT;
    static void BURGER_API StoreAny(uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT;
    static void BURGER_API StoreAny(uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT;
    static void BURGER_API StoreAny(float* pOutput, float fInput) BURGER_NOEXCEPT;
    static void BURGER_API StoreAny(double* pOutput, double dInput) BURGER_NOEXCEPT;
#endif

    static BURGER_INLINE void StoreAny(int16_t* pOutput, int16_t iInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint16_t*>(pOutput), static_cast<uint16_t>(iInput)); }
    static BURGER_INLINE void StoreAny(int32_t* pOutput, int32_t iInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint32_t*>(pOutput), static_cast<uint32_t>(iInput)); }
    static BURGER_INLINE void StoreAny(int64_t* pOutput, int64_t iInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint64_t*>(pOutput),  static_cast<uint64_t>(iInput)); }

#if defined(BURGER_HAS_WCHAR_T) || defined(DOXYGEN)
    static BURGER_INLINE BURGER_CONSTEXPR wchar_t Load(wchar_t uInput) BURGER_NOEXCEPT { return uInput; }
    static BURGER_INLINE BURGER_CONSTEXPR wchar_t Load(const wchar_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE void Store(wchar_t* pOutput, wchar_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE wchar_t LoadAny(const wchar_t* pInput) BURGER_NOEXCEPT {
        return static_cast<wchar_t>(LoadAny(reinterpret_cast<const uint16_t*>(pInput))); }
    static BURGER_INLINE void StoreAny(wchar_t* pOutput, wchar_t uInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint16_t*>(pOutput), uInput);
    }
#endif

#if defined(BURGER_INT_NOT_IN_STDINT) || defined(DOXYGEN)
    static BURGER_INLINE BURGER_CONSTEXPR unsigned int Load(unsigned int uInput) BURGER_NOEXCEPT { return uInput; }
    static BURGER_INLINE BURGER_CONSTEXPR signed int Load(signed int iInput) BURGER_NOEXCEPT { return iInput; }
    static BURGER_INLINE BURGER_CONSTEXPR unsigned int Load(const unsigned int* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR signed int Load(const signed int* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE void Store(unsigned int* pOutput, unsigned int uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void Store(signed int* pOutput, signed int iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE BURGER_CONSTEXPR unsigned int LoadAny(const unsigned int* pInput) BURGER_NOEXCEPT {
        return static_cast<unsigned int>(LoadAny(reinterpret_cast<const uint2uint_t*>(pInput)));}
    static BURGER_INLINE BURGER_CONSTEXPR signed int LoadAny(const signed int* pInput) BURGER_NOEXCEPT {
        return static_cast<signed int>(LoadAny(reinterpret_cast<const uint2uint_t*>(pInput))); }
    static BURGER_INLINE void StoreAny(unsigned int* pOutput, unsigned int uInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint2uint_t*>(pOutput), static_cast<uint2uint_t>(uInput)); }
    static BURGER_INLINE void StoreAny(signed int* pOutput, signed int iInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint2uint_t*>(pOutput), static_cast<uint2uint_t>(iInput)); }
#endif

#if defined(BURGER_LONG_NOT_IN_STDINT) || defined(DOXYGEN)
    static BURGER_INLINE BURGER_CONSTEXPR unsigned long Load(unsigned long uInput) BURGER_NOEXCEPT { return uInput; }
    static BURGER_INLINE BURGER_CONSTEXPR signed long Load(signed long iInput) BURGER_NOEXCEPT { return iInput; }
    static BURGER_INLINE BURGER_CONSTEXPR unsigned long Load(const unsigned long* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR signed long Load(const signed long* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE void Store(unsigned long* pOutput, unsigned long uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void Store(signed long* pOutput, signed long iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE unsigned long LoadAny(const unsigned long* pInput) BURGER_NOEXCEPT {
        return static_cast<unsigned long>(LoadAny(reinterpret_cast<const ulong2uint_t*>(pInput))); }
    static BURGER_INLINE signed long LoadAny(const signed long* pInput) BURGER_NOEXCEPT {
        return static_cast<signed long>(LoadAny(reinterpret_cast<const ulong2uint_t*>(pInput))); }
    static BURGER_INLINE void StoreAny(unsigned long* pOutput, unsigned long uInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<ulong2uint_t*>(pOutput), static_cast<ulong2uint_t>(uInput)); }
    static BURGER_INLINE void StoreAny(signed long* pOutput, signed long iInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<ulong2uint_t*>(pOutput), static_cast<ulong2uint_t>(iInput)); }
#endif
    // clang-format on

    template<typename T>
    static BURGER_INLINE void Fixup(T) BURGER_NOEXCEPT
    {
    }

    template<typename T>
    static BURGER_INLINE void FixupAny(T) BURGER_NOEXCEPT
    {
    }
};

#if (defined(BURGER_METROWERKS) && defined(BURGER_X86))
#pragma volatile_asm reset
#endif

// Which endian to swap, which to not.

#if defined(BURGER_LITTLEENDIAN)

struct LittleEndian: public NativeEndian {
};
struct BigEndian: public SwapEndian {
};
#define BURGER_BIGENDIAN16(x) (((x & 0xFFU) << 8U) + ((x >> 8U) & 0xFFU))
#define BURGER_BIGENDIAN32(x) \
    (((x & 0xFFU) << 24U) + ((x << 8U) & 0xFF0000U) + ((x >> 8U) & 0xFF00U) + \
        ((x >> 24U) & 0xFFU))
#define BURGER_LITTLEENDIAN16(x) (x)
#define BURGER_LITTLEENDIAN32(x) (x)
#else

struct LittleEndian: public SwapEndian {
};
struct BigEndian: public NativeEndian {
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
