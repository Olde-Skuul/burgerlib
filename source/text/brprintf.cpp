/***************************************

	Matt Pritchard's typesafe & enhanced c++ formatted output function replacements

	Note from Matt Pritchard: mprintf is dedicated to the ones I love: Jennifer, Nathan and Lauren.

	Copyright (c) 2012-2013 by Matt Pritchard <mattp_2006@yahoo.com>

	Merged into Burgerlib 2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/


// ================================================================================================
// *************************************** mprintf's license **************************************
// ================================================================================================
//
// mprintf uses the (very permissive) open source ZLIB license:
//
// http://opensource.org/licenses/Zlib
// 
// License text for mprintf.h:
// 
// Copyright (c) 2012-2013 Matt Pritchard
// 
// This software is provided 'as-is', without any express or implied warranty. In no event will the 
//   author(s) be held liable for any damages arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose, including commercial 
//   applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 
//  1. The origin of this software must not be misrepresented; you must not claim that you wrote the 
//     original software. If you use this software in a product, an acknowledgment in the product
//     documentation would be appreciated but is not required.
// 
//  2. Altered source versions must be plainly marked as such, and must not be misrepresented as 
//     being the original software.
// 
//  3. This notice may not be removed or altered from any source distribution.
// 

#include "brprintf.h"
#include "brfpinfo.h"
#include "brnumberto.h"
#include "brstring.h"
#include "brfixedpoint.h"
#include "brfloatingpoint.h"
#include "brassert.h"
#include "brutf8.h"
#include "brmemoryfunctions.h"

/*! ************************************

	\class Burger::GiantWord
	\brief Class for managing a very large integer

	This class is useful in parsing high precision numbers
	and can help in decoding floating point numbers for
	eventual conversion into a string.

	It can perform modulo and multiplication in extremely
	high bit count integers

***************************************/

/*! ************************************

	\brief Default constructor

***************************************/

Burger::GiantWord::GiantWord(void) :
	m_uEntryCount(0),
	m_iHighNonZeroElement(-1),
	m_iLowNonZeroElement(cMaxElements),
	m_bIsZero(TRUE)
{
}

/*! ************************************

	\brief Constructor that sets to a default

	\param uBitsNeeded Number of bits of precision
	\sa Init(Word)

***************************************/

Burger::GiantWord::GiantWord(Word uBitsNeeded)
{
	Init(uBitsNeeded);
}


/*! ************************************

	\fn Word Burger::GiantWord::IsZero(void) const
	\brief Returns \ref TRUE if the value is zero

	If the giant integer's value is zero, return \ref TRUE

	\return \ref TRUE if zero
	\sa IsNotZero(void) const

***************************************/

/*! ************************************

	\fn Word Burger::GiantWord::IsNotZero(void) const
	\brief Returns \ref TRUE if the value is not zero

	If the giant integer's value is not zero, return \ref TRUE

	\return \ref TRUE if not zero
	\sa IsZero(void) const

***************************************/


/*! ************************************

	\brief Initialize the giant integer

	Reset the giant integer to defaults assuming a bit size

	The maximum input value is \ref cTotalBitsInTable -1

	\param uBitsNeeded Number of bits of precision 1 through ( \ref cTotalBitsInTable -1)
	\sa GiantWord(Word)

***************************************/

void BURGER_API Burger::GiantWord::Init(Word uBitsNeeded)
{
	// Test for error
	BURGER_ASSERT(uBitsNeeded && (uBitsNeeded <= cTotalBitsInTable) );
	
	// Round up to the next Word32
	Word uCount = (uBitsNeeded + (cBitsPerElement - 1)) / cBitsPerElement;
	m_uEntryCount = uCount;

	// Zero out all elements we'll use
	if (uCount) {
		Word i = 0;
		do {
			m_uDataTable[i] = 0;	// Zap!
		} while (++i<uCount);
	}

	// Init the default found range
	m_iHighNonZeroElement = -1;
	m_iLowNonZeroElement = cMaxElements;
	m_bIsZero = TRUE;
}

/*! ************************************

	\brief Insert bits into the buffer

	Insert the bits from value into m_uDataTable, shifted in from the bottom (least
	significant end) by the specified number of bits. A shift of zero or less
	means that none of the bits will be shifted in. A shift of one means that
	the high bit of value will be in the bottom of the last element of m_uDataTable -
	the least significant bit. A shift of cBitsPerElement means that value will be
	in the least significant element of m_uDataTable, and so on.   
	It's possible we'll shift in a value too big

	\param uBits 32 bit data to shift in
	\param uShiftAmount Number of bits to shift (0 to cTotalBitsInTable)
	\sa InsertTopBits(Word32,Word) or InsertBits(Word32,Word)

***************************************/

void BURGER_API Burger::GiantWord::InsertLowBits(Word32 uBits,Word uShiftAmount)
{
	// Sanity check
	if (uBits && (uShiftAmount < ((m_uEntryCount+1) * cBitsPerElement))) {

		Word uShiftForBit = uShiftAmount & (cBitsPerElement - 1);	// Which bit?
		Word uShiftForEntry = uShiftAmount / cBitsPerElement;		// Which data entry

		Word64 uShiftedBits = static_cast<Word64>(uBits) << uShiftForBit;

		// Use an unsigned type so that negative numbers will become large,
		// which makes the range checking below simpler.
		Word uEntryIndex = m_uEntryCount - 1 - uShiftForEntry;

		// Write the results to the data array. If the index is too large
		// then that means that the data was shifted off the edge.

		InsertBits(static_cast<Word32>(uShiftedBits >> cBitsPerElement),uEntryIndex);
		InsertBits(static_cast<Word32>(uShiftedBits),uEntryIndex + 1);
	}
}

/*! ************************************

	\brief Insert bits into the buffer from the top

	Insert the bits from value into m_uDataTable, shifted in from the top (most
	significant end) by the specified number of bits. A shift of zero or less
	means that none of the bits will be shifted in. A shift of one means that
	the low bit of value will be in the top of the first element of m_uDataTable -
	the most significant bit. A shift of cBitsPerElement means that value will be
	in the most significant element of m_uDataTable, and so on.

	\param uBits 32 bit data to shift in
	\param uShiftAmount Number of bits to shift (0 to cTotalBitsInTable)
	\sa InsertLowBits(Word32,Word) or InsertBits(Word32,Word)

***************************************/

void BURGER_API Burger::GiantWord::InsertTopBits(Word32 uBits,Word uShiftAmount)
{
	// Reverse the shift from the top end
	InsertLowBits(uBits,((m_uEntryCount + 1) * cBitsPerElement) - uShiftAmount);
}

/*! ************************************
	
	\brief Do the actual work of inserting bits, and updating zero flag, and extents

	\param uBits 32 bit data to shift in
	\param uEntryIndex Index into the entry table
	\sa InsertLowBits(Word32,Word) or InsertTopBits(Word32,Word)

***************************************/

void BURGER_API Burger::GiantWord::InsertBits(Word32 uBits,Word uEntryIndex)
{
	// Sanity check
	if (uEntryIndex < m_uEntryCount) {

		// Anything to or?
		if (uBits) {

			// Or in the bits
		    m_uDataTable[uEntryIndex] |= uBits;
			m_bIsZero = FALSE;			// Not zero anymore

			// Set the bounds of the range of set bits
			if (static_cast<int>(uEntryIndex) > m_iHighNonZeroElement) {
				m_iHighNonZeroElement = static_cast<int>(uEntryIndex);
			}
			if (static_cast<int>(uEntryIndex) < m_iLowNonZeroElement ) {
				m_iLowNonZeroElement = static_cast<int>(uEntryIndex);
			}
		}
	}
}

/*! ************************************

	\brief Divide the long number and return the remainder.
	
	Given a value to perform a modulo, perform the modulo and return the remainder
	and leave the dividend in the data array

	If uDivisor was 0, this function does nothing and returns zero

	\param uDivisor 32 bit unsigned number to divide the long integer
	\return Value from 0 to uDivisor-1.
	\sa MulReturnOverflow(Word32) 

***************************************/

Word32 Burger::GiantWord::DivideAndReturnRemainder(Word32 uDivisor)
{
	// Preset the result
	Word32 uRemainder = 0;
	if (!m_bIsZero && uDivisor) {

		// Accumulator for zero test
		Word32 uBitsForZeroTest = 0;

		// Standard long-division algorithm.
		Word i = static_cast<Word>(m_iLowNonZeroElement);
		if (i<m_uEntryCount) {
			do {
				Word64 uDividend = (static_cast<Word64>(uRemainder) << cBitsPerElement) + m_uDataTable[i];
				Word64 uDivisionResult = uDividend / uDivisor;
				Word32 uDivisionResult32 = static_cast<Word32>(uDivisionResult);
				uRemainder = static_cast<Word32>(uDividend % uDivisor);
				m_uDataTable[i] = uDivisionResult32;
				// Accumulate bits for test
				uBitsForZeroTest |= uDivisionResult32;
			} while (++i<m_uEntryCount);
		}

		// Update zero status
		m_bIsZero = (!uBitsForZeroTest);

		// Did the first 1 bit shift an element to the right? if so, increment starting point
		m_iLowNonZeroElement += (!m_uDataTable[m_iLowNonZeroElement]);
	}
	return uRemainder;
}

/*! ************************************

	\brief Multiply by div and return the overflow, from 0 to div-1

	Multiply the giant integer with a number and return the number that
	overflowed out.

	\param uMultiplier Value to multiply the giant integer with
	\return Overflow value, or zero if there was no overflow or the giant integer was zero.
	\sa DivideAndReturnRemainder(Word32)

***************************************/

Word32 Burger::GiantWord::MulReturnOverflow(Word32 uMultiplier)
{
	// Assume zero for answer
	Word32 uOverflow = 0;

	// Only multiply if non-zero
	if (!m_bIsZero) {

		Word32 uBitsForZeroTest = 0;
		int i;
		for (i = m_iHighNonZeroElement; i >= 0;--i) {
			Word64 uMultiplicand = (static_cast<Word64>(uMultiplier) * m_uDataTable[i]) + uOverflow;

			// Get the overflow from 32 to 64 bits
			uOverflow = static_cast<Word32>(uMultiplicand >> cBitsPerElement);
			
			// Put the bottom bits of the results back.
			m_uDataTable[i] = static_cast<Word32>(uMultiplicand);
			uBitsForZeroTest |= static_cast<Word32>(uMultiplicand);
		}

		// Update zero status
		m_bIsZero = (!uBitsForZeroTest);

		// Did the last 1 bit shift an element to the left? if so, decrement the starting point
		m_iHighNonZeroElement -= (!m_uDataTable[m_iHighNonZeroElement]);
	}
	return uOverflow;
}






/*! ************************************

	\brief Separate integer and fractional values from floating point number.

	Given an \ref FPInfo class instance, extract the integer and the fractional components
	of a floating point number and store the values into separate \ref GiantWord
	classes.

	\param pFPInfo Pointer to the input \ref FPInfo record
	\param pIntegerPart Pointer to the output integer record
	\param pFractionalPart Pointer to the output fractional record

***************************************/

void BURGER_API Burger::LoadHighPrecisionFromFloat(const FPInfo *pFPInfo,GiantWord *pIntegerPart,GiantWord *pFractionalPart)
{
	// Better be valid!
	BURGER_ASSERT(pFPInfo->IsValid());

	// Assuming the normal pattern of exponent bias, for n bits of exponent, 
	// the range of exponents is:  max exponent = + 2^( n-1 ) -1 
	// min exponent = - ( 2^( n-1 ) - 2 )

	// Need 1 + max exponent bits to hold highest value (implied mantissa 1.0 bit << max_exponent )
	Word uNumberIntBitsNeeded = 1U << (pFPInfo->GetExponentBitCount()-1);
	pIntegerPart->Init(uNumberIntBitsNeeded);

	// need min exponent bits plus all the binary fraction bits from the mantissa
	Word uNumberFracBitsNeeded = ( 1U << (pFPInfo->GetExponentBitCount()-1) ) - 2 + (pFPInfo->GetMantissaBitCount() - 1);
	pFractionalPart->Init(uNumberFracBitsNeeded);

	if (!pFPInfo->IsZero()) {
		Word32 uLow32Bits = static_cast<Word32>(pFPInfo->GetMantissa());
		pIntegerPart->InsertLowBits(uLow32Bits,(32+1) - pFPInfo->GetMantissaBitCount() + pFPInfo->GetExponent() );
		pFractionalPart->InsertTopBits(uLow32Bits,(pFPInfo->GetMantissaBitCount()-1) - pFPInfo->GetExponent() );

		if ( pFPInfo->GetMantissaBitCount() > 32 ) {
			Word32 uHigh32Bits = static_cast<Word32>(pFPInfo->GetMantissa() >> 32U);
			pIntegerPart->InsertLowBits(uHigh32Bits,(64+1) - pFPInfo->GetMantissaBitCount() + pFPInfo->GetExponent() );
			pFractionalPart->InsertTopBits(uHigh32Bits,(pFPInfo->GetMantissaBitCount()-1) - pFPInfo->GetExponent() - 32 );
		}
	}
}






/*! ************************************

	\class Burger::FPPrintInfo
	\brief Digit printing class for floating point

	Analyze the floating point number. Determine if it is a 'special' form
	and where the digits start and end on each side of the decimal point.

	We also support optional significant digit cutoff, in order to emulate RTLs
	that quit after a set number of digits (looking at you Microsoft)

***************************************/

const char *Burger::FPPrintInfo::g_SpecialFloatsText[] = { "inf", "-inf", "nan", "-nan", "-nan" };
const char *Burger::FPPrintInfo::g_SpecialFloatsTextUpper[] = { "INF", "-INF", "NAN", "-NAN", "-NAN" };

/*! ************************************

	For compatibility with some compiler RTLs, we can stop writing the digits
	of a floating point number after a specified number of digits.  Set this
	to 0 to print ALL digits allowed by the conversion specification

***************************************/

Word Burger::FPPrintInfo::g_uClipFloatDigitsAfter = cConversionDigitUnlimited;


/*! ************************************

	\fn Burger::FPPrintInfo::FPPrintInfo()
	\brief Default constructor

	Clear out the variables and set the data to zero

***************************************/


/*! ************************************

	\brief Reset the class

	Clear out the variables and set the data to zero

***************************************/

void BURGER_API Burger::FPPrintInfo::Reset(void)
{
	m_bHasIntegerPart = FALSE;
	m_bHasFractionalPart = FALSE;
	m_uSpecialFormDigits = 0;
	m_uFirstNonZeroIntDigitPos = 0;
	m_uLastNonZeroIntDigitPos = 0;
	m_uFirstNonZeroFracDigitPos = 0;
	m_uLastNonZeroFracDigitPos = 0;
	m_uPadIntZeros = 0;
	m_uPadFracZeros = 0;
	m_ePrintResult = cResultZero;
}


/*! ************************************

	\fn Word Burger::FPPrintInfo::HasInteger(void) const
	\brief Does the string have an integer part?

	\return \ref TRUE if there are integer digits

***************************************/

/*! ************************************

	\fn Word Burger::FPPrintInfo::HasFractional(void) const
	\brief Does the string have a fractional part?

	\return \ref TRUE if there are fractional digits

***************************************/

/*! ************************************

	\fn Word Burger::FPPrintInfo::GetSpecialFormDigits(void) const
	\brief Get the number of special form digits

	\return The number of special form digits
	
***************************************/

/*! ************************************

	\fn Word Burger::FPPrintInfo::GetFirstNonZeroIntDigitPosition(void) const
	\brief Get the index to the first integer digit

	\return The index to the first integer digit
	
***************************************/

/*! ************************************

	\fn Word Burger::FPPrintInfo::GetLastNonZeroIntDigitPosition(void) const
	\brief Get the index to the last integer digit

	\return The index to the last integer digit
	
***************************************/



/*! ************************************

	\fn Word Burger::FPPrintInfo::GetFirstNonZeroFracDigitPosition(void) const
	\brief Get the index to the first fractional digit

	\return The index to the first fractional digit
	
***************************************/

/*! ************************************

	\fn Word Burger::FPPrintInfo::GetLastNonZeroFracDigitPosition(void) const
	\brief Get the index to the last fractional digit

	\return The index to the last fractional digit
	
***************************************/


/*! ************************************

	\fn Word Burger::FPPrintInfo::GetIntegerZeroCount(void) const
	\brief Get the number of zero digits in the integer field

	\return The number of zero digits in the integer field
	
***************************************/

/*! ************************************

	\fn Word Burger::FPPrintInfo::GetFracZeroCount(void) const
	\brief Get the number of zero digits in the fractional field

	\return The number of zero digits in the fractional field
	
***************************************/

/*! ************************************

	\fn eResult Burger::FPPrintInfo::GetResult(void) const
	\brief Get the type of number to print.

	\return The type of number that should be printed.
	
***************************************/


/*! ************************************

	\fn Word Burger::FPPrintInfo::IsSpecial(void) const
	\brief Is the number a special?

	\return \ref TRUE if the number is NaN/INF
	
***************************************/

/*! ************************************

	\fn Word Burger::FPPrintInfo::IsInfinity(void) const
	\brief Is the number infinity?

	\return \ref TRUE if the number is INF
	
***************************************/

/*! ************************************

	\fn Word Burger::FPPrintInfo::IsNaN(void) const
	\brief Is the number Not a Number?

	\return \ref TRUE if the number is NaN
	
***************************************/


/*! ************************************

	\fn Word Burger::FPPrintInfo::GetClip(void)
	\brief Get the maximum digits to print

	\return Maximum number of digits to print, 0 means no maximum.
	
***************************************/

/*! ************************************

	\fn void Burger::FPPrintInfo::SetClip(Word uNewClip)
	\brief Set the maximum digits to print

	\param uNewClip Maximum number of digits to print, 0 means no maximum.
	
***************************************/


/*! ************************************

	\brief Analyze a floating point number

	Analyze the floating point number. Determine if it is a 'special' form
	and where the digits start and end on each side of the decimal point.

	We also support optional significant digit cutoff, in order to emulate RTLs
	that quit after a set number of digits (looking at you Microsoft)

	\param pFPInfo Pointer to the floating point number to analyze
	\param uPrecisionDigits Number of digits of precision to print

***************************************/

void BURGER_API Burger::FPPrintInfo::AnalyzeFloat(const FPInfo *pFPInfo,Word uPrecisionDigits)
{
	// Reset the state
	Reset();

	// Special Case: Zero
	if (pFPInfo->IsZero()) {
		//m_ePrintResult = Zero;
		m_bHasIntegerPart = TRUE;
		m_uFirstNonZeroIntDigitPos = 1;
		m_uLastNonZeroIntDigitPos = 1;

	// Special Case: Infinity
	} else if (pFPInfo->IsInfinity()) {
		m_ePrintResult = (pFPInfo->IsNegative()) ? cResultNegativeInfinity : cResultPositiveInfinity;
		m_uSpecialFormDigits = static_cast<Word>(StringLength(g_SpecialFloatsText[(pFPInfo->IsNegative()) ? cNegInfinity : cPosInfinity]));

		// Special Case: NAN
	} else if (pFPInfo->IsNAN()) {
		m_ePrintResult = ( pFPInfo->IsNegative() ) ? cResultNegativeNaN : cResultPositiveNaN;
		m_uSpecialFormDigits = static_cast<Word>(StringLength(g_SpecialFloatsText[(pFPInfo->IsNegative()) ? cNegNan : cPosNan]));		
	} else {

		// How many bits do we need to express this number?
		Word uBitsNeeded = (1U << (pFPInfo->GetExponentBitCount()-1)) + pFPInfo->GetMantissaBitCount();

		GiantWord IntegerPart(uBitsNeeded);
		GiantWord FractionalPart(uBitsNeeded);

		// Initialize our high precision numbers and result status
		LoadHighPrecisionFromFloat(pFPInfo,&IntegerPart,&FractionalPart);

		// All numbers have at least an integer part if not zero

		m_ePrintResult = cResultNormalFloat;
		m_bHasIntegerPart = IntegerPart.IsNotZero();
		//m_uFirstNonZeroIntDigitPos = 0;

		// Now process all the integer digits, from least significant to most (right to left)
		// loop at least once, to get a leading zero.  Get a count of the number of digits, 
		// and the position of the first non-zero digit (from the decimal point)

		Word bNonZeroDigitFound = FALSE;
		Word bIntPartisNotAll9s = FALSE;
		Word uIntDigitsReturned = 0;

		Word bRTLCutOffEmulation = (g_uClipFloatDigitsAfter!=0);
		Word uNineCount = 0;
		int	iLastNonNineDigit = -1;

		// Get integer digits from the integer part

		while (IntegerPart.IsNotZero()) {
			Word32 uTheDigit = IntegerPart.DivideAndReturnRemainder(10);
			++uIntDigitsReturned;

			if (uTheDigit && !bNonZeroDigitFound) {
				bNonZeroDigitFound = TRUE;
				m_uFirstNonZeroIntDigitPos = uIntDigitsReturned;
			}

			// We track all 9's for the special case when we round all 9s up and create an extra digit
			if (!bIntPartisNotAll9s && (uTheDigit != 9)) {
				bIntPartisNotAll9s = TRUE;
			}

			// for RTL cutoff emulation, we track the last run of 9s and the digit prior to it.
			if (uTheDigit != 9) {
				uNineCount = 0;
				iLastNonNineDigit = static_cast<int>(uTheDigit);
			} else {
				++uNineCount;
			}
		} 
		m_uLastNonZeroIntDigitPos = uIntDigitsReturned;

		// RTL significant digit cut-off emulation.
		if (bRTLCutOffEmulation && (uIntDigitsReturned >= g_uClipFloatDigitsAfter)) {
			
			// we're going to just put zeros in any fractional portion
			m_uPadFracZeros = uPrecisionDigits;

			// do we need to look at the fractional portion to get the rounding digit?
			int iRoundingDigit = (uIntDigitsReturned == g_uClipFloatDigitsAfter) ? static_cast<int>(FractionalPart.MulReturnOverflow(10)) : -1;

			// Wacky Edge case:  If we are emulating a RTL that cuts off digits (by just emitting 0s) after 
			// a certain number of digits ( MSVC does this at 17 significant digits, not including any
			// leading zero),   we could conceivably have all 9s up to the cut off point, in which case
			// we would want to check the first zeroed digit, and possibly round up, adding a whole digit
			// to the output width. We have to do this without any dynamic allocations. *sigh*

			if (uNineCount >= g_uClipFloatDigitsAfter) {
				// should we round it up?
				if ((uNineCount > g_uClipFloatDigitsAfter) || (iLastNonNineDigit >= 5) || (iRoundingDigit >= 5)) {
					// add a leading digit to the int part, it's '1' plus some number of zeros now
					++m_uLastNonZeroIntDigitPos;
					m_uFirstNonZeroIntDigitPos = m_uLastNonZeroIntDigitPos;
					m_ePrintResult = cResultFloatRoundedUpADigit;

					// Return it with no fractional part (it's rounded out)
					return;
				}
			}

			// it wont expand via rounding, but we do need to know if we need to round up
			// some number of digits at the end.   And we didn't save all the digits, so...

			// do we need to re-extract the digits?
			if (uIntDigitsReturned > g_uClipFloatDigitsAfter) {
				// Reload the integer portion
				LoadHighPrecisionFromFloat(pFPInfo,&IntegerPart,&FractionalPart);
				Word uDigitNeeded = uIntDigitsReturned - g_uClipFloatDigitsAfter;

				// loop until we get the desired digit
				// note: we could optimize this for high digit positions bu diving against
				// 	  larger power of 10	
				while (uDigitNeeded) {
					iRoundingDigit = static_cast<int>(IntegerPart.DivideAndReturnRemainder(10));
					--uDigitNeeded;
				}
			}

			// mark up the results for the valid digit range, and add the fact that we have
			// padding digits
			m_uFirstNonZeroIntDigitPos = uIntDigitsReturned - g_uClipFloatDigitsAfter + 1;
			m_uPadIntZeros = m_uFirstNonZeroIntDigitPos - 1;

			// will we need to round up the result?
			if (iRoundingDigit >= 5) {
				m_ePrintResult = cResultFloatRoundedUpAtEnd;
			}
			return;
		}


		// Now process all the fractional digits, from most significant to least (left to right)
		// Get a count of the number of digits, and the position of the first non-zero digit (right 
		// from the decimal point) and the last non-zero digit

		m_bHasFractionalPart = FractionalPart.IsNotZero();
		//m_uFirstNonZeroFracDigitPos = 0;

		Word uFracDigitsReturned = 0;
		Word bFracPartIsNotAll9s = FALSE;
		bNonZeroDigitFound = FALSE;

		// needed to support RTL cut off emulation.  We reduce the precision
		// locally if it would result in digits past the cutoff
		if (bRTLCutOffEmulation)	{
			Word uDigitsRemaing = g_uClipFloatDigitsAfter - uIntDigitsReturned;
			if (uPrecisionDigits > uDigitsRemaing) {
				// fake a lesser precision to get correct rounding up and note how many zeros to tack on the end
				m_uPadFracZeros = uPrecisionDigits - uDigitsRemaing;
				uPrecisionDigits = uDigitsRemaing;
			}
		}

		// Loop to extract fractional digits

		while (FractionalPart.IsNotZero() && (uFracDigitsReturned < uPrecisionDigits)) {
			Word32 uTheDigit = FractionalPart.MulReturnOverflow(10);
			++uFracDigitsReturned;

			if (uTheDigit && !bNonZeroDigitFound) {
				m_uFirstNonZeroFracDigitPos = uFracDigitsReturned;
				bNonZeroDigitFound = TRUE;
			}

			// we track all 9's for the special case when we round all 9s up and create an extra digit
			if (!bFracPartIsNotAll9s && (uTheDigit != 9)) {
				bFracPartIsNotAll9s = TRUE;
			}
		}

		m_uLastNonZeroFracDigitPos = uFracDigitsReturned;		// last digit could actually be 0

		// check for rounding up the fraction. 
		if (FractionalPart.IsNotZero()) {
			// get next digit...
			Word32 uTheDigit = FractionalPart.MulReturnOverflow(10);

			// do we round up?
			if (uTheDigit >= 5) {
				// special case: the number is all '9's up to this point!
				if (!bFracPartIsNotAll9s && !bIntPartisNotAll9s) {
					// add a digit to the int part, it's '1' plus some number of zeros now
					++m_uLastNonZeroIntDigitPos;
					m_uFirstNonZeroIntDigitPos = m_uLastNonZeroIntDigitPos;
					m_ePrintResult = cResultFloatRoundedUpADigit;
				} else {
					m_ePrintResult = cResultFloatRoundedUpAtEnd;
				}
			}
		}
	}
}






/*! ************************************

	\class Burger::SafePrintArgument
	\brief Container class for passing data to SafePrint

	Instead of relying on ... for sending parameters to the
	print function, the function accepts only instances of this
	class which identify and encapsulate data for printing.

***************************************/


