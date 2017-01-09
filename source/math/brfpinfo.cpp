/***************************************

	Floating point analysis

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfpinfo.h"
/*! ************************************

	\class Burger::FPInfo
	\brief Class for analyzing a floating point number

	Submitting a number into this class causes it to be broken
	down into its components so it can be queried as to the
	floating point behaviors this number represents.

	It's useful for printing functions or number analysis functions.

***************************************/

/*! ************************************

	\brief Default constructor

***************************************/

Burger::FPInfo::FPInfo(void) :
	m_uMantissa(0),
	m_uBiasedExponent(0),
	m_iExponent(0),
	m_uMantissaBitCount(0),
	m_uExponentBitCount(0),
	m_uFlags(0)
{
}

/*! ************************************

	\brief Constructor for 16 bit float
	\param sValue 16 bit floating point number
	\sa FPInfo(double) or FPInfo(float)

***************************************/

Burger::FPInfo::FPInfo(Word16 sValue)
{
	InitHalf(sValue);
}

/*! ************************************

	\brief Constructor for 32 bit float
	\param fValue 32 bit floating point number
	\sa FPInfo(Word16) or FPInfo(double)

***************************************/

Burger::FPInfo::FPInfo(float fValue)
{
	InitFloat(fValue);
}

/*! ************************************

	\brief Constructor for 64 bit float
	\param dValue 64 bit floating point number
	\sa FPInfo(Word16) or FPInfo(float)

***************************************/

Burger::FPInfo::FPInfo(double dValue)
{
	InitDouble(dValue);
}



/*! ************************************

	\fn Word64 Burger::FPInfo::GetMantissa(void) const
	\brief Get the mantissa value

	\return The mantissa of the analyzed floating point number

***************************************/

/*! ************************************

	\fn Word Burger::FPInfo::GetBiasedExponent(void) const
	\brief Get the biased exponent

	\return The exponent value as it was stored in the floating point number

***************************************/

/*! ************************************

	\fn Int Burger::FPInfo::GetExponent(void) const
	\brief Get the exponent

	Returns the exponent as it was meant to be represented, as such
	it's a signed integer.

	\return The exponent value of the floating point number

***************************************/

/*! ************************************

	\fn Word Burger::FPInfo::GetMantissaBitCount(void) const
	\brief Get the number of bits in the mantissa

	\return The mantissa size in bits of the analyzed floating point number

***************************************/

/*! ************************************

	\fn Word Burger::FPInfo::GetExponentBitCount(void) const
	\brief Get the number of bits in the exponent

	\return The exponent size in bits of the analyzed floating point number

***************************************/

/*! ************************************

	\fn Word Burger::FPInfo::IsValid(void) const
	\brief Detect if a number was analyzed

	\return \ref TRUE if an analyzed number is stored in the class, \ref FALSE if not.

***************************************/

/*! ************************************

	\fn Word Burger::FPInfo::IsNegative(void) const
	\brief Detect if a number is a negative value

	\return \ref TRUE if the number is negative, \ref FALSE if not

***************************************/

/*! ************************************

	\fn Word Burger::FPInfo::IsZero(void) const
	\brief Detect if a number is zero

	This returns \ref TRUE for both positive and negative zero.

	\return \ref TRUE if the number is zero, \ref FALSE if not

***************************************/

/*! ************************************

	\fn Word Burger::FPInfo::IsNAN(void) const
	\brief Detect if a number is Not a Number

	This returns \ref TRUE for all forms of Not a Number.

	\return \ref TRUE if the number is Not a Number, \ref FALSE if it's a valid number

***************************************/

/*! ************************************

	\fn Word Burger::FPInfo::IsQNAN(void) const
	\brief Detect if a number is a quiet Not a Number

	\return \ref TRUE if the number is a quiet Not a Number, \ref FALSE if it's a valid number

***************************************/

