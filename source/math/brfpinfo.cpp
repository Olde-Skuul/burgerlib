/***************************************

    Floating point analysis

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfpinfo.h"

/*! ************************************

    \class Burger::FPInfo
    \brief Class for analyzing a floating point number

    Submitting a number into this class causes it to be broken down into its
    components so it can be queried as to the floating point behaviors this
    number represents.

    It's useful for printing functions or number analysis functions.

***************************************/

/*! ************************************

    \brief Default constructor

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

    \param sValue 16 bit floating point number
    \sa FPInfo(double) or FPInfo(float)

***************************************/

Burger::FPInfo::FPInfo(uint16_t sValue) BURGER_NOEXCEPT
{
    InitHalf(sValue);
}

/*! ************************************

    \brief Constructor for 32 bit float
    \param fValue 32 bit floating point number
    \sa FPInfo(uint16_t) or FPInfo(double)

***************************************/

Burger::FPInfo::FPInfo(float fValue) BURGER_NOEXCEPT
{
    InitFloat(fValue);
}

/*! ************************************

    \brief Constructor for 64 bit float
    \param dValue 64 bit floating point number
    \sa FPInfo(uint16_t) or FPInfo(float)

***************************************/

Burger::FPInfo::FPInfo(double dValue) BURGER_NOEXCEPT
{
    InitDouble(dValue);
}

/*! ************************************

    \fn Burger::FPInfo::GetMantissa(void) const
    \brief Get the mantissa value

    \return The mantissa of the analyzed floating point number

***************************************/

/*! ************************************

    \fn Burger::FPInfo::GetBiasedExponent(void) const
    \brief Get the biased exponent

    \return The exponent value as it was stored in the floating point number

***************************************/

/*! ************************************

    \fn Burger::FPInfo::GetExponent(void) const
    \brief Get the exponent

    Returns the exponent as it was meant to be represented, as such, it's a
    signed integer.

    \return The exponent value of the floating point number

***************************************/

/*! ************************************

    \fn Burger::FPInfo::GetMantissaBitCount(void) const
    \brief Get the number of bits in the mantissa

    \return The mantissa size in bits of the analyzed floating point number

***************************************/

/*! ************************************

    \fn Burger::FPInfo::GetExponentBitCount(void) const
    \brief Get the number of bits in the exponent

    \return The exponent size in bits of the analyzed floating point number

***************************************/

/*! ************************************

    \fn Burger::FPInfo::IsValid(void) const
    \brief Detect if a number was analyzed

    \return \ref TRUE if an analyzed number is stored in the class, \ref FALSE
        if not.

***************************************/

/*! ************************************

    \fn Burger::FPInfo::IsNegative(void) const
    \brief Detect if a number is a negative value

    \return \ref TRUE if the number is negative, \ref FALSE if not

***************************************/

/*! ************************************

    \fn Burger::FPInfo::IsZero(void) const
    \brief Detect if a number is zero

    This returns \ref TRUE for both positive and negative zero.

    \return \ref TRUE if the number is zero, \ref FALSE if not

***************************************/

/*! ************************************

    \fn Burger::FPInfo::IsNAN(void) const
    \brief Detect if a number is Not a Number

    This returns \ref TRUE for all forms of Not a Number.

    \return \ref TRUE if the number is Not a Number, \ref FALSE if it's a valid
        number

***************************************/

/*! ************************************

    \fn Burger::FPInfo::IsQNAN(void) const
    \brief Detect if a number is a quiet Not a Number

    \return \ref TRUE if the number is a quiet Not a Number, \ref FALSE if it's
        a valid number

***************************************/

/*! ************************************

    \fn Burger::FPInfo::IsIndefiniteNAN(void) const
    \brief Detect if a number is an indefinite Not a Number

    \return \ref TRUE if the number is an indefinite Not a Number, \ref FALSE if
        it's a valid number

***************************************/

/*! ************************************

    \fn Burger::FPInfo::IsInfinity(void) const
    \brief Detect if a number is infinity

    \return \ref TRUE if the number is infinity, \ref FALSE if it's not

***************************************/

/*! ************************************

    \fn Burger::FPInfo::IsSubNormal(void) const
    \brief Detect if a number is a non normalized number

    \return \ref TRUE if the number is not normalized, \ref FALSE if it is.

***************************************/

/*! ************************************

    \brief Process a 16 bit floating point number

    Given a 16 bit floating point number, process it into its component parts.

    \param sValue 16 bit float point number

***************************************/