/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(signed int arg)
	\brief Constructor for a signed integer

	\param arg Signed integer for storage

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(unsigned int arg)
	\brief Constructor for an unsigned integer

	\param arg Unsigned integer for storage

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(char arg)
	\brief Constructor for a single char

	\param arg Char for storage

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(int8_t arg)
	\brief Constructor for a signed char

	\param arg Signed char for storage

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(uint8_t arg)
	\brief Constructor for an unsigned char

	\param arg Unsigned char for storage

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(int16_t arg)
	\brief Constructor for a signed short

	\param arg Signed short for storage

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(uint16_t arg)
	\brief Constructor for an unsigned short

	\param arg Unsigned short for storage

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(wchar_t arg)
	\brief Constructor for a wchar_t

	For compilers that support a wchar_t as a native type, this remaps the data to
	an unsigned short.

	\param arg wchar_t for storage

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(signed long arg)
	\brief Constructor for a signed long

	\param arg Signed long for storage

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(unsigned long arg)
	\brief Constructor for an unsigned long

	\param arg Unsigned long for storage

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(int64_t arg)
	\brief Constructor for a signed 64 bit integer

	\param arg Int64 for storage

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(uint64_t arg)
	\brief Constructor for an unsigned 64 bit integer

	\param arg Word64 for storage

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(uint16_t arg,eType uType)
	\brief Constructor for a 16 bit float

	Since 16 bit floats are not native to C++, it has to be manually cast by
	creating an argument manually. Example is below.

	\code
	SafePrintf("Float %hf\n",SafePrintArgument(fHalf,SafePrintArgument::ARG_HALF));
	\endcode

	\param arg Word16 representation of a 16 bit float
	\param uType Type override for the 16 bit short

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(float arg)
	\brief Constructor for a 32 bit floating point number

	\param arg Float for storage

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(double arg)
	\brief Constructor for a 64 bit floating point number

	\param arg Double for storage

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(bool arg)
	\brief Constructor for a boolean

	\param arg Boolean for storage

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const String *arg)
	\brief Constructor for a pointer to a Burgerlib \ref String

	\param arg Pointer to a Burgerlib \ref String

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const String &arg)
	\brief Constructor for a reference to a Burgerlib \ref String

	\param arg Reference to a Burgerlib \ref String

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const char *arg)
	\brief Constructor for a pointer to a "C" string

	\param arg Pointer to a "C" string

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const int8_t *arg)
	\brief Constructor for a pointer to a Int8 string which is treated as a "C" string

	\param arg Pointer to a "C" string stored as Int8

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const uint8_t *arg)
	\brief Constructor for a pointer to a Word8 string which is treated as a "C" string

	\param arg Pointer to a "C" string stored as Word8

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const int16_t *arg)
	\brief Constructor for a pointer to a short

	\param arg Pointer to a short

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const uint16_t *arg)
	\brief Constructor for a pointer to a unsigned short

	\param arg Pointer to a unsigned short

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const wchar_t *arg)
	\brief Constructor for a pointer to a wchar_t "C" string

	\param arg Pointer to a wchar_t "C" string

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const signed int *arg)
	\brief Constructor for a pointer to a int

	\param arg Pointer to a int

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const unsigned int *arg)
	\brief Constructor for a pointer to a unsigned int

	\param arg Pointer to a unsigned int

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const signed long *arg)
	\brief Constructor for a pointer to a long

	\param arg Pointer to a long

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const unsigned long *arg)
	\brief Constructor for a pointer to a unsigned long

	\param arg Pointer to a unsigned long

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const int64_t *arg)
	\brief Constructor for a pointer to an Int64

	\param arg Pointer to an Int64

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const uint64_t *arg)
	\brief Constructor for a pointer to a Word64

	\param arg Pointer to a Word64

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const float *arg)
	\brief Constructor for a pointer to a float

	\param arg Pointer to a float

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const double *arg)
	\brief Constructor for a pointer to a double

	\param arg Pointer to a double

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const __m64 arg)
	\brief Constructor for an MMX 64 bit value

	\param arg A __m64 MMX 64 bit value

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const __m64 *arg)
	\brief Constructor for a pointer to a MMX 64 bit value

	\param arg Pointer to a __m64 MMX 64 bit value

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const __m128 arg)
	\brief Constructor for an SSE 128 bit value (4 floats)

	\param arg A __m128 SSE 128 bit value

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const __m128 *arg)
	\brief Constructor for a pointer to a SSE 128 bit value (4 floats)

	\param arg Pointer to a __m128 SSE 128 bit value

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const __m128d arg)
	\brief Constructor for an SSE 128 bit value (2 doubles)

	\param arg A __m128d SSE 128 bit value

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const __m128d *arg)
	\brief Constructor for a pointer to a SSE 128 bit value (2 doubles

	\param arg Pointer to a __m128d SSE 128 bit value

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const __m128i arg)
	\brief Constructor for an SSE 128 bit value (4 integers)

	\param arg A __m128i SSE 128 bit value

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const __m128i *arg)
	\brief Constructor for a pointer to a SSE 128 bit value (4 integers)

	\param arg Pointer to a __m128 SSE 128 bit value

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const void *arg)
	\brief Constructor for a pointer 

	\param arg Pointer to data that wasn't already captured by a specific constructor

***************************************/

/*! ************************************

	\fn Burger::SafePrintArgument::SafePrintArgument(const SafePrintArgument &arg)
	\brief Copy constructor

	\param arg Reference to a \ref SafePrintArgument to copy

***************************************/




/*! ************************************

	\fn Word Burger::SafePrintArgument::IsNumeric(void) const
	\brief Test if the data is a numeric value

	If the value is a number, return \ref TRUE. This includes
	both floating point and integers.

	\return \ref TRUE if the value contained is a number.

***************************************/

/*! ************************************

	\fn Word Burger::SafePrintArgument::IsInteger(void) const
	\brief Test if the data is an integer numeric value

	If the value is a number, return \ref TRUE. It will
	return \ref FALSE if it's floating point.

	\return \ref TRUE if the value contained is an integer.

***************************************/

/*! ************************************

	\fn Word Burger::SafePrintArgument::IsSigned(void) const
	\brief Test if the data is a signed numeric value

	If the value is a signed number, return \ref TRUE. It will
	return \ref FALSE if it's unsigned.

	\return \ref TRUE if the value contained supported signed values.

***************************************/

/*! ************************************

	\fn Word Burger::SafePrintArgument::IsUnsigned(void) const
	\brief Test if the data is an unsigned numeric value

	If the value is an unsigned number, return \ref TRUE. It will
	return \ref FALSE if it's signed or floating point.

	\return \ref TRUE if the value contained is an unsigned data type.

***************************************/

/*! ************************************

	\fn Word Burger::SafePrintArgument::IsBoolean(void) const
	\brief Test if the data is a boolean

	If the value is a boolean, return \ref TRUE. It will
	return \ref FALSE if it's anything else.

	\return \ref TRUE if the value contained is a boolean.

***************************************/

/*! ************************************

	\fn Word Burger::SafePrintArgument::IsReal(void) const
	\brief Test if the data is a floating point value

	If the value is floating pointer, return \ref TRUE. It will
	return \ref FALSE if it's anything else.

	\return \ref TRUE if the value contained is a floating point value.

***************************************/



/*! ************************************

	\brief Returns \ref TRUE if the stored value is negative

	If the value is negative, return \ref TRUE, if the
	data type has no concept of negative, it will always
	return \ref FALSE

	\return \ref TRUE if negative

***************************************/

Word BURGER_API Burger::SafePrintArgument::IsNegative(void) const
{
	Word bResult;
	switch (m_eType) {
	case ARG_INT8:
		bResult = (m_Data.m_iInt8 < 0);
		break;
	case ARG_INT16:
		bResult = (m_Data.m_iInt16 < 0);
		break;
	case ARG_INT32:
		bResult = (m_Data.m_iInt32 < 0);
		break;
	case ARG_INT64:
		bResult = (m_Data.m_iInt64 < 0);
		break;
	case ARG_HALF:
		// Handle 16 bit floats in a cross platform way
		bResult = (m_Data.m_fHalf&0x8000U)!=0;
		break;
	case ARG_FLOAT:
		bResult = (m_Data.m_fFloat < 0.0f);
		break;
	case ARG_DOUBLE:
		bResult = (m_Data.m_dDouble < 0.0);
		break;
	default:
		// TILT!
		bResult = FALSE;
		break;
	}
	return bResult;
}

/*! ************************************

	\brief Returns \ref TRUE if the stored value is zero

	If the value is zero, return \ref TRUE, if the
	data type has no concept of zero, it will always
	return \ref FALSE

	\return \ref TRUE if zero

***************************************/

Word BURGER_API Burger::SafePrintArgument::IsZero(void) const
{
	Word bResult;
	switch (m_eType) {
	case ARG_INT8:
		bResult = (!m_Data.m_iInt8);
		break;
	case ARG_INT16:
		bResult = (!m_Data.m_iInt16);
		break;
	case ARG_INT32: 
		bResult = (!m_Data.m_iInt32);
		break;
	case ARG_INT64:
		bResult = (!m_Data.m_iInt64);
		break;
	case ARG_WORD8:
		bResult = (!m_Data.m_uWord8);
		break;
	case ARG_WORD16:
		bResult = (!m_Data.m_uWord16);
		break;
	case ARG_WORD32:
		bResult = (!m_Data.m_uWord32);
		break;
	case ARG_WORD64:
		bResult = (!m_Data.m_uWord64);
		break;
	case ARG_HALF:
		// Allow +0 and -0 to be zero
		bResult = !(m_Data.m_fHalf&0x7FFFU);
		break;
	case ARG_FLOAT:
		bResult = (m_Data.m_fFloat == 0.0f);
		break;
	case ARG_DOUBLE:
		bResult = (m_Data.m_dDouble == 0.0);
		break;
	case ARG_BOOL:
		bResult = (!m_Data.m_bBool);
		break;
	default:
		// TILT!
		bResult = FALSE;
		break;
	}
	return bResult;
}

/*! ************************************

	\fn Word Burger::SafePrintArgument::IsCharacter(void) const
	\brief Test if the data is a character type

	If the value is single character, return \ref TRUE. It will
	return \ref FALSE if it's anything else.

	\return \ref TRUE if the value contained is a character value.

***************************************/

/*! ************************************

	\fn Word Burger::SafePrintArgument::IsTextPointer(void) const
	\brief Test if the data is a pointer to a "C" string

	If the value is a pointer to a "C" string, return \ref TRUE. It will
	return \ref FALSE if it's anything else.

	\return \ref TRUE if the value contained is a pointer to a "C" string.

***************************************/

/*! ************************************

	\fn Word Burger::SafePrintArgument::IsPointer(void) const
	\brief Test if the data is a pointer

	If the value is a pointer, return \ref TRUE. It will
	return \ref FALSE if it's anything else.

	\return \ref TRUE if the value contained is a pointer.

***************************************/

/*! ************************************

	\fn Word Burger::SafePrintArgument::Is64Bit(void) const
	\brief Test if the data is a 64 bits wide

	If the value is 64 bits wide, return \ref TRUE. It will
	return \ref FALSE if it's anything else.

	\return \ref TRUE if the value contained is 64 bits wide.

***************************************/

/*! ************************************

	\fn Word Burger::SafePrintArgument::IsSIMD(void) const
	\brief Test if the data is a vector value

	If the value is a vector, return \ref TRUE. It will
	return \ref FALSE if it's anything else.

	\return \ref TRUE if the value contained is a vector.

***************************************/

/*! ************************************

	\fn Word Burger::SafePrintArgument::IsSIMDPointer(void) const
	\brief Test if the data is a pointer to a vector value

	If the value is a pointer to a vector, return \ref TRUE. It will
	return \ref FALSE if it's anything else.

	\return \ref TRUE if the value contained is a pointer to a vector.

***************************************/


/*! ************************************

	\brief Returns the size of the stored data in bytes

	\return Number of bytes the data occupies, zero if undefined

***************************************/

