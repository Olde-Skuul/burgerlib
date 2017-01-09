/***************************************

	Floating point compression

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brpackfloat.h"

/*! ************************************

	\brief Decompress a packed floating point number.

	Decompress a 16 bit floating point number, assuming
	that the range is 0.999999 to -0.999999

	Use Burger::PackFloatTo16(float) to create the 16 bit value.

	The 16 bits are represented in this manner.

	val==0 = The value is zero.
	val&0x8000 = The value is negative.
	(val&0x7800)>>11 = 4 bit exponent where 0->111 and 15->126.
	(val&0x7FF) = 11 bit mantissa.
	
	\param iInput 16 bit representation of a floating point number.
	\return Input decompressed into a true 32 bit float.
	\sa Burger::PackFloatTo16(float), Burger::Unpack16ToFloat(Int16,Word32) or Burger::PackFloatTo16(float,Word32).

***************************************/

float BURGER_API Burger::Unpack16ToFloat(Int16 iInput)
{
	Word32 uInput = static_cast<Word16>(iInput);	// Use a 32 bit register
	Word32 uTemp = (uInput&0x8000U)<<16U;			// Store the sign in the highest bit (0x80000000)
	uInput = (uInput&0x7FFF);
	if (uInput) {
		uTemp |= (uInput&0x7FFU)<<((23-11)-11);		// Repeat the 11 bits a second time for better accuracy

		uTemp |= (uInput<<(23-11))+(111<<23);		// In one operation, create the exponent and the
													// upper 11 bits of the mantissa
	}
	union {
		float f;
		Word32 u;
	} Result;
	Result.u = uTemp;		// Perform the int to float conversion by writing to memory
	return Result.f;		// Return in a float register
}

/*! ************************************

	\brief Convert a float in the range of 0.999999 to -0.999999 into a 16 bit packed integer.

	Apply compression to a floating point number in the range of 0.999999 to -0.999999
	and create a 16 bit version. The input will be clamped.
	
	Use Burger::Unpack16ToFloat(Int16) to reconstruct the number.
	
	Result == 0 if the input was zero or too small to be represented.
	Result & 0x8000 if the input was negative.
	(Result>>11)&0xF The exponent where 0 = 111 and 15 = 126.
	(Result&0x7FF) The 11 bit mantissa.

	\param fInput Floating point number in the range of 0.999999 to -0.999999
	\return 16 bit floating point version of the 32 bit number.
	\sa Burger::Unpack16ToFloat(Int16), Burger::Unpack16ToFloat(Int16,Word32) or Burger::PackFloatTo16(float,Word32).

***************************************/

Int16 BURGER_API Burger::PackFloatTo16(float fInput)
{
	Word32 uInput=reinterpret_cast<const Word32 *>(static_cast<const void *>(&fInput))[0];			// Get the bit representation

	Word32 uResult = uInput&0x7FFFFFFFU;	// Get the exponent
	if (uResult<(127<<23)) {				// Less than 1.0?
		if (static_cast<Int32>(uResult = (uResult-(111<<23)))>0) {	// >=111? 111-126 are acceptable
			uResult = (uResult>>(23-11));	// Hack off the extra mantissa bits to give me the final 16 bit float.
										// Low 11, mantissa, high 4, (111->0, 126->15)<<11
		} else {
			uResult = 0;					// The number is too small
		}
	} else {
		uResult = 0x7FFF;				// 0.999999 (Max)
	}
	uResult |= (uInput>>16)&0x8000;			// Apply the sign
	return static_cast<Int16>(uResult);
}

/*! ************************************

	\brief Decompress a packed floating point number with a supplied base exponent.

	Decompress a 16 bit floating point number, assuming
	that the minimum exponent is the one supplied. For 0.999999
	you should supply the exponent value of 111. This number must match
	the value used to compress the value in the first place. This function
	exists so that floating point compression can scale to different ranges.

	Use Burger::PackFloatTo16(float,Word32) to create the 16 bit value.

	The 16 bits are represented in this manner.

	val==0 = The value is zero.
	val&0x8000 = The value is negative.
	(val&0x7800)>>11 = 4 bit exponent where 0->111 and 15->126.
	(val&0x7FF) = 11 bit mantissa.
	
	\param iInput 16 bit representation of a floating point number.
	\param uBaseExponent Minimum exponent, should match the value used to apply compression (111 is used for a max value of 0.999999).
	\return Input decompressed into a true 32 bit float.
	\sa Burger::PackFloatTo16(float), Burger::Unpack16ToFloat(Int16) or Burger::PackFloatTo16(float,Word32).

***************************************/

float BURGER_API Burger::Unpack16ToFloat(Int16 iInput,Word32 uBaseExponent)
{
	Word32 uInput = static_cast<Word16>(iInput);	// Use a 32 bit register
	Word32 uTemp = (uInput&0x8000U)<<16U;			// Store the sign in the highest bit (0x80000000)
	uInput = (uInput&0x7FFF);
	if (uInput) {
		uTemp |= (uInput&0x7FFU)<<((23-11)-11);	// Repeat the 11 bits a second time for better accuracy

		uTemp |= (uInput<<(23-11))+(uBaseExponent<<23);	// In one operation, create the exponent and the
												// upper 11 bits of the mantissa
	}
	union {
		float f;
		Word32 u;
	} Result;				// Memory bound variable
	Result.u = uTemp;		// Perform the int to float conversion by writing to memory
	return Result.f;		// Return in a float register
}

/*! ************************************

	\brief Convert a float in the range of a supplied exponent into a 16 bit packed integer.

	Apply compression to a floating point number in the range of 0.999999 to -0.999999
	and create a 16 bit version. The input will be clamped.
	
	Use Burger::Unpack16ToFloat(Int16,Word32) to reconstruct the number.
	
	Result == 0 if the input was zero or too small to be represented.
	Result & 0x8000 if the input was negative.
	(Result>>11)&0xF The exponent where 0 = BaseExponent and 15 = BaseExponent+15.
	(Result&0x7FF) The 11 bit mantissa.

	\param fInput Floating point number in the range of the supplied exponent.
	\param uBaseExponent Minimum acceptable exponent, (111 is used for a max value of 0.999999).
	\return 16 bit floating point version of the 32 bit number.
	\sa Burger::Unpack16ToFloat(Int16), Burger::Unpack16ToFloat(Int16,Word32) or Burger::PackFloatTo16(float).

***************************************/

Int16 BURGER_API Burger::PackFloatTo16(float fInput,Word32 uBaseExponent)
{
	uBaseExponent <<= 23;
	Word32 uInput=reinterpret_cast<const Word32 *>(static_cast<const void *>(&fInput))[0];			// Get the bit representation

	Word32 uResult = uInput&0x7FFFFFFFU;	// Get the exponent
	if (uResult<(uBaseExponent+(16U<<23))) {				// Less than 1.0?
		if (static_cast<Int32>(uResult = (uResult-uBaseExponent))>0) {	// >=111? 111-126 are acceptable
			uResult = (uResult>>(23-11));	// Hack off the extra mantissa bits to give me the final 16 bit float.
										// Low 11, mantissa, high 4, (111->0, 126->15)<<11
		} else {
			uResult = 0;					// The number is too small
		}
	} else {
		uResult = 0x7FFF;				// 0.999999 (Max)
	}
	uResult |= (uInput>>16)&0x8000;			// Apply the sign
	return static_cast<Int16>(uResult);
}