/***************************************

    Floating point analysis

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFPINFO_H__
#define __BRFPINFO_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
class FPInfo {
    /** Location of the sign bit in a 16 bit float */
    static BURGER_CONSTEXPR const uint_t kSignBit16 = 0x8000U;
    /** Mask for the exponent in a 16 bit float */
    static BURGER_CONSTEXPR const uint_t kExponentMask16 = 0x7C00U;
    /** Mask for the mantissa in a 16 bit float */
    static BURGER_CONSTEXPR const uint_t kMantissaMask16 = 0x03FFU;
    /** Implicit "1" bit for a 16 bit float */
    static BURGER_CONSTEXPR const uint_t kImplicitBit16 = 0x0400U;
    /** Number of bits in the mantissa for a 16 bit float */
    static BURGER_CONSTEXPR const uint_t kMantissaBitCount16 = 10;
    /** Number of bits in the exponent for a 16 bit float */
    static BURGER_CONSTEXPR const uint_t kExponentBitCount16 = 5;
    /** Bias of the exponent for a 16 bit float */
    static BURGER_CONSTEXPR const uint_t kExponentBias16 = 15;
    /** Special exponent for INF/NAN in a 16 bit float */
    static BURGER_CONSTEXPR const uint_t kSpecialExponent16 = 0x1FU;
    /** QNAN or SNAN bit in a 16 bit float */
    static BURGER_CONSTEXPR const uint_t kNANTypeBit16 = 0x0200U;

    /** Location of the sign bit in a 32 bit float */
    static BURGER_CONSTEXPR const uint_t kSignBit32 = 0x80000000U;
    /** Mask for the exponent in a 32 bit float */
    static BURGER_CONSTEXPR const uint_t kExponentMask32 = 0x7F800000U;
    /** Mask for the mantissa in a 32 bit float */
    static BURGER_CONSTEXPR const uint_t kMantissaMask32 = 0x007FFFFFU;
    /** Implicit "1" bit for a 32 bit float */
    static BURGER_CONSTEXPR const uint_t kImplicitBit32 = 0x00800000U;
    /** Number of bits in the mantissa for a 32 bit float */
    static BURGER_CONSTEXPR const uint_t kMantissaBitCount32 = 23;
    /** Number of bits in the exponent for a 32 bit float */
    static BURGER_CONSTEXPR const uint_t kExponentBitCount32 = 8;
    /** Bias of the exponent for a 32 bit float */
    static BURGER_CONSTEXPR const uint_t kExponentBias32 = 127;
    /** Special exponent for INF/NAN in a 32 bit float */
    static BURGER_CONSTEXPR const uint_t kSpecialExponent32 = 0xFFU;
    /** QNAN or SNAN bit in a 32 bit float */
    static BURGER_CONSTEXPR const uint_t kNANTypeBit32 = 0x00400000U;

    /** Location of the sign bit in a 64 bit float */
    static BURGER_CONSTEXPR const uint64_t kSignBit64 = 0x8000000000000000ULL;
    /** Mask for the exponent in a 64 bit float */
    static BURGER_CONSTEXPR const uint64_t kExponentMask64 = 0x7FF0000000000000ULL;
    /** Mask for the mantissa in a 64 bit float */
    static BURGER_CONSTEXPR const uint64_t kMantissaMask64 = 0x000FFFFFFFFFFFFFULL;
    /** Implicit "1" bit for a 64 bit float */
    static BURGER_CONSTEXPR const uint64_t kImplicitBit64 = 0x0010000000000000ULL;
    /** Number of bits in the mantissa for a 64 bit float */
    static BURGER_CONSTEXPR const uint_t kMantissaBitCount64 = 52;
    /** Number of bits in the exponent for a 64 bit float */
    static BURGER_CONSTEXPR const uint_t kExponentBitCount64 = 11;
    /** Bias of the exponent for a 64 bit float */
    static BURGER_CONSTEXPR const uint_t kExponentBias64 = 1023;
    /** Special exponent for INF/NAN in a 64 bit float */
    static BURGER_CONSTEXPR const uint_t kSpecialExponent64 = 0x7FFU;
    /** QNAN or SNAN bit in a 64 bit float */
    static BURGER_CONSTEXPR const uint64_t kNANTypeBit64 = 0x0008000000000000ULL;

    enum {
        /** Flag that's set if a floating point number was analyzed */
        kFlagValid = 0x01,
        /** Flag that's set if the number is negative */
        kFlagNegative = 0x02,
        /** Flag that's set if	the number is zero */
        kFlagZero = 0x04,
        /** Flag that's set if the number is a "Not a Number" */
        kFlagNAN = 0x08,
        /** Flag that's set if the number is a quiet "Not a Number" */
        kFlagQNAN = 0x10,
        /** Flag that's set if the number is an unknown "Not a Number" */
        kFlagIndefiniteNAN = 0x20,
        /** Flag that's set if the number is infinite */
        kFlagInfinity = 0x40,
        /** Flag that's set if the number is not normalized */
        kFlagSubNormal = 0x80
    };

    /** The mantissa value for the floating point number */
    uint64_t m_uMantissa;
    /** Exponent as stored in the floating point number */
    uint_t m_uBiasedExponent;
    /** Exponent with bias applied */
    int_t m_iExponent;
    /** Number of bits in the mantissa */
    uint_t m_uMantissaBitCount;
    /** Number of bits in the exponent */
    uint_t m_uExponentBitCount;
    /** Flags that contain the determined states */
    uint_t m_uFlags;