WordPtr BURGER_API Burger::SafePrintArgument::GetDataLengthInBytes(void) const
{
	WordPtr uResult;
	switch (m_eType) {
	case ARG_INT8:
	case ARG_WORD8:
		uResult = 1;
		break;

	case ARG_INT16:
	case ARG_WORD16:
	case ARG_HALF:
		uResult = 2;
		break;

	case ARG_INT32:
	case ARG_WORD32:
	case ARG_FLOAT:
		uResult = 4;
		break;

	case ARG_INT64:
	case ARG_WORD64:
	case ARG_DOUBLE:
	case ARG_SIMD64:
		uResult = 8;
		break;

	case ARG_BOOL:
		// Varies with each compiler
		uResult = sizeof(bool);
		break;
	case ARG_PSTRING:
	case ARG_PCHAR:
	case ARG_PSCHAR:
	case ARG_PUCHAR:
	case ARG_PINT16:
	case ARG_PWORD16:
	case ARG_PINT32:
	case ARG_PWORD32:
	case ARG_PINT64:
	case ARG_PWORD64:
	case ARG_PFLOAT:
	case ARG_PDOUBLE:
	case ARG_PSIMD64:
	case ARG_PSIMD128:
	case ARG_PSIMD128D:
	case ARG_PSIMD128I:
	case ARG_PVOID:
		// Can vary with each compiler
		uResult = sizeof(void *);
		break;

	case ARG_SIMD128:
	case ARG_SIMD128D:
	case ARG_SIMD128I:
		uResult = 16;
		break;

	default:
		BURGER_ASSERT(FALSE);
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Returns the pointer the stored data

	\return Number of bytes the data occupies, zero if undefined

***************************************/

const void* BURGER_API Burger::SafePrintArgument::GetDataAddress( void ) const
{
	// Due to the fact the some compilers can move data around in
	// a union, this function is needed instead of returning
	// the base pointer of m_Data

	const void *pResult;
	switch (m_eType) {
	case ARG_INT8:
		pResult = &m_Data.m_iInt8;
		break;
	case ARG_INT16:
		pResult = &m_Data.m_iInt16;
		break;
	case ARG_INT32:
		pResult = &m_Data.m_iInt32;
		break;
	case ARG_INT64:
		pResult = &m_Data.m_iInt64;
		break;
	case ARG_WORD8:
		pResult = &m_Data.m_uWord8;
		break;
	case ARG_WORD16:
		pResult = &m_Data.m_uWord16;
		break;
	case ARG_WORD32:
		pResult = &m_Data.m_uWord32;
		break;
	case ARG_WORD64:
		pResult = &m_Data.m_uWord64;
		break;
	case ARG_HALF:
		pResult = &m_Data.m_fHalf;
		break;
	case ARG_FLOAT:
		pResult = &m_Data.m_fFloat;
		break;
	case ARG_DOUBLE:
		pResult = &m_Data.m_dDouble;
		break;
	case ARG_BOOL:
		pResult = &m_Data.m_bBool;
		break;
	case ARG_PCHAR:
		pResult = &m_Data.m_pChar;
		break;
	case ARG_PSCHAR:
		pResult = &m_Data.m_pInt8;
		break;
	case ARG_PUCHAR:
		pResult = &m_Data.m_pWord8;
		break;
	case ARG_PINT16:
		pResult = &m_Data.m_pInt16;
		break;
	case ARG_PWORD16:
		pResult = &m_Data.m_pWord16;
		break;
	case ARG_PINT32:
		pResult = &m_Data.m_pInt32;
		break;
	case ARG_PWORD32:
		pResult = &m_Data.m_pWord32;
		break;
	case ARG_PINT64:
		pResult = &m_Data.m_pInt64;
		break;
	case ARG_PWORD64:
		pResult = &m_Data.m_pWord64;
		break;
	case ARG_PFLOAT:
		pResult = &m_Data.m_pFloat;
		break;
	case ARG_PDOUBLE:
		pResult = &m_Data.m_pDouble;
		break;
	case ARG_PVOID:
		pResult = &m_Data.m_pVoid;
		break;
	case ARG_SIMD64:
		pResult = m_Data.m_pSIMD;		// SIMD types are only pointed to, not stored
		break;
	case ARG_SIMD128:
		pResult = m_Data.m_pSIMD;
		break;
	case ARG_SIMD128D:
		pResult = m_Data.m_pSIMD;
		break;
	case ARG_SIMD128I:
		pResult = m_Data.m_pSIMD;
		break;
	case ARG_PSIMD64:
		pResult = &m_Data.m_pVoid;
		break;
	case ARG_PSIMD128:
		pResult = &m_Data.m_pVoid;
		break;
	case ARG_PSIMD128D:
		pResult = &m_Data.m_pVoid;
		break;
	case ARG_PSIMD128I:
		pResult = &m_Data.m_pVoid;
		break;
	default:
		BURGER_ASSERT(FALSE);
		pResult = &m_Data;
		break;
	}
	return pResult;
}

/*! ************************************

	\fn eType Burger::SafePrintArgument::GetType(void) const
	\brief Return the type of data contained in the container.

	\return \ref eType of the data contained in the class.

***************************************/

/*! ************************************

	\fn void Burger::SafePrintArgument::SetType(eType uType)
	\brief Set the type of data contained in the container.

	\param uType \ref eType of the new data contained in the class.

***************************************/


/*! ************************************

	\brief Returns the name of the data type.

	Return a "C" string containing a user readable name of
	the data type. 

	Example: \ref Int8 data, with an enumeration of \ref ARG_INT8 returns "Int8"

	\param uType Enumeration of the data type to check
	\return Pointer to a "C" string with the name.

***************************************/

const char* BURGER_API Burger::SafePrintArgument::GetTypeName(eType uType)
{
	const char *pResult;
	switch (uType) {
	case ARG_INT8:
		pResult = "Int8";
		break;
	case ARG_INT16:
		pResult = "Int16";
		break;
	case ARG_INT32:
		pResult = "Int32";
		break;
	case ARG_INT64:
		pResult = "Int64";
		break;
	case ARG_WORD8:
		pResult = "Word8";
		break;
	case ARG_WORD16:
		pResult = "Word16";
		break;
	case ARG_WORD32:
		pResult = "Word32";
		break;
	case ARG_WORD64:
		pResult = "Word64";
		break;
	case ARG_HALF:
		pResult = "half";
		break;
	case ARG_FLOAT:	
		pResult = "float";
		break;
	case ARG_DOUBLE:
		pResult = "double";
		break;
	case ARG_BOOL:
		pResult = "bool";
		break;
	case ARG_PSTRING:
		pResult = "String ";
		break;
	case ARG_PCHAR:
		pResult = "char *";
		break;
	case ARG_PSCHAR:
		pResult = "Int8 *";
		break;
	case ARG_PUCHAR:
		pResult = "Word8 *";
		break;
	case ARG_PINT16:
		pResult = "Int16*";
		break;
	case ARG_PWORD16:
		pResult = "Word16*";
		break;
	case ARG_PINT32:
		pResult = "Int32 *";
		break;
	case ARG_PWORD32:
		pResult = "Word32 *";
		break;
	case ARG_PINT64:
		pResult = "Int64 *";
		break;
	case ARG_PWORD64:
		pResult = "Word64 *";
		break;
	case ARG_PFLOAT:
		pResult = "float *";
		break;
	case ARG_PDOUBLE:
		pResult = "double *";
		break;
	case ARG_PVOID:
		pResult = "void *";
		break;
	case ARG_SIMD64:
		pResult = "__m64";
		break;
	case ARG_SIMD128:
		pResult = "__m128";
		break;
	case ARG_SIMD128D:
		pResult = "__m128d";
		break;
	case ARG_SIMD128I:
		pResult = "__m128i";
		break;
	case ARG_PSIMD64:
		pResult = "__m64 *";
		break;
	case ARG_PSIMD128:
		pResult = "__m128 *";
		break;
	case ARG_PSIMD128D:
		pResult = "__m128d *";
		break;
	case ARG_PSIMD128I:
		pResult = "__m128i *";
		break;
	default:
		BURGER_ASSERT(FALSE);
		pResult = "<unknown>";
		break;
	}
	return pResult;
}

/*! ************************************

	\fn const char* BURGER_API Burger::SafePrintArgument::GetTypeName(void) const
	\brief Returns the name of the data type contained.

	Return a "C" string containing a user readable name of
	the data type contained in this class. 

	Example: \ref Int8 data, with an enumeration of \ref ARG_INT8 returns "Int8"

	\return Pointer to a "C" string with the name.

***************************************/



/*! ************************************

	\brief Returns the value as a signed 32 bit integer

	If the value is a signed integer, convert to 
	a 32 bit signed integer and return the converted value.

	If it's not of this type, return 0.

	\return Signed value or 0 if not compatible.

***************************************/

Int32 BURGER_API Burger::SafePrintArgument::GetInt32(void) const
{
	Int32 iResult;
	switch (m_eType) {
	case ARG_INT8:
		iResult = m_Data.m_iInt8;
		break;
	case ARG_INT16:
		iResult = m_Data.m_iInt16;
		break;
	case ARG_INT32:
		iResult = m_Data.m_iInt32;
		break;
	case ARG_INT64:
		iResult = static_cast<Int32>(m_Data.m_iInt64);
		break;

	case ARG_WORD8:
		iResult = static_cast<Int32>(m_Data.m_uWord8);
		break;
	case ARG_WORD16:
		iResult = static_cast<Int32>(m_Data.m_uWord16);
		break;
	case ARG_WORD32:
		iResult = static_cast<Int32>(m_Data.m_uWord32);
		break;
	case ARG_WORD64:
		iResult = static_cast<Int32>(m_Data.m_uWord64);
		break;

	case ARG_FLOAT:
		iResult = static_cast<Int32>(m_Data.m_fFloat);
		break;
	case ARG_DOUBLE:
		iResult = static_cast<Int32>(m_Data.m_dDouble);
		break;

	default:
		iResult = 0;
		break;
	}
	return iResult;
}

/*! ************************************

	\brief Returns the value as a signed 64 bit integer

	If the value is a signed integer, convert to 
	a 64 bit signed integer and return the converted value.

	If it's not of this type, return 0.

	\return Signed value or 0 if not compatible.

***************************************/

Int64 BURGER_API Burger::SafePrintArgument::GetInt64(void) const
{
	Int64 iResult;
	switch (m_eType) {
	case ARG_INT8:
		iResult = m_Data.m_iInt8;
		break;
	case ARG_INT16:
		iResult = m_Data.m_iInt16;
		break;
	case ARG_INT32:
		iResult = m_Data.m_iInt32;
		break;
	case ARG_INT64:
		iResult = m_Data.m_iInt64;
		break;

	case ARG_WORD8:
		iResult = static_cast<Int64>(m_Data.m_uWord8);
		break;
	case ARG_WORD16:
		iResult = static_cast<Int64>(m_Data.m_uWord16);
		break;
	case ARG_WORD32:
		iResult = static_cast<Int64>(m_Data.m_uWord32);
		break;
	case ARG_WORD64:
		iResult = static_cast<Int64>(m_Data.m_uWord64);
		break;

	case ARG_FLOAT:
		iResult = static_cast<Int64>(m_Data.m_fFloat);
		break;
	case ARG_DOUBLE:
		iResult = static_cast<Int64>(m_Data.m_dDouble);
		break;

	default:
		iResult = 0;
		break;
	}
	return iResult;
}

/*! ************************************

	\brief Returns the value as an unsigned 32 bit integer

	If the value is an unsigned integer, convert to 
	a 32 bit unsigned integer and return the converted value.

	If it's not of this type, return 0.

	\return Unsigned value or 0 if not compatible.

***************************************/

Word32 BURGER_API Burger::SafePrintArgument::GetUInt32(void) const
{
	Word32 uResult;
	switch (m_eType) {
	case ARG_WORD8:
		uResult = m_Data.m_uWord8;
		break;
	case ARG_WORD16:
		uResult = m_Data.m_uWord16;
		break;
	case ARG_WORD32:
		uResult = m_Data.m_uWord32;
		break;
	case ARG_WORD64:
		uResult = static_cast<Word32>(m_Data.m_uWord64);
		break;

	// Return the absolute form of signed numbers

	case ARG_INT8:
		uResult = static_cast<Word32>(Abs(static_cast<Int32>(m_Data.m_iInt8)));
		break;
	case ARG_INT16:
		uResult = static_cast<Word32>(Abs(static_cast<Int32>(m_Data.m_iInt16)));
		break;
	case ARG_INT32:
		uResult = static_cast<Word32>(Abs(m_Data.m_iInt32));
		break;
	case ARG_INT64:
		uResult = static_cast<Word32>(Abs(m_Data.m_iInt64));
		break;

	case ARG_FLOAT:
		uResult = static_cast<Word32>(Abs(m_Data.m_fFloat));
		break;
	case ARG_DOUBLE:
		uResult = static_cast<Word32>(Abs(m_Data.m_dDouble));
		break;

	default:
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Returns the value as an unsigned 64 bit integer

	If the value is an unsigned integer, convert to 
	a 64 bit unsigned integer and return the converted value.

	If it's not of this type, return 0.

	\return Unsigned value or 0 if not compatible.

***************************************/

Word64 BURGER_API Burger::SafePrintArgument::GetUInt64(void) const
{
	Word64 uResult;
	switch (m_eType) {
	case ARG_WORD8:
		uResult = m_Data.m_uWord8;
		break;
	case ARG_WORD16:
		uResult = m_Data.m_uWord16;
		break;
	case ARG_WORD32:
		uResult = m_Data.m_uWord32;
		break;
	case ARG_WORD64:
		uResult = m_Data.m_uWord64;
		break;

	// Return the absolute form of signed numbers

	case ARG_INT8:
		uResult = static_cast<Word64>(Abs(static_cast<Int32>(m_Data.m_iInt8)));
		break;
	case ARG_INT16:
		uResult = static_cast<Word64>(Abs(static_cast<Int32>(m_Data.m_iInt16)));
		break;
	case ARG_INT32:
		uResult = static_cast<Word64>(Abs(m_Data.m_iInt32));
		break;
	case ARG_INT64:
		uResult = static_cast<Word64>(Abs(m_Data.m_iInt64));
		break;

	case ARG_FLOAT:
		uResult = static_cast<Word64>(Abs(m_Data.m_fFloat));
		break;
	case ARG_DOUBLE:
		uResult = static_cast<Word64>(Abs(m_Data.m_dDouble));
		break;

	default:
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Returns the value as a single character

	If the value is a compatible to be a character,
	return the converted value.

	If it's not of this type, return \ref cInvalidCharConversion

	\return Character or \ref cInvalidCharConversion if not compatible.

***************************************/

char BURGER_API Burger::SafePrintArgument::GetChar(void) const
{
	char iResult;
	switch (m_eType) {
	case ARG_INT8:
		iResult = m_Data.m_iInt8;
		break;
	case ARG_INT16:
		iResult = static_cast<char>(m_Data.m_iInt16);
		break;
	case ARG_INT32:
		iResult = static_cast<char>(m_Data.m_iInt32);
		break;
	case ARG_INT64:
		iResult = static_cast<char>(m_Data.m_iInt64);
		break;


	// we let the compiler handle the conversion from various int formats
	// to a single character. 
	case ARG_WORD8:
		iResult = static_cast<char>(m_Data.m_uWord8);
		break;
	case ARG_WORD16:
		iResult = static_cast<char>(m_Data.m_uWord16);
		break;
	case ARG_WORD32:
		iResult = static_cast<char>(m_Data.m_uWord32);
		break;
	case ARG_WORD64:
		iResult = static_cast<char>(m_Data.m_uWord64);
		break;

	case ARG_FLOAT:
		iResult = static_cast<char>(m_Data.m_fFloat);
		break;
	case ARG_DOUBLE:
		iResult = static_cast<char>(m_Data.m_dDouble);
		break;

	// we don't return zero so it will be easier to spot in the output
	default:
		iResult = cInvalidCharConversion;
		break;
	}
	return iResult;
}


/*! ************************************

	\brief Returns the value as a boolean

	If the value is a compatible to be a boolean,
	return the converted value.

	If it's not of this type, return 0.

	\return \ref TRUE or \ref FALSE, and \ref FALSE if not compatible.

***************************************/

Word BURGER_API Burger::SafePrintArgument::GetBool( void ) const
{
	// For non-boolean, but numeric types, return if it is zero. 
	// if not a type that can compare to zero, return false;
	return (m_eType == ARG_BOOL) ? (m_Data.m_bBool!=0) : !IsZero();
}

/*! ************************************

	\brief Returns the value as a pointer

	If the value is a compatible to be a pointer,
	and return the pointer or \ref NULL of not.

	\return Pointer or \ref NULL if not a pointer

***************************************/

const void* BURGER_API Burger::SafePrintArgument::GetPointer(void) const
{
	// m_pVoid is a union, so all pointer types are mapped into the same memory
	return ((m_eType < ARG_FIRSTPTR) || (m_eType > ARG_LASTPTR)) ? NULL : m_Data.m_pVoid;
}


/*! ************************************

	\brief Returns the value as a "C" string pointer

	If the value is a compatible to a "C" string,
	return the pointer or \ref NULL of not.

	\return Pointer or \ref NULL if not a pointer

***************************************/

const char* BURGER_API Burger::SafePrintArgument::GetText(void) const
{
	const char *pResult;

	// Special case for a Burgerlib string
	if (m_eType == ARG_PSTRING) {
		if (m_Data.m_pString) {
			pResult = m_Data.m_pString->GetPtr();
		} else {
			pResult = NULL;
		}
	} else if ((m_eType >= ARG_FIRSTTEXT) && (m_eType <= ARG_LASTTEXT)) {
		// Standard char pointer
		pResult = m_Data.m_pChar;
	} else {
		// None of the above
		pResult = NULL;
	}
	return pResult;
}

/*! ************************************

	\brief Returns the length of the "C" string

	If the value is a compatible to a "C" string,
	return the length of the string or zero.

	\return Length of the string or zero.

***************************************/

WordPtr BURGER_API Burger::SafePrintArgument::GetTextLength(void) const
{
	WordPtr uResult;
	if (m_eType == ARG_PSTRING) {
		if (m_Data.m_pString) {
			uResult = m_Data.m_pString->GetLength();
		} else {
			uResult = StringLength(SafePrint::g_NullString);
		}
	} else if (m_eType == ARG_PWORD16) {
		if (m_Data.m_pWord16) {
			uResult = UTF8::FromUTF16(NULL,0,m_Data.m_pWord16);
		} else {
			uResult = StringLength(SafePrint::g_NullString);
		}
	} else if ((m_eType >= ARG_FIRSTTEXT) && (m_eType <= ARG_LASTTEXT)) {
		// C string
		if (m_Data.m_pChar) {
			uResult = StringLength(m_Data.m_pChar);
		} else {
			uResult = StringLength(SafePrint::g_NullString);
		}
	} else {
		// None of the above
		uResult = 0;
	}
	return uResult;
}







/*! ************************************

	\def BURGER_EXPAND_FORMATTING_FUNCTION
	\brief Function to generate SafePrint functions

	To invoke SafePrint via a parameter list, it's necessary
	to fake the ability to do the "C" ... style parameter passing.
	To handle this, generate a code fragment that will
	create the argument table and then call the SafePrint functions
	and this macro will create 24 functions that handle 1 through
	24 parameters.

***************************************/





/*! ************************************

	\class Burger::SafePrint
	\brief Container class for SafePrint functions

	This class contains all the static functions to handle the
	safe printf for Burgerlib.

***************************************/


/*! ************************************

	\struct Burger::SafePrint::ParamInfo_t
	\brief Container struct for SafePrint arguments

	Describes a single conversion to be done
	It can represent a block of literal text to be copied or
	it can describe and argument to be converted

	An array of ParamInfo_t objects is built during the parsing phase 
	from the format string (with optional arguments) and contains the 
	decoded and validated copy and formatted conversion commands that
	use the actual argument values to determine things like how many
	characters are outputted

	\note This is a structure, as such, it's the application's responsibility
	to zero out this structure on instantiation manually. Generally, this is
	created as an array, so it's encouraged to call \ref MemoryClear(void *,WordPtr) on
	the entire array on creation.

***************************************/

/*! ************************************

	\fn eConversionCommand Burger::SafePrint::ParamInfo_t::GetCommand(void) const
	\brief Get the \ref eConversionCommand

	This structure stores this as a byte for compactness, this accessor enforces type
	safety

	\return The stored \ref eConversionCommand
	\sa SetCommand(eConversionCommand)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetCommand(eConversionCommand uCommand)
	\brief Set the \ref eConversionCommand

	This structure stores this as a byte for compactness, this accessor enforces type
	safety

	\param uCommand The \ref eConversionCommand to store in the structure
	\sa GetCommand(void) const

***************************************/



/*! ************************************

	\fn eConversionSpecifier Burger::SafePrint::ParamInfo_t::GetConversion(void) const
	\brief Get the \ref eConversionSpecifier

	This structure stores this as a byte for compactness, this accessor enforces type
	safety

	\return The stored \ref eConversionSpecifier
	\sa SetConversion(eConversionSpecifier) or GetConversionAsFlag(void) const

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetConversion(eConversionSpecifier uOutputConversion)
	\brief Set the \ref eConversionSpecifier

	This structure stores this as a byte for compactness, this accessor enforces type
	safety

	\param uOutputConversion The \ref eConversionSpecifier to store in the structure
	\sa GetConversion(void) const or GetConversionAsFlag(void) const

***************************************/

/*! ************************************

	\fn Word32 Burger::SafePrint::ParamInfo_t::GetConversionAsFlag(void) const
	\brief Get the \ref eConversionSpecifier as a shifted flag

	Get the value of 1 << \ref eConversionSpecifier

	\return The value of 1<< \ref eConversionSpecifier
	\sa SetConversion(eConversionSpecifier) or GetConversion(void) const

***************************************/


/*! ************************************

	\fn Word Burger::SafePrint::ParamInfo_t::GetArgIndex(void) const
	\brief Get the argument index

	This structure stores this as a byte for compactness, this accessor enforces type
	safety

	\return The index into the argument array this structure refers to
	\sa SetArgIndex(Word)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetArgIndex(Word uArgIndex)
	\brief Set the argument index

	This structure stores this as a byte for compactness, this accessor enforces type
	safety

	\param uArgIndex The new index into the argument array this structure refers to
	\sa GetArgIndex(void) const

***************************************/


/*! ************************************

	\fn Word Burger::SafePrint::ParamInfo_t::GetWarnings(void) const
	\brief Get the accumulated warnings

	This structure stores this as a byte for compactness, this accessor enforces type
	safety

	\return The accumulated warning bits
	\sa SetWarning(eWarningFlags)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetWarning(eWarningFlags uWarning)
	\brief Accumulate a warning

	Take the warning flag and OR it into the existing warning state. This function does
	not clear any previous bits.

	This structure stores this as a byte for compactness, this accessor enforces type
	safety.

	\param uWarning The warning bit to set in the \ref m_uWarnings member variable
	\sa GetWarnings(void) const

***************************************/



/*! ************************************

	\fn Word32 Burger::SafePrint::ParamInfo_t::GetFlags(void) const
	\brief Get the accumulated \ref eConversionFlags bits

	The individual bits are defined in \ref eConversionFlags .

	\return All of the active conversion flags
	\sa SetFlags(Word32), SetFlag(eConversionFlags), or IsFlagSet(eConversionFlags) const

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetFlags(Word32 uFormatOptionFlags)
	\brief Set the accumulated \ref eConversionFlags bits

	The individual bits are defined in \ref eConversionFlags.

	\param uFormatOptionFlags All of the \ref eConversionFlags to store in the structure
	\sa GetFlags(void) const, SetFlag(eConversionFlags), or ClearFlag(eConversionFlags)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetFlag(eConversionFlags uFormatOptionFlags)
	\brief Set the specific \ref eConversionFlags bit

	Or the specific flag into the flags.

	\param uFormatOptionFlags The single type safe flag to accumulate into the structure
	\sa IsFlagSet(eConversionFlags) const, GetFlags(void) const, SetFlags(Word32), or ClearFlag(eConversionFlags)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::ClearFlag(eConversionFlags uFormatOptionFlags)
	\brief Clear a specific \ref eConversionFlags bit

	Clear the specific flag stored in the structure.

	\sa ClearFlags(void), IsFlagSet(eConversionFlags) const, GetFlags(void) const, SetFlags(Word32), or SetFlag(eConversionFlags)

***************************************/

/*! ************************************

	\fn Word Burger::SafePrint::ParamInfo_t::IsFlagSet(eConversionFlags uFormatOptionFlags)
	\brief Test a specific \ref eConversionFlags bit

	Test if the specific flag is set and return \ref TRUE if so, otherwise return \ref FALSE.

	\param uFormatOptionFlags The single type safe flag to test against
	\return \ref TRUE if set, \ref FALSE if not.
	\sa SetFlag(eConversionFlags), GetFlags(void) const, SetFlags(Word32), or ClearFlag(eConversionFlags)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::ClearFlags(void)
	\brief Clear the \ref eConversionFlags bits

	Set the bits to zero.

	\sa ClearFlag(eConversionFlags), IsFlagSet(eConversionFlags) const, GetFlags(void) const, SetFlags(Word32), or SetFlag(eConversionFlags)

***************************************/


/*! ************************************

	\fn Word Burger::SafePrint::ParamInfo_t::GetWidth(void) const
	\brief Get the width of the output for the argument

	This structure stores this as a \ref Word16 for compactness, this accessor enforces type
	safety

	\return The width override parsed in the "C" string
	\sa SetWidth(Word)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetWidth(Word uCharacterWidth)
	\brief Set the width of the output for the argument

	This structure stores this as a \ref Word16 for compactness, this accessor enforces type
	safety

	\param uCharacterWidth The new width of the output for the argument
	\sa GetWidth(void) const

***************************************/


/*! ************************************

	\fn Word Burger::SafePrint::ParamInfo_t::GetPrecision(void) const
	\brief Get the desired precision of the argument

	This structure stores this as a \ref Word16 for compactness, this accessor enforces type
	safety

	\return The precision requested for the argument
	\sa SetPrecision(Word)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetPrecision(Word uPrecision)
	\brief Set the width of the output for the argument

	This structure stores this as a \ref Word16 for compactness, this accessor enforces type
	safety

	\param uPrecision The new precision requested for the argument
	\sa GetPrecision(void) const

***************************************/


/*! ************************************

	\fn WordPtr Burger::SafePrint::ParamInfo_t::GetFormattedLength(void) const
	\brief Get the parsed length of the '%%' command

	This structure stores this as a \ref Word16 for compactness, this accessor enforces type
	safety

	\return The number of bytes in the parsed "C" string this format code occupies
	\sa SetFormattedLength(WordPtr)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetFormattedLength(WordPtr uFormattedLength)
	\brief Get the parsed length of the '%%' command

	This structure stores this as a \ref Word16 for compactness, this accessor enforces type
	safety

	\param uFormattedLength The new number of bytes in the parsed "C" string this format code occupies
	\sa GetFormattedLength(void) const

***************************************/


/*! ************************************

	\fn WordPtr Burger::SafePrint::ParamInfo_t::GetOutputLength(void) const
	\brief Get the length of the processed argument

	This structure stores this as a \ref Word16 for compactness, this accessor enforces type
	safety

	\return The number of bytes the printed argument will occupy
	\sa SetOutputLength(WordPtr)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetOutputLength(WordPtr uOutputLength)
	\brief Set the length of the processed argument

	This structure stores this as a \ref Word16 for compactness, this accessor enforces type
	safety

	\param uOutputLength The new number of bytes the printed argument will occupy
	\sa GetOutputLength(void) const

***************************************/


/*! ************************************

	\brief Set the parsed floating point information

	This structure stores these variables in a compact fashion, this accessor enforces type
	safety

	\param uIntSpecialDigits Number of digits to the left of the decimal point
	\param uFracDigits Number of digits to the right of the decimal point
	\param bSign \ref TRUE if a sign flag should be displayed
	\param uDecimalPoint \ref TRUE if a decimal point should be displayed
	\param uPadDigits Number of digits to pad
	\param uExponentDigits Number of exponent digits
	
	\sa GetFloatInfo(Word *,Word *,Word *,Word *,Word *,Word *) const

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::SetFloatInfo(Word uIntSpecialDigits,Word uFracDigits,Word bSign,Word uDecimalPoint,Word uPadDigits,Word uExponentDigits)
{
	m_FloatInfo.m_uIntSpecialDigits = static_cast<Word16>(uIntSpecialDigits);
	m_FloatInfo.m_uFracDigits = static_cast<Word16>(uFracDigits);
	m_FloatInfo.m_uSignDecimalDigits = static_cast<Word8>((bSign<<4U) | (uDecimalPoint&0x0fU));
	m_FloatInfo.m_uPadDigits = static_cast<Word16>(uPadDigits);
	m_FloatInfo.m_uExponentDigits = static_cast<Word8>(uExponentDigits);
}

/*! ************************************

	\brief Get the parsed floating point information

	This structure stores these variables in a compact fashion, this accessor enforces type
	safety

	\param pIntSpecialDigits Pointer to store the number of digits to the left of the decimal point
	\param pFracDigits Pointer to store the Number of digits to the right of the decimal point
	\param pSign Pointer to store \ref TRUE or \ref FALSE if a sign flag should be displayed
	\param pDecimalPoint Pointer to store \ref TRUE or \ref FALSE if a decimal point should be displayed
	\param pPadDigits Pointer to store the number of digits to pad
	\param pExponentDigits Pointer to store the number of exponent digits
	
	\sa SetFloatInfo(Word,Word,Word,Word,Word,Word)

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::GetFloatInfo(Word *pIntSpecialDigits,Word *pFracDigits,Word *pSign,Word *pDecimalPoint,Word *pPadDigits,Word *pExponentDigits) const
{
	pIntSpecialDigits[0] = m_FloatInfo.m_uIntSpecialDigits;
	pFracDigits[0] = m_FloatInfo.m_uFracDigits;
	pSign[0] = static_cast<Word>(m_FloatInfo.m_uSignDecimalDigits >> 4U);
	pDecimalPoint[0] = (m_FloatInfo.m_uSignDecimalDigits & 0x0FU);
	pPadDigits[0] = m_FloatInfo.m_uPadDigits;
	pExponentDigits[0] = m_FloatInfo.m_uExponentDigits;
}

/*! ************************************

	\brief Set the analyzed floating point information

	This structure stores these variables in a compact fashion, this accessor enforces type
	safety

	\param uFPInfoResult Type of floating point number detected
	\param uPadIntZeros Number of characters to pad on the integer
	\param uPadFracZeros Number of character to pad on the fraction
	\param uFirstNonZeroIntPos Position of the first non-zero digit in the integer
	
	\sa GetFloatAnalysisInfo(FPPrintInfo::eResult *,Word *,Word *,Word *) const

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::SetFloatAnalysisInfo(FPPrintInfo::eResult uFPInfoResult,Word uPadIntZeros,Word uPadFracZeros,Word uFirstNonZeroIntPos)
{
	m_FloatInfo.m_uFPInfoResult = static_cast<Word16>(uFPInfoResult);
	m_FloatInfo.m_uPadIntZeros = static_cast<Word16>(uPadIntZeros);
	m_FloatInfo.m_uPadFracZeros = static_cast<Word16>(uPadFracZeros);
	m_FloatInfo.m_uFirstNonZeroIntPos = static_cast<Word16>(uFirstNonZeroIntPos);
}

/*! ************************************

	\brief Get the analyzed floating point information

	This structure stores these variables in a compact fashion, this accessor enforces type
	safety

	\param pFPInfoResult Pointer to the type of floating point number detected
	\param pPadIntZeros Pointer to the number of characters to pad on the integer
	\param pPadFracZeros Pointer to the number of character to pad on the fraction
	\param pFirstNonZeroIntPos Pointer to the position of the first non-zero digit in the integer
	
	\sa SetFloatAnalysisInfo(FPPrintInfo::eResult,Word,Word,Word)

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::GetFloatAnalysisInfo(FPPrintInfo::eResult *pFPInfoResult,Word *pPadIntZeros,Word *pPadFracZeros,Word *pFirstNonZeroIntPos) const
{
	pFPInfoResult[0] = static_cast<FPPrintInfo::eResult>(m_FloatInfo.m_uFPInfoResult);
	pPadIntZeros[0] = m_FloatInfo.m_uPadIntZeros;
	pPadFracZeros[0] = m_FloatInfo.m_uPadFracZeros;
	pFirstNonZeroIntPos[0] = m_FloatInfo.m_uFirstNonZeroIntPos;
}

/*! ************************************

	\brief Set the special floating point information for NAN or INF

	This structure stores these variables in a compact fashion, this accessor enforces type
	safety

	\param uFPInfoResult Type of floating point number detected
	\param uIntSpecialDigits Number of characters in the string
	\param uPadIntZeros Number of characters to pad on the integer
	\param uIsNegative Set to non-zero if negative
	
	\sa SetFloatAnalysisInfo(FPPrintInfo::eResult,Word,Word,Word)

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::SetFloatSpecialResult(FPPrintInfo::eResult uFPInfoResult,Word uIntSpecialDigits,Word uPadIntZeros,Word uIsNegative)
{
	m_FloatInfo.m_uFPInfoResult = static_cast<Word16>(uFPInfoResult);
	m_FloatInfo.m_uIntSpecialDigits = static_cast<Word16>(uIntSpecialDigits);
	m_FloatInfo.m_uPadIntZeros = static_cast<Word16>(uPadIntZeros);
	m_FloatInfo.m_uExponentDigits = static_cast<Word8>(uIsNegative);
}

/*! ************************************

	\fn Word Burger::SafePrint::ParamInfo_t::GetFloatSpecialChars(void) const
	\brief Get the number of special character for a special floating point number

	\return The number of bytes the special string will occupy
	\sa SetFloatSpecialResult(FPPrintInfo::eResult,Word,Word,Word)

***************************************/

/*! ************************************

	\fn Word Burger::SafePrint::ParamInfo_t::GetFloatSpecialLeadingZeros(void) const
	\brief Get the number leading zeros for a special floating point number

	\return The number of leading zeros
	\sa SetFloatSpecialResult(FPPrintInfo::eResult,Word,Word,Word)

***************************************/

/*! ************************************

	\fn Word Burger::SafePrint::ParamInfo_t::GetFloatSpecialIsNegative(void) const
	\brief Is the special floating pointer number negative?

	\return \ref TRUE if negative, \ref FALSE if not.
	\sa SetFloatSpecialResult(FPPrintInfo::eResult,Word,Word,Word)

***************************************/

/*! ************************************

	\brief Output padding for field width

	Handles the case when a field width is specified and
	and it's wider than the formatted argument output.
	Fills in the remainder of the field with the pad character and
	handles any justification

	\param pOutBuffer Pointer to the output buffer
	\return Number of bytes that was added via padding

***************************************/

WordPtr BURGER_API Burger::SafePrint::ParamInfo_t::PadFieldWidth(char *pOutBuffer) const
{
	// if the width was not explicitly specified in the formatting command,
	// or it isn't wider than the formatted argument data,
	// then we won't be padding anything

	WordPtr uFormattedLength = GetFormattedLength();
	WordPtr uOutputLength = GetOutputLength();
	WordPtr uResult;
	if (!IsFlagSet(CONVFLAG_WIDTH_SPECIFIED) || (GetWidth() <= uFormattedLength)) {
		BURGER_ASSERT(uFormattedLength == uOutputLength);
		uResult = 0;
	} else {
		BURGER_ASSERT(uFormattedLength < uOutputLength);

		Word bLeftJustify = IsFlagSet(CONVFLAG_LEFT_JUSTIFY);
		WordPtr uPadLength = uOutputLength - uFormattedLength;
		if (uPadLength) {
			// Adjust to the output if left justified
			if (bLeftJustify) {
				pOutBuffer+=uFormattedLength;
			}
			WordPtr i = 0;
			do {
				pOutBuffer[i] = cWIDTH_PAD_CHARACTER;
			} while (++i<uPadLength);
		}

		// return number of characters into field to put argument data at
		uResult = (bLeftJustify) ? 0 : uPadLength;
	}
	return uResult;
}


/*! ************************************
	
	\brief Calculate the number of character that will be printed for this argument

	Gets the actual # of characters that will
	be output given the argument passed in and the format options

	\param pArg Pointer to a \ref SafePrintArgument to check

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::GetFormattedOutputLength(const SafePrintArgument* pArg)
{
	eConversionSpecifier uConversionSpecifier = GetConversion();
	Word bHasWidth = IsFlagSet(CONVFLAG_WIDTH_SPECIFIED);
	Word uWidth = (bHasWidth) ? GetWidth() : 0;

	// Init the final count
	WordPtr uCharsCounted = 0;

	// display data in binary
	// flags that modify output length: CommaSeparate (insert space between nibbles)
	if (uConversionSpecifier == CONVSPEC_BINNUMBER) {
		WordPtr uArgBytes = pArg->GetDataLengthInBytes();
		uCharsCounted = 8 * uArgBytes;	// 8 bits per byte

		// Account for commas
		uCharsCounted+= (IsFlagSet(CONVFLAG_COMMA_SEPARATE)) ? (uArgBytes * 2 - 1) : 0;

	// display as boolean text ("true" or "false")
	// flags that modify output length: none
	} else if (uConversionSpecifier == CONVSPEC_BOOLTEXT) {
		uCharsCounted = StringLength(g_BoolText[pArg->GetBool()]);		// length of "true" : "false"

	// Char is one character, 'nuff said
	} else if (uConversionSpecifier == CONVSPEC_CHAR) {
		if (IsFlagSet(CONVFLAG_INTERPRET_LONG) && (pArg->GetType()==SafePrintArgument::ARG_WORD16)) {
			// Convert wide character to UTF-8
			char TempBufferUTF8[8];
			// The buffer is discard, only the length is of interest
			uCharsCounted = UTF8::FromUTF16(TempBufferUTF8,static_cast<Word16>(pArg->GetUInt32()));
		} else {
			uCharsCounted = 1;
		}

	// Handle all the integer variations
	} else if ((uConversionSpecifier == CONVSPEC_DECIMAL_INT) || (uConversionSpecifier == CONVSPEC_DECIMAL_WORD)) {
		// In relaxed mode, we allow signed ints with unsigned outputs
		// and unsigned ints with signed output and bools with either

		Word bSign = (pArg->IsSigned()) ? pArg->IsNegative() : 0;
		// Number of digits to print
		Word uNumericDigits = pArg->Is64Bit() ? NumberStringLength(pArg->GetUInt64()) : NumberStringLength(pArg->GetUInt32());
		Word uCommaCount = 0;

		// Precision can expand number of numeric chars output
		if (IsFlagSet(CONVFLAG_PRECISION_SPECIFIED))	{
			Word uPrecision = GetPrecision();
			// will we be adding leading zeros?
			if (uPrecision > uNumericDigits) {
				uNumericDigits = uPrecision;
			}

			// Special case: The result of converting a zero value with 
			//  a precision of zero is no characters
			if (!uPrecision && pArg->IsZero()) {
				uNumericDigits = 0;
			}
		}

		// Ok, we got the number of numeric digits in the int... now what about commas?
		if (IsFlagSet(CONVFLAG_COMMA_SEPARATE) && (uNumericDigits > 3)) {
			uCommaCount = (uNumericDigits - 1) / 3;
		}

		// check flags for displaying a sign character (signed int conversion only)
		if ((uConversionSpecifier == CONVSPEC_DECIMAL_INT) && (IsFlagSet(CONVFLAG_DISPLAY_SIGN) || IsFlagSet(CONVFLAG_BLANK_SIGN))) {
			bSign = 1;	// 1 digit for sign character
		}

		// If padding zeros and width specified, they will pad out the width 
		// (we know there is no precision, comma or left justify)
		if (bHasWidth && IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS)) {
			if ((bSign + uNumericDigits) < uWidth) {
				uNumericDigits = uWidth - bSign;
			}
		}
		// total up characters to output
		uCharsCounted = bSign + uNumericDigits + uCommaCount;


	// Handle a floating point type conversion (regular, scientific notation, or shorter of the two)

	} else if ((uConversionSpecifier == CONVSPEC_DECIMAL_FLOAT) ||
		(uConversionSpecifier == CONVSPEC_SCI_NOTE_FLOAT) || 
		(uConversionSpecifier == CONVSPEC_SHORTER_FLOAT)) {

		// Break the real number up into mantissa, exponent, etc
		FPInfo TheFPInfo;
		switch (pArg->GetType()) {
		case SafePrintArgument::ARG_HALF:
			TheFPInfo.InitHalf(pArg->m_Data.m_fHalf);
			break;
		case SafePrintArgument::ARG_FLOAT:
			TheFPInfo.InitFloat(pArg->m_Data.m_fFloat);
			break;
		case SafePrintArgument::ARG_DOUBLE:
			TheFPInfo.InitDouble(pArg->m_Data.m_dDouble);
			break;
		default:
			// This is an error condition
			TheFPInfo.InitFloat(0.0f);
			break;
		}

		// if no precision is specified, we are supposed to use 6 digits of precision
		Word uFloatPrecision = 6;
		if (IsFlagSet(CONVFLAG_PRECISION_MARKER)) {
			if (IsFlagSet(CONVFLAG_PRECISION_SPECIFIED)) {
				uFloatPrecision = GetPrecision();
			} else {
				uFloatPrecision = 0;	// "." only = means zero precision
			}
		}

		Word uSignLength = 0;
		// printing a sign character?
		if (TheFPInfo.IsNegative() || IsFlagSet(CONVFLAG_DISPLAY_SIGN) || IsFlagSet(CONVFLAG_BLANK_SIGN)) {
			uSignLength = 1;
		}

		// now run a digit check on it...
		FPPrintInfo TheFPPrintInfo;
		TheFPPrintInfo.AnalyzeFloat(&TheFPInfo,uFloatPrecision);

		// is it a "special" output such as NAN or infinity?
		if (TheFPPrintInfo.IsSpecial()) {
			// save the result
			SetFloatSpecialResult(TheFPPrintInfo.GetResult(),TheFPPrintInfo.GetSpecialFormDigits(),0,TheFPInfo.IsNegative());
			uCharsCounted = TheFPPrintInfo.GetSpecialFormDigits();


		} else {

			// Create the formatting defaults
			Word uExponentSpecialDigits = 0;
			Word uIntSpecialDigits = 0;
			Word uDecimalPoint = 0;
			Word uFracDigits = 0;
			Word uCommaCount = 0;
			Word uExponentDigits = 0;
			Word uPadDigits = 0;
			Word uExponentPadDigits = 0;
				
			// =========================================================
			// get the width for f conversion if f or g specified
			if (uConversionSpecifier == CONVSPEC_DECIMAL_FLOAT /* || (uConversionSpecifier == CONVSPECFLAG_SHORTER_FLOAT) */ ) {
				// Get the number of integer digits
				uIntSpecialDigits = TheFPPrintInfo.GetLastNonZeroIntDigitPosition();

				// zero point something?  then include the zero the left of the decimal
				if (!uIntSpecialDigits) {
					uIntSpecialDigits = 1;
				}

				// how many digits to the right of the decimal point?
				uFracDigits = uFloatPrecision;

				// do we have a decimal point?
				if (uFloatPrecision || IsFlagSet( CONVFLAG_ALTERNATIVE_FORM )) {
					uDecimalPoint = 1;
				}

				// Ok, we got the number of digits in the int part... now what about commas?
				if (IsFlagSet(CONVFLAG_COMMA_SEPARATE) && (uIntSpecialDigits > 3) ) {
					uCommaCount = (uIntSpecialDigits - 1) / 3;
				}

				// If padding zeros and width specified, they will pad out the width 
				// (we know there is no comma or left justify flag)
				if (bHasWidth && IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS)) {
					Word uTotalRealDigits = uSignLength + uIntSpecialDigits + uDecimalPoint + uFracDigits;
					if (uTotalRealDigits < uWidth) {
						uPadDigits = uWidth - uTotalRealDigits;
					}
				}
			}

			// =========================================================
			// get the width for e conversion if e or g specified
			if (uConversionSpecifier == CONVSPEC_SCI_NOTE_FLOAT /* || (uConversionSpecifier == CONVSPECFLAG_SHORTER_FLOAT) */ ) {
				// scientific notation, always 1 digit to the left of the decimal
				uExponentSpecialDigits = 1;

				// do we have a decimal point?
				if (uFloatPrecision || IsFlagSet(CONVFLAG_ALTERNATIVE_FORM)) {
					uDecimalPoint = 1;
				}

				// how many digits to the right of the decimal point?
				uFracDigits = uFloatPrecision;

				Int32 iExponent = static_cast<Int32>(TheFPPrintInfo.HasInteger() ? TheFPPrintInfo.GetLastNonZeroIntDigitPosition() - 1 : 0-TheFPPrintInfo.GetFirstNonZeroFracDigitPosition());

				// How many digits for the exponent?
				Word uExpDigits = NumberStringLength(static_cast<Word32>(Abs(iExponent)));

				// we support a configurable minimum number of exponent digits
				if (uExpDigits < cMIN_FLOAT_EXPONENT_DIGITS) {
					uExpDigits = cMIN_FLOAT_EXPONENT_DIGITS;
				}

				uExponentDigits = 2 + uExpDigits;		// add the 'e' and the sign character

				// If padding zeros and width specified, they will pad out the width 
				// (we know there is no comma or left justify flag)
				if (bHasWidth && IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS)) {
					Word uTotalRealDigits = uSignLength + uExponentSpecialDigits + uDecimalPoint + uFracDigits + uExponentDigits;
					if (uTotalRealDigits < uWidth) {
						uExponentPadDigits = uWidth - uTotalRealDigits;
					}
				}
			}

			if (uConversionSpecifier == CONVSPEC_DECIMAL_FLOAT) {
				// tally up all the parts
				uCharsCounted = uSignLength + uIntSpecialDigits + uCommaCount + uDecimalPoint + uFracDigits + uPadDigits;

				// cache off the info from the analysis
				SetFloatInfo(uIntSpecialDigits,uFracDigits,uSignLength,uDecimalPoint,uPadDigits,0);
				SetFloatAnalysisInfo(TheFPPrintInfo.GetResult(),TheFPPrintInfo.GetIntegerZeroCount(),TheFPPrintInfo.GetFracZeroCount(),TheFPPrintInfo.GetFirstNonZeroIntDigitPosition());

			} else if (uConversionSpecifier == CONVSPEC_SCI_NOTE_FLOAT) {
				// tally up all the parts
				uCharsCounted = uSignLength + uExponentSpecialDigits +  uDecimalPoint + uFracDigits + uExponentPadDigits;

				// cache off the info
				SetFloatInfo(uExponentSpecialDigits,uFracDigits,uSignLength,uDecimalPoint,uExponentPadDigits,uExponentDigits);
			}
		}


	// Octal conversion takes unsigned ints and in relaxed mode signed ints and bools
	} else if (uConversionSpecifier == CONVSPEC_OCTAL) {

		// A negative signed int is displayed as if it were interpreted (not cast) to unsigned
		if (pArg->IsNegative()) {
			uCharsCounted = ((pArg->GetDataLengthInBytes() * 8) + 2 ) / 3;
		} else {
			uCharsCounted = (pArg->Is64Bit()) ? NumberOctalStringLength(pArg->GetUInt64()) : NumberOctalStringLength(pArg->GetUInt32());
		}
		// the only time we have a leading zero is when the value is 0
		WordPtr uNonZeroDigits = (pArg->IsZero()) ? uCharsCounted - 1 : uCharsCounted;

		// Precision can expand number of octal digits output
		if (IsFlagSet(CONVFLAG_PRECISION_SPECIFIED)) {
			Word uPrecision	= GetPrecision();
			// will we be adding leading zeros?
			if (uPrecision > uCharsCounted) {
				uCharsCounted = uPrecision;
			}

			// Special case: The result of converting a zero value with 
			//  a precision of zero is no characters
			if (!uPrecision && pArg->IsZero()) {
				uCharsCounted = 0;
			}
		}

		// if alternate Form ('#') is specified and there isn't a leading zero
		//  already due to value = 0 or precision > digits then we have to add one
		if (IsFlagSet(CONVFLAG_ALTERNATIVE_FORM)) {
			if (uCharsCounted == uNonZeroDigits) {
				++uCharsCounted;
			}
		}

		// If padding zeros and width specified, they will pad out the width 
		// (we know there is no precision, comma or left justify)
		if (bHasWidth && IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS)) {
			if (uCharsCounted < uWidth) {
				uCharsCounted = uWidth;
			}
		}

	// Hex conversion takes unsigned ints and in relaxed mode signed ints and bools
	//
	// Per the C++ spec, "The value of the pointer is converted to a sequence of 
	//                    printing characters, in an implementation defined manner."
	// So we are displaying pointers are as hex at full precision
	//  and allow for the alternative form to prefix a '0x'
	} else if ((uConversionSpecifier == CONVSPEC_HEX) || (uConversionSpecifier == CONVSPEC_POINTER)) {
		
		// Adding the 0x?
		Word bAlternativeForm = IsFlagSet(CONVFLAG_ALTERNATIVE_FORM);
		Word uPrefixCount = (bAlternativeForm) ? 2U : 0;

		// for Hex (but not pointer), Alternative form (base prefix) is suppressed by zero value
		if ((uConversionSpecifier == CONVSPEC_HEX) && bAlternativeForm && pArg->IsZero()) {
			uPrefixCount = 0;
		}

		if (uConversionSpecifier == CONVSPEC_POINTER) {
			uCharsCounted = pArg->GetDataLengthInBytes() * 2;
		} else {
			// A negative signed int is displayed as if it were interpreted (not cast) to unsigned
			if (pArg->IsNegative() ) {
				uCharsCounted = pArg->GetDataLengthInBytes() * 2;
			} else {
				uCharsCounted = (pArg->Is64Bit()) ? NumberHexStringLength(pArg->GetUInt64()) : NumberHexStringLength(pArg->GetUInt32());
			}
		}

		// Precision can expand number of octal digits output
		if (IsFlagSet(CONVFLAG_PRECISION_SPECIFIED)) {
			Word uPrecision = GetPrecision();
			// will we be adding leading zeros?
			if (uPrecision > uCharsCounted) {
				uCharsCounted = uPrecision;
			}

			// Special case: The result of converting a zero value with 
			//  a precision of zero is no characters
			if (!uPrecision && pArg->IsZero()) {
				uCharsCounted = 0;
			}
		}

		// If padding zeros and width specified, they will pad out the width 
		// (we know there is no precision, comma or left justify)
		if (bHasWidth && IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS)) {
			if ((uCharsCounted + uPrefixCount) < uWidth) {
				uCharsCounted = uWidth - uPrefixCount;
			}
		}

		uCharsCounted += uPrefixCount;
		

	// "C" string support.

	} else if (uConversionSpecifier == CONVSPEC_TEXT_STRING) {

		// Get the length of the text being pointed to
		if (pArg->IsTextPointer()) {
			uCharsCounted = pArg->GetTextLength();
			// for strings, precision sets the *maximum number of bytes to be written
			if (IsFlagSet(CONVFLAG_PRECISION_SPECIFIED)) {
				Word uPrecision	= GetPrecision();
				if (uCharsCounted > uPrecision) {
					uCharsCounted = uPrecision;
				}
			}
		} else { // Expansion space for string classes, etc..
			uCharsCounted = 0;
		}
		
	} else {
		// Fell through and not found anything?
		BURGER_ASSERT(FALSE);
	}
	
	// fill out length of formatted argument
	SetFormattedLength(uCharsCounted);
	if (bHasWidth && (uWidth > uCharsCounted)) {
		uCharsCounted = uWidth;
	}

	// fill out total length of output
	SetOutputLength(uCharsCounted);
}


