/***************************************

	High precision floating point print class.

	Copyright (c) 2020-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFPPRINTINFO_H__
#define __BRFPPRINTINFO_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFPINFO_H__
#include "brfpinfo.h"
#endif

#ifndef __BRFPLARGEINT_H__
#include "brfplargeint.h"
#endif

/* BEGIN */
namespace Burger {

class FPPrintInfo {
	enum eTextIndex {
		/** Index to "inf" */
		kPosInfinity,
		/** Index to "-inf" */
		kNegInfinity,
		/** Index to "nan" */
		kPosNan,
		/** Index to "-nan" */
		kNegNan,
		/** Index to "-nan" */
		kIndNan,
		/** Number of string indexes */
		kTextCount
	};

public:
	enum eResult {
		/** Print a zero */
		kResultZero,
		/** Print a normal number */
		kResultNormalFloat,
		/** Print a number that is rounded in the fraction */
		kResultFloatRoundedUpAtEnd,
		/** Print a number that is rounded in the integer */
		kResultFloatRoundedUpADigit,
		/** Print a positive infinity */
		kResultPositiveInfinity,
		/** Print a negative infinity */
		kResultNegativeInfinity,
		/** Print a positive NaN */
		kResultPositiveNaN,
		/** Print a negative NaN */
		kResultNegativeNaN,
		/** Print a signaling NaN */
		kResultSignalingNaN,
		/** Print a quiet NaN */
		kResultQuietNaN,
		/** Print an indefinite NaN */
		kResultIndefiniteNaN,
		/** End of print list */
		kResultCount,
		/** First special string to print */
		kResultFirstSpecial = kResultPositiveInfinity,
		/** Last special string to print */
		kResultLastSpecial = kResultIndefiniteNaN
	};

	/** Special float message strings */
	static const char* g_SpecialFloatsTextLower[kTextCount];
	/** Special float message string in uppercase */
	static const char* g_SpecialFloatsTextUpper[kTextCount];

private:
	/** \ref TRUE if there is an integer component */
	uint_t m_bHasIntegerPart;

	/** \ref TRUE if there is a fractional component */
	uint_t m_bHasFractionalPart;

	/** Number of digits for special text */
	uint_t m_uSpecialFormDigits;

	/** Index to the first non zero integer digit */
	uint_t m_uFirstNonZeroIntDigitPos;

	/** Index to the last non zero integer digit */
	uint_t m_uLastNonZeroIntDigitPos;

	/** Index to the first non zero fractional digit */
	uint_t m_uFirstNonZeroFracDigitPos;

	/** Index to the last non zero integer digit */
	uint_t m_uLastNonZeroFracDigitPos;

	/** Type of floating point number */
	eResult m_uPrintResult;

public:
	FPPrintInfo(void) BURGER_NOEXCEPT
	{
		Clear();
	}
	FPPrintInfo(const FPInfo* pFPInfo, uint_t uPrecisionDigits) BURGER_NOEXCEPT
	{
		AnalyzeFloat(pFPInfo, uPrecisionDigits);
	}

	void BURGER_API Clear(void) BURGER_NOEXCEPT;

	BURGER_INLINE uint_t HasInteger(void) const BURGER_NOEXCEPT
	{
		return m_bHasIntegerPart;
	}
	BURGER_INLINE uint_t HasFractional(void) const BURGER_NOEXCEPT
	{
		return m_bHasFractionalPart;
	}
	BURGER_INLINE uint_t GetSpecialFormDigits(void) const BURGER_NOEXCEPT
	{
		return m_uSpecialFormDigits;
	}
	BURGER_INLINE uint_t GetFirstNonZeroIntDigitPosition(
		void) const BURGER_NOEXCEPT
	{
		return m_uFirstNonZeroIntDigitPos;
	}
	BURGER_INLINE uint_t GetLastNonZeroIntDigitPosition(
		void) const BURGER_NOEXCEPT
	{
		return m_uLastNonZeroIntDigitPos;
	}
	BURGER_INLINE uint_t GetFirstNonZeroFracDigitPosition(
		void) const BURGER_NOEXCEPT
	{
		return m_uFirstNonZeroFracDigitPos;
	}
	BURGER_INLINE uint_t GetLastNonZeroFracDigitPosition(
		void) const BURGER_NOEXCEPT
	{
		return m_uLastNonZeroFracDigitPos;
	}
	BURGER_INLINE eResult GetResult(void) const BURGER_NOEXCEPT
	{
		return m_uPrintResult;
	}
	BURGER_INLINE uint_t IsSpecial(void) const BURGER_NOEXCEPT
	{
		return (m_uPrintResult >= kResultFirstSpecial) &&
			(m_uPrintResult <= kResultLastSpecial);
	}
	BURGER_INLINE uint_t IsInfinity(void) const BURGER_NOEXCEPT
	{
		return (m_uPrintResult == kResultPositiveInfinity) ||
			(m_uPrintResult == kResultNegativeInfinity);
	}
	BURGER_INLINE uint_t IsNaN(void) const BURGER_NOEXCEPT
	{
		return (m_uPrintResult >= kResultPositiveNaN) &&
			(m_uPrintResult <= kResultIndefiniteNaN);
	}
	void BURGER_API AnalyzeFloat(
		const FPInfo* pFPInfo, uint_t uPrecisionDigits) BURGER_NOEXCEPT;
};

}
/* END */
#endif