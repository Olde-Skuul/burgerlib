/***************************************

	Floating point analysis

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfpinfo.h"
#include "brstructs.h"

/*! ************************************

	\class Burger::FPInfo
	\brief Class for analyzing a floating point number

	Submitting a number into this class causes it to be broken down into its
	components so it can be queried as to the floating point behaviors this
	number represents.

	It's useful for printing functions or number analysis functions.

	For lightweight tests, use the explicit calls such at is_NaN(float),
	is_infinite(double), etc

	When a floating point number is assigned to this class, it is broken down to
	the type of value (Infinity, NaN, Subnormal), the mantissa, the exponent and
	the sign. The values can be accessed with the calls get_mantissa(),
	get_exponent(), get_mantissa_bit_count(), etc.

	\sa is_NaN(float), is_NaN(), get_mantissa(), get_exponent()

***************************************/

/*! ************************************

	\brief Default constructor

	All values are set to zero. Use the Init() functions or assignment operators
	to perform floating point analysis.

	\sa init_float(float), or FPInfo::operator = (float)

***************************************/

Burger::FPInfo::FPInfo(void) BURGER_NOEXCEPT: m_uMantissa(0),
											  m_uBiasedExponent(0),
											  m_iExponent(0),
											  m_uMantissaBitCount(0),
											  m_uExponentBitCount(0),
											  m_uFlags(0)
{
}

/*! ************************************

	\brief Constructor for 16 bit float

	\param uValue 16 bit floating point number

	\sa init_half(uint16_t), FPInfo(double) or FPInfo(float)

***************************************/

Burger::FPInfo::FPInfo(uint16_t uValue) BURGER_NOEXCEPT
{
	init_half(uValue);
}

/*! ************************************

	\brief Constructor for 32 bit float

	\param fValue 32 bit floating point number

	\sa init_float(float), FPInfo(uint16_t) or FPInfo(double)

***************************************/

Burger::FPInfo::FPInfo(float fValue) BURGER_NOEXCEPT
{
	init_float(fValue);
}

/*! ************************************

	\brief Constructor for 64 bit float

	\param dValue 64 bit floating point number

	\sa init_double(double), FPInfo(uint16_t) or FPInfo(float)

***************************************/

Burger::FPInfo::FPInfo(double dValue) BURGER_NOEXCEPT
{
	init_double(dValue);
}

/*! ************************************

	\brief Process a 16 bit floating point number

	Given a 16 bit floating point number, process it into its component parts.

	\param uValue 16 bit float point number

***************************************/

void BURGER_API Burger::FPInfo::init_half(uint16_t uValue) BURGER_NOEXCEPT
{
	const uint32_t uTemp = uValue;

	// Break it down
	const uint32_t uBiasedExponent =
		(uTemp & kExponentMask16) >> kMantissaBitCount16;
	m_uMantissa = (uTemp & kMantissaMask16);
	m_uBiasedExponent = uBiasedExponent;
	m_iExponent = static_cast<int32_t>(uBiasedExponent - kExponentBias16);

	// Save the size constants
	m_uMantissaBitCount = kMantissaBitCount16 + 1U;
	m_uExponentBitCount = kExponentBitCount16;

	// Test if negative
	uint32_t uFlags = 1U << kFlagValidShift;
	if (uTemp & kSignBit16) {
		uFlags |= (1U << kFlagNegativeShift);
	}

	// Test if zero
	if (!(uTemp & (kExponentMask16 | kMantissaMask16))) {
		uFlags |= (1U << kFlagZeroShift);
	} else {

		// Do we have have a NAN or Infinity?
		if (uBiasedExponent == kSpecialExponent16) {

			// Zero mantissa means infinity
			if (!(uTemp & kMantissaMask16)) {
				uFlags |= (1U << kFlagInfinityShift);
			} else {

				// It's a NAN baby!
				uFlags |= (1U << kFlagNANShift);

				// Get the bit that determines if it is a NAN
				BURGER_CONSTEXPR const uint32_t uNANBit = 1U
					<< (kMantissaBitCount16 - 1);

				// Determine if is a quiet NaN and possibly also an Indefinite
				// NaN in addition
				if (uTemp & uNANBit) {
					// Quiet nan
					uFlags |= (1U << kFlagQNANShift);

					// Test for indefinite NAN
					if ((uFlags & (1U << kFlagNegativeShift)) &&
						!(uTemp & (uNANBit - 1))) {
						uFlags |= (1U << kFlagIndefiniteNANShift);
					}
				}
			}
		} else if (!uBiasedExponent) {

			// Do we have a subnormal number?
			uFlags |= (1U << kFlagSubNormalShift);

			// Adjust exponent, but no implied bit
			++m_iExponent;
		} else {
			// Normal number, needs the implicit bit
			m_uMantissa |= (1U << kMantissaBitCount16);
		}
	}

	// Store the flags
	m_uFlags = uFlags;
}

