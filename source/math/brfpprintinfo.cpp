/***************************************

	High precision floating point print class.

	Copyright (c) 2020-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
	and where the digits start and end on each side of the decimal point. It is
	used to determine exactly how the final result of the string would be for
	the floating point number in question. With this information, a floating
	point number can be printed at any precision.

	It also supports optional significant digit cutoff, in order to emulate run
	time libraries that quit after a set number of digits (looking at you
	Microsoft)

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

	\enum Burger::FPPrintInfo::eResult
	\brief Detected floating point number

	After parsing a floating point number for printing, this enumeration is one
	of the many types of output that the final result will be.

***************************************/

/*! ************************************

	\fn Burger::FPPrintInfo::FPPrintInfo()
	\brief Default constructor

	Clear out the variables and set the data to zero

***************************************/

/*! ************************************

	\fn Burger::FPPrintInfo::FPPrintInfo(const FPInfo* pFPInfo,
		uint32_t uPrecisionDigits)

	\brief Construct with input data

	\param pFPInfo Pointer to the floating point number to analyze
	\param uPrecisionDigits Number of digits of precision to print

	\sa analyze_float(const FPInfo*, uint32_t)

***************************************/

/*! ************************************

	\brief Reset the class

	Clear out the variables and set the data to zero

***************************************/

void BURGER_API Burger::FPPrintInfo::clear(void) BURGER_NOEXCEPT
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

	\fn Burger::FPPrintInfo::has_integer(void) const
	\brief Does the string have an integer part?

	\return \ref TRUE if there are integer digits

***************************************/

/*! ************************************

	\fn Burger::FPPrintInfo::has_fractional(void) const
	\brief Does the string have a fractional part?

	\return \ref TRUE if there are fractional digits

***************************************/

/*! ************************************

	\fn Burger::FPPrintInfo::get_special_form_digits(void) const
	\brief Get the number of special form digits

	\return The number of special form digits

***************************************/

/*! ************************************

	\fn Burger::FPPrintInfo::get_first_non_zero_int_digit_position(void) const
	\brief Get the index to the first integer digit

	\return The index to the first integer digit

***************************************/

/*! ************************************

	\fn Burger::FPPrintInfo::get_last_non_zero_int_digit_position(void) const
	\brief Get the index to the last integer digit

	\return The index to the last integer digit

***************************************/

/*! ************************************

	\fn Burger::FPPrintInfo::get_first_non_zero_frac_digit_position(void) const
	\brief Get the index to the first fractional digit

	\return The index to the first fractional digit

***************************************/

/*! ************************************

	\fn Burger::FPPrintInfo::get_last_non_zero_frac_digit_position(void) const
	\brief Get the index to the last fractional digit

	\return The index to the last fractional digit

***************************************/

/*! ************************************

	\fn Burger::FPPrintInfo::get_result(void) const
	\brief Get the type of number to print.

	\return The type of number that should be printed.

***************************************/

/*! ************************************

	\fn Burger::FPPrintInfo::is_special(void) const
	\brief Is the number a special?

	\return \ref TRUE if the number is NaN/INF

***************************************/

/*! ************************************

	\fn Burger::FPPrintInfo::is_infinite(void) const
	\brief Is the number infinity?

	\return \ref TRUE if the number is INF

***************************************/