/*! ************************************

	\fn Word Burger::FPInfo::IsIndefiniteNAN(void) const
	\brief Detect if a number is an indefinite Not a Number

	\return \ref TRUE if the number is an indefinite Not a Number, \ref FALSE if it's a valid number

***************************************/

/*! ************************************

	\fn Word Burger::FPInfo::IsInfinity(void) const
	\brief Detect if a number is infinity

	\return \ref TRUE if the number is infinity, \ref FALSE if it's not

***************************************/

/*! ************************************

	\fn Word Burger::FPInfo::IsSubNormal(void) const
	\brief Detect if a number is a non normalized number

	\return \ref TRUE if the number is not normalized, \ref FALSE if it is.

***************************************/






/*! ************************************

	\brief Process a 16 bit floating point number

	Given a 16 bit floating point number, process it into its
	component parts.

	\param sValue 16 bit float point number

***************************************/

void BURGER_API Burger::FPInfo::InitHalf(Word16 sValue)
{
	Word uTemp = sValue;

	// Break it down
	Word uBiasedExponent = (uTemp & cExponentMask16) >> cMantissaBitCount16;
	m_uMantissa = (uTemp & cMantissaMask16);
	m_uBiasedExponent = uBiasedExponent;
	m_iExponent = static_cast<Int32>(uBiasedExponent - cExponentBias16);

	// Save the size constants
	m_uMantissaBitCount = cMantissaBitCount16+1;
	m_uExponentBitCount = cExponentBitCount16;

	// Test if negative
	Word uFlags = cFlagValid;
	if (uTemp & cSignBit16) {
		uFlags |= cFlagNegative;
	}

	// Test if zero
	if (!(uTemp & (cExponentMask16|cMantissaMask16))) {
		uFlags |= cFlagZero;
	} else {

		// Do we have have a NAN or Infinity?
		if (uBiasedExponent == cSpecialExponent16) {
			// Zero mantissa means infinity
			if (!(uTemp & cMantissaMask16)) {
				uFlags |= cFlagInfinity;
			} else {
				// It's a NAN baby!
				uFlags |= cFlagNAN;

				// Get the bit that determines if it is a NAN
				const Word uNANBit = 1U << (cMantissaBitCount16-1);

				// Determine if is a quiet NaN and possibly also an Indefinite NaN in addition
				if (uTemp & uNANBit) {
					// Quiet nan
					uFlags |= cFlagQNAN;

					// Test for indefinite NAN
					if ((uFlags&cFlagNegative) && !(uTemp & (uNANBit-1))) {
						uFlags |= cFlagIndefiniteNAN;
					}
				}
			}
		} else if (!uBiasedExponent) {
			// Do we have a subnormal number?
			uFlags |= cFlagSubNormal;
			++m_iExponent;			// Adjust exponent, but no implied bit
		} else {
			// Normal number, needs the implicit bit
			m_uMantissa |= (1U << cMantissaBitCount16);
		}
	}

	// Store the flags
	m_uFlags = uFlags;
}

/*! ************************************

	\brief Process a 32 bit floating point number

	Given a 32 bit floating point number, process it into its
	component parts.

	\param fValue 32 bit float point number

***************************************/