/*! ************************************
	
	\brief Sanity check the conversion flags

	Checks the flags specified in a given conversion
	for validity.  Compiler specific variations can be allowed here

	Warnings are set if issues were detected and corrected.

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::CheckConversionFlags(void)
{
	// Don't bother if it's not a conversion
	if (GetCommand() != COMMAND_COPY_TEXT) {

		eConversionSpecifier uConversionSpecifier = GetConversion();
		Word bHasPrecision = IsFlagSet(CONVFLAG_PRECISION_SPECIFIED);

		// Check to see if precision is valid for this conversion
		if (bHasPrecision) {
			if (!(g_ValidFlagsForConversion[uConversionSpecifier] & CONVFLAG_PRECISION_SPECIFIED) ) {
				// Note warning that this doesn't support precision
				SetWarning(WARN_PRECISION_UNDEFINED);

				// Remove precision info from the conversion
				ClearFlag(CONVFLAG_PRECISION_SPECIFIED);
				ClearFlag(CONVFLAG_PRECISION_MARKER);
				SetPrecision(0);
				bHasPrecision = FALSE;
			}
		}

		// See if any output flags are not supported by this conversion
		const Word32 cFlagsToCheck = CONVFLAG_LEFT_JUSTIFY | CONVFLAG_DISPLAY_SIGN | CONVFLAG_BLANK_SIGN | CONVFLAG_ALTERNATIVE_FORM | CONVFLAG_LEFT_PAD_ZEROS | CONVFLAG_COMMA_SEPARATE;
		Word32 uIsolatedFlags = GetFlags() & cFlagsToCheck;
		Word32 uValidFlags = g_ValidFlagsForConversion[uConversionSpecifier] & cFlagsToCheck;

		// Are there any flags set that aren't valid for this conversion type?
		if ((uIsolatedFlags & uValidFlags) != uIsolatedFlags ) {
			// Note warning 
			SetWarning(WARN_FLAG_UNDEFINED_FOR_CONVERSION);
			// Clear the unsupported flags
			Word32 uKeepMask = (~cFlagsToCheck) | uValidFlags;
			SetFlags(GetFlags()&uKeepMask);
		}

		Word bFlagsdiouxXAllowed = ((uConversionSpecifier == CONVSPEC_DECIMAL_INT) || 
			(uConversionSpecifier == CONVSPEC_DECIMAL_WORD) ||
			(uConversionSpecifier == CONVSPEC_OCTAL) ||
			(uConversionSpecifier == CONVSPEC_HEX));

		// Per C++ spec:  If a precision is specified, the 0 flag is ignored for integer conversions
		// Per C++ Spec:  If the 0 and - flags both appear, the 0 flag is ignored.
		// Our extension: If the 0 and , flags both appear, the 0 flag is ignored.
		if (IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS) &&		// '0'
			(IsFlagSet(CONVFLAG_LEFT_JUSTIFY) ||		// '-'
			IsFlagSet(CONVFLAG_COMMA_SEPARATE) ||		// ','
			(bHasPrecision && bFlagsdiouxXAllowed))) {
			ClearFlag(CONVFLAG_LEFT_PAD_ZEROS );
			SetWarning(WARN_FLAG_SUPPRESSED);
		}

		// Per C++ Spec: if ' ' and + both appear, ' ' is ignored
		if (IsFlagSet(CONVFLAG_DISPLAY_SIGN) && IsFlagSet(CONVFLAG_BLANK_SIGN)) {
			ClearFlag(CONVFLAG_BLANK_SIGN);
			SetWarning(WARN_FLAG_SUPPRESSED);
		}
	}
}



/*! ************************************
	
	\brief Sanity check the conversion modifier flags

	Checks the flags specified in a given conversion modifier
	for validity.  Compiler specific variations can be allowed here

	Warnings are set if issues were detected and corrected.

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::CheckConversionForWarnings(const SafePrintArgument* pArg)
{
	eConversionSpecifier uConversionSpecifier = GetConversion();

	// check for signed/unsigned mismatches
	if (pArg->IsNumeric()) {
		Word bIsUnsigned = pArg->IsUnsigned();

		switch (uConversionSpecifier) {
		case CONVSPEC_DECIMAL_INT:
			if (bIsUnsigned) {
				SetWarning(WARN_SIGNED_UNSIGNED_MISMATCH);
			}
			break;

		case CONVSPEC_DECIMAL_WORD:
		case CONVSPEC_OCTAL:
		case CONVSPEC_HEX:
			if (!bIsUnsigned) {
				SetWarning(WARN_SIGNED_UNSIGNED_MISMATCH);
			}
			break;
		default:
			break;
		}
	}

	// check for length specifiers
	WordPtr uDataInBytes = pArg->GetDataLengthInBytes();
	WordPtr uExpectedLength = g_ConversionArgumentSizes[uConversionSpecifier];

	// were any length flags given?

	if (IsFlagSet(CONVFLAG_INTERPRET_SPECIFIED)) {
		Word bIntFlags_diouxX = ( uConversionSpecifier == CONVSPEC_DECIMAL_INT || uConversionSpecifier == CONVSPEC_DECIMAL_WORD ||
			uConversionSpecifier == CONVSPEC_OCTAL || uConversionSpecifier == CONVSPEC_HEX );
		Word bFloatFlags_aAeEfFgG = ( uConversionSpecifier == CONVSPEC_DECIMAL_FLOAT || uConversionSpecifier == CONVSPEC_SCI_NOTE_FLOAT || 
			uConversionSpecifier == CONVSPEC_SHORTER_FLOAT );

		// Need to implement all 'n' modifiers

		if (IsFlagSet(CONVFLAG_INTERPRET_SSHORT)) {		// hh = char
			uExpectedLength = (bIntFlags_diouxX) ? 1 : UINTPTR_MAX;
		}

		if (IsFlagSet(CONVFLAG_INTERPRET_SHORT)) {		// h = short
			uExpectedLength = (bIntFlags_diouxX) ? 2 : UINTPTR_MAX;
		}

		if (IsFlagSet(CONVFLAG_INTERPRET_LONG)) {		// l = long int
			if (bIntFlags_diouxX) {
				uExpectedLength = sizeof(long);
			} else if (uConversionSpecifier == CONVSPEC_CHAR) {
				uExpectedLength = sizeof(Word16);
			} else if (uConversionSpecifier == CONVSPEC_TEXT_STRING) {
				// expected type = Word16 *, however we're only checking the sizeof( pointer )
				uExpectedLength = sizeof(void *);
			} else if (bFloatFlags_aAeEfFgG) {
				SetWarning(WARN_FLAG_NO_EFFECT);
			} else {
				uExpectedLength = UINTPTR_MAX;
			}
		}

		if (IsFlagSet(CONVFLAG_INTERPRET_LLONG)) {		// ll = long long int
			uExpectedLength = (bIntFlags_diouxX) ? sizeof(Int64) : UINTPTR_MAX;
		}

		if (IsFlagSet(CONVFLAG_INTERPRET_DOUBLE)) {		// L = long double
			uExpectedLength = (bFloatFlags_aAeEfFgG) ? sizeof(long double) : UINTPTR_MAX;
		}

		if (IsFlagSet(CONVFLAG_INTERPRET_INTMAX)) {		// j = intmax_t or uintmax_t
			uExpectedLength = (bIntFlags_diouxX ) ? sizeof(Int32) : UINTPTR_MAX;
		}

		if (IsFlagSet(CONVFLAG_INTERPRET_SIZET)) {		// z = size_t
			uExpectedLength = (bIntFlags_diouxX) ? sizeof(WordPtr) : UINTPTR_MAX;
		}

		if (IsFlagSet(CONVFLAG_INTERPRET_WORDPTR)) {	// t = ptrdiff_t
			uExpectedLength = (bIntFlags_diouxX) ? sizeof(WordPtr) : UINTPTR_MAX;
		}

		// is the length modifier not defined for the conversion?
		if (uExpectedLength == UINTPTR_MAX) {
			SetWarning(WARN_LENGTH_NOT_VALID_FOR_CONVERSION);
		}
	}

	// Special case for character conversion.  The conversion actually takes an int, which chars are normally up-promoted to
	// but because we capture the actual type without up-promotion we want to not warn if it's smaller and up-promoted
	if ((uConversionSpecifier == CONVSPEC_CHAR) && !IsFlagSet(CONVFLAG_INTERPRET_SPECIFIED)) {
		if (uDataInBytes < uExpectedLength) {
			uExpectedLength = uDataInBytes;
		}
	}

	// is the expected size not the argument size?
	if (uDataInBytes != uExpectedLength) {
		SetWarning(WARN_LENGTH_SPECIFIER_MISMATCH);
	}
}


/*! ************************************

	\brief Store the precision or width value

	When a precision or width value is parsed, store it in the
	appropriate variable in the structure

	\param uStage Parsing stage to determine if parsing precision or width
	\param uCurrentNumValue Value to store
	\return Error code if the value is out of bounds.

***************************************/

Burger::SafePrint::eError BURGER_API Burger::SafePrint::ParamInfo_t::StoreOffNumericValue(eParseStage uStage,Word32 uCurrentNumValue)
{
	eError uResult = ERROR_NONE;
	if (uStage == PARSE_PRECISION) {
		if (uCurrentNumValue > cMaxPrintFPrecision) {
			uResult = ERROR_PRECISION_VALUE_TOO_LARGE;
		} else {
			SetFlag(CONVFLAG_PRECISION_SPECIFIED);
			SetPrecision(uCurrentNumValue);
		}
	} else { // if ( stage	== PARSE_WIDTH) // store off the width parameter
		if (uCurrentNumValue > cMaxPrintFWidth) {
			uResult = ERROR_WIDTH_VALUE_TOO_LARGE;
		} else {
			SetFlag(CONVFLAG_WIDTH_SPECIFIED);
			SetWidth(uCurrentNumValue);
		}
	}
	return uResult;
}


