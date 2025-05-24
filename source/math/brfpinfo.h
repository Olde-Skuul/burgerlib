/***************************************

	Floating point analysis

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
	static BURGER_CONSTEXPR const uint32_t kSignBit16 = 0x8000U;

	/** Mask for the exponent in a 16 bit float */
	static BURGER_CONSTEXPR const uint32_t kExponentMask16 = 0x7C00U;

	/** Mask for the mantissa in a 16 bit float */
	static BURGER_CONSTEXPR const uint32_t kMantissaMask16 = 0x03FFU;

	/** Implicit "1" bit for a 16 bit float */
	static BURGER_CONSTEXPR const uint32_t kImplicitBit16 = 0x0400U;

	/** Number of bits in the mantissa for a 16 bit float */
	static BURGER_CONSTEXPR const uint32_t kMantissaBitCount16 = 10;

	/** Number of bits in the exponent for a 16 bit float */
	static BURGER_CONSTEXPR const uint32_t kExponentBitCount16 = 5;

	/** Bias of the exponent for a 16 bit float */
	static BURGER_CONSTEXPR const uint32_t kExponentBias16 = 15;

	/** Special exponent for INF/NAN in a 16 bit float */
	static BURGER_CONSTEXPR const uint32_t kSpecialExponent16 = 0x1FU;

	/** QNAN or SNAN bit in a 16 bit float */
	static BURGER_CONSTEXPR const uint32_t kNANTypeBit16 = 0x0200U;

	/** Location of the sign bit in a 32 bit float */
	static BURGER_CONSTEXPR const uint32_t kSignBit32 = 0x80000000U;

	/** Mask for the exponent in a 32 bit float */
	static BURGER_CONSTEXPR const uint32_t kExponentMask32 = 0x7F800000U;

	/** Mask for the mantissa in a 32 bit float */
	static BURGER_CONSTEXPR const uint32_t kMantissaMask32 = 0x007FFFFFU;

	/** Implicit "1" bit for a 32 bit float */
	static BURGER_CONSTEXPR const uint32_t kImplicitBit32 = 0x00800000U;

	/** Number of bits in the mantissa for a 32 bit float */
	static BURGER_CONSTEXPR const uint32_t kMantissaBitCount32 = 23;

	/** Number of bits in the exponent for a 32 bit float */
	static BURGER_CONSTEXPR const uint32_t kExponentBitCount32 = 8;

	/** Bias of the exponent for a 32 bit float */
	static BURGER_CONSTEXPR const uint32_t kExponentBias32 = 127;

	/** Special exponent for INF/NAN in a 32 bit float */
	static BURGER_CONSTEXPR const uint32_t kSpecialExponent32 = 0xFFU;

	/** QNAN or SNAN bit in a 32 bit float */
	static BURGER_CONSTEXPR const uint32_t kNANTypeBit32 = 0x00400000U;

	/** Location of the sign bit in a 64 bit float */
	static BURGER_CONSTEXPR const uint64_t kSignBit64 = 0x8000000000000000ULL;

	/** Mask for the exponent in a 64 bit float */
	static BURGER_CONSTEXPR const uint64_t kExponentMask64 =
		0x7FF0000000000000ULL;

	/** Mask for the mantissa in a 64 bit float */
	static BURGER_CONSTEXPR const uint64_t kMantissaMask64 =
		0x000FFFFFFFFFFFFFULL;

	/** Implicit "1" bit for a 64 bit float */
	static BURGER_CONSTEXPR const uint64_t kImplicitBit64 =
		0x0010000000000000ULL;

	/** Number of bits in the mantissa for a 64 bit float */
	static BURGER_CONSTEXPR const uint32_t kMantissaBitCount64 = 52;

	/** Number of bits in the exponent for a 64 bit float */
	static BURGER_CONSTEXPR const uint32_t kExponentBitCount64 = 11;

	/** Bias of the exponent for a 64 bit float */
	static BURGER_CONSTEXPR const uint32_t kExponentBias64 = 1023;

	/** Special exponent for INF/NAN in a 64 bit float */
	static BURGER_CONSTEXPR const uint32_t kSpecialExponent64 = 0x7FFU;

	/** QNAN or SNAN bit in a 64 bit float */
	static BURGER_CONSTEXPR const uint64_t kNANTypeBit64 =
		0x0008000000000000ULL;

	/** Flag that's set if a floating point number was analyzed */
	static BURGER_CONSTEXPR const uint32_t kFlagValidShift = 0U;

	/** Flag that's set if the number is negative */
	static BURGER_CONSTEXPR const uint32_t kFlagNegativeShift = 1U;

	/** Flag that's set if	the number is zero */
	static BURGER_CONSTEXPR const uint32_t kFlagZeroShift = 2U;

	/** Flag that's set if the number is a "Not a Number" */
	static BURGER_CONSTEXPR const uint32_t kFlagNANShift = 3U;

	/** Flag that's set if the number is a quiet "Not a Number" */
	static BURGER_CONSTEXPR const uint32_t kFlagQNANShift = 4U;

	/** Flag that's set if the number is an unknown "Not a Number" */
	static BURGER_CONSTEXPR const uint32_t kFlagIndefiniteNANShift = 5U;

	/** Flag that's set if the number is infinite */
	static BURGER_CONSTEXPR const uint32_t kFlagInfinityShift = 6U;

	/** Flag that's set if the number is not normalized */
	static BURGER_CONSTEXPR const uint32_t kFlagSubNormalShift = 7U;

	/** The mantissa value for the floating point number */
	uint64_t m_uMantissa;

	/** Exponent as stored in the floating point number */
	uint32_t m_uBiasedExponent;

	/** Exponent with bias applied */
	int32_t m_iExponent;

	/** Number of bits in the mantissa */
	uint32_t m_uMantissaBitCount;

	/** Number of bits in the exponent */
	uint32_t m_uExponentBitCount;

	/** Flags that contain the determined states */
	uint32_t m_uFlags;