public:
    explicit FPInfo(void) BURGER_NOEXCEPT;
    explicit FPInfo(uint16_t sValue) BURGER_NOEXCEPT;
    explicit FPInfo(float fValue) BURGER_NOEXCEPT;
    explicit FPInfo(double dValue) BURGER_NOEXCEPT;
    BURGER_INLINE uint64_t GetMantissa(void) const BURGER_NOEXCEPT
    {
        return m_uMantissa;
    }
    BURGER_INLINE uint_t GetBiasedExponent(void) const BURGER_NOEXCEPT
    {
        return m_uBiasedExponent;
    }
    BURGER_INLINE int_t GetExponent(void) const BURGER_NOEXCEPT
    {
        return m_iExponent;
    }
    BURGER_INLINE uint_t GetMantissaBitCount(void) const BURGER_NOEXCEPT
    {
        return m_uMantissaBitCount;
    }
    BURGER_INLINE uint_t GetExponentBitCount(void) const BURGER_NOEXCEPT
    {
        return m_uExponentBitCount;
    }
    BURGER_INLINE uint_t IsValid(void) const BURGER_NOEXCEPT
    {
        return (m_uFlags & kFlagValid) != 0;
    }
    BURGER_INLINE uint_t IsNegative(void) const BURGER_NOEXCEPT
    {
        return (m_uFlags & kFlagNegative) != 0;
    }
    BURGER_INLINE uint_t IsZero(void) const BURGER_NOEXCEPT
    {
        return (m_uFlags & kFlagZero) != 0;
    }
    BURGER_INLINE uint_t IsNAN(void) const BURGER_NOEXCEPT
    {
        return (m_uFlags & kFlagNAN) != 0;
    }
    BURGER_INLINE uint_t IsQNAN(void) const BURGER_NOEXCEPT
    {
        return (m_uFlags & kFlagQNAN) != 0;
    }
    BURGER_INLINE uint_t IsIndefiniteNAN(void) const BURGER_NOEXCEPT
    {
        return (m_uFlags & kFlagIndefiniteNAN) != 0;
    }
    BURGER_INLINE uint_t IsInfinity(void) const BURGER_NOEXCEPT
    {
        return (m_uFlags & kFlagInfinity) != 0;
    }
    BURGER_INLINE uint_t IsSubNormal(void) const BURGER_NOEXCEPT
    {
        return (m_uFlags & kFlagSubNormal) != 0;
    }
    void BURGER_API InitHalf(uint16_t sValue) BURGER_NOEXCEPT;
    void BURGER_API InitFloat(float fValue) BURGER_NOEXCEPT;
    void BURGER_API InitDouble(double dValue) BURGER_NOEXCEPT;
};

}
/* END */

#endif
