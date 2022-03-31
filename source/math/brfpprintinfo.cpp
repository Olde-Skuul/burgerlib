/***************************************

	High precision floating point print class.

	Copyright (c) 2020-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfpprintinfo.h"
#include "brassert.h"
#include "brstringfunctions.h"

/*! ************************************

	\class Burger::FPPrintInfo
	\brief Digit printing class for floating point

	Analyze the floating point number. Determine if it is a 'special' form
	and where the digits start and end on each side of the decimal point.

	We also support optional significant digit cutoff, in order to emulate RTLs
	that quit after a set number of digits (looking at you Microsoft)

***************************************/

/*! ************************************

	\enum Burger::FPPrintInfo::eTextIndex
	\brief Indexes for special floating point number names.

	In the tables \ref g_SpecialFloatsTextLower and \ref
	g_SpecialFloatsTextUpper, the names of the special case floating point
	numbers are stored and the names are in the order of this enumeration.

***************************************/

const char* Burger::FPPrintInfo::g_SpecialFloatsTextLower[kTextCount] = {
	"inf", "-inf", "nan", "-nan", "-nan"};
const char* Burger::FPPrintInfo::g_SpecialFloatsTextUpper[kTextCount] = {
	"INF", "-INF", "NAN", "-NAN", "-NAN"};

/*! ************************************

	\fn Burger::FPPrintInfo::FPPrintInfo()
	\brief Default constructor

	Clear out the variables and set the data to zero

***************************************/

/*! ************************************

	\fn Burger::FPPrintInfo::FPPrintInfo(const FPInfo* pFPInfo,
		uint_t uPrecisionDigits)

	\brief Construct with input data

	\param pFPInfo Pointer to the floating point number to analyze
	\param uPrecisionDigits Number of digits of precision to print

	\sa AnalyzeFloat(const FPInfo*, uint_t)

***************************************/

/*! ************************************

	\brief Reset the class

	Clear out the variables and set the data to zero

***************************************/

void BURGER_API Burger::FPPrintInfo::Clear(void) BURGER_NOEXCEPT
{
	m_bHasIntegerPart = FALSE;
	m_bHasFractionalPart = FALSE;
	m_uSpecialFormDigits = 0;
	m_uFirstNonZeroIntDigitPos = 0;
	m_uLastNonZeroIntDigitPos = 0;
	m_uFirstNonZeroFracDigitPos = 0;
	m_uLastNonZeroFracDigitPos = 0;
	m_uPrintResult = kResultZero;
}

/*! ************************************

	\fn uint_t Burger::FPPrintInfo::HasInteger(void) const
	\brief Does the string have an integer part?

	\return \ref TRUE if there are integer digits

***************************************/

/*! ************************************

	\fn uint_t Burger::FPPrintInfo::HasFractional(void) const
	\brief Does the string have a fractional part?

	\return \ref TRUE if there are fractional digits

***************************************/

/*! ************************************

	\fn uint_t Burger::FPPrintInfo::GetSpecialFormDigits(void) const
	\brief Get the number of special form digits

	\return The number of special form digits

***************************************/

/*! ************************************

	\fn uint_t Burger::FPPrintInfo::GetFirstNonZeroIntDigitPosition(void) const
	\brief Get the index to the first integer digit

	\return The index to the first integer digit

***************************************/

/*! ************************************

	\fn uint_t Burger::FPPrintInfo::GetLastNonZeroIntDigitPosition(void) const
	\brief Get the index to the last integer digit

	\return The index to the last integer digit

***************************************/

/*! ************************************

	\fn uint_t Burger::FPPrintInfo::GetFirstNonZeroFracDigitPosition(void) const
	\brief Get the index to the first fractional digit

	\return The index to the first fractional digit

***************************************/

/*! ************************************

	\fn uint_t Burger::FPPrintInfo::GetLastNonZeroFracDigitPosition(void) const
	\brief Get the index to the last fractional digit

	\return The index to the last fractional digit

***************************************/