/*! ************************************

	\brief Outputs an integer value

	Relevant flags and inputs:
	* * Width value	- if width > output length, fill with spaces
	* *	Left Justify flag - Left justifies the output
	* *	Display sign - forces a sign to be displayed
    * *	Blank sign - displays a blank in front of a positive number
	* *	Left Pad Zeros - fills zeros in front of the number
	* *	Comma flag - insert commas in front of every group of 3 digits

	Caveats:
	* *	Left Justify overrides padding with zeros
	* *	Comma Flag overrides padding with zeros
	* *	Display Sign overrides Blank Sign

	Composes numeric text from right to left (Except for padding)

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

WordPtr BURGER_API Burger::SafePrint::ParamInfo_t::FormatInteger(char *pOutBuffer,const SafePrintArgument *pArg) const
{
	// sanity checks	
	BURGER_ASSERT(pArg->IsInteger());

	// we can assume all inputs have been validated
	Word bCommaFlag = IsFlagSet(CONVFLAG_COMMA_SEPARATE);
	Word uPrecision = GetPrecision();

	// handle field width & left justify flags
	char *pOutText = pOutBuffer + PadFieldWidth(pOutBuffer);

	// Point to last character - we're filling in backwards
	pOutText = pOutText + GetFormattedLength()-1;
	
	Word bIs64Bit = pArg->Is64Bit();		
	Word bIsNegative = pArg->IsNegative();	
	Word bHasSignChar = IsFlagSet(CONVFLAG_BLANK_SIGN) | IsFlagSet(CONVFLAG_DISPLAY_SIGN) | bIsNegative;

	Word32 uNumber32 = 0;
	Word64 uNumber64 = 0;
	if (!bIs64Bit) {
		uNumber32 = pArg->GetUInt32();
	} else {
		uNumber64 = pArg->GetUInt64();
	}

	WordPtr uCharsWritten = 0;
	Word uDigitswritten = 0;
	Word uTotalDigitsWritten = 0;
	Word bEmitDigits = TRUE;

	// Special case: The result of converting a zero value with 
	//  a precision of zero is no characters
	if (IsFlagSet(CONVFLAG_PRECISION_SPECIFIED) && !uPrecision && pArg->IsZero()) {
		bEmitDigits = FALSE;
	}

	// Loop to emit decimal digits, from right to left
	while (bEmitDigits) {
		Word uCurDigit;
		if (bIs64Bit) {
			Word64 uRemainder64 = uNumber64 / 10ULL;
			uCurDigit = static_cast<Word>(uNumber64 - (uRemainder64 * 10ULL));
			uNumber64 = uRemainder64;
			bEmitDigits = (uNumber64!=0);
		} else {
			Word32 uRemainder32 = uNumber32 / 10U;
			uCurDigit = static_cast<Word>(uNumber32 - (uRemainder32 * 10U));
			uNumber32 = uRemainder32;
			bEmitDigits = (uNumber32!=0);
		}

		// emit character
		*pOutText-- = static_cast<char>('0' + uCurDigit);
		++uCharsWritten;
		++uTotalDigitsWritten;
		++uDigitswritten;

		// Handle any 
		if (bCommaFlag && (uDigitswritten == 3) && bEmitDigits) {
			*pOutText-- = cNUMERIC_GROUP_SEPARATOR;
			++uCharsWritten;
			uDigitswritten = 0;
		}
	}

	// take care of leading zeros caused by precision field
	Word bEmitLeadingZeros = (IsFlagSet(CONVFLAG_PRECISION_SPECIFIED) && (uTotalDigitsWritten < uPrecision));
	while (bEmitLeadingZeros) {
		*pOutText-- = '0';
		++uCharsWritten;
		++uTotalDigitsWritten;
		++uDigitswritten;

		// update emit status
		bEmitLeadingZeros = (uTotalDigitsWritten < uPrecision);

		if (bCommaFlag && (uDigitswritten == 3) && bEmitLeadingZeros) {
			*pOutText-- = cNUMERIC_GROUP_SEPARATOR;
			++uCharsWritten;
			uDigitswritten = 0;
		}
	}

	// Take care of padding with zeros (no comma or left justify)
	if (IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS)) {
		char* pLastZero = pOutBuffer;
		if (bHasSignChar) {
			++pLastZero;
		}

		while (pOutText >= pLastZero) {
			*pOutText-- = '0';
			++uCharsWritten;
		}
	}

	// take care of writing out any sign character 
	if (bHasSignChar) {
		*pOutText-- = static_cast<char>((bIsNegative) ? cNUMERIC_NEGATIVE_SIGN : (IsFlagSet(CONVFLAG_DISPLAY_SIGN)) ? cNUMERIC_POSITIVE_SIGN : cNUMERIC_BLANK_SIGN);
		++uCharsWritten;
	}

	BURGER_ASSERT(uCharsWritten == GetFormattedLength());
	return GetOutputLength();
}


/*! ************************************

	\brief Outputs an integral value as octal or hex digits

	Outputs an integral value as octal or hex digits
	also handles pointer conversion

	Relevant flags and inputs:	
	* * Width value - if width > output length, fill with spaces
	* * Left Justify flag - Left justifies the output
	* * Left Pad Zeros - fills zeros in front of the number
	* * Comma flag - insert commas? in front of every group of 3 digits
	* * AddBasePrefix - inserts base prefix in front of output

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

WordPtr BURGER_API Burger::SafePrint::ParamInfo_t::FormatHexOrOctal(char *pOutBuffer,const SafePrintArgument *pArg) const
{
	// sanity checks	
	BURGER_ASSERT(pArg->IsInteger());

	// we can assume all inputs have been validated
	Word bCommaFlag = IsFlagSet(CONVFLAG_COMMA_SEPARATE);
	Word bAddBasePrefix = IsFlagSet(CONVFLAG_ALTERNATIVE_FORM);
	Word bDisplayUpperCase = IsFlagSet(CONVFLAG_UPPERCASE);
	Word bHasPrecision = IsFlagSet(CONVFLAG_PRECISION_SPECIFIED);
	Word uPrecision = GetPrecision();

	// Special case for hex (but not pointer), zero value suppresses alternate form prefix

	if (bAddBasePrefix && (GetConversion() == CONVSPEC_HEX) && pArg->IsZero()) {
		bAddBasePrefix = FALSE;
	}

	// handle field width & left justify flags
	char *pOutText = pOutBuffer + PadFieldWidth(pOutBuffer);

	Word bIsHex = (GetConversion() != CONVSPEC_OCTAL);

	Word uBitsToShift;
	Word32 uDigitMask;
	Word uGroupBreakAt;

	if (bIsHex) {
		uBitsToShift = 4;
		uDigitMask = 0x0F;
		uGroupBreakAt = 4;
	} else {
		uBitsToShift = 3;
		uDigitMask = 0x07;
		uGroupBreakAt = 3;
	}

	Word uPrefixDigits = (bAddBasePrefix && bIsHex) ? 2U : 0;

	const char* pOutputDigits = (bDisplayUpperCase) ? g_NibbleToAsciiUppercase : g_NibbleToAsciiLowercase;
	
	// Point to last character - we're filling in backwards
	pOutText = pOutText + GetFormattedLength()-1;

	Word bIs64Bit = pArg->Is64Bit();		
	Word32 uNumber32 = 0;
	Word64 uNumber64 = 0;
	if (!bIs64Bit) {
		uNumber32 = pArg->GetUInt32();
	} else {
		uNumber64 = pArg->GetUInt64();
	}

	WordPtr uCharsWritten = 0;
	Word uDigitswritten = 0;
	Word uTotalDigitsWritten = 0;
	Word uLeadingZerosWritten = pArg->IsZero();		// for zero only, we'll emit a technically leading zero
	Word bEmitDigits = TRUE;

	// Special case: The result of converting a zero value with 
	//  a precision of zero is no characters
	if (bHasPrecision && !uPrecision && pArg->IsZero()) {
		bEmitDigits = FALSE;
		uLeadingZerosWritten = 0;
	}

	// Loop to emit decimal digits, from right to left
	while (bEmitDigits) {
		Word32 uCurDigit;

		if (bIs64Bit) {
			uCurDigit = static_cast<Word32>(uNumber64) & uDigitMask;
			uNumber64 = uNumber64 >> uBitsToShift;
			bEmitDigits = (uNumber64 != 0);
		} else {
			uCurDigit = static_cast<Word32>(uNumber32) & uDigitMask;
			uNumber32 = uNumber32 >> uBitsToShift;
			bEmitDigits = (uNumber32 != 0);
		}

		// emit character
		*pOutText-- = pOutputDigits[uCurDigit];
		++uCharsWritten;
		++uDigitswritten;
		++uTotalDigitsWritten;

		// Handle any 
		if ( bCommaFlag && (uDigitswritten == uGroupBreakAt) && bEmitDigits) {
			*pOutText-- = cNUMERIC_GROUP_SEPARATOR;
			++uCharsWritten;
			uDigitswritten = 0;
		}
	}

	// take care of leading zeros caused by precision field
	Word bEmitLeadingZeros = (bHasPrecision && (uTotalDigitsWritten < uPrecision));
	while (bEmitLeadingZeros) {
		*pOutText-- = '0';
		++uCharsWritten;
		++uTotalDigitsWritten;
		++uDigitswritten;
		++uLeadingZerosWritten;

		// update emit status
		bEmitLeadingZeros = (uTotalDigitsWritten < uPrecision );

		if (bCommaFlag && (uDigitswritten == uGroupBreakAt) && bEmitLeadingZeros) {
			*pOutText-- = cNUMERIC_GROUP_SEPARATOR;
			++uCharsWritten;
			uDigitswritten = 0;
		}
	}

	// Take care of padding with zeros (no comma or left justify)
	if (IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS)) {
		while (pOutText >= (pOutBuffer+uPrefixDigits)) {
			*pOutText-- = '0';
			++uCharsWritten;
			++uLeadingZerosWritten;
		}
	}

	// Special case for octal: if Alternate Form ('#') is specified and no 
	// leading zero has been written, then we need to add just one, so if
	// any zeros HAVE already been written, we turn off the zero prefix
	if (!bIsHex && bAddBasePrefix) {
		bAddBasePrefix = (!uLeadingZerosWritten);
	}

	// alternate form: add '0' for octal and '0x' for Hex/Pointer
	if (bAddBasePrefix) {
		if (bIsHex) {
			*pOutText-- = ((bDisplayUpperCase) ? 'X' : 'x' );
			++uCharsWritten;
		}
		*pOutText-- = '0';
		++uCharsWritten;
	}
	BURGER_ASSERT(uCharsWritten == GetFormattedLength());
	return GetOutputLength();
}


/*! ************************************
	
	\brief Outputs an pointer as a hex address value

	Relevant flags and inputs:	
	* * Width value - if width > output length, fill with spaces
	* * Left Justify flag - Left justifies the output
	* * DisplayUpperCase - modifies textual output

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

WordPtr BURGER_API Burger::SafePrint::ParamInfo_t::FormatPointer(char* pOutBuffer,const SafePrintArgument *pArg) const
{
	// sanity checks	
	BURGER_ASSERT(pArg->IsPointer() );

	// make a copy of the output command as hex output with full precision
	ParamInfo_t TempParamInfo(*this);
	TempParamInfo.SetPrecision(static_cast<Word>(2U * sizeof(WordPtr)));
	TempParamInfo.SetFlag(CONVFLAG_PRECISION_SPECIFIED);

	// make a copy of the argument as a Unsigned int
#if UINTPTR_MAX==0xFFFFFFFFU
	SafePrintArgument TempArg(reinterpret_cast<Word32>(pArg->m_Data.m_pVoid));
	TempArg.SetType(SafePrintArgument::ARG_WORD32);
#else
	SafePrintArgument TempArg(reinterpret_cast<Word64>(pArg->m_Data.m_pVoid));
	TempArg.SetType(SafePrintArgument::ARG_WORD64);
#endif
	return TempParamInfo.FormatHexOrOctal(pOutBuffer,&TempArg);	
}


/*! ************************************

	\brief Outputs a character value

	Relevant flags and inputs:	
	* * Left Justify flag	- left justifies the output in the field
	* * Width value			- if width > output length, fill with spaces or zeros
	* * Left Justify flag	- obvious flag is obvious

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

WordPtr BURGER_API Burger::SafePrint::ParamInfo_t::FormatChar(char* pOutBuffer,const Burger::SafePrintArgument *pArg) const
{
	// sanity checks	
	BURGER_ASSERT(pArg->IsCharacter());

	// special case check - we will have an output length of zero if
	// we have an invalid character conversion.  Width doesn't matter
	if (!GetOutputLength()) {
		return 0;
	}

	// handle field width & left justify flags
	char *pOutText = pOutBuffer + PadFieldWidth(pOutBuffer);
	WordPtr uCharsWritten = 0;

	// do we need to convert a wide char or a regular char?
	if (IsFlagSet(CONVFLAG_INTERPRET_LONG)) {
		// skip the wchar_t conversion if we already determined it's invalid
		if (GetFormattedLength()) {
			
			// The function FromUTF16 adds a terminating zero, this is not
			// desired. So, parse into a temp buffer and then copy
			// the characters into the output to prevent a buffer
			// overrun by the terminating zero.

			char TempBufferUTF8[8];
			uCharsWritten = UTF8::FromUTF16(TempBufferUTF8,static_cast<Word16>(pArg->GetUInt32()));
			if (uCharsWritten) {
				WordPtr i = 0;
				do {
					pOutText[i] = TempBufferUTF8[i];
				} while (++i<uCharsWritten);
			}
		}
	} else {
		// Copy the single character
		*pOutText = pArg->GetChar();
		uCharsWritten = 1;
	}
	BURGER_ASSERT(uCharsWritten == GetFormattedLength());
	return GetOutputLength();
}

/*! ************************************

	\brief Outputs a text string

	Relevant flags and inputs:	
	* * Width value - if width > output length, fill with spaces
	* * Left Justify flag - Left justifies the output

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

WordPtr BURGER_API Burger::SafePrint::ParamInfo_t::FormatTextString(char *pOutBuffer,const SafePrintArgument *pArg) const
{
	// sanity checks	
	BURGER_ASSERT(pArg->IsTextPointer());

	// handle field width & left justify flags
	char* pOutText = pOutBuffer + PadFieldWidth(pOutBuffer);
	WordPtr uCharsWritten = GetFormattedLength();

	// copy the text string (if we determined we have any text)
	if (uCharsWritten) {
		if (pArg->GetType()==SafePrintArgument::ARG_PWORD16) {
			if (pArg->m_Data.m_pWord16) {
				char Temp = pOutText[uCharsWritten];
				UTF8::FromUTF16(pOutText,uCharsWritten+1,pArg->m_Data.m_pWord16);
				pOutText[uCharsWritten] = Temp;
			} else {
				MemoryCopy(pOutText,g_NullString,uCharsWritten);
			}
		} else {
			const char* pSrcText = pArg->GetText();
			if (!pSrcText) {
				pSrcText = g_NullString;
			}
			MemoryCopy(pOutText,pSrcText,uCharsWritten);
		}
	}
	return GetOutputLength();
}

/*! ************************************
	
	\brief Outputs a boolean values value as "true"/"false"

	Relevant flags and inputs:	
	* * Width value - if width > output length, fill with spaces
	* * Left Justify flag - obvious flag is obvious
	* * DisplayUpperCase - modifies textual output

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

WordPtr BURGER_API Burger::SafePrint::ParamInfo_t::FormatBool(char* pOutBuffer,const SafePrintArgument* pArg) const
{
	// we can assume all inputs have been validated
	Word bIsValueZero = pArg->IsZero();
	WordPtr uNumChars = 4 + bIsValueZero;

	// sanity checks	
	BURGER_ASSERT(GetFormattedLength() == uNumChars);
	BURGER_ASSERT(pArg->IsNumeric());

	// handle field width & left justify flags
	char* pOutText = pOutBuffer + PadFieldWidth(pOutBuffer);
	const char* pSrcText = (IsFlagSet(CONVFLAG_UPPERCASE)) ? (bIsValueZero) ?  "FALSE" : "TRUE": (bIsValueZero) ? "false" : "true";
	MemoryCopy(pOutText,pSrcText,uNumChars);
	return GetOutputLength();
}

/*! ************************************

	\brief Output a data value as a binary bit pattern, MSB to LSB

	Relevant flags and inputs:	
	* * Width value - if width > output length, fill with spaces
	* * Left Justify flag - obvious flag is obvious
	* * Comma flag - inserts spaces between each nibbles worth of digits
	* * Alternative form - display bits LSB to MSB

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

WordPtr BURGER_API Burger::SafePrint::ParamInfo_t::FormatBinary(char* pOutBuffer,const SafePrintArgument *pArg) const
{
	// we can assume all inputs have been validated
	Word bCommaFlag = IsFlagSet(CONVFLAG_COMMA_SEPARATE);

	// Display the bits normally, or reversed
	Word bSwapBits = IsFlagSet(CONVFLAG_ALTERNATIVE_FORM);

	// Display the BYTES normally or reversed
	Word bBigEndian;

#if defined(BURGER_BIGENDIAN)
	bBigEndian = !bSwapBits;
#else
	bBigEndian = bSwapBits;
#endif

	// determine how many bytes to process and data characters we are to output
	WordPtr uByteCount = pArg->GetDataLengthInBytes();
	const Word8 *pBinaryData = static_cast<const Word8*>(pArg->GetDataAddress());

	// handle field width & left justify flags
	char* pBinDigits = pOutBuffer + PadFieldWidth(pOutBuffer);

	// sanity checks	
	BURGER_ASSERT(GetFormattedLength() == ((bCommaFlag) ? ((10U * uByteCount) - 1) : 8U * uByteCount));

	// Assume little endian

	WordPtr uOffset = uByteCount - 1U;
	IntPtr iDelta  = -1;

	// alternative form, we start at the Least significant byte, going towards the most
	if (bBigEndian) {
		uOffset = 0;
		iDelta = 1;
	}

	// convert data bytes into bits
	while (uByteCount) {
		Word8 uTheByte = pBinaryData[uOffset];
		Word8 uBitMask = static_cast<Word8>((bSwapBits) ? 0x01U : 0x80U);
		Word uDigitsWritten = 0;

		do {
			*pBinDigits++ = (uTheByte & uBitMask) ? '1' : '0';
			++uDigitsWritten;

			if (bCommaFlag) {
				if ((uDigitsWritten == 4) || ((uDigitsWritten == 8) && (uByteCount > 1))) {
					*pBinDigits++ = cBINARY_GROUP_SEPARATOR;
				}
			}

			// slide the bitmask over one bit
			if (bSwapBits) {
				uBitMask<<= 1U;
			} else {
				uBitMask>>= 1U;
			}
		} while (uBitMask);
		// move to next byte according to endian-ness 
		uOffset += iDelta;
		--uByteCount;
	}
	return GetOutputLength();
}

/*! ************************************

	\brief Print NaN's and Infinity
	
	Helper function to write out the text for special
	floating point values: NaN's and Infinity

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param uSpecialResult Special floating point number to print
	\return Number of characters printed

***************************************/

WordPtr BURGER_API Burger::SafePrint::ParamInfo_t::FormatSpecialReal(char* pOutBuffer,FPPrintInfo::eResult uSpecialResult) const
{
	// ISO Spec Infinity/NaN - look up the correct string
	WordPtr uIndex = static_cast<WordPtr>(uSpecialResult - FPPrintInfo::cResultFirstSpecial);
	const char *pText = (IsFlagSet(CONVFLAG_UPPERCASE)) ? FPPrintInfo::g_SpecialFloatsTextUpper[uIndex] : FPPrintInfo::g_SpecialFloatsText[uIndex];

	// copy out the special string to the output buffer
	WordPtr uCharsWritten = 0;
	while(*pText) {
		*pOutBuffer++ = *pText++;
		++uCharsWritten;
	}
	BURGER_ASSERT(uCharsWritten == GetFormattedLength());
	return GetOutputLength();
}

/*! ************************************

	\brief Outputs a floating point value

	Relevant flags and inputs:	
	* * Width value - if width > output length, fill with spaces
	* * Precision Value - minimum number of characters/digits to emit
	* * Left Justify flag - Left justifies the output
	* * Display sign - forces a sign to be displayed
	* * Blank sign - displays a blank in front of a positive number
	* * Left Pad Zeros - fills zeros in front of the number
	* * Comma flag - insert commas in front of every group of 3 digits

	Caveats:
	* * Left Justify overrides padding with zeros
	* * Comma Flag overrides padding with zeros

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

WordPtr BURGER_API Burger::SafePrint::ParamInfo_t::FormatReal(char *pOutBuffer,const SafePrintArgument *pArg) const
{
	// sanity checks	
	BURGER_ASSERT( pArg->IsReal() );

	// we can assume all inputs have been validated
	Word bCommaFlag = IsFlagSet(CONVFLAG_COMMA_SEPARATE);
	Word bBlankSign = IsFlagSet(CONVFLAG_BLANK_SIGN);

	// handle field width & left justify flags
	char* pOutText = pOutBuffer + PadFieldWidth(pOutBuffer);
	WordPtr uCharsWritten = 0;

	// all these will be initialized with cached off values 
	Word uIntDigits, uFracDigits, uSign, uDecimal, uPadDigits, uExponentDigits;
	Word uPadIntZeros, uPadFracZeros, uFirstNonZeroIntPos;
	FPPrintInfo::eResult uPrintResult;

	// get all the cached info on what we will be outputting
	GetFloatInfo(&uIntDigits,&uFracDigits,&uSign,&uDecimal,&uPadDigits,&uExponentDigits);
	GetFloatAnalysisInfo(&uPrintResult,&uPadIntZeros,&uPadFracZeros,&uFirstNonZeroIntPos);

	// Break the real number up into mantissa, exponent, etc
	FPInfo TheFPInfo;
	switch (pArg->GetType()) {
	case SafePrintArgument::ARG_HALF:
		TheFPInfo.InitHalf(pArg->m_Data.m_fHalf);
		break;
	case SafePrintArgument::ARG_FLOAT: 
		TheFPInfo.InitFloat(pArg->m_Data.m_fFloat);
		break;
	case SafePrintArgument::ARG_DOUBLE:
		TheFPInfo.InitDouble(pArg->m_Data.m_dDouble);
		break;
	default:	
		BURGER_ASSERT(FALSE);
		return 0;
	}

	// We check for the special cases (infinity, NANs) first
	if ((uPrintResult >= FPPrintInfo::cResultFirstSpecial) && (uPrintResult <= FPPrintInfo::cResultLastSpecial)) {
		FormatSpecialReal(pOutText,uPrintResult);
		return GetOutputLength();
	}

	// printing a sign character?
	if (TheFPInfo.IsNegative() || IsFlagSet(CONVFLAG_DISPLAY_SIGN) || bBlankSign) {
		uSign = 1;
	}

	// how many commas are we inserting?
	Word uCommaCount = (bCommaFlag && (uIntDigits > 3U)) ? (uIntDigits - 1U) / 3U : 0;

	// Is this a series of all '9's rounded up to '1' (plus zeros?)
	// Alternatively, is this a true zero value?
	if ((uPrintResult == FPPrintInfo::cResultFloatRoundedUpADigit) || (uPrintResult == FPPrintInfo::cResultZero)) {
		char* pTextPtr = pOutText;
		if (uSign) {
			*pTextPtr++ = static_cast<char>((TheFPInfo.IsNegative()) ? cNUMERIC_NEGATIVE_SIGN : (bBlankSign) ? cNUMERIC_BLANK_SIGN : cNUMERIC_POSITIVE_SIGN);
			++uCharsWritten;
		}

		// any leading zeros to pad the field?
		while (uPadDigits) {
			*pTextPtr++ = '0';
			++uCharsWritten;
			--uPadDigits;
		}

		// to do.. add numeric separator support
		BURGER_ASSERT(uIntDigits);
		Word uZerosToWrite = uIntDigits;

		if (uPrintResult == FPPrintInfo::cResultFloatRoundedUpADigit)	{
			*pTextPtr++ = '1';
			--uZerosToWrite;
			++uCharsWritten;
		}

		while (uZerosToWrite) {
			*pTextPtr++ = '0';
			--uZerosToWrite;
			++uCharsWritten;
		}

		if (uDecimal) {
			*pTextPtr++ = cNUMERIC_DECIMAL_POINT;
			++uCharsWritten;
		}

		while (uFracDigits) {
			*pTextPtr++ = '0';
			--uFracDigits;
			++uCharsWritten;
		}

		BURGER_ASSERT (uCharsWritten == GetFormattedLength());
		return GetOutputLength();
	}


	// setup a high precision object big enough to hold the number
	Word bitsNeeded = (1U << ( TheFPInfo.GetExponentBitCount()-1 ) ) + TheFPInfo.GetMantissaBitCount();
	GiantWord intPart(bitsNeeded);
	GiantWord fracPart(bitsNeeded);

	LoadHighPrecisionFromFloat(&TheFPInfo,&intPart,&fracPart);

	// we'll be writing this out right to left
	// either padDigits or commas will be zero
	char* pIntDigit = pOutText + uSign + uPadDigits + uIntDigits + uCommaCount - 1;
	char* pLastDigit = pIntDigit;

	// write out any sign character
	if (uSign) {
		*pOutText = static_cast<char>((TheFPInfo.IsNegative()) ? cNUMERIC_NEGATIVE_SIGN : (bBlankSign) ? cNUMERIC_BLANK_SIGN : cNUMERIC_POSITIVE_SIGN);
		++uCharsWritten;
	}

	// do we have any leading zeros to write out?
	if (uPadDigits) {
		for (Word n = 0; n < uPadDigits; n++ ) {
			pOutText[n+uSign] = '0';
			++uCharsWritten;
		}
	}

	// Output the integer portion a digit at a time, right to left
	Word uZeroFirstDigits = uPadIntZeros;		// needed for RTL cut-off emulation
	Word uIntDigitsReturned = 0;
	Word uDigitsInGroup = 0;
	Word uFracDigitsReturned = 0;

	do {
		// sanity check
		BURGER_ASSERT(pIntDigit >= pOutText);

		char theDigit = static_cast<char>(intPart.DivideAndReturnRemainder(10));
		++uIntDigitsReturned;

		if (uZeroFirstDigits) {
			theDigit = 0;
			--uZeroFirstDigits;
		}

		// need to put a group separator in before (after) the digit?
		if (bCommaFlag && (uDigitsInGroup == 3)) {
			*pIntDigit-- = cNUMERIC_GROUP_SEPARATOR;
			++uCharsWritten;
			uDigitsInGroup = 0;
		}

		// do we need to grab the pointer to this digit for later rounding up?
		if (uIntDigitsReturned == uFirstNonZeroIntPos) {
			pLastDigit = pIntDigit;
		}

		*pIntDigit = static_cast<char>('0' + theDigit);
		--pIntDigit;

		++uDigitsInGroup;
		++uCharsWritten;
	} while (intPart.IsNotZero());

	BURGER_ASSERT(uIntDigits == uIntDigitsReturned);

	// do we output a decimal point?   if we have fractional digits, we must
	if (uDecimal) {
		// compute the position of the decimal point, and write it out
		char* pDecimalPt = pOutText + uSign + uPadDigits + uIntDigits + uCommaCount;
		*pDecimalPt = cNUMERIC_DECIMAL_POINT;
		++uCharsWritten;

		// see how many fractional digits to actually calculate
		Word uRealFracDigits = uFracDigits - uPadFracZeros;
		char* pFracDigit = ++pDecimalPt;

		// if we have fractional digits, then output them left to right
		if (uRealFracDigits) {
			while (uFracDigitsReturned < uRealFracDigits) {
				char theDigit = static_cast<char>(fracPart.MulReturnOverflow(10));
				++uFracDigitsReturned;

				// note last written digit position if we have to round up
				pLastDigit = pFracDigit;

				*pFracDigit++ = static_cast<char>('0' + theDigit);
				++uCharsWritten;
			}
		}

		// for RTL cutoff emulation, we pad zeros on the end after the cut-off
		while (uPadFracZeros) {
			*pFracDigit++ = '0';
			++uCharsWritten;
			--uPadFracZeros;
		}
	}

	// do we need to round the result up?  if so we have a pointer to
	// the rightmost significant digit.  We will increment that digit,
	// if there is a carry, move left, ignoring separators & decimals
	// until nothing carries.  We don't worry about it adding a digit to
	// the beginning of the number, as the 'all 9s' case has been
	// accounted for separately.
	if (uPrintResult == FPPrintInfo::cResultFloatRoundedUpAtEnd) {
		while(pLastDigit) {		// suppress warning by checking pointer...
			Word8 theDigit = static_cast<Word8>(*pLastDigit - '0');

			BURGER_ASSERT (theDigit <= 9U);

			// '0' through '8' means increment digit and we are done
			if (theDigit != 9) {
				// failsafe in case we find something that's not a number (shouldn't happen)
				if (theDigit < 9U) {
					*pLastDigit = static_cast<char>('0' + theDigit + 1);
				}
				break;
			} else {
				// the digit is a '9', which means we increment, get '10', write the '0'
				// and carry over the 1 to the next digit to the left
				*pLastDigit = '0';
				--pLastDigit;			// back up a digit

				// skip over the decimal point or any group separators
				if ((*pLastDigit == static_cast<char>(cNUMERIC_DECIMAL_POINT)) || (*pLastDigit == static_cast<char>(cNUMERIC_GROUP_SEPARATOR))) {
					pLastDigit--;
				}
			}

			// sanity check
			BURGER_ASSERT (pLastDigit >= pOutText);
		}
	}

	BURGER_ASSERT(uCharsWritten == GetFormattedLength());
	return GetOutputLength();
}








/*! ************************************

	\struct Burger::SafePrint::ProcessResults_t
	
	\brief struct to hold the results of parsing.
	
	struct to hold the results of the parsing phase and
	output phases of a formatted output operation

***************************************/

/*! ************************************

	\brief Error handler

	Your opportunity to do additional error handling
	and reporting.  The format string and parameters that were passed in
	are provided to this function so that detailed information on exactly
	what the error was 

	\param pFormatString Pointer to the original format string
	\param uArgCount Count of arguments in array
	\param ppArgs Array of parsed arguments
	\return Error code (Always negative)

***************************************/

IntPtr BURGER_API Burger::SafePrint::ProcessResults_t::ErrorHandler(const char* pFormatString,WordPtr uArgCount,const SafePrintArgument** ppArgs)
{
	BURGER_UNUSED(pFormatString);
	BURGER_UNUSED(uArgCount);
	BURGER_UNUSED(ppArgs);

	// Uncomment to get a text description of the error that occurred
	//const char* pErrorMsgText = GetErrorDescription(m_uPhaseResults);

	// return the error value to be returned to the original caller
	return static_cast<IntPtr>(m_uPhaseResults);
}

/*! ************************************

	\brief Output argument text

	Given an argument, the output buffer is sanity checked and then
	the argument is converted to text and stored in the output buffer.

	\param pOutputBuffer Pointer to the output buffer
	\param uOutputBufferSize Number of bytes in the output buffer
	\param pParamInfo Pointer to the structure with the information on how to print out the argument
	\param pArgument Pointer to the structure with the argument data
	\return Number of bytes outputted

***************************************/

WordPtr BURGER_API Burger::SafePrint::ProcessResults_t::FormatArgument(char *pOutputBuffer,WordPtr uOutputBufferSize,const ParamInfo_t *pParamInfo,const SafePrintArgument *pArgument)
{
	WordPtr uCharCount = 0;

	// Sanity check
	if (IsPointerInvalid(pOutputBuffer) || IsPointerInvalid(pParamInfo) || IsPointerInvalid(pArgument) || !uOutputBufferSize) {	
		m_uPhaseResults = ERROR_BAD_INPUT_PARAMETER;
	
	} else {
		eConversionSpecifier uConversionSpecifier = pParamInfo->GetConversion();
		SafePrintArgument::eType uArgumentType = pArgument->GetType();
		if ((uConversionSpecifier == CONVSPEC_INVALID) || (uConversionSpecifier >= CONVSPEC_COUNT)) {
			m_uPhaseResults = ERROR_INVALID_CONVERSION_TYPE;

		} else {

			Word32 uConversionAsFlag = pParamInfo->GetConversionAsFlag();
			if (!(g_ValidOutputDisplayTypesStrict[uArgumentType] & uConversionAsFlag) &&
				!(g_ValidOutputDisplayTypesRelaxed[uArgumentType] & uConversionAsFlag)) {
				m_uPhaseResults = ERROR_INCOMPATIBLE_ARG;
			} else {

				// check remaining output width vs formatted argument length

				switch (uConversionSpecifier) {
				case CONVSPEC_BINNUMBER:
					uCharCount = pParamInfo->FormatBinary(pOutputBuffer,pArgument);
					break;
					
				case CONVSPEC_BOOLTEXT:
					uCharCount = pParamInfo->FormatBool(pOutputBuffer,pArgument);
					break;
				
				case CONVSPEC_CHAR:
					uCharCount = pParamInfo->FormatChar(pOutputBuffer,pArgument);
					break;
				
				case CONVSPEC_DECIMAL_INT:
				case CONVSPEC_DECIMAL_WORD:
					uCharCount = pParamInfo->FormatInteger(pOutputBuffer,pArgument);
					break;
				
				case CONVSPEC_DECIMAL_FLOAT:
				case CONVSPEC_SCI_NOTE_FLOAT:
				case CONVSPEC_SHORTER_FLOAT:
					uCharCount = pParamInfo->FormatReal(pOutputBuffer,pArgument);
					break;
				
				case CONVSPEC_OCTAL:
					uCharCount = pParamInfo->FormatHexOrOctal(pOutputBuffer,pArgument);
					break;
				
				case CONVSPEC_HEX:
					uCharCount = pParamInfo->FormatHexOrOctal(pOutputBuffer,pArgument);
					break;
				
				case CONVSPEC_POINTER:
					uCharCount = pParamInfo->FormatPointer(pOutputBuffer,pArgument);
					break;
				
				case CONVSPEC_TEXT_STRING:
					uCharCount = pParamInfo->FormatTextString(pOutputBuffer,pArgument);
					break;
				
				default:
					break;
				}
			}
		}
	}
	return uCharCount;
}

/*! ************************************

	\brief Output formatted text

	Given a format string and a list of processed arguments, 
	output the final string into the output buffer. If there are issues
	with the output or if the buffer is exhausted, parsing terminates.

	\param pOutputBuffer Pointer to the output buffer
	\param uOutputBufferSize Number of bytes in the output buffer
	\param uArgCount Number of available arguments
	\param ppArgs Pointer to the array of pointers to arguments
	\param uParamInfoCount Number of parameter commands to process
	\param pParamInfos Pointer to an array of parameter commands

	\return Number of bytes outputted

***************************************/

WordPtr BURGER_API Burger::SafePrint::ProcessResults_t::GenerateFormatOutputToBuffer(char *pOutputBuffer,WordPtr uOutputBufferSize,WordPtr uArgCount,const SafePrintArgument** ppArgs,WordPtr uParamInfoCount,const ParamInfo_t *pParamInfos)
{
	BURGER_UNUSED(uArgCount);
	WordPtr uCharCount = 0;

	// Sanity checks.  It should be impossible to call this function with any of these bad
	BURGER_ASSERT(IsPointerValid(pOutputBuffer) && uOutputBufferSize);
	BURGER_ASSERT(uParamInfoCount && IsPointerValid(pParamInfos));
	BURGER_ASSERT(!(uArgCount && IsPointerInvalid(ppArgs)));

	// go through all the output commands we got from the preprocessing phase

	for (WordPtr x = 0; x < uParamInfoCount; x++ ) {
		switch(pParamInfos->GetCommand()) {
		case COMMAND_COPY_TEXT:
			{
				WordPtr uBytesToCopy = pParamInfos->GetOutputLength();
				const char* pSrcText = pParamInfos->m_pText;

				if ((uCharCount + uBytesToCopy) > uOutputBufferSize ) {
					uBytesToCopy = uOutputBufferSize - uCharCount;
				}

				// can we do it fast?
				if (pParamInfos->IsFlagSet(CONVFLAG_HAS_SEQUENCE_CHAR)) {
					// nope.  copy a character at a time, looking for the '%%' sequence
					char* pOutText = &pOutputBuffer[uCharCount];
					while (uBytesToCopy) {
						char c = *pSrcText++;
						if (c == '%') {
							pSrcText++;		// skip second %
						}
						*pOutText++ = c;
						--uBytesToCopy;
						++uCharCount;
					}
				} else {
					// fast path, we don't have '%%' sequences to convert
					MemoryCopy(pOutputBuffer + uCharCount,pSrcText,uBytesToCopy);
					uCharCount += uBytesToCopy;
				}
				break;
			}

		case COMMAND_FORMAT_ARGUMENT:
			{
				WordPtr uCharsAdded = FormatArgument(&pOutputBuffer[uCharCount],uOutputBufferSize-uCharCount,pParamInfos,ppArgs[pParamInfos->GetArgIndex()] );
				if (m_uPhaseResults != ERROR_NONE ) {
					return uCharCount;
				}
				uCharCount += uCharsAdded;
				break;
			}

		default:
			{
				m_uPhaseResults = ERROR_INVALID_OUTPUT_CMD;
				m_uErrorCharPos = uCharCount;
				return uCharCount;
			}
		}

		// did we fill up the output buffer?
		if (uCharCount >= uOutputBufferSize) {
			break;
		}
		++pParamInfos;
	}
	return uCharCount;
}

