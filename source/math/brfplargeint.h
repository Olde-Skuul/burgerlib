/***************************************

	Large integer parsing class.

	Copyright (c) 2016-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
#if defined(BURGER_64BITCPU) || defined(DOXYGEN)
	/** Size of each data chunk */
	typedef uint32_t ChunkSize_t;
	/** Size of data chunk for math (Double size of \ref ChunkSize_t) */
	typedef uint64_t MathSize_t;
#else
	typedef uint16_t ChunkSize_t;
	typedef uint32_t MathSize_t;
#endif

	/** Number of bits per uint32_t */
	static BURGER_CONSTEXPR const uint_t kBitsPerElement =
		static_cast<uint_t>(sizeof(ChunkSize_t)) * 8;
	/** Number of bits in the table to handle double precision */
	static BURGER_CONSTEXPR const uint_t kTotalBitsInTable = 1024 + 64;
	/** Number of elements in the data table */
	static BURGER_CONSTEXPR const uint_t kMaxElements =
		kTotalBitsInTable / kBitsPerElement;

protected:
	/** Number of entries in the data table */
	uint_t m_uEntryCount;
	/** Highest index with a non-zero entry in the data table */
	int_t m_iHighestNonZeroElement;
	/** Lowest index with a non-zero entry in the data table */
	int_t m_iLowestNonZeroElement;
	/** \ref TRUE if a zero length buffer */
	uint_t m_bIsZero;
	/** Data table of bits for locating digits */
	ChunkSize_t m_uDataTable[kMaxElements];

public:
	FPLargeInt(void) BURGER_NOEXCEPT;
	explicit FPLargeInt(uint_t uBitsNeeded) BURGER_NOEXCEPT;

	BURGER_INLINE uint_t IsZero(void) const BURGER_NOEXCEPT
	{
		return m_bIsZero;
	}
	BURGER_INLINE uint_t IsNotZero(void) const BURGER_NOEXCEPT
	{
		return !m_bIsZero;
	}

	void BURGER_API Init(uint_t uBitsNeeded) BURGER_NOEXCEPT;
	void BURGER_API InsertBitsAtEnd(
		uint32_t uBits, uint_t uShiftAmount) BURGER_NOEXCEPT;
	void BURGER_API InsertBitAtStart(
		uint32_t uBits, uint_t uShiftAmount) BURGER_NOEXCEPT;
	uint_t BURGER_API DivideReturnRemainder(uint_t uDivisor) BURGER_NOEXCEPT;
	uint_t BURGER_API MultiplyReturnOverflow(
		uint_t uMultiplier) BURGER_NOEXCEPT;

protected:
	void BURGER_API InsertChunkBits(
		ChunkSize_t uBits, uint_t uEntryIndex) BURGER_NOEXCEPT;
};

extern void BURGER_API LoadHighPrecisionFromFloat(FPLargeInt* pIntegerPart,
	FPLargeInt* pFractionalPart, const FPInfo* pFPInfo) BURGER_NOEXCEPT;

}
/* END */
#endif
