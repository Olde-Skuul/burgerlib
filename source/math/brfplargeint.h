/***************************************

	Large integer parsing class.

	Copyright (c) 2016-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFPLARGEINT_H__
#define __BRFPLARGEINT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFPINFO_H__
#include "brfpinfo.h"
#endif

/* BEGIN */
namespace Burger {

class FPLargeInt {
public:
	// Use half of the largest integer size that the CPU can handle
	// This will allow 16x16 -> 32 or 32x32 -> 64 single instruction
	// multiplication

#if defined(BURGER_64BITCPU) || defined(DOXYGEN)
	/** Size of each data chunk */
	typedef uint32_t chunk_size_t;
	/** Size of data chunk for math (Double size of \ref chunk_size_t) */
	typedef uint64_t math_size_t;
#else
	typedef uint16_t chunk_size_t;
	typedef uint32_t math_size_t;
#endif

	/** Number of bits per uint32_t */
	static BURGER_CONSTEXPR const uint32_t kBitsPerElement =
		static_cast<uint32_t>(sizeof(chunk_size_t)) * 8U;

	/** Number of bits in the table to handle double precision */
	static BURGER_CONSTEXPR const uint32_t kTotalBitsInTable = 1024 + 64;

	/** Number of elements in the data table */
	static BURGER_CONSTEXPR const uint32_t kMaxElements =
		kTotalBitsInTable / kBitsPerElement;

protected:
	/** Number of entries in the data table */
	uint32_t m_uEntryCount;

	/** Highest index with a non-zero entry in the data table */
	int32_t m_iHighestNonZeroElement;

	/** Lowest index with a non-zero entry in the data table */
	int32_t m_iLowestNonZeroElement;

	/** \ref TRUE if a zero length buffer */
	uint32_t m_bIsZero;

	/** Data table of bits for locating digits */
	chunk_size_t m_uDataTable[kMaxElements];

public:
	FPLargeInt(void) BURGER_NOEXCEPT;
	explicit FPLargeInt(uint32_t uBitsNeeded) BURGER_NOEXCEPT;

	BURGER_INLINE uint_t is_zero(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(m_bIsZero);
	}

	BURGER_INLINE uint_t is_not_zero(void) const BURGER_NOEXCEPT
	{
		return m_bIsZero == 0;
	}

	void BURGER_API init(uint32_t uBitsNeeded) BURGER_NOEXCEPT;
	void BURGER_API insert_bits_at_end(
		uint32_t uBits, uint32_t uShiftAmount) BURGER_NOEXCEPT;
	void BURGER_API insert_bits_at_start(
		uint32_t uBits, uint32_t uShiftAmount) BURGER_NOEXCEPT;
	uint32_t BURGER_API divide_return_remainder(
		uint32_t uDivisor) BURGER_NOEXCEPT;
	uint32_t BURGER_API multiply_return_overflow(
		uint32_t uMultiplier) BURGER_NOEXCEPT;

protected:
	void BURGER_API insert_chunk_bits(
		chunk_size_t uBits, uint32_t uEntryIndex) BURGER_NOEXCEPT;
};

extern void BURGER_API separate_integer_fraction(FPLargeInt* pIntegerPart,
	FPLargeInt* pFractionalPart, const FPInfo* pFPInfo) BURGER_NOEXCEPT;

}
/* END */
#endif