/*! ************************************

	\brief Process a 32 bit floating point number

	Given a 32 bit floating point number, process it into its component parts.

	\param fValue 32 bit float point number

***************************************/

void BURGER_API Burger::FPInfo::init_float(float fValue) BURGER_NOEXCEPT
{
	// Get the binary representation of the float
	uint32_float_t Converter;
	Converter.set_float(fValue);
	const uint32_t uTemp = Converter.get_uint32();

	// Break it down
	const uint32_t uBiasedExponent =
		(uTemp & kExponentMask32) >> kMantissaBitCount32;
	m_uMantissa = (uTemp & kMantissaMask32);
	m_uBiasedExponent = uBiasedExponent;
	m_iExponent = static_cast<int32_t>(uBiasedExponent - kExponentBias32);

	// Save the size constants
	m_uMantissaBitCount = kMantissaBitCount32 + 1U;
	m_uExponentBitCount = kExponentBitCount32;

	// Test if negative
	uint32_t uFlags = 1U << kFlagValidShift;
	if (uTemp & kSignBit32) {
		uFlags |= (1U << kFlagNegativeShift);
	}

	// Test if zero
	if (!(uTemp & (kExponentMask32 | kMantissaMask32))) {
		uFlags |= (1U << kFlagZeroShift);
	} else {

		// Do we have have a NAN or Infinity?
		if (uBiasedExponent == kSpecialExponent32) {

			// Zero mantissa means infinity
			if (!(uTemp & kMantissaMask32)) {
				uFlags |= (1U << kFlagInfinityShift);
			} else {

				// It's a NAN baby!
				uFlags |= (1U << kFlagNANShift);

				// Get the bit that determines if it is a NAN
				BURGER_CONSTEXPR const uint32_t uNANBit = 1U
					<< (kMantissaBitCount32 - 1);

				// Determine if is a quiet NaN and possibly also an Indefinite
				// NaN in addition
				if (uTemp & uNANBit) {

					// Quiet nan
					uFlags |= (1U << kFlagQNANShift);

					// Test for indefinite NAN
					if ((uFlags & (1U << kFlagNegativeShift)) &&
						!(uTemp & (uNANBit - 1))) {
						uFlags |= (1U << kFlagIndefiniteNANShift);
					}
				}
			}
		} else if (!uBiasedExponent) {

			// Do we have a subnormal number?
			uFlags |= (1U << kFlagSubNormalShift);

			// Adjust exponent, but no implied bit
			++m_iExponent;
		} else {
			// Normal number, needs the implicit bit
			m_uMantissa |= (1U << kMantissaBitCount32);
		}
	}

	// Store the flags
	m_uFlags = uFlags;
}

/*! ************************************

	\brief Process a 64 bit floating point number

	Given a 64 bit floating point number, process it into its component parts.

	\param dValue 64 bit float point number

***************************************/

void BURGER_API Burger::FPInfo::init_double(double dValue) BURGER_NOEXCEPT
{
	uint64_double_t Converter;
	Converter.set_double(dValue);
	const uint64_t uTemp = Converter.get_uint64();

	// Break it down
	const uint32_t uBiasedExponent =
		static_cast<uint32_t>((uTemp & kExponentMask64) >> kMantissaBitCount64);
	m_uMantissa = (uTemp & kMantissaMask64);
	m_uBiasedExponent = uBiasedExponent;
	m_iExponent = static_cast<int32_t>(uBiasedExponent - kExponentBias64);

	// Save the size constants
	m_uMantissaBitCount = kMantissaBitCount64 + 1;
	m_uExponentBitCount = kExponentBitCount64;

	// Test if negative
	uint32_t uFlags = 1U << kFlagValidShift;
	if (uTemp & kSignBit64) {
		uFlags |= (1U << kFlagNegativeShift);
	}

	// Test if zero
	if (!(uTemp & (kExponentMask64 | kMantissaMask64))) {
		uFlags |= (1U << kFlagZeroShift);
	} else {

		// Do we have have a NAN or Infinity?
		if (uBiasedExponent == kSpecialExponent64) {

			// Zero mantissa means infinity
			if (!(uTemp & kMantissaMask64)) {
				uFlags |= (1U << kFlagInfinityShift);
			} else {

				// It's a NAN baby!
				uFlags |= (1U << kFlagNANShift);

				// Get the bit that determines if it is a NAN
				BURGER_CONSTEXPR const uint64_t uNANBit = 1ULL
					<< (kMantissaBitCount64 - 1);

				// Determine if is a quiet NaN and possibly also an Indefinite
				// NaN in addition
				if (uTemp & uNANBit) {

					// Quiet nan
					uFlags |= (1U << kFlagQNANShift);

					// Test for indefinite NAN
					if ((uFlags & (1U << kFlagNegativeShift)) &&
						!(uTemp & (uNANBit - 1))) {
						uFlags |= (1U << kFlagIndefiniteNANShift);
					}
				}
			}
		} else if (!uBiasedExponent) {

			// Do we have a subnormal number?
			uFlags |= (1U << kFlagSubNormalShift);
			// Adjust exponent, but no implied bit
			++m_iExponent;
		} else {
			// Normal number, needs the implicit bit
			m_uMantissa |= (1ULL << kMantissaBitCount64);
		}
	}

	// Store the flags
	m_uFlags = uFlags;
}

