/***************************************

	Floating point analysis

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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

	static const Word cSignBit16 = 0x8000U;							///< Location of the sign bit in a 16 bit float
	static const Word cExponentMask16 = 0x7C00U;					///< Mask for the exponent in a 16 bit float
	static const Word cMantissaMask16 = 0x03FFU;					///< Mask for the mantissa in a 16 bit float
	static const Word cImplicitBit16 = 0x0400U;						///< Implicit "1" bit for a 16 bit float
	static const Word cMantissaBitCount16 = 10;						///< Number of bits in the mantissa for a 16 bit float
	static const Word cExponentBitCount16 = 5;						///< Number of bits in the exponent for a 16 bit float
	static const Word cExponentBias16 = 15;							///< Bias of the exponent for a 16 bit float
	static const Word cSpecialExponent16 = 0x1FU;					///< Special exponent for INF/NAN in a 16 bit float
	static const Word cNANTypeBit16 = 0x0200U;						///< QNAN or SNAN bit in a 16 bit float

	static const Word cSignBit32 = 0x80000000U;						///< Location of the sign bit in a 32 bit float
	static const Word cExponentMask32 = 0x7F800000U;				///< Mask for the exponent in a 32 bit float
	static const Word cMantissaMask32 = 0x007FFFFFU;				///< Mask for the mantissa in a 32 bit float
	static const Word cImplicitBit32 = 0x00800000U;					///< Implicit "1" bit for a 32 bit float
	static const Word cMantissaBitCount32 = 23;						///< Number of bits in the mantissa for a 32 bit float
	static const Word cExponentBitCount32 = 8;						///< Number of bits in the exponent for a 32 bit float
	static const Word cExponentBias32 = 127;						///< Bias of the exponent for a 32 bit float
	static const Word cSpecialExponent32 = 0xFFU;					///< Special exponent for INF/NAN in a 32 bit float
	static const Word cNANTypeBit32 = 0x00400000U;					///< QNAN or SNAN bit in a 32 bit float

	static const Word64 cSignBit64 = 0x8000000000000000ULL;			///< Location of the sign bit in a 64 bit float
	static const Word64 cExponentMask64 = 0x7FF0000000000000ULL;	///< Mask for the exponent in a 64 bit float
	static const Word64 cMantissaMask64 = 0x000FFFFFFFFFFFFFULL;	///< Mask for the mantissa in a 64 bit float
	static const Word64 cImplicitBit64 = 0x0010000000000000ULL;		///< Implicit "1" bit for a 64 bit float
	static const Word cMantissaBitCount64 = 52;						///< Number of bits in the mantissa for a 64 bit float
	static const Word cExponentBitCount64 = 11;						///< Number of bits in the exponent for a 64 bit float	
	static const Word cExponentBias64 = 1023;						///< Bias of the exponent for a 64 bit float
	static const Word cSpecialExponent64 = 0x7FFU;					///< Special exponent for INF/NAN in a 64 bit float
	static const Word64 cNANTypeBit64 = 0x0008000000000000ULL;		///< QNAN or SNAN bit in a 64 bit float

	enum {
		cFlagValid = 0x01,			///< Flag that's set if a floating point number was analyzed
		cFlagNegative = 0x02,		///< Flag that's set if the number is negative
		cFlagZero = 0x04,			///< Flag that's set if	the number is zero
		cFlagNAN = 0x08,			///< Flag that's set if the number is a "Not a Number"
		cFlagQNAN = 0x10,			///< Flag that's set if the number is a quiet "Not a Number"
		cFlagIndefiniteNAN = 0x20,	///< Flag that's set if the number is an unknown "Not a Number"
		cFlagInfinity = 0x40,		///< Flag that's set if the number is infinite
		cFlagSubNormal = 0x80		///< Flag that's set if the number is not normalized
	};

	Word64 m_uMantissa;			///< The mantissa value for the floating point number
	Word m_uBiasedExponent;		///< Exponent as stored in the floating point number
	Int32 m_iExponent;			///< Exponent with bias applied
	Word m_uMantissaBitCount;	///< Number of bits in the mantissa
	Word m_uExponentBitCount;	///< Number of bits in the exponent
	Word m_uFlags;				///< Flags that contain the determined states

public:
	explicit FPInfo(void);
	explicit FPInfo(Word16 sValue);
	explicit FPInfo(float fValue);
	explicit FPInfo(double dValue);
	BURGER_INLINE Word64 GetMantissa(void) const { return m_uMantissa; }
	BURGER_INLINE Word GetBiasedExponent(void) const { return m_uBiasedExponent; }
	BURGER_INLINE Int GetExponent(void) const { return m_iExponent; }
	BURGER_INLINE Word GetMantissaBitCount(void) const { return m_uMantissaBitCount; }
	BURGER_INLINE Word GetExponentBitCount(void) const { return m_uExponentBitCount; }
	BURGER_INLINE Word IsValid(void) const { return (m_uFlags&cFlagValid)!=0; }
	BURGER_INLINE Word IsNegative(void) const { return (m_uFlags&cFlagNegative)!=0; }
	BURGER_INLINE Word IsZero(void) const { return (m_uFlags&cFlagZero)!=0; }
	BURGER_INLINE Word IsNAN(void) const { return (m_uFlags&cFlagNAN)!=0; }
	BURGER_INLINE Word IsQNAN(void) const { return (m_uFlags&cFlagQNAN)!=0; }
	BURGER_INLINE Word IsIndefiniteNAN(void) const { return (m_uFlags&cFlagIndefiniteNAN)!=0; }
	BURGER_INLINE Word IsInfinity(void) const { return (m_uFlags&cFlagInfinity)!=0; }
	BURGER_INLINE Word IsSubNormal(void) const { return (m_uFlags&cFlagSubNormal)!=0; }
	void BURGER_API InitHalf(Word16 sValue);
	void BURGER_API InitFloat(float fValue);
	void BURGER_API InitDouble(double dValue);
};

}
/* END */

#endif