void BURGER_API Burger::FPInfo::InitHalf(uint16_t sValue) BURGER_NOEXCEPT
{
    const uint_t uTemp = sValue;

    // Break it down
    const uint_t uBiasedExponent =
        (uTemp & kExponentMask16) >> kMantissaBitCount16;
    m_uMantissa = (uTemp & kMantissaMask16);
    m_uBiasedExponent = uBiasedExponent;
    m_iExponent = static_cast<int_t>(uBiasedExponent - kExponentBias16);

    // Save the size constants
    m_uMantissaBitCount = kMantissaBitCount16 + 1;
    m_uExponentBitCount = kExponentBitCount16;

    // Test if negative
    uint_t uFlags = kFlagValid;
    if (uTemp & kSignBit16) {
        uFlags |= kFlagNegative;
    }

    // Test if zero
    if (!(uTemp & (kExponentMask16 | kMantissaMask16))) {
        uFlags |= kFlagZero;
    } else {

        // Do we have have a NAN or Infinity?
        if (uBiasedExponent == kSpecialExponent16) {
            // Zero mantissa means infinity
            if (!(uTemp & kMantissaMask16)) {
                uFlags |= kFlagInfinity;
            } else {
                // It's a NAN baby!
                uFlags |= kFlagNAN;

                // Get the bit that determines if it is a NAN
                BURGER_CONSTEXPR const uint_t uNANBit = 1U
                    << (kMantissaBitCount16 - 1);

                // Determine if is a quiet NaN and possibly also an Indefinite
                // NaN in addition
                if (uTemp & uNANBit) {
                    // Quiet nan
                    uFlags |= kFlagQNAN;

                    // Test for indefinite NAN
                    if ((uFlags & kFlagNegative) && !(uTemp & (uNANBit - 1))) {
                        uFlags |= kFlagIndefiniteNAN;
                    }
                }
            }
        } else if (!uBiasedExponent) {
            // Do we have a subnormal number?
            uFlags |= kFlagSubNormal;
            ++m_iExponent; // Adjust exponent, but no implied bit
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

void BURGER_API Burger::FPInfo::InitFloat(float fValue) BURGER_NOEXCEPT
{
    union {
        uint32_t m_uBits;
        float m_fFloat;
    } Converter;

    Converter.m_fFloat = fValue;
    const uint_t uTemp = Converter.m_uBits;

    // Break it down
    const uint_t uBiasedExponent =
        (uTemp & kExponentMask32) >> kMantissaBitCount32;
    m_uMantissa = (uTemp & kMantissaMask32);
    m_uBiasedExponent = uBiasedExponent;
    m_iExponent = static_cast<int_t>(uBiasedExponent - kExponentBias32);

    // Save the size constants
    m_uMantissaBitCount = kMantissaBitCount32 + 1;
    m_uExponentBitCount = kExponentBitCount32;

    // Test if negative
    uint_t uFlags = kFlagValid;
    if (uTemp & kSignBit32) {
        uFlags |= kFlagNegative;
    }

    // Test if zero
    if (!(uTemp & (kExponentMask32 | kMantissaMask32))) {
        uFlags |= kFlagZero;
    } else {

        // Do we have have a NAN or Infinity?
        if (uBiasedExponent == kSpecialExponent32) {
            // Zero mantissa means infinity
            if (!(uTemp & kMantissaMask32)) {
                uFlags |= kFlagInfinity;
            } else {
                // It's a NAN baby!
                uFlags |= kFlagNAN;

                // Get the bit that determines if it is a NAN
                BURGER_CONSTEXPR const uint_t uNANBit = 1U
                    << (kMantissaBitCount32 - 1);

                // Determine if is a quiet NaN and possibly also an Indefinite
                // NaN in addition
                if (uTemp & uNANBit) {
                    // Quiet nan
                    uFlags |= kFlagQNAN;

                    // Test for indefinite NAN
                    if ((uFlags & kFlagNegative) && !(uTemp & (uNANBit - 1))) {
                        uFlags |= kFlagIndefiniteNAN;
                    }
                }
            }
        } else if (!uBiasedExponent) {
            // Do we have a subnormal number?
            uFlags |= kFlagSubNormal;
            ++m_iExponent; // Adjust exponent, but no implied bit
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

void BURGER_API Burger::FPInfo::InitDouble(double dValue) BURGER_NOEXCEPT
{
    union {
        uint64_t m_uBits;
        double m_dDouble;
    } Converter;

    Converter.m_dDouble = dValue;
    const uint64_t uTemp = Converter.m_uBits;

    // Break it down
    const uint_t uBiasedExponent =
        static_cast<uint_t>((uTemp & kExponentMask64) >> kMantissaBitCount64);
    m_uMantissa = (uTemp & kMantissaMask64);
    m_uBiasedExponent = uBiasedExponent;
    m_iExponent = static_cast<int_t>(uBiasedExponent - kExponentBias64);

    // Save the size constants
    m_uMantissaBitCount = kMantissaBitCount64 + 1;
    m_uExponentBitCount = kExponentBitCount64;

    // Test if negative
    uint_t uFlags = kFlagValid;
    if (uTemp & kSignBit64) {
        uFlags |= kFlagNegative;
    }

    // Test if zero
    if (!(uTemp & (kExponentMask64 | kMantissaMask64))) {
        uFlags |= kFlagZero;
    } else {

        // Do we have have a NAN or Infinity?
        if (uBiasedExponent == kSpecialExponent64) {
            // Zero mantissa means infinity
            if (!(uTemp & kMantissaMask64)) {
                uFlags |= kFlagInfinity;
            } else {
                // It's a NAN baby!
                uFlags |= kFlagNAN;

                // Get the bit that determines if it is a NAN
                BURGER_CONSTEXPR const uint64_t uNANBit = 1ULL
                    << (kMantissaBitCount64 - 1);

                // Determine if is a quiet NaN and possibly also an Indefinite
                // NaN in addition
                if (uTemp & uNANBit) {
                    // Quiet nan
                    uFlags |= kFlagQNAN;

                    // Test for indefinite NAN
                    if ((uFlags & kFlagNegative) && !(uTemp & (uNANBit - 1))) {
                        uFlags |= kFlagIndefiniteNAN;
                    }
                }
            }
        } else if (!uBiasedExponent) {
            // Do we have a subnormal number?
            uFlags |= kFlagSubNormal;
            ++m_iExponent; // Adjust exponent, but no implied bit
        } else {
            // Normal number, needs the implicit bit
            m_uMantissa |= (1ULL << kMantissaBitCount64);
        }
    }

    // Store the flags
    m_uFlags = uFlags;
}