/*! ************************************

	\fn Burger::FPInfo::get_mantissa(void) const
	\brief Get the mantissa value

	\return The mantissa of the analyzed floating point number

***************************************/

/*! ************************************

	\fn Burger::FPInfo::get_biased_exponent(void) const
	\brief Get the biased exponent

	\return The exponent value as it was stored in the floating point number

***************************************/

/*! ************************************

	\fn Burger::FPInfo::get_exponent(void) const
	\brief Get the exponent

	Returns the exponent as it was meant to be represented, as such, it's a
	signed integer.

	\return The exponent value of the floating point number

***************************************/

/*! ************************************

	\fn Burger::FPInfo::get_mantissa_bit_count(void) const
	\brief Get the number of bits in the mantissa

	\return The mantissa size in bits of the analyzed floating point number

***************************************/

/*! ************************************

	\fn Burger::FPInfo::get_exponent_bit_count(void) const
	\brief Get the number of bits in the exponent

	\return The exponent size in bits of the analyzed floating point number

***************************************/

/*! ************************************

	\fn Burger::FPInfo::is_valid(void) const
	\brief Detect if a number was analyzed

	\return \ref TRUE if an analyzed number is stored in the class, \ref FALSE
		if not.

***************************************/

/*! ************************************

	\fn Burger::FPInfo::is_negative(void) const
	\brief Detect if a number is a negative value

	\return \ref TRUE if the number is negative, \ref FALSE if not

***************************************/

/*! ************************************

	\fn Burger::FPInfo::is_zero(void) const
	\brief Detect if a number is zero

	This returns \ref TRUE for both positive and negative zero.

	\return \ref TRUE if the number is zero, \ref FALSE if not

***************************************/

/*! ************************************

	\fn Burger::FPInfo::is_NaN(void) const
	\brief Detect if a number is Not a Number

	This returns \ref TRUE for all forms of Not a Number.

	\return \ref TRUE if the number is Not a Number, \ref FALSE if it's a valid
		number

	\sa is_NaN(float) or is_NaN(double)

***************************************/

/*! ************************************

	\fn Burger::FPInfo::is_QNaN(void) const
	\brief Detect if a number is a quiet Not a Number

	\return \ref TRUE if the number is a quiet Not a Number, \ref FALSE if it's
		a valid number

***************************************/

/*! ************************************

	\fn Burger::FPInfo::is_indefinite_NaN(void) const
	\brief Detect if a number is an indefinite Not a Number

	\return \ref TRUE if the number is an indefinite Not a Number, \ref FALSE if
		it's a valid number

***************************************/

/*! ************************************

	\fn Burger::FPInfo::is_infinite(void) const
	\brief Detect if a number is infinity

	\return \ref TRUE if the number is infinity, \ref FALSE if it's not

	\sa is_infinite(float) or is_infinite(double)

***************************************/

/*! ************************************

	\fn Burger::FPInfo::is_subnormal(void) const
	\brief Detect if a number is a non normalized number

	\return \ref TRUE if the number is not normalized, \ref FALSE if it is.

***************************************/

/*! ************************************

	\fn Burger::FPInfo::is_finite(void) const
	\brief Detect if a number is a finite number

	If the number is infinity or NaN, it will return \ref FALSE. Otherwise it
	will return \ref TRUE.

	\return \ref TRUE if the number is finite, \ref FALSE if it is.

	\sa is_finite(float) or is_finite(double)

***************************************/

/*! ************************************

	\fn Burger::FPInfo::operator=(uint16_t usValue)
	\brief Process a 16 bit floating point number

	Given a 16 bit floating point number, process it into its component parts.

	\param usValue 16 bit float point number

***************************************/

/*! ************************************

	\fn Burger::FPInfo::operator=(float fValue)
	\brief Process a 32 bit floating point number

	Given a 32 bit floating point number, process it into its component parts.

	\param fValue 32 bit float point number

***************************************/

/*! ************************************

	\fn Burger::FPInfo::operator=(double dValue)
	\brief Process a 64 bit floating point number

	Given a 64 bit floating point number, process it into its component parts.

	\param dValue 64 bit float point number

***************************************/