/*! ************************************

	\brief Write data to ANSI FILE record

	Write a data buffer to a FILE

	\param fp Pointer to the FILE record
	\param bUsingSTDOUT Set to \ref TRUE if the FILE record is stdout or stdin
	\param pBuffer Pointer to the buffer to write to the FILE
	\param uBufferSize Number of bytes in the buffer to write
	\param uCharsSoFar Number of bytes written so far (Needed for error updating)

	\return \ref TRUE if successful, \ref FALSE on failure

***************************************/

Word BURGER_API Burger::SafePrint::ProcessResults_t::WriteBufferToFile(FILE *fp,Word bUsingSTDOUT,const char *pBuffer,WordPtr uBufferSize,WordPtr uCharsSoFar)
{
	Word uResult = TRUE;
	// somehow get asked to send nothing?
	if (uBufferSize) {

		BURGER_ASSERT(IsPointerValid(pBuffer));

		// We write the buffer using fwrite.  In the case of printf, outputFile will be set to stdin 
		WordPtr uCharsPassed = fwrite(pBuffer,1,uBufferSize,fp);

		// did the RTL function report something other than expected success?
		if (uCharsPassed != uBufferSize) {
			m_uPhaseResults = (bUsingSTDOUT) ? ERROR_WRITING_STDOUT : ERROR_WRITING_TO_FILE;
			m_uErrorCharPos = uCharsSoFar + uCharsPassed;
			uResult = FALSE;
		}
	}
	// guess we're ok
	return uResult;
}

/*! ************************************

	\brief Output formatted text to a file

	Given a format string and a list of processed arguments, 
	output the final string into a FILE. If there are issues
	with the output or if the buffer is exhausted, parsing terminates.

	\param fp Pointer to the FILE
	\param bUsingSTDOUT \ref TRUE if stdin or stdout, \ref FALSE if a different stream or file
	\param uArgCount Number of available arguments
	\param ppArgs Pointer to the array of pointers to arguments
	\param uParamInfoCount Number of parameter commands to process
	\param pParamInfos Pointer to an array of parameter commands

	\return Number of bytes outputted

***************************************/

WordPtr BURGER_API Burger::SafePrint::ProcessResults_t::GenerateFormattedOutputToFile(FILE *fp,Word bUsingSTDOUT,WordPtr uArgCount,const SafePrintArgument **ppArgs,WordPtr uParamInfoCount,const ParamInfo_t* pParamInfos)
{
	BURGER_UNUSED(uArgCount);
	WordPtr uCharCount = 0;

	// Sanity checks.  It should be impossible to call this function with any of these bad
	BURGER_ASSERT( !(!bUsingSTDOUT && IsPointerInvalid(fp)));
	BURGER_ASSERT( uParamInfoCount && IsPointerInvalid(pParamInfos));
	BURGER_ASSERT( !(uArgCount && IsPointerInvalid(ppArgs)));

	// get a work buffer on the stack	
	char Buffer[cMAX_CONVERSION_OUTPUT_SIZE+1];

	// go through all the output commands we got from the preprocessing phase
	for (WordPtr x = 0; x < uParamInfoCount; x++) {
		switch(pParamInfos->GetCommand()) {
		case COMMAND_COPY_TEXT:
			{
				WordPtr uBytesToCopy = pParamInfos->GetOutputLength();
				const char* pSrcText = pParamInfos->m_pText;

				// can we do it fast?
				if (pParamInfos->IsFlagSet(CONVFLAG_HAS_SEQUENCE_CHAR)) {
					// nope.  we'll have to copy as much of the text to our work buffer as we can,
					// converting the '%%' to '%' as we go, then sending it to the output device
					// when done or when we fill up the buffer

					WordPtr uSrcBytesRemaining = uBytesToCopy;
					WordPtr uBufferUsed = 0;

					while (uSrcBytesRemaining) {
						// copy a block of text to our work buffer
						while ((uBufferUsed < cMAX_CONVERSION_OUTPUT_SIZE) && uSrcBytesRemaining) {
							char c = *pSrcText++;
							if (c == '%') {
								++pSrcText;				// skip second %
								--uSrcBytesRemaining;	// keep count correct
							}
							Buffer[uBufferUsed++] = c;
							--uSrcBytesRemaining;
						}

						// write out what we've got in the work buffer
						if (uBufferUsed) {
							if (!WriteBufferToFile(fp,bUsingSTDOUT,pSrcText,uBufferUsed,uCharCount)) {
								return uCharCount;
							}

							uCharCount += uBufferUsed;		// update stats
							uBufferUsed = 0;				// reset buffer
						}
					}
				} else {		// fast path, we don't have '%%' sequences to convert
					if (!WriteBufferToFile(fp,bUsingSTDOUT,pSrcText,uBytesToCopy,uCharCount)) {
						return uCharCount;
					}
					uCharCount += uBytesToCopy;
				}
				break;
			}

		case COMMAND_FORMAT_ARGUMENT:
			{
				// write the formatted argument to our work buffer
				WordPtr uCharsAdded = FormatArgument(Buffer,cMAX_CONVERSION_OUTPUT_SIZE,pParamInfos,ppArgs[pParamInfos->GetArgIndex()] );

				if (m_uPhaseResults != ERROR_NONE) {
					return uCharCount;
				}

				// this sucks because if a string passed into '%s' is greater than the work buffer size, we have to fail
				// idea: maybe we could special case it?
				if (uCharsAdded > cMAX_CONVERSION_OUTPUT_SIZE) {
					m_uPhaseResults = ERROR_CONVERSION_OUTPUT_TOO_LARGE;
					return uCharCount;
				}

				Buffer[uCharsAdded] = 0;

				// Send the formatted output to stdout or the FILE*
				if (!WriteBufferToFile(fp,bUsingSTDOUT,Buffer,uCharsAdded,uCharCount)) {
					return uCharCount;
				}
				uCharCount += uCharsAdded;
				break;
			}

		default:
			m_uPhaseResults = ERROR_INVALID_OUTPUT_CMD;
			m_uErrorCharPos = uCharCount;
			return uCharCount;
		}
		++pParamInfos;
	}
	return uCharCount;
}


/*! ************************************

	\brief Analyzes a format string, and creates a list 
		of output commands that would created the output using it

	Output commands indicated either a section of text to literally* copy, 
	or an argument to evaluate and convert (format).
  
	Arguments and their corresponding conversion sequences are checked for 
	syntactic errors and type compatibility

	It can optionally suppress all argument checking - useful for quick error 
	checking externally supplied format strings at load time

	Returns results in \ref ProcessResults_t :
	* * Success in parsing the format string ( and arguments )
	* * Accurate total OR estimated length of formatted output in bytes
	* * Number of output commands created
	* * Info on the format sequence that caused an error (if any)

	\param uOptions
	\param pFormat
	\param uFormatLength
	\param uArgCount Number of available arguments
	\param ppArgs Pointer to the array of pointers to arguments
	\param uParamInfoCount Number of parameter commands to process
	\param pParamInfos Pointer to an array of parameter commands

	\return \ref TRUE if successful in parsing, \ref FALSE if there was an error

***************************************/

Word BURGER_API Burger::SafePrint::ProcessResults_t::FormatPreProcess(eParseOptions uOptions,const char *pFormat,WordPtr uFormatLength,WordPtr uArgCount,const SafePrintArgument** ppArgs,WordPtr uParamInfoCount,ParamInfo_t* pParamInfos)
{
	// Initialize return values

	m_uPhaseResults = ERROR_UNSPECIFIED;
	m_uFormattedOutputLength = 0;
	m_uNumOutputCommands = 0;
	m_uErrorFormatSequencePos = UINTPTR_MAX;
	m_uErrorCharPos = UINTPTR_MAX;

	// Check format string for validity. An empty string (just a null) is ok
	if (IsPointerInvalid(pFormat)) {
		m_uPhaseResults = ERROR_BAD_INPUT_PARAMETER;
		return false;
	}

	// Check the format string for size problems
	if (uFormatLength >= cMAX_TOTAL_OUTPUT_SIZE) {
		m_uPhaseResults = ERROR_FORMAT_STRING_TOO_LONG;
		return false;
	}

	// check arguments for validity.  Should never see an error here, but this catches new bugs introduced
	if ((uOptions&PARSEOPTION_CHECK_ARG_TYPES) && uArgCount) {
		// do we have have crap data passed in?  should be impossible to trigger this error
		if (uArgCount && IsPointerInvalid(ppArgs)) {
			m_uPhaseResults = ERROR_BAD_INPUT_PARAMETER;
			return false;
		}

		// check each argument to make sure the CprintfArg object looks ok
		for (WordPtr i = 0; i < uArgCount; i++ ) {
			if (IsPointerInvalid(ppArgs[i]) || (ppArgs[i]->GetType() == SafePrintArgument::ARG_INVALID) || (ppArgs[i]->GetType() >= SafePrintArgument::ARG_COUNT)) {
				m_uPhaseResults = ERROR_BAD_ARGUMENT_TYPE;
				return false;
			}
		}
	}

	// Initialize the parsing engine
	WordPtr uParsePosition = 0;					// index of character currently being parsed
	WordPtr uFormatSequencePosition = 0;		// index of 1st character of the argument format section being parsed
	WordPtr uCurrentCommandIndex = UINTPTR_MAX;	// index of the current output command being composed
	Word32 uCurrentArgumentNext = 0;			// index of the next argument to use for formatting

	Word bParsingParam = false;					// parsing mode: decoding an argument format section
	Word bParsingText = false;					// parsing mode: scanning literal text to copy

	Word bArgIndexSpecified = false;			// arg parsing: has an explicit argument index been specified?
	Word bParsingNumVal = false;				// arg parsing: are we parsing a numeric value?
	Word bCurNumValueValid = false;				// arg parsing: do we have a valid numeric value to use
	Word32 uCurrentNumberValue = 0;				// arg parsing: value of the number begin parsed

	WordPtr	uTotalLength = 0;					// also here for the same reason as i

	eParseStage	stage = PARSE_START;
	eError uErrorCode = ERROR_NONE;

	// loop through format string one character at a time
	while (uParsePosition < uFormatLength) {
		Word8 c = static_cast<const Word8 *>(static_cast<const void *>(pFormat))[uParsePosition];

		// Are we evaluating an argument format section?
		if (!bParsingParam) {
			// Is it a normal text character?
			if (c != cFORMAT_ESCAPE_CHAR) {
				// do we need to start a new literal text copy command
				if (!bParsingText) {
					++uCurrentCommandIndex;
					if ( uCurrentCommandIndex >= uParamInfoCount ) {
						uErrorCode = ERROR_TOO_MANY_FORMAT_SEQUENCES;
						goto parse_error;
					}

					pParamInfos[uCurrentCommandIndex].SetCommand(COMMAND_COPY_TEXT);
					pParamInfos[uCurrentCommandIndex].ClearFlags();
					pParamInfos[uCurrentCommandIndex].m_pText = &pFormat[uParsePosition];
					pParamInfos[uCurrentCommandIndex].SetOutputLength(0);
					bParsingText = true;
				}

				// add another char to copy
				pParamInfos[ uCurrentCommandIndex ].SetOutputLength(pParamInfos[ uCurrentCommandIndex ].GetOutputLength()+1);
			} else {	 
				// hit the formatting escape char ( '%' )?
				// were we previously doing a literal text copy? 
				// if so, (and it's not a '%%') we close out the previous literal text copy.. 
				//
				// if this turns out to be the literal '%' character via '%%', we flag the
				// text copy so it know it has to convert the sequence when copying

				// check for edge case:  a single '%' as the last character in the format string
				if ((uParsePosition+1) == uFormatLength) {
					uErrorCode = ERROR_ESCAPE_IS_LAST_CHAR;
					goto parse_error;
				}

				// We need to snoop ahead and see if we're doing the escape 
				// sequence to output the literal text

				// do we have a literal '%' to output
				if (pFormat[uParsePosition+1] == static_cast<char>(cFORMAT_ESCAPE_CHAR)) {
					// first character in the format string? or right after a conversion command?
					// if so, we need to make a new command
					if ((uCurrentCommandIndex == UINTPTR_MAX) || (pParamInfos[uCurrentCommandIndex].GetCommand() == COMMAND_FORMAT_ARGUMENT)) {
						BURGER_ASSERT(!bParsingText);

						// emit text copy command
						++uCurrentCommandIndex;
						if (uCurrentCommandIndex >= uParamInfoCount ) {
							uErrorCode = ERROR_TOO_MANY_FORMAT_SEQUENCES;
							goto parse_error;
						}

						pParamInfos[uCurrentCommandIndex].SetCommand(COMMAND_COPY_TEXT);
						pParamInfos[uCurrentCommandIndex].ClearFlags();
						pParamInfos[uCurrentCommandIndex].m_pText = &pFormat[uParsePosition];
						pParamInfos[uCurrentCommandIndex].SetOutputLength(0);

						bParsingText = true;
					}

					// Mark the output command as needing to check for the '%%' sequence
					pParamInfos[ uCurrentCommandIndex ].SetFlag( CONVFLAG_HAS_SEQUENCE_CHAR );
					// we'll only be outputting 1 char (but reading 2)
					pParamInfos[ uCurrentCommandIndex ].SetOutputLength(pParamInfos[ uCurrentCommandIndex ].GetOutputLength()+1);
					// skip over second '%' char in escape sequence
					uParsePosition++;
				} else {
					// ok, we have the start of an argument formatting / conversion sequence
					// begin a new argument formatting command
					// default it to the current arg which could be invalid but
					// also can be specified later
					++uCurrentCommandIndex;
					if (uCurrentCommandIndex >= uParamInfoCount ) {
						uErrorCode = ERROR_TOO_MANY_FORMAT_SEQUENCES;
						goto parse_error;
					}

					pParamInfos[ uCurrentCommandIndex ].SetCommand(COMMAND_FORMAT_ARGUMENT);
					pParamInfos[ uCurrentCommandIndex ].SetConversion(CONVSPEC_INVALID);
					pParamInfos[ uCurrentCommandIndex ].SetArgIndex(uCurrentArgumentNext);
					pParamInfos[ uCurrentCommandIndex ].ClearFlags();
					pParamInfos[ uCurrentCommandIndex ].SetWidth(0);
					pParamInfos[ uCurrentCommandIndex ].SetPrecision(0);
					pParamInfos[ uCurrentCommandIndex ].SetFormattedLength(0);
					pParamInfos[ uCurrentCommandIndex ].SetOutputLength(0);

					// initialize argument parsing state
					uFormatSequencePosition = uParsePosition;
					bParsingNumVal = false;
					bCurNumValueValid = false;
					bArgIndexSpecified = false;
					bParsingParam = true;
					uCurrentNumberValue = 0;

					// reset our state to not text parsing
					bParsingText = false;

					stage = PARSE_START;
				}
			}
		} else {
			// we are parsing an argument formatting sequence ( bParsingParam == true )
			eOpcode uOpCode = ( c < 127 ) ? g_CodeMap[c] : OP_NOT_A_SYMBOL;

			// Did we get a character that is invalid in a format sequence?
			if ((uOpCode == OP_NOT_A_SYMBOL) || (uOpCode == OP_LITERAL_PERCENT)) {
				uErrorCode = ERROR_ILLEGAL_CHARACTER_FORMAT_SEQUENCE;
				goto parse_error;
			}

			// are we parsing a number?
			if (uOpCode == OP_NUMERIC_VALUE) {
				// is this the first numeric character?
				if (!bParsingNumVal) {
					// is this a good time to be seeing numbers?
					if (stage >= PARSE_LENGTH) {
						uErrorCode = ERROR_NUMERIC_CHAR_IN_WRONG_PLACE;
						goto parse_error;
					}

					// Starting out with a zero?
					if ((c == '0') && stage < PARSE_WIDTH ) {
						// we could check for a double zero here (but it is valid)
						// leading 0 means to left pad with zeros 
						// (we'll undo this if this turns out to be the argument index)
						pParamInfos[ uCurrentCommandIndex ].SetFlag( CONVFLAG_LEFT_PAD_ZEROS );
					} else {
						// starting out with a non-zero number?
						// now we are extracting a numeric value
						bParsingNumVal = true;
						bCurNumValueValid = true;
						uCurrentNumberValue = 0;
					}
				}

				if ( bParsingNumVal ) {
					// if the numeric sequence has gotten too big to be valid for arg #, width or precision,
					// stop adding digits but don't report error here - we let other code determine which value is
					// being parsed (we may not know yet) and let that code provide the correctly detailed error code
					if (uCurrentNumberValue <= cMAX_TOTAL_OUTPUT_SIZE ) {
						uCurrentNumberValue = (uCurrentNumberValue * 10U) + static_cast<Int>(c - '0');		// add digit
					}
				}
			} else {
				bParsingNumVal = false;	// it's not a number, so turn off numeric value parsing mode

				// are we specifying the argument to use?
				if ( uOpCode == OP_ARG_SPECIFIER ) {

					if ( stage > PARSE_ARGNUM ) {
						uErrorCode = ERROR_ARG_INDEX_IN_WRONG_PLACE;
						goto parse_error;
					}

					// already specified the index?
					if ( bArgIndexSpecified ) {
						uErrorCode = ERROR_ARG_INDEX_DOUBLY_SPECIFIED;
						goto parse_error;
					}

					// do we have a valid index value?
					if (!bCurNumValueValid) {
						uErrorCode = ERROR_ARG_INDEX_NOT_SPECIFIED;
						goto parse_error;
					}

					if ((uCurrentNumberValue < 1) || (uCurrentNumberValue > uArgCount) ) {
						uErrorCode = ERROR_ARG_INDEX_INVALID;
						goto parse_error;
					}

					// change the current argument number to the one specified
					uCurrentArgumentNext = uCurrentNumberValue-1;
					pParamInfos[ uCurrentCommandIndex ].SetArgIndex( uCurrentArgumentNext );

					bArgIndexSpecified = true;	// note argument index as explicitly specified
					bCurNumValueValid = false;	// we used up the numeric value

					// reset the zero padding flag if set by mistake
					pParamInfos[ uCurrentCommandIndex ].ClearFlags();

					// update the parsing stage
					stage = PARSE_ARGNUM;

				}

				// check for various formatting flags ( left justify, sign, base prefix, etc)
				if ((uOpCode >= OP_FLAGS_BEGIN) && (uOpCode <= OP_FLAGS_END)) {
					// do we have numeric value preceding the flags or
					if ((stage > PARSE_FLAGS) || bCurNumValueValid) {
						uErrorCode = ERROR_OPTION_FLAG_IN_WRONG_PLACE;
						goto parse_error;
					}

					// Get the corresponding flag, see if we did it twice?
					eConversionFlags optionFlag = g_PrintFlagMappings[ uOpCode - OP_FLAGS_BEGIN ];

					if ( pParamInfos[ uCurrentCommandIndex ].IsFlagSet( optionFlag ) ) {
						uErrorCode = ERROR_OPTION_FLAG_DOUBLY_SPECIFIED;
						goto parse_error;
					}

					// Record flag
					pParamInfos[ uCurrentCommandIndex ].SetFlag( optionFlag );
					stage = PARSE_FLAGS;
				}

				// did we hit the precision specifier?
				if ( uOpCode == OP_PRECISION_SPECIFIER ) {
					// seen it before?
					if ( pParamInfos[ uCurrentCommandIndex ].IsFlagSet( CONVFLAG_PRECISION_MARKER ) ) {
						uErrorCode = ERROR_PRECISION_DOUBLY_SPECIFIED;
						goto parse_error;
					}

					// is it out of proper sequence?
					if ( stage >= PARSE_PRECISION ) {
						uErrorCode = ERROR_PRECISION_IN_WRONG_PLACE;
						goto parse_error;
					}

					// do we have a width to save off?
					if ( bCurNumValueValid ) {
						uErrorCode = pParamInfos[uCurrentCommandIndex].StoreOffNumericValue(stage,uCurrentNumberValue);
						if (uErrorCode != ERROR_NONE) {
							goto parse_error;
						}
						bCurNumValueValid = false;
					}

					// record that we found a "." (even if no number after it)
					pParamInfos[ uCurrentCommandIndex ].SetFlag( CONVFLAG_PRECISION_MARKER );
					stage = PARSE_PRECISION;
				}

				// Did we get a length specifier
				if ( uOpCode >= OP_LENGTH_BEGIN && uOpCode <= OP_LENGTH_END ) {
					// do we have a width or precision to store off?
					if ( bCurNumValueValid ) {
						uErrorCode = pParamInfos[uCurrentCommandIndex].StoreOffNumericValue(stage,uCurrentNumberValue);
						if (uErrorCode != ERROR_NONE) {
							goto parse_error;
						}
						bCurNumValueValid = false;
					}

					//  'hh' and 'll' can be specified but no other double letter combo
					if ( stage >= PARSE_LENGTH ) {
						// 'hh' length specifier?
						if ( stage == PARSE_LENGTH && 
							uOpCode == OP_LENGTH_SHORT && pParamInfos[ uCurrentCommandIndex ].IsFlagSet( CONVFLAG_INTERPRET_SHORT ) ) {
							pParamInfos[ uCurrentCommandIndex ].ClearFlag( CONVFLAG_INTERPRET_SHORT );
							pParamInfos[ uCurrentCommandIndex ].SetFlag( CONVFLAG_INTERPRET_SSHORT );
						
							// 'll' length specifier?
						} else if ( stage == PARSE_LENGTH && 
							uOpCode == OP_LENGTH_LONG && pParamInfos[ uCurrentCommandIndex ].IsFlagSet( CONVFLAG_INTERPRET_LONG ) ) {
							pParamInfos[ uCurrentCommandIndex ].ClearFlag( CONVFLAG_INTERPRET_LONG );
							pParamInfos[ uCurrentCommandIndex ].SetFlag( CONVFLAG_INTERPRET_LLONG );
						} else {
							// must be an invalid length specifier combo
							uErrorCode = ERROR_ARG_LENGTH_FLAG_DOUBLY_SPECIFIED;
							goto parse_error;
						}
					} else {
						// found our first length specifier
						pParamInfos[ uCurrentCommandIndex ].SetFlag(g_PrintfLengthMappings[ uOpCode - OP_LENGTH_BEGIN ]);

						// Note that we found an explicit length modifier
						pParamInfos[ uCurrentCommandIndex ].SetFlag( CONVFLAG_INTERPRET_SPECIFIED );
						stage = PARSE_LENGTH;
					}
				}

				// Finally... did we get a conversion specifier?
				if ((uOpCode >= OP_OUTPUT_BEGIN) && (uOpCode <= OP_OUTPUT_END)) {
					// are we checking the arguments? (or just the format string)
					if (uOptions& PARSEOPTION_CHECK_ARG_TYPES) {
						// Is the argument # to use valid?
						if ( uCurrentArgumentNext >= uArgCount ) {
							uErrorCode = ( bArgIndexSpecified ) ? ERROR_ARG_INDEX_INVALID : ERROR_INSUFFICIENT_ARGUMENTS;
							goto parse_error;
						}
					}

					// do we have a width or precision to store off?
					if (bCurNumValueValid) {
						uErrorCode = pParamInfos[uCurrentCommandIndex].StoreOffNumericValue(stage,uCurrentNumberValue);
						if (uErrorCode != ERROR_NONE) {
							goto parse_error;
						}
						bCurNumValueValid = false;
					}

					// are we checking the arguments? (or just the format string)
					if (uOptions&PARSEOPTION_CHECK_ARG_TYPES) {
						// do we determine the type?
						if ( uOpCode == OP_OUTPUT_DEFAULT ) {
							uOpCode = GetDefaultArgumentType(ppArgs[uCurrentArgumentNext]);
							if (uOpCode==OP_NOT_A_SYMBOL) {
								// Should never get this unless someone forgot to provide one in code
								uErrorCode = ERROR_DEFAULT_CONVERSION_NOT_FOUND;
								goto parse_error;
							}
						}

						// Get the display type, and any modifier flags
						pParamInfos[ uCurrentCommandIndex ].SetConversion( g_PrintfOutputType[ uOpCode - OP_OUTPUT_BEGIN ].m_uOutputDisplayType );
						pParamInfos[ uCurrentCommandIndex ].SetFlag( g_PrintfOutputType[ uOpCode - OP_OUTPUT_BEGIN ].m_uOutputFlagsModifier );

						// validate the output type is compatible with the argument.
						SafePrintArgument::eType uParameterType	= ppArgs[ uCurrentArgumentNext ]->GetType();						// get the type of paramter we are going format (ArgType::xxx)
						Word32 uDisplayTypeFlag = pParamInfos[ uCurrentCommandIndex ].GetConversionAsFlag();	// get the output display type in bitfield form

						// we have 2 different levels of format compatibility
						Word bStrictOk = (uOptions& PARSEOPTION_STRICT_OUTPUT_TYPES_OK) ? (g_ValidOutputDisplayTypesStrict[ uParameterType ] & uDisplayTypeFlag)!=0 : false;
						Word bRelaxedOk = (uOptions& PARSEOPTION_RELAXED_OUTPUT_TYPES_OK) ? (g_ValidOutputDisplayTypesRelaxed[ uParameterType ]& uDisplayTypeFlag)!=0 : false;

						if (!bStrictOk && !bRelaxedOk) {
							uErrorCode = ERROR_INCOMPATIBLE_ARG;
							goto parse_error;
						}

						// Now check the flags for this conversion
						pParamInfos[ uCurrentCommandIndex ].CheckConversionFlags();

						// Check the conversion for any iffy things we want to issue warnings for
						pParamInfos[ uCurrentCommandIndex ].CheckConversionForWarnings(ppArgs[uCurrentArgumentNext]);

						// Might as well determine the output length of the formatted argument here....
						pParamInfos[uCurrentCommandIndex].GetFormattedOutputLength(ppArgs[uCurrentArgumentNext]);
					} else {
						// make the output command the best we can
						eConversionSpecifier uConversionSpec = ( uOpCode == OP_OUTPUT_DEFAULT ) ? CONVSPEC_NOT_SUPPLIED : g_PrintfOutputType[uOpCode - OP_OUTPUT_BEGIN].m_uOutputDisplayType;
						eConversionFlags uConversionFlags = ( uOpCode == OP_OUTPUT_DEFAULT ) ? CONVFLAG_NO_OPTIONS : g_PrintfOutputType[uOpCode - OP_OUTPUT_BEGIN].m_uOutputFlagsModifier;

						pParamInfos[ uCurrentCommandIndex ].SetConversion( uConversionSpec );
						pParamInfos[ uCurrentCommandIndex ].SetFlag( uConversionFlags );

						//todo:  Get default widths for known output types?

						// Get an estimated output size by checking width and precision
						Word theLength = 8;
						if (theLength < pParamInfos[uCurrentCommandIndex ].GetPrecision()) {
							theLength = pParamInfos[uCurrentCommandIndex ].GetPrecision();
						}
						if (theLength < pParamInfos[ uCurrentCommandIndex ].GetWidth()) {
							theLength = pParamInfos[ uCurrentCommandIndex ].GetWidth();
						}

						pParamInfos[uCurrentCommandIndex ].SetFormattedLength(theLength);
						pParamInfos[uCurrentCommandIndex ].SetOutputLength(theLength);

						// Now check the flags for this conversion
						pParamInfos[ uCurrentCommandIndex ].CheckConversionFlags();
					}

					// Finished parsing the arg specifier, we're done with this argument
					bParsingParam = false;

					// next format command will use the next argument unless it specifies explicitly
					++uCurrentArgumentNext;
				}

			}

		}	// ( bParsingParam == true )

		// Advance one character ( ASCII or UTF-8 )
		++uParsePosition;

	}	// parsing format string

	// Sum up the results to report back
	m_uPhaseResults = ERROR_NONE;
	m_uNumOutputCommands = uCurrentCommandIndex + 1;

	// Get the total length of the formatted result
	for (WordPtr uIndex2 = 0; uIndex2 <= uCurrentCommandIndex; uIndex2++) {
		uTotalLength += pParamInfos[uIndex2].GetOutputLength();
	}
	m_uFormattedOutputLength = uTotalLength;
	return true;

	// error handler needed here...
parse_error:

	m_uErrorCharPos = uErrorCode;
	m_uErrorFormatSequencePos = uFormatSequencePosition;
	m_uErrorCharPos = uParsePosition;
	return false;
}