public:
	explicit FPInfo(void) BURGER_NOEXCEPT;
	explicit FPInfo(uint16_t uValue) BURGER_NOEXCEPT;
	explicit FPInfo(float fValue) BURGER_NOEXCEPT;
	explicit FPInfo(double dValue) BURGER_NOEXCEPT;

	void BURGER_API init_half(uint16_t uValue) BURGER_NOEXCEPT;
	void BURGER_API init_float(float fValue) BURGER_NOEXCEPT;
	void BURGER_API init_double(double dValue) BURGER_NOEXCEPT;

	BURGER_INLINE uint64_t get_mantissa(void) const BURGER_NOEXCEPT
	{
		return m_uMantissa;
	}

	BURGER_INLINE uint32_t get_biased_exponent(void) const BURGER_NOEXCEPT
	{
		return m_uBiasedExponent;
	}

	BURGER_INLINE int32_t get_exponent(void) const BURGER_NOEXCEPT
	{
		return m_iExponent;
	}

	BURGER_INLINE uint32_t get_mantissa_bit_count(void) const BURGER_NOEXCEPT
	{
		return m_uMantissaBitCount;
	}

	BURGER_INLINE uint32_t get_exponent_bit_count(void) const BURGER_NOEXCEPT
	{
		return m_uExponentBitCount;
	}

	BURGER_INLINE uint_t is_valid(void) const BURGER_NOEXCEPT
	{
		return (m_uFlags >> kFlagValidShift) & 1U;
	}

	BURGER_INLINE uint_t is_negative(void) const BURGER_NOEXCEPT
	{
		return (m_uFlags >> kFlagNegativeShift) & 1U;
	}

	BURGER_INLINE uint_t is_zero(void) const BURGER_NOEXCEPT
	{
		return (m_uFlags >> kFlagZeroShift) & 1U;
	}

	BURGER_INLINE uint_t is_NaN(void) const BURGER_NOEXCEPT
	{
		return (m_uFlags >> kFlagNANShift) & 1U;
	}

	BURGER_INLINE uint_t is_QNaN(void) const BURGER_NOEXCEPT
	{
		return (m_uFlags >> kFlagQNANShift) & 1U;
	}

	BURGER_INLINE uint_t is_indefinite_NaN(void) const BURGER_NOEXCEPT
	{
		return (m_uFlags >> kFlagIndefiniteNANShift) & 1U;
	}

	BURGER_INLINE uint_t is_infinite(void) const BURGER_NOEXCEPT
	{
		return (m_uFlags >> kFlagInfinityShift) & 1U;
	}

	BURGER_INLINE uint_t is_subnormal(void) const BURGER_NOEXCEPT
	{
		return (m_uFlags >> kFlagSubNormalShift) & 1U;
	}

	BURGER_INLINE uint_t is_finite(void) const BURGER_NOEXCEPT
	{
		return (m_uFlags &
				   ((1U << kFlagInfinityShift) | (1U << kFlagNANShift))) == 0;
	}

	BURGER_INLINE FPInfo& operator=(uint16_t usValue) BURGER_NOEXCEPT
	{
		init_half(usValue);
		return *this;
	}

	BURGER_INLINE FPInfo& operator=(float fValue) BURGER_NOEXCEPT
	{
		init_float(fValue);
		return *this;
	}

	BURGER_INLINE FPInfo& operator=(double dValue) BURGER_NOEXCEPT
	{
		init_double(dValue);
		return *this;
	}
};

}
/* END */

#endif