/*! ************************************

	\fn Burger::FPPrintInfo::is_NaN(void) const
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

void BURGER_API Burger::FPPrintInfo::analyze_float(
	const FPInfo* pFPInfo, uint32_t uPrecisionDigits) BURGER_NOEXCEPT
{
	// Handle the easy cases.

	// Zero?
	if (pFPInfo->is_zero()) {
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
	if (pFPInfo->is_infinite()) {

		// Get the string and type depending if it is signed or not.
		eResult uResult;
		const char* pText;
		if (pFPInfo->is_negative()) {
			pText = g_SpecialFloatsTextLower[kNegInfinity];
			uResult = kResultNegativeInfinity;
		} else {
			pText = g_SpecialFloatsTextLower[kPosInfinity];
			uResult = kResultPositiveInfinity;
		}

		// No digits, but has a special string.
		m_bHasIntegerPart = FALSE;
		m_bHasFractionalPart = FALSE;
		m_uSpecialFormDigits = static_cast<uint32_t>(string_length(pText));
		m_uFirstNonZeroIntDigitPos = 0;
		m_uLastNonZeroIntDigitPos = 0;
		m_uFirstNonZeroFracDigitPos = 0;
		m_uLastNonZeroFracDigitPos = 0;
		m_uPrintResult = uResult;
		return;
	}

	// Not even a number?
	if (pFPInfo->is_NaN()) {

		// Get the string and type depending if it is signed or not.
		eResult uResult;
		const char* pText;
		if (pFPInfo->is_negative()) {
			pText = g_SpecialFloatsTextLower[kNegNan];
			uResult = kResultNegativeNaN;
		} else {
			pText = g_SpecialFloatsTextLower[kPosNan];
			uResult = kResultPositiveNaN;
		}

		m_bHasIntegerPart = FALSE;
		m_bHasFractionalPart = FALSE;
		m_uSpecialFormDigits = static_cast<uint32_t>(string_length(pText));
		m_uFirstNonZeroIntDigitPos = 0;
		m_uLastNonZeroIntDigitPos = 0;
		m_uFirstNonZeroFracDigitPos = 0;
		m_uLastNonZeroFracDigitPos = 0;
		m_uPrintResult = uResult;
		return;
	}

	// It's an actual number, let's do it the hard way.

	// How many bits do we need to express this number?
	// When we say bits, how many from the centerline based on the exponent
	const uint32_t uBitsNeeded =
		(1U << (pFPInfo->get_exponent_bit_count() - 1)) +
		pFPInfo->get_mantissa_bit_count();

	// Parse out both the integer and the fraction
	FPLargeInt IntegerPart(uBitsNeeded);
	FPLargeInt FractionalPart(uBitsNeeded);

	// Initialize our high precision numbers and result status
	separate_integer_fraction(&IntegerPart, &FractionalPart, pFPInfo);

	// All numbers have at least an integer part if not zero

	m_uPrintResult = kResultNormalFloat;
	m_bHasIntegerPart = IntegerPart.is_not_zero();
	m_uFirstNonZeroIntDigitPos = 0;
	m_uSpecialFormDigits = 0;

	// Now process all the integer digits, from least significant to
	// most (right to left) loop at least once, to get a leading zero.
	// Get a count of the number of digits, and the position of the
	// first non-zero digit (from the decimal point)

	uint_t bNonZeroDigitFound = FALSE;
	uint_t bIntPartisNotAll9s = FALSE;
	uint32_t uIntDigitsReturned = 0;

	// Get integer digits from the integer part

	while (IntegerPart.is_not_zero()) {
		const uint32_t uTheDigit = IntegerPart.divide_return_remainder(10U);
		++uIntDigitsReturned;

		// Found the first non zero digit
		if (uTheDigit && !bNonZeroDigitFound) {
			bNonZeroDigitFound = TRUE;
			m_uFirstNonZeroIntDigitPos = uIntDigitsReturned;
		}

		// We track all 9's for the special case when we round all 9s up
		// and create an extra digit
		if (!bIntPartisNotAll9s && (uTheDigit != 9U)) {
			bIntPartisNotAll9s = TRUE;
		}
	}
	m_uLastNonZeroIntDigitPos = uIntDigitsReturned;

	// Now process all the fractional digits, from most significant to
	// least (left to right) Get a count of the number of digits, and
	// the position of the first non-zero digit (right from the decimal
	// point) and the last non-zero digit

	m_bHasFractionalPart = FractionalPart.is_not_zero();
	m_uFirstNonZeroFracDigitPos = 0;

	uint32_t uFracDigitsReturned = 0;
	uint_t bFracPartIsNotAll9s = FALSE;
	bNonZeroDigitFound = FALSE;

	// Loop to extract fractional digits

	while (FractionalPart.is_not_zero() &&
		(uFracDigitsReturned < uPrecisionDigits)) {
		const uint32_t uTheDigit = FractionalPart.multiply_return_overflow(10U);
		++uFracDigitsReturned;

		// Found the first non zero digit.
		if (uTheDigit && !bNonZeroDigitFound) {
			m_uFirstNonZeroFracDigitPos = uFracDigitsReturned;
			bNonZeroDigitFound = TRUE;
		}

		// Track all 9's for the special case when we round all 9s up
		// and create an extra digit
		if (!bFracPartIsNotAll9s && (uTheDigit != 9U)) {
			bFracPartIsNotAll9s = TRUE;
		}
	}

	// Last digit could actually be 0
	m_uLastNonZeroFracDigitPos = uFracDigitsReturned;

	// Check for rounding up the fraction.
	if (FractionalPart.is_not_zero()) {
		// Get next digit...
		const uint32_t uTheDigit = FractionalPart.multiply_return_overflow(10U);

		// do we round up or truncate?
		if (uTheDigit >= 5U) {
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