/*! ************************************

	\brief String to print when a \ref NULL pointer is found

***************************************/

const char Burger::SafePrint::g_NullString[] = "(null)";

/*! ************************************

	\brief Strings to print for a boolean

***************************************/

const char *Burger::SafePrint::g_BoolText[2] = { "false","true" };

/*! ************************************

	\brief Mapping table for UTF-8 characters to SafePrint opcodes

	Mapping table for ASCII/UTF-8 characters ranged 0-127
	used do decode/parse a character that is part of a 
	conversion specification.

	    !   "   #   $   %   &   '   (   )   *   +   ,   -   .   /
	0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
	@   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O 
	P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
	`   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
	p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~  del

***************************************/

const Burger::SafePrint::eOpcode Burger::SafePrint::g_CodeMap[128] = {
	OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,	// 0-7	(0x00 - 0x07)
	OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,	// 8-15 (0x08 - 0x0f)
	OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,	// 16-23 (0x10 - 0x17)
	OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,OP_NOT_A_SYMBOL,	// 24-31 (0x18 - 0x1f)
	OP_FLAG_LEAD_SPACE_POSITIVE,		// (space) 0x20
	OP_OUTPUT_BINARY,					// !
	OP_NOT_A_SYMBOL,					// "
	OP_FLAG_ALTERNATIVE_FORM,			// #
	OP_ARG_SPECIFIER,					// $ 0x24
	OP_LITERAL_PERCENT,					// %
	OP_NOT_A_SYMBOL,					// &
	OP_NOT_A_SYMBOL,					// '	
	OP_NOT_A_SYMBOL,					// ( 0x28
	OP_NOT_A_SYMBOL,					// )
	OP_VALUE_FROM_ARG,					// *
	OP_FLAG_FORCESIGN,					// +
	OP_FLAG_COMMA_SEPERATE,				// , 0x2C
	OP_FLAG_LEFTJUSTIFY,				// -
	OP_PRECISION_SPECIFIER,				// .
	OP_NOT_A_SYMBOL,					// /
	OP_NUMERIC_VALUE,					// 0 0x30
	OP_NUMERIC_VALUE,					// 1
	OP_NUMERIC_VALUE,					// 2
	OP_NUMERIC_VALUE,					// 3
	OP_NUMERIC_VALUE,					// 4 0x34
	OP_NUMERIC_VALUE,					// 5
	OP_NUMERIC_VALUE,					// 6
	OP_NUMERIC_VALUE,					// 7
	OP_NUMERIC_VALUE,					// 8 0x38
	OP_NUMERIC_VALUE,					// 9
	OP_NOT_A_SYMBOL,					// :
	OP_NOT_A_SYMBOL,					// ;
	OP_NOT_A_SYMBOL,					// < 0x3C
	OP_NOT_A_SYMBOL,					// =
	OP_NOT_A_SYMBOL,					// >
	OP_NOT_A_SYMBOL,					// ?
	OP_OUTPUT_DEFAULT,					// @ 0x40
	OP_OUTPUT_HEXFLOATUPPER,			// A
	OP_OUTPUT_BOOLTEXTUPPER,			// B
	OP_OUTPUT_WIDECHARACTER,			// C
	OP_NOT_A_SYMBOL,					// D
	OP_OUTPUT_SCI_NOTEFLOATUPPER,		// E
	OP_OUTPUT_DECIMALFLOATUPPER,		// F
	OP_OUTPUT_SCI_SHORTNOTEUPPER,		// G
	OP_NOT_A_SYMBOL,					// H
	OP_NOT_A_SYMBOL,					// I
	OP_NOT_A_SYMBOL,					// J
	OP_NOT_A_SYMBOL,					// K
	OP_LENGTH_DOUBLE,					// L
	OP_NOT_A_SYMBOL,					// M
	OP_NOT_A_SYMBOL,					// N
	OP_NOT_A_SYMBOL,					// O
	OP_OUTPUT_POINTERUPPER,				// P 0x50
	OP_NOT_A_SYMBOL,					// Q
	OP_NOT_A_SYMBOL,					// R
	OP_OUTPUT_CSTRING16,				// S
	OP_NOT_A_SYMBOL,					// T
	OP_NOT_A_SYMBOL,					// U
	OP_NOT_A_SYMBOL,					// V
	OP_NOT_A_SYMBOL,					// W
	OP_OUTPUT_HEXUPPER,					// X
	OP_NOT_A_SYMBOL,					// Y
	OP_NOT_A_SYMBOL,					// Z
	OP_NOT_A_SYMBOL,					// [
	OP_NOT_A_SYMBOL,					// \ (line cont)
	OP_NOT_A_SYMBOL,					// ]
	OP_NOT_A_SYMBOL,					// ^
	OP_NOT_A_SYMBOL,					// _
	OP_NOT_A_SYMBOL,					// ` 0x60
	OP_OUTPUT_HEXFLOAT,					// a
	OP_OUTPUT_BOOLTEXT,					// b
	OP_OUTPUT_CHARACTER,				// c
	OP_OUTPUT_DECIMALINT,				// d
	OP_OUTPUT_SCI_NOTEFLOAT,			// e
	OP_OUTPUT_DECIMALFLOAT,				// f
	OP_OUTPUT_SCI_SHORTNOTE,			// g
	OP_LENGTH_SHORT,					// h
	OP_OUTPUT_DECIMALINT,				// i
	OP_LENGTH_INTMAX,					// j
	OP_NOT_A_SYMBOL,					// k
	OP_LENGTH_LONG,						// l
	OP_NOT_A_SYMBOL,					// m
	OP_OUTPUT_STORELENGTH,				// n
	OP_OUTPUT_OCTAL,					// o
	OP_OUTPUT_POINTER,					// p 0x70
	OP_NOT_A_SYMBOL,					// q
	OP_NOT_A_SYMBOL,					// r
	OP_OUTPUT_CSTRING,					// s
	OP_LENGTH_WORDPTR,					// t
	OP_OUTPUT_DECIMALWORD,				// u
	OP_NOT_A_SYMBOL,					// v
	OP_NOT_A_SYMBOL,					// w
	OP_OUTPUT_HEX,						// x
	OP_NOT_A_SYMBOL,					// y
	OP_LENGTH_SIZET,					// z
	OP_NOT_A_SYMBOL,					// {
	OP_NOT_A_SYMBOL,					// |
	OP_NOT_A_SYMBOL,					// }
	OP_NOT_A_SYMBOL,					// ~
	OP_NOT_A_SYMBOL						// (DEL)
};

/*! ************************************

	\brief Mapping table for OP_FLAG_* for printing

	Lookup table to convert \ref eOpcode in the OP_FLAG_* range 
	to the appropriate conversion formatting flag

***************************************/

const Burger::SafePrint::eConversionFlags Burger::SafePrint::g_PrintFlagMappings[5] = {
	CONVFLAG_LEFT_JUSTIFY,			// OP_FLAG_LEFTJUSTIFY,		
	CONVFLAG_DISPLAY_SIGN,			// OP_FLAG_FORCESIGN,			
	CONVFLAG_BLANK_SIGN,			// OP_FLAG_LEAD_SPACE_POSITIVE,	
	CONVFLAG_ALTERNATIVE_FORM,		// OP_FLAG_ALTERNATIVE_FORM,		
	CONVFLAG_COMMA_SEPARATE			// OP_FLAG_COMMA_SEPERATE
};

/*! ************************************

	\brief Mapping table for OP_LENGTH_* for printing
	
	Lookup table to convert \ref eOpcode in the OP_LENGTH_* range 
	to the appropriate conversion argument length modifier

***************************************/

const Burger::SafePrint::eConversionFlags Burger::SafePrint::g_PrintfLengthMappings[6] = {
	CONVFLAG_INTERPRET_SHORT,		// OP_LENGTH_SHORT
	CONVFLAG_INTERPRET_LONG,		// OP_LENGTH_LONG
	CONVFLAG_INTERPRET_INTMAX,		// OP_LENGTH_INTMAX
	CONVFLAG_INTERPRET_SIZET,		// OP_LENGTH_SIZET
	CONVFLAG_INTERPRET_WORDPTR,		// OP_LENGTH_WORDPTR
	CONVFLAG_INTERPRET_DOUBLE		// OP_LENGTH_DOUBLE
};


/*! ************************************

	\brief Mapping table for OP_OUTPUT_* for printing

	Lookup table to convert \ref eOpcode in the OP_OUTPUT_* range 
	to the actual output conversion specification and includes 
	any additional formatting options that are needed (such
	as using upper case characters) and info the expected
	argument type (which we only use to issue warnings)
	
***************************************/

const Burger::SafePrint::OutputTypeAndFlags_t Burger::SafePrint::g_PrintfOutputType[22] = 
{
	{CONVSPEC_BINNUMBER,		CONVFLAG_NO_OPTIONS},	// OP_OUTPUT_BINARY					
	{CONVSPEC_BOOLTEXT,			CONVFLAG_NO_OPTIONS},	// OP_OUTPUT_BOOLTEXT
	{CONVSPEC_BOOLTEXT,			CONVFLAG_UPPERCASE},	// OP_OUTPUT_BOOLTEXTUPPER
	{CONVSPEC_CHAR,				CONVFLAG_NO_OPTIONS},	// OP_OUTPUT_CHARACTER
	{CONVSPEC_CHAR,				CONVFLAG_OUTPUT_WCHAR},	// OP_OUTPUT_WIDECHARACTER
	{CONVSPEC_DECIMAL_INT,		CONVFLAG_NO_OPTIONS},	// OP_OUTPUT_DECIMALINT
	{CONVSPEC_DECIMAL_WORD,		CONVFLAG_NO_OPTIONS},	// OP_OUTPUT_DECIMALWORD
	{CONVSPEC_DECIMAL_FLOAT,	CONVFLAG_NO_OPTIONS},	// OP_OUTPUT_DECIMALFLOAT
	{CONVSPEC_DECIMAL_FLOAT,	CONVFLAG_UPPERCASE},	// OP_OUTPUT_DECIMALFLOATUPPER
	{CONVSPEC_HEX_FLOAT,		CONVFLAG_NO_OPTIONS},	// OP_OUTPUT_HEXFLOAT
	{CONVSPEC_HEX_FLOAT,		CONVFLAG_UPPERCASE},	// OP_OUTPUT_HEXFLOATUPPER
	{CONVSPEC_SCI_NOTE_FLOAT,	CONVFLAG_NO_OPTIONS},	// OP_OUTPUT_SCI_NOTEFLOAT	
	{CONVSPEC_SCI_NOTE_FLOAT,	CONVFLAG_UPPERCASE},	// OP_OUTPUT_SCI_NOTEFLOATUPPER	
	{CONVSPEC_SHORTER_FLOAT,	CONVFLAG_NO_OPTIONS},	// OP_OUTPUT_SCI_SHORTNOTE
	{CONVSPEC_SHORTER_FLOAT,	CONVFLAG_UPPERCASE},	// OP_OUTPUT_SCI_SHORTNOTEUPPER
	{CONVSPEC_OCTAL,			CONVFLAG_NO_OPTIONS},	// OP_OUTPUT_OCTAL
	{CONVSPEC_HEX,				CONVFLAG_NO_OPTIONS},	// OP_OUTPUT_HEX
	{CONVSPEC_HEX,				CONVFLAG_UPPERCASE},	// OP_OUTPUT_HEXUPPER
	{CONVSPEC_POINTER,			CONVFLAG_NO_OPTIONS},	// OP_OUTPUT_POINTER
	{CONVSPEC_POINTER,			CONVFLAG_UPPERCASE},	// OP_OUTPUT_POINTERUPPER
	{CONVSPEC_TEXT_STRING,		CONVFLAG_NO_OPTIONS},	// OP_OUTPUT_CSTRING
	{CONVSPEC_TEXT_STRING,		CONVFLAG_OUTPUT_WCHAR}	// OP_OUTPUT_CSTRING16
};

/*! ************************************

	\brief CONVSPEC_* expected data sizes

	Table of expected argument sizes for each conversion

***************************************/

const WordPtr Burger::SafePrint::g_ConversionArgumentSizes[14] = {
    UINTPTR_MAX,		// CONVSPEC_INVALID
	0,						// CONVSPEC_BINNUMBER
	sizeof(bool),			// CONVSPEC_BOOLTEXT
	sizeof(int),			// CONVSPEC_CHAR
	sizeof(int),			// CONVSPEC_DECIMAL_INT
	sizeof(unsigned int),	// CONVSPEC_DECIMAL_WORD
	sizeof(unsigned int),	// CONVSPEC_OCTAL
	sizeof(unsigned int),	// CONVSPEC_HEX
	sizeof(double),			// CONVSPEC_HEX_FLOAT
	sizeof(double),			// CONVSPEC_DECIMAL_FLOAT
	sizeof(double),			// CONVSPEC_SCI_NOTE_FLOAT
	sizeof(double),			// CONVSPEC_SHORTER_FLOAT
	sizeof(void*),			// CONVSPEC_POINTER
	sizeof(char*)			// CONVSPEC_TEXT_STRING
};

/*! ************************************

	\brief CONVSPEC_* valid output conversion flags

	Table of which flags are valid for what output conversions

***************************************/

const Word32 Burger::SafePrint::g_ValidFlagsForConversion[14] = {
	CONVFLAG_NO_OPTIONS,		// CONVSPEC_INVALID
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_ALTERNATIVE_FORM | CONVFLAG_COMMA_SEPARATE,	// CONVSPEC_BINNUMBER
	CONVFLAG_LEFT_JUSTIFY,		// CONVSPEC_BOOLTEXT
	CONVFLAG_LEFT_JUSTIFY,		// CONVSPEC_CHAR
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_DISPLAY_SIGN | CONVFLAG_BLANK_SIGN | CONVFLAG_LEFT_PAD_ZEROS | CONVFLAG_COMMA_SEPARATE | CONVFLAG_PRECISION_SPECIFIED,		// CONVSPEC_DECIMAL_INT
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_LEFT_PAD_ZEROS | CONVFLAG_COMMA_SEPARATE | CONVFLAG_PRECISION_SPECIFIED,													// CONVSPEC_DECIMAL_WORD
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_ALTERNATIVE_FORM | CONVFLAG_LEFT_PAD_ZEROS | CONVFLAG_COMMA_SEPARATE | CONVFLAG_PRECISION_SPECIFIED,						// CONVSPEC_OCTAL
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_ALTERNATIVE_FORM | CONVFLAG_LEFT_PAD_ZEROS | CONVFLAG_COMMA_SEPARATE| CONVFLAG_PRECISION_SPECIFIED,						// CONVSPEC_HEX
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_DISPLAY_SIGN | CONVFLAG_BLANK_SIGN | CONVFLAG_ALTERNATIVE_FORM | CONVFLAG_LEFT_PAD_ZEROS | CONVFLAG_PRECISION_SPECIFIED,	// CONVSPEC_HEX_FLOAT
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_DISPLAY_SIGN | CONVFLAG_BLANK_SIGN | CONVFLAG_ALTERNATIVE_FORM | CONVFLAG_LEFT_PAD_ZEROS | CONVFLAG_PRECISION_SPECIFIED,	// CONVSPEC_DECIMAL_FLOAT
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_DISPLAY_SIGN | CONVFLAG_BLANK_SIGN | CONVFLAG_ALTERNATIVE_FORM | CONVFLAG_LEFT_PAD_ZEROS | CONVFLAG_PRECISION_SPECIFIED,	// CONVSPEC_SCI_NOTE_FLOAT
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_DISPLAY_SIGN | CONVFLAG_BLANK_SIGN | CONVFLAG_ALTERNATIVE_FORM | CONVFLAG_LEFT_PAD_ZEROS | CONVFLAG_PRECISION_SPECIFIED,	// CONVSPEC_SHORTER_FLOAT
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_ALTERNATIVE_FORM,		// CONVSPEC_POINTER
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_PRECISION_SPECIFIED	// CONVSPEC_TEXT_STRING
};


/*! ************************************

	\brief ARG_* valid output conversion flags

	Bit table of Valid output conversion specifiers for each argument type
	when applying strict argument conversion rules

***************************************/

const Word32 Burger::SafePrint::g_ValidOutputDisplayTypesStrict[SafePrintArgument::ARG_COUNT] = {
	0,				// ARG_INVALID
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_INT | CONVSPECFLAG_CHARACTER,		// ARG_INT8
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_INT | CONVSPECFLAG_CHARACTER,		// ARG_INT16
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_INT | CONVSPECFLAG_CHARACTER,		// ARG_INT32
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_INT | CONVSPECFLAG_CHARACTER,		// ARG_INT64

	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_UNSIGNEDINT | CONVSPECFLAG_OCTAL | CONVSPECFLAG_HEX | CONVSPECFLAG_CHARACTER,	// ARG_WORD8
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_UNSIGNEDINT | CONVSPECFLAG_OCTAL | CONVSPECFLAG_HEX | CONVSPECFLAG_CHARACTER,	// ARG_WORD16
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_UNSIGNEDINT | CONVSPECFLAG_OCTAL | CONVSPECFLAG_HEX | CONVSPECFLAG_CHARACTER,	// ARG_WORD32
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_UNSIGNEDINT | CONVSPECFLAG_OCTAL | CONVSPECFLAG_HEX | CONVSPECFLAG_CHARACTER,	// ARG_WORD64

	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_FLOAT | CONVSPECFLAG_SCINOTE_FLOAT | CONVSPECFLAG_SHORTER_FLOAT |  CONVSPECFLAG_HEX_FLOAT,	// ARG_HALF
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_FLOAT | CONVSPECFLAG_SCINOTE_FLOAT | CONVSPECFLAG_SHORTER_FLOAT |  CONVSPECFLAG_HEX_FLOAT,	// ARG_FLOAT
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_FLOAT | CONVSPECFLAG_SCINOTE_FLOAT | CONVSPECFLAG_SHORTER_FLOAT |  CONVSPECFLAG_HEX_FLOAT,	// ARG_DOUBLE

	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_BOOL_TEXT,		// ARG_BOOL
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,			// ARG_SIMD64
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,			// ARG_SIMD128
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,			// ARG_SIMD128I
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,			// ARG_SIMD128D

	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_TEXT_STRING | CONVSPECFLAG_POINTER,	// ARG_PSTRING
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_TEXT_STRING | CONVSPECFLAG_POINTER,	// ARG_PCHAR
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_TEXT_STRING | CONVSPECFLAG_POINTER,	// ARG_PSCHAR
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_TEXT_STRING | CONVSPECFLAG_POINTER,	// ARG_PUCHAR

	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,  // ARG_PINT16
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER | CONVSPECFLAG_TEXT_STRING,	// ARG_PWORD16
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,  // ARG_PINT32
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,  // ARG_PWORD32
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,	// ARG_PINT64
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,	// ARG_PWORD64
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,	// ARG_PFLOAT
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,	// ARG_PDOUBLE
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,	// ARG_PSIMD64
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,	// ARG_PSIMD128
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,	// ARG_PSIMD128I
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,	// ARG_PSIMD128D
	CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER	// ARG_PVOID
};

/*! ************************************

	\brief ARG_* valid output relaxed conversion flags

	Bit table of Additional Valid output conversion specifiers for each argument 
	type when allowing for "relaxed" conversions

***************************************/

const Word32 Burger::SafePrint::g_ValidOutputDisplayTypesRelaxed[SafePrintArgument::ARG_COUNT] = {
	0,			// ARG_INVALID
	CONVSPECFLAG_DECIMAL_UNSIGNEDINT | CONVSPECFLAG_OCTAL | CONVSPECFLAG_HEX | CONVSPECFLAG_BOOL_TEXT,	// ARG_INT8
	CONVSPECFLAG_DECIMAL_UNSIGNEDINT | CONVSPECFLAG_OCTAL | CONVSPECFLAG_HEX | CONVSPECFLAG_BOOL_TEXT,	// ARG_INT16
	CONVSPECFLAG_DECIMAL_UNSIGNEDINT | CONVSPECFLAG_OCTAL | CONVSPECFLAG_HEX | CONVSPECFLAG_BOOL_TEXT,	// ARG_INT32
	CONVSPECFLAG_DECIMAL_UNSIGNEDINT | CONVSPECFLAG_OCTAL | CONVSPECFLAG_HEX | CONVSPECFLAG_BOOL_TEXT,	// ARG_INT64

	CONVSPECFLAG_DECIMAL_INT | CONVSPECFLAG_BOOL_TEXT,	// ARG_WORD8
	CONVSPECFLAG_DECIMAL_INT | CONVSPECFLAG_BOOL_TEXT,	// ARG_WORD16
	CONVSPECFLAG_DECIMAL_INT | CONVSPECFLAG_BOOL_TEXT,	// ARG_WORD32
	CONVSPECFLAG_DECIMAL_INT | CONVSPECFLAG_BOOL_TEXT,	// ARG_WORD64

	CONVSPECFLAG_HEX,	// ARG_HALF
	CONVSPECFLAG_HEX,	// ARG_FLOAT
	CONVSPECFLAG_HEX,	// ARG_DOUBLE

	CONVSPECFLAG_DECIMAL_INT | CONVSPECFLAG_DECIMAL_UNSIGNEDINT | CONVSPECFLAG_OCTAL | CONVSPECFLAG_HEX,	// ARG_BOOL

	CONVSPECFLAG_HEX,	// ARG_SIMD64
	CONVSPECFLAG_HEX,	// ARG_SIMD128
	CONVSPECFLAG_HEX,	// ARG_SIMD128I
	CONVSPECFLAG_HEX,	// ARG_SIMD128D

	CONVSPECFLAG_HEX,	// ARG_PSTRING
	CONVSPECFLAG_HEX,	// ARG_PCHAR
	CONVSPECFLAG_HEX,	// ARG_PSCHAR
	CONVSPECFLAG_HEX,	// ARG_PUCHAR

	CONVSPECFLAG_HEX,	// ARG_PINT16
	CONVSPECFLAG_HEX,	// ARG_PWORD16
	CONVSPECFLAG_HEX,	// ARG_PINT32
	CONVSPECFLAG_HEX,	// ARG_PWORD32
	CONVSPECFLAG_HEX,	// ARG_PINT64
	CONVSPECFLAG_HEX,	// ARG_PWORD64
	CONVSPECFLAG_HEX,	// ARG_PFLOAT
	CONVSPECFLAG_HEX,	// ARG_PDOUBLE
	CONVSPECFLAG_HEX,	// ARG_PSIMD64
	CONVSPECFLAG_HEX,	// ARG_PSIMD128
	CONVSPECFLAG_HEX,	// ARG_PSIMD128I
	CONVSPECFLAG_HEX,	// ARG_PSIMD128D
	CONVSPECFLAG_HEX	// ARG_PVOID
};



/*! ************************************

	\brief Convert error code to message in English

	\param uType Error code
	\return Pointer to a "C" string of the error message

***************************************/

const char* BURGER_API Burger::SafePrint::GetErrorDescription(eError uType)
{
	const char *pResult;
	switch(uType) {
	case ERROR_NONE:
		pResult = "No Error";
		break;
	case ERROR_BAD_INPUT_PARAMETER:
		pResult = "Bad Input Parameter";
		break;
	case ERROR_BAD_ARGUMENT_TYPE:
		pResult = "Invalid or Corrupted Argument";
		break;
	case ERROR_FORMAT_STRING_TOO_LONG:
		pResult = "Format String too long";
		break;

	case ERROR_TOO_MANY_FORMAT_SEQUENCES:
		pResult = "Too many format sequences encountered";
		break;
	case ERROR_INSUFFICIENT_ARGUMENTS:
		pResult = "Not enough Arguments provided to the format string";
		break;
	case ERROR_ESCAPE_IS_LAST_CHAR:
		pResult = "Escape character '%' is last character in format string (undefined)";
		break;

	case ERROR_WIDTH_VALUE_TOO_LARGE:
		pResult = "Width specified is too large";
		break;
	case ERROR_PRECISION_VALUE_TOO_LARGE:
		pResult = "Precision specified is too large";
		break;
	case ERROR_ILLEGAL_CHARACTER_FORMAT_SEQUENCE:
		pResult = "Illegal character encountered in format sequence";
		break;
	case ERROR_NUMERIC_CHAR_IN_WRONG_PLACE:
		pResult = "Numeric character encountered in wrong place in format sequence";
		break;

	case ERROR_ARG_INDEX_INVALID:
		pResult = "Argument index not in valid range";
		break;
	case ERROR_ARG_INDEX_IN_WRONG_PLACE:
		pResult = "Argument index specified in wrong place in format sequence";
		break;
	case ERROR_ARG_INDEX_DOUBLY_SPECIFIED:
		pResult = "Argument index specified twice";
		break;
	case ERROR_ARG_INDEX_NOT_SPECIFIED:
		pResult = "Argument index not specified";
		break;

	case ERROR_OPTION_FLAG_IN_WRONG_PLACE:
		pResult = "Option flag encountered in wrong place in format sequence";
		break;
	case ERROR_OPTION_FLAG_DOUBLY_SPECIFIED:
		pResult = "Option flag specified twice";
		break;
	case ERROR_PRECISION_IN_WRONG_PLACE:
		pResult = "Precision specifier encountered in wrong place in format sequence";
		break;
	case ERROR_PRECISION_DOUBLY_SPECIFIED:
		pResult = "Precision specified twice";
		break;

	case ERROR_ARG_LENGTH_FLAG_DOUBLY_SPECIFIED:
		pResult = "Multiple Argument length flags encountered";
		break;
	case ERROR_DEFAULT_CONVERSION_NOT_FOUND:
		pResult = "No default conversion found for argument type";
		break;
	case ERROR_INCOMPATIBLE_ARG:
		pResult = "Argument type is incompatible with the conversion";
		break;
	case ERROR_INVALID_WIDECHAR_IN_ARG:
		pResult = "Invalid wchar_t in argument, no conversion performed";
		break;
	case ERROR_INVALID_CONVERSION_TYPE:
		pResult = "Invalid conversion type";
		break;

	case ERROR_WRITING_TO_FILE:
		pResult = "Error occurred in RTL function fprintf while writing formatted output to file";
		break;
	case ERROR_WRITING_STDOUT:
		pResult = "Error occurred in RTL function printf while writing formatted output to stdout";
		break;
	case ERROR_RTL_TRANSMITTED_COUNT:
		pResult = "The RTL function used to transmit formatted output reported a different amount of chars sent";
		break;
	case ERROR_INVALID_OUTPUT_CMD:
		pResult = "Invalid Output Command encountered during formatting";
		break;
	case ERROR_CONVERSION_OUTPUT_TOO_LARGE:
		pResult = "Single conversion output too large";
		break;

	default:
		pResult = "Unspecified error";
		break;
	}
	return pResult;
}