/*! ************************************

	\fn eResult Burger::FPPrintInfo::GetResult(void) const
	\brief Get the type of number to print.

	\return The type of number that should be printed.

***************************************/

/*! ************************************

	\fn uint_t Burger::FPPrintInfo::IsSpecial(void) const
	\brief Is the number a special?

	\return \ref TRUE if the number is NaN/INF

***************************************/

/*! ************************************

	\fn uint_t Burger::FPPrintInfo::IsInfinity(void) const
	\brief Is the number infinity?

	\return \ref TRUE if the number is INF

***************************************/

/*! ************************************

	\fn uint_t Burger::FPPrintInfo::IsNaN(void) const
	\brief Is the number Not a Number?

	\return \ref TRUE if the number is NaN

***************************************/

/*! ************************************

	\brief Analyze a floating point number

	Analyze the floating point number. Determine if it is a 'special' form
	and where the digits start and end on each side of the decimal point.

	\param pFPInfo Pointer to the floating point number to analyze
	\param uPrecisionDigits Number of digits of precision to print

***************************************/

void BURGER_API Burger::FPPrintInfo::AnalyzeFloat(
	const FPInfo* pFPInfo, uint_t uPrecisionDigits) BURGER_NOEXCEPT
{
	// Handle the easy cases.

	// Zero?
	if (pFPInfo->IsZero()) {
		m_bHasIntegerPart = TRUE;
		m_bHasFractionalPart = FALSE;
		m_uSpecialFormDigits = 0;
		m_uFirstNonZeroIntDigitPos = 1;
		m_uLastNonZeroIntDigitPos = 1;
		m_uFirstNonZeroFracDigitPos = 0;
		m_uLastNonZeroFracDigitPos = 0;
		m_uPrintResult = kResultZero;
		return;
	}

	// Infinity and beyond?
	if (pFPInfo->IsInfinity()) {

		// Get the string and type depending if it is signed or not.
		eResult uResult;
		const char* pText;
		if (pFPInfo->IsNegative()) {
			pText = g_SpecialFloatsTextLower[kNegInfinity];
			uResult = kResultNegativeInfinity;
		} else {
			pText = g_SpecialFloatsTextLower[kPosInfinity];
			uResult = kResultPositiveInfinity;
		}

		// No digits, but has a special string.
		m_bHasIntegerPart = FALSE;
		m_bHasFractionalPart = FALSE;
		m_uSpecialFormDigits = static_cast<uint_t>(StringLength(pText));
		m_uFirstNonZeroIntDigitPos = 0;
		m_uLastNonZeroIntDigitPos = 0;
		m_uFirstNonZeroFracDigitPos = 0;
		m_uLastNonZeroFracDigitPos = 0;
		m_uPrintResult = uResult;
		return;
	}

	// Not even a number?
	if (pFPInfo->IsNAN()) {
		// Get the string and type depending if it is signed or not.
		eResult uResult;
		const char* pText;
		if (pFPInfo->IsNegative()) {
			pText = g_SpecialFloatsTextLower[kNegNan];
			uResult = kResultNegativeNaN;
		} else {
			pText = g_SpecialFloatsTextLower[kPosNan];
			uResult = kResultPositiveNaN;
		}

		m_bHasIntegerPart = FALSE;
		m_bHasFractionalPart = FALSE;
		m_uSpecialFormDigits = static_cast<uint_t>(StringLength(pText));
		m_uFirstNonZeroIntDigitPos = 0;
		m_uLastNonZeroIntDigitPos = 0;
		m_uFirstNonZeroFracDigitPos = 0;
		m_uLastNonZeroFracDigitPos = 0;
		m_uPrintResult = uResult;
		return;
	}

	// It's an actual number, let's do it the hard way.

	// How many bits do we need to express this number?
	const uint_t uBitsNeeded = (1U << (pFPInfo->GetExponentBitCount() - 1)) +
		pFPInfo->GetMantissaBitCount();

	// Parse out both the integer and the fraction
	FPLargeInt IntegerPart(uBitsNeeded);
	FPLargeInt FractionalPart(uBitsNeeded);

	// Initialize our high precision numbers and result status
	LoadHighPrecisionFromFloat(&IntegerPart, &FractionalPart, pFPInfo);

	// All numbers have at least an integer part if not zero

	m_uPrintResult = kResultNormalFloat;
	m_bHasIntegerPart = IntegerPart.IsNotZero();
	m_uFirstNonZeroIntDigitPos = 0;
	m_uSpecialFormDigits = 0;

	// Now process all the integer digits, from least significant to
	// most (right to left) loop at least once, to get a leading zero.
	// Get a count of the number of digits, and the position of the
	// first non-zero digit (from the decimal point)

	uint_t bNonZeroDigitFound = FALSE;
	uint_t bIntPartisNotAll9s = FALSE;
	uint_t uIntDigitsReturned = 0;

	// Get integer digits from the integer part

	while (IntegerPart.IsNotZero()) {
		const uint_t uTheDigit = IntegerPart.DivideReturnRemainder(10);
		++uIntDigitsReturned;

		// Leading zero?
		if (uTheDigit && !bNonZeroDigitFound) {
			bNonZeroDigitFound = TRUE;
			m_uFirstNonZeroIntDigitPos = uIntDigitsReturned;
		}

		// We track all 9's for the special case when we round all 9s up
		// and create an extra digit
		if (!bIntPartisNotAll9s && (uTheDigit != 9)) {
			bIntPartisNotAll9s = TRUE;
		}
	}
	m_uLastNonZeroIntDigitPos = uIntDigitsReturned;

	// Now process all the fractional digits, from most significant to
	// least (left to right) Get a count of the number of digits, and
	// the position of the first non-zero digit (right from the decimal
	// point) and the last non-zero digit

	m_bHasFractionalPart = FractionalPart.IsNotZero();
	m_uFirstNonZeroFracDigitPos = 0;

	uint_t uFracDigitsReturned = 0;
	uint_t bFracPartIsNotAll9s = FALSE;
	bNonZeroDigitFound = FALSE;

	// Loop to extract fractional digits

	while (FractionalPart.IsNotZero() &&
		(uFracDigitsReturned < uPrecisionDigits)) {
		const uint_t uTheDigit = FractionalPart.MultiplyReturnOverflow(10);
		++uFracDigitsReturned;

		if (uTheDigit && !bNonZeroDigitFound) {
			m_uFirstNonZeroFracDigitPos = uFracDigitsReturned;
			bNonZeroDigitFound = TRUE;
		}

		// we track all 9's for the special case when we round all 9s up
		// and create an extra digit
		if (!bFracPartIsNotAll9s && (uTheDigit != 9)) {
			bFracPartIsNotAll9s = TRUE;
		}
	}

	// Last digit could actually be 0
	m_uLastNonZeroFracDigitPos = uFracDigitsReturned;

	// Check for rounding up the fraction.
	if (FractionalPart.IsNotZero()) {
		// Get next digit...
		const uint_t uTheDigit = FractionalPart.MultiplyReturnOverflow(10);

		// do we round up or truncate?
		if (uTheDigit >= 5) {
			// Special case: the number is all '9's up to this point!
			if (!bFracPartIsNotAll9s && !bIntPartisNotAll9s) {
				// Add a digit to the int part, it's '1' plus some
				// number of zeros now
				++m_uLastNonZeroIntDigitPos;
				m_uFirstNonZeroIntDigitPos = m_uLastNonZeroIntDigitPos;
				m_uPrintResult = kResultFloatRoundedUpADigit;
			} else {
				m_uPrintResult = kResultFloatRoundedUpAtEnd;
			}
		}
	}
}