void BURGER_API Burger::FPInfo::InitFloat(float fValue)
{
	union {
		Word32 m_uBits;
		float m_fFloat;
	} Converter;

	Converter.m_fFloat = fValue;
	Word uTemp = Converter.m_uBits;

	// Break it down
	Word uBiasedExponent = (uTemp & cExponentMask32) >> cMantissaBitCount32;
	m_uMantissa = (uTemp & cMantissaMask32);
	m_uBiasedExponent = uBiasedExponent;
	m_iExponent = static_cast<Int32>(uBiasedExponent - cExponentBias32);

	// Save the size constants
	m_uMantissaBitCount = cMantissaBitCount32+1;
	m_uExponentBitCount = cExponentBitCount32;

	// Test if negative
	Word uFlags = cFlagValid;
	if (uTemp & cSignBit32) {
		uFlags |= cFlagNegative;
	}

	// Test if zero
	if (!(uTemp & (cExponentMask32|cMantissaMask32))) {
		uFlags |= cFlagZero;
	} else {

		// Do we have have a NAN or Infinity?
		if (uBiasedExponent == cSpecialExponent32) {
			// Zero mantissa means infinity
			if (!(uTemp & cMantissaMask32)) {
				uFlags |= cFlagInfinity;
			} else {
				// It's a NAN baby!
				uFlags |= cFlagNAN;

				// Get the bit that determines if it is a NAN
				const Word uNANBit = 1U << (cMantissaBitCount32-1);

				// Determine if is a quiet NaN and possibly also an Indefinite NaN in addition
				if (uTemp & uNANBit) {
					// Quiet nan
					uFlags |= cFlagQNAN;

					// Test for indefinite NAN
					if ((uFlags&cFlagNegative) && !(uTemp & (uNANBit-1))) {
						uFlags |= cFlagIndefiniteNAN;
					}
				}
			}
		} else if (!uBiasedExponent) {
			// Do we have a subnormal number?
			uFlags |= cFlagSubNormal;
			++m_iExponent;			// Adjust exponent, but no implied bit
		} else {
			// Normal number, needs the implicit bit
			m_uMantissa |= (1U << cMantissaBitCount32);
		}
	}

	// Store the flags
	m_uFlags = uFlags;
}

/*! ************************************

	\brief Process a 64 bit floating point number

	Given a 64 bit floating point number, process it into its
	component parts.

	\param dValue 64 bit float point number

***************************************/

void BURGER_API Burger::FPInfo::InitDouble(double dValue)
{
	union {
		Word64 m_uBits;
		double m_dDouble;
	} Converter;

	Converter.m_dDouble = dValue;
	Word64 uTemp = Converter.m_uBits;

	// Break it down
	Word uBiasedExponent = static_cast<Word>((uTemp & cExponentMask64) >> cMantissaBitCount64);
	m_uMantissa = (uTemp & cMantissaMask64);
	m_uBiasedExponent = uBiasedExponent;
	m_iExponent = static_cast<Int32>(uBiasedExponent - cExponentBias64);

	// Save the size constants
	m_uMantissaBitCount = cMantissaBitCount64+1;
	m_uExponentBitCount = cExponentBitCount64;

	// Test if negative
	Word uFlags = cFlagValid;
	if (uTemp & cSignBit64) {
		uFlags |= cFlagNegative;
	}

	// Test if zero
	if (!(uTemp & (cExponentMask64|cMantissaMask64))) {
		uFlags |= cFlagZero;
	} else {

		// Do we have have a NAN or Infinity?
		if (uBiasedExponent == cSpecialExponent64) {
			// Zero mantissa means infinity
			if (!(uTemp & cMantissaMask64)) {
				uFlags |= cFlagInfinity;
			} else {
				// It's a NAN baby!
				uFlags |= cFlagNAN;

				// Get the bit that determines if it is a NAN
				const Word64 uNANBit = 1ULL << (cMantissaBitCount64-1);

				// Determine if is a quiet NaN and possibly also an Indefinite NaN in addition
				if (uTemp & uNANBit) {
					// Quiet nan
					uFlags |= cFlagQNAN;

					// Test for indefinite NAN
					if ((uFlags&cFlagNegative) && !(uTemp & (uNANBit-1))) {
						uFlags |= cFlagIndefiniteNAN;
					}
				}
			}
		} else if (!uBiasedExponent) {
			// Do we have a subnormal number?
			uFlags |= cFlagSubNormal;
			++m_iExponent;			// Adjust exponent, but no implied bit
		} else {
			// Normal number, needs the implicit bit
			m_uMantissa |= (1ULL << cMantissaBitCount64);
		}
	}

	// Store the flags
	m_uFlags = uFlags;
}