/*! ************************************

	\brief Convert error flag to message in English

	\param uFlags Error code
	\return Pointer to a "C" string of the warning message

***************************************/

const char* BURGER_API Burger::SafePrint::GetWarningDescription(eWarningFlags uFlags)
{
	const char *pResult;
	switch(uFlags) {
	case WARN_NONE:
		pResult = "No Warning";
		break;
	case WARN_SIGNED_UNSIGNED_MISMATCH:
		pResult = "Argument signed mismatch with conversion";
		break;
	case WARN_FLAG_UNDEFINED_FOR_CONVERSION:
		pResult = "Flag undefined for conversion (ignored)";
		break;
	case WARN_FLAG_SUPPRESSED:
		pResult = "Flag suppressed by another flag";
		break;
	case WARN_PRECISION_UNDEFINED:
		pResult = "Precision is undefined for conversion";
		break;
	case WARN_LENGTH_SPECIFIER_MISMATCH:
		pResult = "Length modifier doesn't match argument";
		break;
	case WARN_LENGTH_NOT_VALID_FOR_CONVERSION:
		pResult = "Length modifier isn't valid for conversion";
		break;
	case WARN_FLAG_NO_EFFECT:
		pResult = "Flag had no effect";
		break;
	default:
		pResult = "Unspecified warning";
		break;
	}
	return pResult;
}


/*! ************************************

	\brief Get the default opcode from the argument data type

	\param pArg Error code
	\return Opcode that is best to process this argument or \ref OP_NOT_A_SYMBOL

***************************************/

Burger::SafePrint::eOpcode BURGER_API Burger::SafePrint::GetDefaultArgumentType(const SafePrintArgument* pArg)
{
	eOpcode uCode = OP_NOT_A_SYMBOL;
	if (pArg) {
		switch ( pArg->GetType()) {
		case SafePrintArgument::ARG_INT8:
			uCode = OP_OUTPUT_DECIMALINT;
			break;
		case SafePrintArgument::ARG_INT16:
			uCode = OP_OUTPUT_DECIMALINT;
			break;
		case SafePrintArgument::ARG_INT32:
			uCode = OP_OUTPUT_DECIMALINT;
			break;
		case SafePrintArgument::ARG_INT64:
			uCode = OP_OUTPUT_DECIMALINT;
			break;

		case SafePrintArgument::ARG_WORD8:
			uCode = OP_OUTPUT_DECIMALWORD;
			break;
		case SafePrintArgument::ARG_WORD16:
			uCode = OP_OUTPUT_DECIMALWORD;
			break;
		case SafePrintArgument::ARG_WORD32:
			uCode = OP_OUTPUT_DECIMALWORD;
			break;
		case SafePrintArgument::ARG_WORD64:
			uCode = OP_OUTPUT_DECIMALWORD;
			break;

		case SafePrintArgument::ARG_HALF:
			uCode = OP_OUTPUT_DECIMALFLOAT;
			break;
		case SafePrintArgument::ARG_FLOAT:
			uCode = OP_OUTPUT_DECIMALFLOAT;
			break;
		case SafePrintArgument::ARG_DOUBLE:
			uCode = OP_OUTPUT_DECIMALFLOAT;
			break;

		case SafePrintArgument::ARG_BOOL:
			uCode = OP_OUTPUT_BOOLTEXT;
			break;

		case SafePrintArgument::ARG_PSTRING:
			uCode = OP_OUTPUT_CSTRING;
			break;
		case SafePrintArgument::ARG_PCHAR:
			uCode = OP_OUTPUT_CSTRING;
			break;
		case SafePrintArgument::ARG_PSCHAR:
			uCode = OP_OUTPUT_CSTRING;
			break;
		case SafePrintArgument::ARG_PUCHAR:
			uCode = OP_OUTPUT_CSTRING;
			break;

		case SafePrintArgument::ARG_PINT16:
			uCode = OP_OUTPUT_POINTER;
			break;
		case SafePrintArgument::ARG_PWORD16:
			uCode = OP_OUTPUT_POINTER;
			break;
		case SafePrintArgument::ARG_PINT32:
			uCode = OP_OUTPUT_POINTER;
			break;
		case SafePrintArgument::ARG_PWORD32:
			uCode = OP_OUTPUT_POINTER;
			break;
		case SafePrintArgument::ARG_PINT64:
			uCode = OP_OUTPUT_POINTER;
			break;
		case SafePrintArgument::ARG_PWORD64:
			uCode = OP_OUTPUT_POINTER;
			break;
		case SafePrintArgument::ARG_PFLOAT:
			uCode = OP_OUTPUT_POINTER;
			break;
		case SafePrintArgument::ARG_PDOUBLE:
			uCode = OP_OUTPUT_POINTER;
			break;
		case SafePrintArgument::ARG_PVOID:
			uCode = OP_OUTPUT_POINTER;
			break;
		default:
			break;
		}
	}
	return uCode;
}

/*! ************************************

	\brief Convert conversion specifier to message in English

	\param uType Specifier type to convert
	\return Pointer to a "C" string of the conversion specifier type

***************************************/

const char* BURGER_API Burger::SafePrint::GetOutputDisplayType(eConversionSpecifier uType)
{
	const char *pResult;
	switch(uType) {
	case CONVSPEC_INVALID:
		pResult = "<INVALID>";
		break;
	case CONVSPEC_BINNUMBER:
		pResult = "binary";
		break;
	case CONVSPEC_BOOLTEXT:
		pResult = "bool as text";
		break;
	case CONVSPEC_CHAR:
		pResult = "char";
		break;
	case CONVSPEC_DECIMAL_INT:
		pResult = "decimal int";
		break;
	case CONVSPEC_DECIMAL_WORD:
		pResult = "decimal unsigned int";
		break;
	case CONVSPEC_OCTAL:
		pResult = "octal";
		break;
	case CONVSPEC_HEX:
		pResult = "hex";
		break;
	case CONVSPEC_HEX_FLOAT:
		pResult = "hex float";
		break;
	case CONVSPEC_DECIMAL_FLOAT:
		pResult = "decimal float";
		break;
	case CONVSPEC_SCI_NOTE_FLOAT:
		pResult = "scientific notation float";
		break;
	case CONVSPEC_SHORTER_FLOAT:
		pResult = "shorter of decimal or scientific float";
		break;
	case CONVSPEC_POINTER:
		pResult = "pointer";
		break;
	case CONVSPEC_TEXT_STRING:
		pResult = "text string";
		break;
	default:
		pResult = "<invalid>";
		break;
	}
	return pResult;
}

/*! ************************************

	\brief Convert conversion command to message in English

	\param uCommand Command to convert
	\return Pointer to a "C" string of the conversion command

***************************************/

const char* BURGER_API Burger::SafePrint::GetOutputCommandName(eConversionCommand uCommand)
{
	const char *pResult;
	switch(uCommand) {
	case COMMAND_COPY_TEXT:
		pResult = "COPY TEXT";
		break;
	case COMMAND_FORMAT_ARGUMENT:
		pResult = "FORMAT ARGUMENT";
		break;
	default:
		pResult = "<invalid>";
		break;
	}
	return pResult;
}



/***************************************

	public SafePrint functions are below

***************************************/


/*! ************************************

	\brief Reports the size of a formatted output in bytes without actually creating any formatted output.

	* * Step 1) Parse the format string and arguments
	* * Step 2) Report the format results and call back for an output buffer.
	* * Step 3) If no issues found in Step 1 and a buffer was returned in step 2,
		execute the list of format & copy output commands.

	\param pFormat Pointer to a "C" format string
	\param uArgCount Number of arguments in the argument array
	\param ppArgs Argument array, can be \ref NULL if uArgCount is zero
	\return Negative number for error, number of bytes processed on success

***************************************/

IntPtr BURGER_API Burger::GetFormattedLength(const char *pFormat,WordPtr uArgCount,const SafePrintArgument **ppArgs)
{
	// Any invalid parameters passed in?
	if (IsStringEmpty(pFormat) || (uArgCount && IsPointerInvalid(ppArgs))) {
		return 0;
	}

	// Setup for parsing engine

	SafePrint::ProcessResults_t	TheProcessResults;
	SafePrint::ParamInfo_t ParamIntoTable[SafePrint::cMAX_OUTPUT_COMMANDS];
	MemoryClear(ParamIntoTable,sizeof(ParamIntoTable));

	// Analyze everything and determine what we'll output
	Word bParseOk = TheProcessResults.FormatPreProcess(SafePrint::PARSEOPTION_DEFAULT,pFormat,StringLength(pFormat),uArgCount,ppArgs,BURGER_ARRAYSIZE(ParamIntoTable),ParamIntoTable);

	// If there were no problems, return the output size... otherwise report 0
	if (bParseOk && (TheProcessResults.m_uPhaseResults == SafePrint::ERROR_NONE)) {
		return static_cast<IntPtr>(TheProcessResults.m_uFormattedOutputLength);
	}

	// An error must have occurred.  Handle it, and get the proper return value 
	return TheProcessResults.ErrorHandler(pFormat,uArgCount,ppArgs);
}

/*! ************************************

	\brief Print into a buffer that a callback allocates

	Version of sprintf that calls back to an allocator
	to get the output buffer *AFTER* validating the formatting and 
	determining the required size.  A null terminator written to the
	end of the output is optional requested by the caller.

	* * Step 1) Parse the format string and arguments
	* * Step 2) Report the format results and call back for an output buffer.
	* * Step 3) If no issues found in Step 1 and a buffer was returned in step 2,
		execute the list of format & copy output commands.

	\param pCallback Pointer to a callback function
	\param pContext Pointer to data the callback function requires, can be \ref NULL
	\param bAddNull \ref TRUE if a 0 should be appended to the end of the string, \ref FALSE if not
	\param pFormat Pointer to a "C" format string
	\param uArgCount Number of arguments in the argument array
	\param ppArgs Argument array, can be \ref NULL if uArgCount is zero
	\return Negative number for error, number of bytes processed on success

	\sa Sprintf(char *,const char *,WordPtr,const SafePrintArgument **)

***************************************/

IntPtr BURGER_API Burger::SprintfUserAlloc(SafePrint::SprintfCallbackProc pCallback,void *pContext,Word bAddNull,const char *pFormat,WordPtr uArgCount,const SafePrintArgument **ppArgs)
{
	// Any invalid parameters passed in?
	if (!pCallback || IsStringEmpty(pFormat) || (uArgCount && IsPointerInvalid(ppArgs))) {
		return 0;
	}

	// Setup for parsing engine
	WordPtr uFormatLength = StringLength(pFormat);

	SafePrint::ProcessResults_t TheProcessResults;
	SafePrint::ParamInfo_t ParamIntoTable[SafePrint::cMAX_OUTPUT_COMMANDS];
	MemoryClear(ParamIntoTable,sizeof(ParamIntoTable));

	// Analyze everything and determine what we'll output
	Word bParseOk = TheProcessResults.FormatPreProcess(SafePrint::PARSEOPTION_DEFAULT,pFormat,uFormatLength,uArgCount,ppArgs,BURGER_ARRAYSIZE(ParamIntoTable),ParamIntoTable);

	// If there were no problems, format the output to the supplied buffer
	Word bNoErrors = (bParseOk && TheProcessResults.m_uPhaseResults == SafePrint::ERROR_NONE);
	WordPtr uBufferSizeNeeded = TheProcessResults.m_uFormattedOutputLength + bAddNull;
	void *pOutputBuffer = NULL;

	// Ask the external allocator for a buffer
	Word bProceed = (pCallback)(bNoErrors,uBufferSizeNeeded,&pOutputBuffer,pContext);

	// Did we get back the OK and a buffer to use, and we're error free?
	if (bNoErrors && bProceed && IsPointerValid(pOutputBuffer)) {
		// format the output to the buffer
		char* pWorkBuffer = static_cast<char*>(pOutputBuffer);
		WordPtr uProcessedCount = TheProcessResults.GenerateFormatOutputToBuffer(pWorkBuffer,uBufferSizeNeeded,uArgCount,ppArgs,TheProcessResults.m_uNumOutputCommands, ParamIntoTable );

		// Were we error free?
		if (TheProcessResults.m_uPhaseResults == SafePrint::ERROR_NONE) {
			// Were we asked to append a null?
			if (bAddNull && (uProcessedCount < uBufferSizeNeeded)) {
				pWorkBuffer[uProcessedCount] = 0;
				++uProcessedCount;
			}
			return static_cast<IntPtr>(uProcessedCount);
		}
	}

	// An error must have occurred.  Handle it, and get the proper return value 
	return TheProcessResults.ErrorHandler(pFormat, uArgCount, ppArgs );
}

/*! ************************************

	\brief sprintf() clone
	
	Return value is the number of characters written to the buffer, or a 
	negative value indicating an error occurred during the attempt.

	* * Step 1) Parse the format string and arguments
	* * Step 2) if no issues found in Step 1, execute the list of format & copy output commands.

	\note This function can easily overrun the buffer since no buffer
		is supplied.  It is recommended you use Snprintf() instead.

	\param pOutput Pointer to a buffer large enough to accept the output string
	\param pFormat Pointer to a "C" format string
	\param uArgCount Number of arguments in the argument array
	\param ppArgs Argument array, can be \ref NULL if uArgCount is zero

	\return Negative number for error, number of bytes processed on success

	\sa Snprintf(char *,WordPtr,const char *,WordPtr,const SafePrintArgument **) or
		SprintfUserAlloc(SafePrint::SprintfCallbackProc,void *,Word,const char *,WordPtr,const SafePrintArgument **)

***************************************/

IntPtr BURGER_API Burger::Sprintf(char *pOutput,const char* pFormat,WordPtr uArgCount,const SafePrintArgument **ppArgs)
{
	// Any invalid parameters passed in?
	if (IsPointerInvalid(pOutput) || IsStringEmpty(pFormat) || (uArgCount && IsPointerInvalid(ppArgs))) {
		return 0;
	}

	// Setup for parsing engine
	SafePrint::ProcessResults_t	TheProcessResults;
	SafePrint::ParamInfo_t ParamIntoTable[SafePrint::cMAX_OUTPUT_COMMANDS];
	MemoryClear(ParamIntoTable,sizeof(ParamIntoTable));

	// Analyze everything and determine what we'll output
	Word bParseOk = TheProcessResults.FormatPreProcess(SafePrint::PARSEOPTION_DEFAULT,pFormat,StringLength(pFormat),uArgCount, ppArgs,BURGER_ARRAYSIZE(ParamIntoTable),ParamIntoTable );

	// If there were no problems, format the output to the supplied buffer
	if (bParseOk && (TheProcessResults.m_uPhaseResults == SafePrint::ERROR_NONE)) {
		// need space for terminating null
		if (TheProcessResults.m_uFormattedOutputLength < SafePrint::cMAX_TOTAL_OUTPUT_SIZE ) {
			WordPtr uProcessedCount = TheProcessResults.GenerateFormatOutputToBuffer(pOutput,SafePrint::cMAX_TOTAL_OUTPUT_SIZE,uArgCount,ppArgs,TheProcessResults.m_uNumOutputCommands,ParamIntoTable);

			// Were we error free?
			if (TheProcessResults.m_uPhaseResults == SafePrint::ERROR_NONE) {
				pOutput[uProcessedCount] = 0;
				return static_cast<IntPtr>(uProcessedCount);
			}
		}
	}

	// An error must have occurred.  Handle it, and get the proper return value 
	return TheProcessResults.ErrorHandler(pFormat,uArgCount,ppArgs);
}

/*! ************************************

	\brief snprintf() clone

	Return value is the number of characters to be written if the buffer is
	sufficiently large to hold the entire formatted output, or a 
	negative value indicating an error occurred during the attempt.

	* * Step 1) Parse the format string and arguments
	* * Step 2) if no issues found in Step 1, execute the list of format & copy output commands.

	\param pOutput Pointer to the output buffer
	\param uOutputSize Size of the output buffer in bytes
	\param pFormat Pointer to a "C" format string
	\param uArgCount Number of arguments in the argument array
	\param ppArgs Argument array, can be \ref NULL if uArgCount is zero

	\return Negative number for error, number of bytes processed on success

	\sa Sprintf(char *,const char *,WordPtr,const SafePrintArgument **)

***************************************/

IntPtr BURGER_API Burger::Snprintf(char* pOutput,WordPtr uOutputSize,const char* pFormat,WordPtr uArgCount,const SafePrintArgument** ppArgs)
{
	// Any invalid parameters passed in?
	if (IsPointerInvalid(pOutput) || !uOutputSize || IsStringEmpty(pFormat) || (uArgCount && IsPointerInvalid(ppArgs))) {
		return 0;
	}

	// Setup for parsing engine

	SafePrint::ProcessResults_t	TheProcessResults;
	SafePrint::ParamInfo_t ParamIntoTable[SafePrint::cMAX_OUTPUT_COMMANDS];
	MemoryClear(ParamIntoTable,sizeof(ParamIntoTable));
	
	// Analyze everything and determine what we'll output
	Word bParseOk = TheProcessResults.FormatPreProcess(SafePrint::PARSEOPTION_DEFAULT,pFormat,StringLength(pFormat),uArgCount, ppArgs,BURGER_ARRAYSIZE(ParamIntoTable), ParamIntoTable );

	// If there were no problems, format the output to the supplied buffer
	if (bParseOk && (TheProcessResults.m_uPhaseResults == SafePrint::ERROR_NONE)) {
		// need space for terminating null
		if (TheProcessResults.m_uFormattedOutputLength < uOutputSize) {
			WordPtr uProcessedCount = TheProcessResults.GenerateFormatOutputToBuffer(pOutput,uOutputSize,uArgCount,ppArgs,TheProcessResults.m_uNumOutputCommands, ParamIntoTable );

			// Were we error free?
			if (TheProcessResults.m_uPhaseResults == SafePrint::ERROR_NONE) {
				pOutput[uProcessedCount] = 0;
				return static_cast<IntPtr>(uProcessedCount);	// this is wrong!
			}
		}
	}
	// An error must have occurred.  Handle it, and get the proper return value 
	return TheProcessResults.ErrorHandler(pFormat,uArgCount,ppArgs);
}

/*! ************************************

	\brief printf() clone

	Return value is the number of characters transmitted, or a negative value
	indicating an error occurred during the attempt.

	* * Step 1) Parse the format string and arguments
	* * Step 2) if no issues found in Step 1, execute the list of format & copy output commands.

	\param pFormat Pointer to a "C" format string
	\param uArgCount Number of arguments in the argument array
	\param ppArgs Argument array, can be \ref NULL if uArgCount is zero

	\return Negative number for error, number of bytes processed on success

***************************************/

IntPtr BURGER_API Burger::Printf(const char *pFormat,WordPtr uArgCount,const SafePrintArgument **ppArgs)
{
	// Any invalid parameters passed in?
	if (IsStringEmpty(pFormat) || (uArgCount && IsPointerInvalid(ppArgs))) {
		return 0;
	}

	// Setup for parsing engine
	SafePrint::ProcessResults_t TheProcessResults;
	SafePrint::ParamInfo_t ParamIntoTable[SafePrint::cMAX_OUTPUT_COMMANDS];
	MemoryClear(ParamIntoTable,sizeof(ParamIntoTable));

	// Analyze everything and determine what we'll output
	Word bParseOk = TheProcessResults.FormatPreProcess(SafePrint::PARSEOPTION_DEFAULT,pFormat,StringLength(pFormat),uArgCount,ppArgs,BURGER_ARRAYSIZE(ParamIntoTable),ParamIntoTable);

	// If there were no problems, format the output and send the results out via the RTL printf() function
	if (bParseOk && (TheProcessResults.m_uPhaseResults == SafePrint::ERROR_NONE)) {
		// format the output and send it along
		WordPtr uProcessedCount = TheProcessResults.GenerateFormattedOutputToFile(stdin,TRUE,uArgCount,ppArgs,TheProcessResults.m_uNumOutputCommands,ParamIntoTable);

		// Were we error free?
		if (TheProcessResults.m_uPhaseResults == SafePrint::ERROR_NONE ) {
			return static_cast<IntPtr>(uProcessedCount);
		}
	}

	// An error must have occurred.  Handle it, and get the proper return value 
	return TheProcessResults.ErrorHandler(pFormat,uArgCount,ppArgs);
}

/*! ************************************

	\brief printf() clone
 
	Return value is the number of characters transmitted, or a negative value
	indicating an error occurred during the attempt.

	* * Step 1) Parse the format string and arguments
	* * Step 2) if no issues found in Step 1, execute the list of format & copy output commands.

	\param fp Pointer to a FILE stream
	\param pFormat Pointer to a "C" format string
	\param uArgCount Number of arguments in the argument array
	\param ppArgs Argument array, can be \ref NULL if uArgCount is zero

	\return Negative number for error, number of bytes processed on success

***************************************/

IntPtr BURGER_API Burger::Fprintf(FILE *fp,const char *pFormat,WordPtr uArgCount,const SafePrintArgument **ppArgs)
{
	// Any invalid parameters passed in?
	if (IsStringEmpty(pFormat) || IsPointerInvalid(fp) || (uArgCount && IsPointerInvalid(ppArgs))) {
		return 0;
	}

	// Setup for parsing engine

	SafePrint::ProcessResults_t	TheProcessResults;
	SafePrint::ParamInfo_t ParamIntoTable[SafePrint::cMAX_OUTPUT_COMMANDS];
	MemoryClear(ParamIntoTable,sizeof(ParamIntoTable));

	// Analyze everything and determine what we'll output
	Word bParseOk = TheProcessResults.FormatPreProcess(SafePrint::PARSEOPTION_DEFAULT,pFormat,StringLength(pFormat),uArgCount,ppArgs,BURGER_ARRAYSIZE(ParamIntoTable),ParamIntoTable);

	// If there were no problems, format the output and send the results out via the RTL fprintf() function
	if (bParseOk && (TheProcessResults.m_uPhaseResults == SafePrint::ERROR_NONE)) {
		// format the output and send it along
		WordPtr uProcessedCount = TheProcessResults.GenerateFormattedOutputToFile(fp,FALSE,uArgCount,ppArgs,TheProcessResults.m_uNumOutputCommands,ParamIntoTable);

		// Were we error free?
		if (TheProcessResults.m_uPhaseResults == SafePrint::ERROR_NONE) {
			return static_cast<IntPtr>(uProcessedCount);
		}
	}

	// An error must have occurred.  Handle it, and get the proper return value 
	return TheProcessResults.ErrorHandler(pFormat,uArgCount,ppArgs);
}


/*! ************************************

	\brief Debug snprintf()
 
	Parse the parameters and do a printf() dump of the contents, used exclusively
	for debugging.

	* * Step 1) Parse the format string and arguments
	* * Step 2) if no issues found in Step 1, execute the list of format & copy output commands.

	\param pOutput Pointer to a output buffer
	\param uOutputSize Number of bytes in the output buffer
	\param pFormat Pointer to a "C" format string
	\param uArgCount Number of arguments in the argument array
	\param ppArgs Argument array, can be \ref NULL if uArgCount is zero

	\return Always returns zero

***************************************/

IntPtr BURGER_API Burger::DebugSnprintf(char *pOutput,WordPtr uOutputSize,const char *pFormat,WordPtr uArgCount,const SafePrintArgument **ppArgs)
{
	if (IsPointerInvalid(pOutput) || IsPointerInvalid(pFormat) || (uArgCount && IsPointerInvalid(ppArgs))) {
		return 0;
	}

	// parse loop
	SafePrint::ProcessResults_t	TheProcessResults;
	SafePrint::ParamInfo_t ParamIntoTable[SafePrint::cMAX_OUTPUT_COMMANDS];
	MemoryClear(ParamIntoTable,sizeof(ParamIntoTable));

	WordPtr uFormatLength = StringLength(pFormat);
	Word bParseOk = TheProcessResults.FormatPreProcess(SafePrint::PARSEOPTION_DEFAULT,pFormat,uFormatLength,uArgCount, ppArgs,BURGER_ARRAYSIZE(ParamIntoTable),ParamIntoTable);

	printf("\n\nprintf parse check\nFormat String Len=%zd is '%s'\n",uFormatLength,pFormat);
	printf("printfFormatPreProcess returned success=%d\n",static_cast<int>(bParseOk));

	printf("Results:  Error='%s' at char pos #%zd (char %zd in sequence)\n",SafePrint::GetErrorDescription(TheProcessResults.m_uPhaseResults),TheProcessResults.m_uErrorCharPos,TheProcessResults.m_uErrorFormatSequencePos);
	printf("          Num Output Commands =%zd (of %zd max)\n",TheProcessResults.m_uNumOutputCommands,BURGER_ARRAYSIZE(ParamIntoTable));
	printf("          output Size =%zd (of %zd buffer)\n",TheProcessResults.m_uFormattedOutputLength,uOutputSize);

	for (WordPtr i = 0 ; i < TheProcessResults.m_uNumOutputCommands; i++) {
		printf("Output Command %u:   CMD=%s\n",static_cast<Word>(i),SafePrint::GetOutputCommandName(ParamIntoTable[i].GetCommand()));
		printf("   display arg as:   %s\n",SafePrint::GetOutputDisplayType(ParamIntoTable[i].GetConversion()));
		printf("        arg index:   %d\n",static_cast<int>(ParamIntoTable[i].GetArgIndex()));
		printf("     option flags:   %4.4x\n",static_cast<int>(ParamIntoTable[i].GetFlags()));
		printf("            width:   %d",static_cast<int>(ParamIntoTable[i].GetWidth()));
		printf((ParamIntoTable[i].IsFlagSet(SafePrint::CONVFLAG_WIDTH_SPECIFIED)) ? "\n" : " (not specified)\n");
		printf("        precision:   %d",static_cast<int>(ParamIntoTable[i].GetPrecision()));
		printf((ParamIntoTable[i].IsFlagSet(SafePrint::CONVFLAG_PRECISION_SPECIFIED)) ? "\n" : " (not specified)\n");
		printf("    formatted len:  %d\n",static_cast<int>(ParamIntoTable[i].GetFormattedLength()));
		printf("       output len:  %d\n",static_cast<int>(ParamIntoTable[i].GetOutputLength()));
		printf("\n");
	}

	return 0;
}
