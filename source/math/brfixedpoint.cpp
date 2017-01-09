/***************************************

	Fixed point math functions

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfixedpoint.h"

/*! ************************************

	\def FLOATTOFIXED
	\brief Define to convert a float into a \ref Fixed32.
	
	Macro to convert at compile time a floating point
	value into 16.16 fixed point. Used for compile
	assignment of \ref Fixed32 constants.
	
	\param x Floating point value to convert to a \ref Fixed32.

	\note Due to the float to integer conversion, use
	of this macro at runtime may be a performance
	penalty on the Xbox 360 and PS3.
	
	\code
	// Fast use
	static const Fixed32 MyData = FLOATTOFIXED(4.503f);
	// Slow use
	void Foo(float fInput)
	{
		Fixed32 iInput = FLOATTOFIXED(fInput);
	}
	\endcode

***************************************/

/*! ************************************

	\def FIXEDTOFLOAT
	\brief Define to convert a \ref Fixed32 into a float.
	
	Macro to convert at compile time a 16.16 fixed point
	number into a floating point value
	Used for compile assignment of float constants using
	\ref Fixed32 constants..
	
	\param x 16.16 fixed point value to convert to a float.

	\note Due to the integer to float conversion, use
	of this macro at runtime on non constant values may be a performance
	penalty on the Xbox 360 and PS3.
	
	\code
	// Fast use
	static const float MyData = FIXEDTOFLOAT(Burger::PiFixed32);
	// Slow use
	void Foo(Fixed32 iInput)
	{
		float fInput = FIXEDTOFLOAT(iInput);
	}
	\endcode

***************************************/

/*! ************************************

	\def INTTOFIXED
	\brief Define to convert a integer into a \ref Fixed32.
	
	Macro to convert at compile time an integer
	value into 16.16 fixed point. Used for compile
	assignment of \ref Fixed32 constants.
	
	\param x Integer value to convert to a \ref Fixed32.

	\note There is no bounds checking performed by this
	macro. As a result, integers larger than 32767 or less
	than -32768 will cause undefined behavior due to over/underflow.
	
	\code
	static const Fixed32 MyData = INTTOFIXED(443);
	\endcode

***************************************/

/*! ************************************

	\def FIXEDTOINT
	\brief Define to convert a \ref Fixed32 value to an integer.
	
	Macro to convert at compile time a 16.16 fixed point
	value into an integer. Used for compile time
	assignment of integers using \ref Fixed32 constants.
	
	\param x \ref Fixed32 value to convert to an integer.

	\note This macro uses round to zero in the removal
	of the fraction. Negative numbers like -0.8 become 0,
	as well as 0.8 becomes 0.
	
	\code
	static const int MyData = FIXEDTOINT(Burger::PiFixed32);
	\endcode

***************************************/

/*! ************************************

	\brief Helper constants for float to integer conversions.
	
	These constants are used for the translation of 
	floats to ints using floor, ceil or zero.
	The first constant is added to yield floor()
	and the second is for getting ceil()
	(1.0f/(65536.0f*256.0f)) is the 32-bit floating point
	epsilon just under 1.0f
	
	\sa Burger::FloatToInt(float) or Burger::FloatToIntFloor(float)

***************************************/

#if defined(BURGER_INTELARCHITECTURE) || defined(DOXYGEN)
extern "C" const float g_fBurgerIntMathNearesttable[2] = {-0.5f,0.5f-(1.0f/(65536.0f*256.0f))};
#endif

/*! ************************************

	\brief Floating point constant of 65536.0f

***************************************/

#if defined(BURGER_INTELARCHITECTURE) || defined(DOXYGEN)
extern "C" const float g_fBurgerMath65536 = 65536.0f;
#endif



/*! ************************************

	\fn Word Burger::GetLoWord(Word32 uInput)
	\brief Retrieves the low-order word from the specified value.
	
	Mask off the upper 16 bit of the 32 bit input and set the
	bits to zero. This is an equivalent to the function
	<a href="http://msdn.microsoft.com/en-us/library/windows/desktop/ms632659(v=vs.85).aspx">LOWORD(DWORD)</a>
	
	\note This is a direct replacement of the MacOS macro LoWord().

	\param uInput The value to be converted.
	\return The low order 16 bits of uInput.
	
	\sa GetHiWord(Word32)
	
***************************************/

/*! ************************************

	\fn Word Burger::GetHiWord(Word32 uInput)
	\brief Retrieves the low-order word from the specified value.
	
	Shift the input 16 bits to the right and set the upper
	bits to zero. This is an equivalent to the function
	<a href="http://msdn.microsoft.com/en-us/library/windows/desktop/ms632657(v=vs.85).aspx">HIWORD(DWORD)</a>
	
	\note This is a direct replacement of the MacOS macro HiWord().

	\param uInput The value to be converted.
	\return The high order 16 bits of uInput.
	
	\sa GetLoWord(Word32)
	
***************************************/

/*! ************************************

	\fn Fixed32 Burger::IntToFixed(Int32 iInput)
	\brief Convert an Int32 into a \ref Fixed32 value.

	Convert an integer into a fixed point number. No bounds
	checking is performed so values that exceed 32767 or are
	less than -32768 will yield undefined results.

	\param iInput Integer to convert
	\return Result of the conversion without saturation

	\sa IntToFixedSaturate(Int32)
		
***************************************/

/*! ************************************

	\brief Convert an Int32 into a Fixed32 value with saturation.

	Convert an integer into a fixed point number. Bounds
	checking is performed so values that exceed 32767 or are
	less than -32768 will yield max and min values..

	Examples of clamping:
	\li 0x8000 -> 0x7FFFFFFF
	\li 0xFEEDBEEF -> 0x80000000
		
	\param iInput Integer to convert
	\return Result of the conversion with saturation

	\sa IntToFixed(Int32)

***************************************/

Fixed32 BURGER_API Burger::IntToFixedSaturate(Int32 iInput)
{
	if (iInput<0x8000) {		// Not too big?
		if (iInput>=-0x8000) {	// Not too small?
			return static_cast<Fixed32>(iInput<<16);	// Convert to fixed
		}
		return MinFixed32;		// Return minimum
	}
	return MaxFixed32;			// Return maximum
}

/*! ************************************

	\fn Burger::FixedToIntFloor(Fixed32 fInput)
	\brief Convert a fixed point value to an integer using round down.
	
	Convert a \ref Fixed32 into an integer using the same formula as floor().
		
	\param fInput Value to convert to an integer.
	\return The input converted to an integer using the truth table below.
	
	\code
	int iResult;
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(1.1f));		//1
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(1.95f));		//1
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(-1.1f));		//-2
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(-1.95f));	//-2
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(0.1f));		//0
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(0.95f));		//0
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(-0.1f));		//-1
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(-0.95f));	//-1
	\endcode

	\sa Floor(float), Floor(double), FixedToInt(Fixed32), FixedToIntCeil(Fixed32), or FixedToIntNearest(Fixed32)

***************************************/


/*! ************************************

	\fn Burger::FixedToInt(Fixed32 fInput)
	\brief Convert a fixed point value to an integer using round to zero.
	
	Convert a \ref Fixed32 into an integer using round to zero.
		
	\param fInput Value to convert to an integer.
	\return The input converted to an integer using the truth table below.
	
	\code
	floorint = Burger::FixedToInt(FLOATTOFIXED(1.1f));		//1
	floorint = Burger::FixedToInt(FLOATTOFIXED(1.95f));		//1
	floorint = Burger::FixedToInt(FLOATTOFIXED(-1.1f));		//-1
	floorint = Burger::FixedToInt(FLOATTOFIXED(-1.95f));	//-1
	floorint = Burger::FixedToInt(FLOATTOFIXED(0.1f));		//0
	floorint = Burger::FixedToInt(FLOATTOFIXED(0.95f));		//0
	floorint = Burger::FixedToInt(FLOATTOFIXED(-0.1f));		//0
	floorint = Burger::FixedToInt(FLOATTOFIXED(-0.95f));	//0
	\endcode

	\sa FixedToIntFloor(Fixed32), FixedToIntCeil(Fixed32), or FixedToIntNearest(Fixed32)

***************************************/

/*! ************************************

	\fn Burger::FixedToIntCeil(Fixed32 fInput)
	\brief Convert a fixed point value to an integer using round up.
	
	Convert a \ref Fixed32 into an integer using the same formula as ceil().
		
	\param fInput Value to convert to an integer.
	\return The input converted to an integer using the truth table below.
	
	\code
	int iResult;
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(1.1f));		//2
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(1.95f));		//2
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(-1.1f));		//-1
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(-1.95f));	//-1
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(0.1f));		//1
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(0.95f));		//1
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(-0.1f));		//0
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(-0.95f));	//0
	\endcode

	\sa Ceil(float), Ceil(double), FixedToIntFloor(Fixed32), FixedToInt(Fixed32), or FixedToIntNearest(Fixed32)

***************************************/

/*! ************************************

	\fn Burger::FixedToIntNearest(Fixed32 fInput)
	\brief Convert a fixed point value to an integer using round to nearest.
	
	Convert a \ref Fixed32 into an integer using round to nearest.
		
	\param fInput Value to convert to an integer.
	\return The input converted to an integer using the truth table below.
	
	\code
	int iResult;
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(1.1f));	//1
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(1.95f));	//2
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(-1.1f));	//-1
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(-1.95f));	//-2
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(0.1f));	//0
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(0.95f));	//1
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(-0.1f));	//0
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(-0.95f));	//-1
	\endcode

	\note This can be used to replace FixRound() from MacOS.

	\sa Round(fkoat), Round(double), FixedToIntFloor(Fixed32), FixedToInt(Fixed32), or FixedToIntCeil(Fixed32)

***************************************/

/*! ************************************

	\fn Int32 Burger::FloatToIntFloor(float fInput)
	\brief Convert a 32 bit float to an integer using floor().

	Convert a single precision floating point number to an integer
	using the floor() form of fractional truncation (Round down)

	\code
	floorint = Burger::FloatToIntFloor(1.1f);	//1
	floorint = Burger::FloatToIntFloor(1.95f);	//1
	floorint = Burger::FloatToIntFloor(-1.1f);	//-2
	floorint = Burger::FloatToIntFloor(-1.95f);	//-2
	floorint = Burger::FloatToIntFloor(0.1f);	//0
	floorint = Burger::FloatToIntFloor(0.95f);	//0
	floorint = Burger::FloatToIntFloor(-0.1f);	//-1
	floorint = Burger::FloatToIntFloor(-0.95f);	//-1
	\endcode

	\param fInput A valid single precision floating point number.
	\return Signed integer equivalent value after applying floor() on the floating point number.

	\sa FloatToIntFloor(Int32 *,float), FloatToInt(float), FloatToIntCeil(float), or FloatToIntNearest(float)

***************************************/

#if defined(BURGER_WATCOM)
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
#elif defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)
#elif defined(BURGER_X86) && defined(BURGER_METROWERKS)
#elif defined(BURGER_X86) && defined(BURGER_MSVC)
#elif defined(BURGER_AMD64) && defined(BURGER_MSVC)
#else
Int32 BURGER_API Burger::FloatToIntFloor(float fInput)
{
	if (fInput<0.0f) {
		fInput-=1.0f;
	}
	return static_cast<Int32>(fInput);		// Round to zero
}
#endif

/*! ************************************

	\fn Int32 Burger::FloatToInt(float fInput)
	\brief Convert a 32 bit float to an integer using round to zero.

	Convert a single precision floating point number to an integer
	using the round to zero fractional truncation

	\code
	floorint = Burger::FloatToInt(1.1f);	//1
	floorint = Burger::FloatToInt(1.95f);	//1
	floorint = Burger::FloatToInt(-1.1f);	//-1
	floorint = Burger::FloatToInt(-1.95f);	//-1
	floorint = Burger::FloatToInt(0.1f);	//0
	floorint = Burger::FloatToInt(0.95f);	//0
	floorint = Burger::FloatToInt(-0.1f);	//0
	floorint = Burger::FloatToInt(-0.95f);	//0
	\endcode

	\param fInput A valid single precision floating point number.
	\return Signed integer equivalent value after applying round to zero on the floating point number.

	\sa FloatToInt(Int32 *,float), FloatToIntFloor(float), FloatToIntCeil(float), or FloatToIntNearest(float)

***************************************/

#if defined(BURGER_WATCOM)
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
#elif defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)
#elif defined(BURGER_X86) && defined(BURGER_METROWERKS)
#elif defined(BURGER_X86) && defined(BURGER_MSVC)
#elif defined(BURGER_AMD64) && defined(BURGER_MSVC)
#else
Int32 BURGER_API Burger::FloatToInt(float fInput)
{
	return static_cast<Int32>(fInput);		// Round to zero
}
#endif

/*! ************************************

	\fn Int32 Burger::FloatToIntCeil(float fInput)
	\brief Convert a 32 bit float to an integer using ceil().

	Convert a single precision floating point number to an integer
	using the ceil() form of fractional truncation

	\code
	floorint = Burger::FloatToIntCeil(1.1f);	//2
	floorint = Burger::FloatToIntCeil(1.95f);	//2
	floorint = Burger::FloatToIntCeil(-1.1f);	//-1
	floorint = Burger::FloatToIntCeil(-1.95f);	//-1
	floorint = Burger::FloatToIntCeil(0.1f);	//1
	floorint = Burger::FloatToIntCeil(0.95f);	//1
	floorint = Burger::FloatToIntCeil(-0.1f);	//0
	floorint = Burger::FloatToIntCeil(-0.95f);	//0
	\endcode

	\param fInput A valid single precision floating point number.
	\return Signed integer equivalent value after applying ceil() on the floating point number.

	\sa FloatToIntCeil(Int32 *,float), FloatToIntFloor(float), FloatToInt(float), or FloatToIntNearest(float)
	
***************************************/

#if defined(BURGER_WATCOM)
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
#elif defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)
#elif defined(BURGER_X86) && defined(BURGER_METROWERKS)
#elif defined(BURGER_X86) && defined(BURGER_MSVC)
#elif defined(BURGER_AMD64) && defined(BURGER_MSVC)
#else
Int32 BURGER_API Burger::FloatToIntCeil(float fInput)
{
	if (fInput>=0.0f) {
		fInput+=(1.0f-(1.0f/(65536.0f*256.0f)));
	}
	return static_cast<Int32>(fInput);		// Round to zero
}
#endif

/*! ************************************

	\fn Int32 Burger::FloatToIntNearest(float fInput)
	\brief Convert a 32 bit float to an integer using round to nearest.

	Convert a single precision floating point number to an integer
	using the round to nearest fractional truncation
		
	\code
	floorint = Burger::FloatToIntNearest(1.1f);		//1
	floorint = Burger::FloatToIntNearest(1.95f);	//2
	floorint = Burger::FloatToIntNearest(-1.1f);	//-1
	floorint = Burger::FloatToIntNearest(-1.95f);	//-2
	floorint = Burger::FloatToIntNearest(0.1f);		//0
	floorint = Burger::FloatToIntNearest(0.95f);	//1
	floorint = Burger::FloatToIntNearest(-0.1f);	//0
	floorint = Burger::FloatToIntNearest(-0.95f);	//-1
	\endcode

	\param fInput A valid single precision floating point number.
	\return Signed integer equivalent value after applying round to nearest on the floating point number.

	\sa FloatToIntNearest(Int32 *,float), FloatToIntFloor(float), FloatToInt(float), or FloatToIntCeil(float)

***************************************/

#if defined(BURGER_WATCOM)
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
#elif defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)
#elif defined(BURGER_X86) && defined(BURGER_METROWERKS)
#elif defined(BURGER_X86) && defined(BURGER_MSVC)
#elif defined(BURGER_AMD64) && defined(BURGER_MSVC)
#else
Int32 BURGER_API Burger::FloatToIntNearest(float fInput)
{
	fInput+=0.5f;
	if (fInput<0.0f) {
		fInput-=1.0f;
	}
	return static_cast<Int32>(fInput);		// Round to zero
}

#endif

/*! ************************************

	\fn void Burger::FloatToIntFloor(Int32 *pOutput,float fInput)
	\brief Convert a 32 bit float to an integer using floor().

	Convert a single precision floating point number to an integer
	using the floor() form of fractional truncation and store it to memory

	\code
	Burger::FloatToIntFloor(&floorint,1.1f);	//1
	Burger::FloatToIntFloor(&floorint,1.95f);	//1
	Burger::FloatToIntFloor(&floorint,-1.1f);	//-2
	Burger::FloatToIntFloor(&floorint,-1.95f);	//-2
	Burger::FloatToIntFloor(&floorint,0.1f);	//0
	Burger::FloatToIntFloor(&floorint,0.95f);	//0
	Burger::FloatToIntFloor(&floorint,-0.1f);	//-1
	Burger::FloatToIntFloor(&floorint,-0.95f);	//-1
	\endcode


	\param pOutput A valid pointer to a 32-bit integer to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToIntFloor(float), FloatToInt(Int32 *,float), FloatToIntCeil(Int32 *,float), or FloatToIntNearest(Int32 *,float)
	
***************************************/

/*! ************************************

	\fn void Burger::FloatToInt(Int32 *pOutput,float fInput)
	\brief Convert a 32 bit float to an integer using round to zero.

	Convert a single precision floating point number to an integer
	using the round to zero fractional truncation and store it to memory
		
	\code
	Burger::FloatToInt(&floorint,1.1f);		//1
	Burger::FloatToInt(&floorint,1.95f);	//1
	Burger::FloatToInt(&floorint,-1.1f);	//-1
	Burger::FloatToInt(&floorint,-1.95f);	//-1
	Burger::FloatToInt(&floorint,0.1f);		//0
	Burger::FloatToInt(&floorint,0.95f);	//0
	Burger::FloatToInt(&floorint,-0.1f);	//0
	Burger::FloatToInt(&floorint,-0.95f);	//0
	\endcode

	\param pOutput A valid pointer to a 32-bit integer to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToInt(float), FloatToIntFloor(Int32 *,float), FloatToIntCeil(Int32 *,float), or FloatToIntNearest(Int32 *,float)
	
***************************************/

/*! ************************************

	\fn void Burger::FloatToIntCeil(Int32 *pOutput,float fInput)
	\brief Convert a 32 bit float to an integer using ceil().

	Convert a single precision floating point number to an integer
	using the ceil() form of fractional truncation and store it to memory
		
	\code
	Burger::FloatToIntCeil(&floorint,1.1f);		//2
	Burger::FloatToIntCeil(&floorint,1.95f);	//2
	Burger::FloatToIntCeil(&floorint,-1.1f);	//-1
	Burger::FloatToIntCeil(&floorint,-1.95f);	//-1
	Burger::FloatToIntCeil(&floorint,0.1f);		//1
	Burger::FloatToIntCeil(&floorint,0.95f);	//1
	Burger::FloatToIntCeil(&floorint,-0.1f);	//0
	Burger::FloatToIntCeil(&floorint,-0.95f);	//0
	\endcode

	\param pOutput A valid pointer to a 32-bit integer to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToIntCeil(float), FloatToIntFloor(Int32 *,float), FloatToInt(Int32 *,float), or FloatToIntNearest(Int32 *,float)
	
***************************************/

/*! ************************************

	\fn void Burger::FloatToIntNearest(Int32 *pOutput,float fInput)
	\brief Convert a 32 bit float to an integer using round to nearest.

	Convert a single precision floating point number to an integer
	using the round to nearest fractional truncation and store it to memory
		
	\code
	Burger::FloatToIntNearest(&floorint,1.1f);		//1
	Burger::FloatToIntNearest(&floorint,1.95f);		//2
	Burger::FloatToIntNearest(&floorint,-1.1f);		//-1
	Burger::FloatToIntNearest(&floorint,-1.95f);	//-2
	Burger::FloatToIntNearest(&floorint,0.1f);		//0
	Burger::FloatToIntNearest(&floorint,0.95f);		//1
	Burger::FloatToIntNearest(&floorint,-0.1f);		//0
	Burger::FloatToIntNearest(&floorint,-0.95f);	//-1
	\endcode

	\param pOutput A valid pointer to a 32-bit integer to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToIntNearest(float), FloatToIntFloor(Int32 *,float), FloatToInt(Int32 *,float), or FloatToIntCeil(Int32 *,float)
	
***************************************/

/*! ************************************

	\fn Fixed32 Burger::FloatToFixedFloor(float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using floor().

	Convert a single precision floating point number to a \ref Fixed32
	using the floor() form of fractional truncation
		
	\param fInput A valid single precision floating point number.
	\return a \ref Fixed32 equivalent value after applying floor() on the floating point number.
	\sa FloatToFixedFloor(Fixed32 *,float), FloatToFixed(float), FloatToFixedCeil(float), or FloatToFixedNearest(float)
	
***************************************/

/*! ************************************

	\fn Fixed32 Burger::FloatToFixed(float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using round to zero.

	Convert a single precision floating point number to a \ref Fixed32
	using the round to zero fractional truncation
		
	\param fInput A valid single precision floating point number.
	\return a \ref Fixed32 equivalent value after applying round to zero on the floating point number.
	\sa FloatToFixed(Fixed32 *,float), FloatToFixedFloor(float), FloatToFixedCeil(float), or FloatToFixedNearest(float)
	
***************************************/

/*! ************************************

	\fn Fixed32 Burger::FloatToFixedCeil(float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using ceil().

	Convert a single precision floating point number to a \ref Fixed32
	using the ceil() form of fractional truncation
		
	\param fInput A valid single precision floating point number.
	\return a \ref Fixed32 equivalent value after applying ceil() on the floating point number.
	\sa FloatToFixedCeil(Fixed32 *,float), FloatToFixedFloor(float), FloatToFixed(float), or FloatToFixedNearest(float)
	
***************************************/

/*! ************************************

	\fn Fixed32 Burger::FloatToFixedNearest(float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using round to nearest.

	Convert a single precision floating point number to a \ref Fixed32
	using the round to nearest fractional truncation
		
	\param fInput A valid single precision floating point number.
	\return a \ref Fixed32 equivalent value after applying round to nearest on the floating point number.
	\sa FloatToFixedNearest(Fixed32 *,float), FloatToFixedFloor(float), FloatToFixed(float), or FloatToFixedCeil(float)
	
***************************************/

/*! ************************************

	\fn void Burger::FloatToFixedFloor(Fixed32 *pOutput,float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using floor().

	Convert a single precision floating point number to a \ref Fixed32
	using the floor() form of fractional truncation and store it to memory
	
	\param pOutput A valid pointer to a \ref Fixed32 to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToFixedFloor(float), FloatToFixed(Fixed32 *,float), FloatToFixedCeil(Fixed32 *,float), or FloatToFixedNearest(Fixed32 *,float)
	
***************************************/

/*! ************************************

	\fn void Burger::FloatToFixed(Fixed32 *pOutput,float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using round to zero.

	Convert a single precision floating point number to a \ref Fixed32
	using the round to zero fractional truncation and store it to memory
		
	\param pOutput A valid pointer to a \ref Fixed32 to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToFixed(float), FloatToFixedFloor(Fixed32 *,float), FloatToFixedCeil(Fixed32 *,float), or FloatToFixedNearest(Fixed32 *,float)
	
***************************************/

/*! ************************************

	\fn void Burger::FloatToFixedCeil(Fixed32 *pOutput,float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using ceil().

	Convert a single precision floating point number to a \ref Fixed32
	using the ceil() form of fractional truncation and store it to memory
		
	\param pOutput A valid pointer to a \ref Fixed32 to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToFixedCeil(float), FloatToFixedFloor(Fixed32 *,float), FloatToFixed(Fixed32 *,float), or FloatToFixedNearest(Fixed32 *,float)
	
***************************************/

/*! ************************************

	\fn void Burger::FloatToFixedNearest(Fixed32 *pOutput,float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using round to nearest.

	Convert a single precision floating point number to a \ref Fixed32
	using the round to nearest fractional truncation and store it to memory
		
	\param pOutput A valid pointer to a \ref Fixed32 to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToFixedNearest(float), FloatToFixedFloor(Fixed32 *,float), FloatToFixed(Fixed32 *,float), or FloatToFixedCeil(Fixed32 *,float)
	
***************************************/


/*! ************************************

	\fn Int8 Burger::Abs(Int8 iInput)
	\brief Get the absolute value of an integer.
	
	Without branching, calculate the absolute value of an integer.
		
	\param iInput Value to get the absolute value of.
	\return The absolute value of iInput.
	
	\sa Abs(Int16), Abs(Int32), Abs(Int64), Abs(float) and Abs(double) 
	
***************************************/

/*! ************************************

	\fn Int16 Burger::Abs(Int16 iInput)
	\brief Get the absolute value of an integer.
	
	Without branching, calculate the absolute value of an integer.
		
	\param iInput Value to get the absolute value of.
	\return The absolute value of iInput.
	
	\sa Abs(Int8), Abs(Int32), Abs(Int64), Abs(float) and Abs(double) 
	
***************************************/

/*! ************************************

	\fn Int32 Burger::Abs(Int32 iInput)
	\brief Get the absolute value of an integer.
	
	Without branching, calculate the absolute value of an integer.
		
	\param iInput Value to get the absolute value of.
	\return The absolute value of iInput.
	
	\sa Abs(Int8), Abs(Int16), Abs(Int64), Abs(float) and Abs(double) 
	
***************************************/

/*! ************************************

	\fn Int64 Burger::Abs(Int64 iInput)
	\brief Get the absolute value of a 64 bit integer.
	
	Without branching, calculate the absolute value of an integer.
		
	\param iInput Value to get the absolute value of.
	\return The absolute value of iInput.
	
	\sa Abs(Int8), Abs(Int16), Abs(Int32), Abs(float) and Abs(double) 
	
***************************************/



/*! ************************************

	\fn Int8 Burger::ClampZero(Int8 iInput)
	\brief Clamp an integer to 0
	
	Without branching, if the integer is less than zero, set it to zero.
		
	\param iInput Value to test for less than zero.
	\return The zero clamped value of iInput.
	
	\sa ClampZero(Int16), ClampZero(Int32) or ClampZero(Int64)
	
***************************************/

/*! ************************************

	\fn Int16 Burger::ClampZero(Int16 iInput)
	\brief Clamp an integer to 0
	
	Without branching, if the integer is less than zero, set it to zero.
		
	\param iInput Value to test for less than zero.
	\return The zero clamped value of iInput.
	
	\sa ClampZero(Int8), ClampZero(Int32) or ClampZero(Int64)
	
***************************************/

/*! ************************************

	\fn Int32 Burger::ClampZero(Int32 iInput)
	\brief Clamp an integer to 0
	
	Without branching, if the integer is less than zero, set it to zero.
		
	\param iInput Value to test for less than zero.
	\return The zero clamped value of iInput.
	
	\sa ClampZero(Int8), ClampZero(Int16) or ClampZero(Int64)
	
***************************************/

/*! ************************************

	\fn Int64 Burger::ClampZero(Int64 iInput)
	\brief Clamp an integer to 0
	
	Without branching, if the integer is less than zero, set it to zero.
		
	\param iInput Value to test for less than zero.
	\return The zero clamped value of iInput.
	
	\sa ClampZero(Int8), ClampZero(Int16) or ClampZero(Int32)
	
***************************************/



/*! ************************************

	\fn Int8 Burger::Sign(Int8 iInput)
	\brief Get the sign value of an integer.
	
	Without branching, calculate the sign value of an integer. If
	the input was zero, return zero, if it's positive, return 1 and
	if negative, return -1.
		
	\param iInput Value to get the sign value of.
	\return 0, 1 or -1.
	
	\sa Sign(Int16), Sign(Int32), Sign(Int64), Sign(float) and Sign(double) 
	
***************************************/

/*! ************************************

	\fn Int16 Burger::Sign(Int16 iInput)
	\brief Get the sign value of an integer.
	
	Without branching, calculate the sign value of an integer. If
	the input was zero, return zero, if it's positive, return 1 and
	if negative, return -1.
		
	\param iInput Value to get the sign value of.
	\return 0, 1 or -1.
	
	\sa Sign(Int8), Sign(Int32), Sign(Int64), Sign(float) and Sign(double) 
	
***************************************/

/*! ************************************

	\fn Int32 Burger::Sign(Int32 iInput)
	\brief Get the sign value of an integer.
	
	Without branching, calculate the sign value of an integer. If
	the input was zero, return zero, if it's positive, return 1 and
	if negative, return -1.
		
	\param iInput Value to get the sign value of.
	\return 0, 1 or -1.
	
	\sa Sign(Int8), Sign(Int16), Sign(Int64), Sign(float) and Sign(double) 
	
***************************************/

/*! ************************************

	\fn Int64 Burger::Sign(Int64 iInput)
	\brief Get the sign value of a 64 bit integer.
	
	Without branching, calculate the sign value of an integer. If
	the input was zero, return zero, if it's positive, return 1 and
	if negative, return -1.
		
	\param iInput Value to get the sign value of.
	\return 0, 1 or -1.
	
	\sa Sign(Int8), Sign(Int16), Sign(Int32), Sign(float) and Sign(double) 
	
***************************************/







/*! ************************************

	\fn Int8 Burger::Min(Int8 iA,Int8 iB)
	\brief Return the lesser of two numbers

	Compare the two input values and return the lesser of the two.
		
	\param iA First value to test
	\param iB Second value to test
	\return The lesser of the two inputs
	
	\sa Max(Int8,Int8), Min(Word8,Word8), or Min(Int32,Int32)
	
***************************************/

/*! ************************************

	\fn Int8 Burger::Min(Int16 iA,Int16 iB)
	\brief Return the lesser of two numbers

	Compare the two input values and return the lesser of the two.
		
	\param iA First value to test
	\param iB Second value to test
	\return The lesser of the two inputs
	
	\sa Max(Int16,Int16), Min(Word16,Word16), or Min(Int32,Int32)
	
***************************************/

/*! ************************************

	\fn Int32 Burger::Min(Int32 iA,Int32 iB)
	\brief Return the lesser of two numbers

	Compare the two input values and return the lesser of the two.
		
	\param iA First value to test
	\param iB Second value to test
	\return The lesser of the two inputs
	
	\sa Max(Int32,Int32), Min(Word32,Word32), or Min(Int64,Int64)
	
***************************************/

/*! ************************************

	\fn Int64 Burger::Min(Int64 iA,Int64 iB)
	\brief Return the lesser of two numbers

	Compare the two input values and return the lesser of the two.
		
	\param iA First value to test
	\param iB Second value to test
	\return The lesser of the two inputs
	
	\sa Max(Int64,Int64), Min(Word64,Word64), or Min(Int32,Int32)
	
***************************************/

/*! ************************************

	\fn Word8 Burger::Min(Word8 uA,Word8 uB)
	\brief Return the lesser of two numbers

	Compare the two input values and return the lesser of the two.
		
	\param uA First value to test
	\param uB Second value to test
	\return The lesser of the two inputs
	
	\sa Max(Word8,Word8), Min(Int32,Int32), or Min(Word64,Word64)
	
***************************************/

/*! ************************************

	\fn Word16 Burger::Min(Word16 uA,Word16 uB)
	\brief Return the lesser of two numbers

	Compare the two input values and return the lesser of the two.
		
	\param uA First value to test
	\param uB Second value to test
	\return The lesser of the two inputs
	
	\sa Max(Word16,Word16), Min(Int32,Int32), or Min(Word64,Word64)
	
***************************************/

/*! ************************************

	\fn Word32 Burger::Min(Word32 uA,Word32 uB)
	\brief Return the lesser of two numbers

	Compare the two input values and return the lesser of the two.
		
	\param uA First value to test
	\param uB Second value to test
	\return The lesser of the two inputs
	
	\sa Max(Word32,Word32), Min(Int32,Int32), or Min(Word64,Word64)
	
***************************************/

/*! ************************************

	\fn Word64 Burger::Min(Word64 uA,Word64 uB)
	\brief Return the lesser of two numbers

	Compare the two input values and return the lesser of the two.
		
	\param uA First value to test
	\param uB Second value to test
	\return The lesser of the two inputs
	
	\sa Max(Word64,Word64), Min(Int64,Int64), or Min(Word32,Word32)
	
***************************************/





/*! ************************************

	\fn Int8 Burger::Max(Int8 iA,Int8 iB)
	\brief Return the greater of two numbers

	Compare the two input values and return the greater of the two.
		
	\param iA First value to test
	\param iB Second value to test
	\return The greater of the two inputs
	
	\sa Min(Int8,Int8), Max(Word32,Word32), or Max(Int64,Int64)
	
***************************************/

/*! ************************************

	\fn Int16 Burger::Max(Int16 iA,Int16 iB)
	\brief Return the greater of two numbers

	Compare the two input values and return the greater of the two.
		
	\param iA First value to test
	\param iB Second value to test
	\return The greater of the two inputs
	
	\sa Min(Int16,Int16), Max(Word32,Word32), or Max(Int64,Int64)
	
***************************************/

/*! ************************************

	\fn Int32 Burger::Max(Int32 iA,Int32 iB)
	\brief Return the greater of two numbers

	Compare the two input values and return the greater of the two.
		
	\param iA First value to test
	\param iB Second value to test
	\return The greater of the two inputs
	
	\sa Min(Int32,Int32), Max(Word32,Word32), or Max(Int64,Int64)
	
***************************************/

/*! ************************************

	\fn Int64 Burger::Max(Int64 iA,Int64 iB)
	\brief Return the greater of two numbers

	Compare the two input values and return the greater of the two.
		
	\param iA First value to test
	\param iB Second value to test
	\return The greater of the two inputs
	
	\sa Min(Int64,Int64), Max(Word64,Word64), or Max(Int32,Int32)
	
***************************************/

/*! ************************************

	\fn Word8 Burger::Max(Word8 uA,Word8 uB)
	\brief Return the greater of two numbers

	Compare the two input values and return the greater of the two.
		
	\param uA First value to test
	\param uB Second value to test
	\return The greater of the two inputs
	
	\sa Min(Word8,Word8), Max(Int32,Int32), or Max(Word64,Word64)
	
***************************************/

/*! ************************************

	\fn Word16 Burger::Max(Word16 uA,Word16 uB)
	\brief Return the greater of two numbers

	Compare the two input values and return the greater of the two.
		
	\param uA First value to test
	\param uB Second value to test
	\return The greater of the two inputs
	
	\sa Min(Word16,Word16), Max(Int32,Int32), or Max(Word64,Word64)
	
***************************************/

/*! ************************************

	\fn Word32 Burger::Max(Word32 uA,Word32 uB)
	\brief Return the greater of two numbers

	Compare the two input values and return the greater of the two.
		
	\param uA First value to test
	\param uB Second value to test
	\return The greater of the two inputs
	
	\sa Min(Word32,Word32), Max(Int32,Int32), or Max(Word64,Word64)
	
***************************************/

/*! ************************************

	\fn Word64 Burger::Max(Word64 uA,Word64 uB)
	\brief Return the greater of two numbers

	Compare the two input values and return the greater of the two.
		
	\param uA First value to test
	\param uB Second value to test
	\return The greater of the two inputs
	
	\sa Min(Word64,Word64), Max(Int64,Int64), or Max(Word32,Word32)
	
***************************************/

/*! ************************************

	\fn Int32 Burger::Clamp(Int32 iInput,Int32 iMin,Int32 iMax)
	\brief Clamp the input between a bounds

	If the input value is less than the minimum, return the minimum
	or if the input value is greater than the maximum, return 
	the maximum, otherwise return the input value. No
	checking is performed to determine if the minimum is
	less than the maximum.
		
	\param iInput First value to test
	\param iMin Minimum allowed value
	\param iMax Maximum allowed value
	\return The value clamped between the bounds

	\sa Clamp(Word64,Word64,Word64), Clamp(Int64,Int64,Int64), or Clamp(Word32,Word32,Word32)
	
***************************************/

/*! ************************************

	\fn Int64 Burger::Clamp(Int64 iInput,Int64 iMin,Int64 iMax)
	\brief Clamp the input between a bounds

	If the input value is less than the minimum, return the minimum
	or if the input value is greater than the maximum, return 
	the maximum, otherwise return the input value. No
	checking is performed to determine if the minimum is
	less than the maximum.
		
	\param iInput First value to test
	\param iMin Minimum allowed value
	\param iMax Maximum allowed value
	\return The value clamped between the bounds

	\sa Clamp(Word32,Word32,Word32), Clamp(Int32,Int32,Int32), or Clamp(Word64,Word64,Word64)
	
***************************************/

/*! ************************************

	\fn Word32 Burger::Clamp(Word32 uInput,Word32 uMin,Word32 uMax)
	\brief Clamp the input between a bounds

	If the input value is less than the minimum, return the minimum
	or if the input value is greater than the maximum, return 
	the maximum, otherwise return the input value. No
	checking is performed to determine if the minimum is
	less than the maximum.
		
	\param uInput First value to test
	\param uMin Minimum allowed value
	\param uMax Maximum allowed value
	\return The value clamped between the bounds

	\sa Clamp(Int64,Int64,Int64), Clamp(Word64,Word64,Word64), or Clamp(Int32,Int32,Int32)
	
***************************************/

/*! ************************************

	\fn Word64 Burger::Clamp(Word64 uInput,Word64 uMin,Word64 uMax)
	\brief Clamp the input between a bounds

	If the input value is less than the minimum, return the minimum
	or if the input value is greater than the maximum, return 
	the maximum, otherwise return the input value. No
	checking is performed to determine if the minimum is
	less than the maximum.
		
	\param uInput First value to test
	\param uMin Minimum allowed value
	\param uMax Maximum allowed value
	\return The value clamped between the bounds

	\sa Clamp(Int32,Int32,Int32), Clamp(Word32,Word32,Word32), or Clamp(Int64,Int64,Int64)
	
***************************************/

/*! ************************************

	\fn Fixed32 Burger::FixedMultiply(Fixed32 fInput1,Fixed32 fInput2)
	\brief Multiply two 16.16 fixed point numbers.

	Perform a signed multiplication of two 32-bit fixed point
	numbers and return the fixed point result. No
	error checking is performed, so overflow or underflow
	conditions yield undefined results.

	\param fInput1 First signed value to multiply
	\param fInput2 Second signed value to multiply
	\return Result of fInput1 * fInput2 without bounds checking

	\sa FixedMultiplySaturate(Fixed32,Fixed32) or FixedDivide(Fixed32,Fixed32)
		
***************************************/

#if defined(BURGER_WATCOM)
#elif (defined(BURGER_X86) && defined(BURGER_METROWERKS))
#elif (defined(BURGER_X86) && defined(BURGER_MSVC))
#elif (defined(BURGER_POWERPC) || defined(BURGER_64BITCPU))
#else
Fixed32 BURGER_API Burger::FixedMultiply(Fixed32 fInput1,Fixed32 fInput2)
{
	Int64 iFoo = static_cast<Int64>(fInput1)*static_cast<Int64>(fInput2);
	return static_cast<Fixed32>(iFoo>>16);
}
#endif

/*! ************************************

	\fn Fixed32 Burger::FixedDivide(Fixed32 fInputNumerator,Fixed32 fInputDenominator)
	\brief Divide two 16.16 fixed point numbers.

	Divide a 16.16 fixed point number by another 16.16 fixed point number
	without saturation or checking for division by zero..

	\param fInputNumerator Signed value to divide
	\param fInputDenominator Signed value to divide with
	\return Result of fInputNumerator / fInputDenominator without saturation

	\note A divide by zero or an overflow can cause an exception error!

	\sa FixedDivideSaturate(Fixed32,Fixed32), FixedReciprocal(Fixed32) or FixedMultiply(Fixed32,Fixed32)

***************************************/

#if defined(BURGER_WATCOM)
#elif (defined(BURGER_X86) && defined(BURGER_METROWERKS))
#elif (defined(BURGER_X86) && defined(BURGER_MSVC))
#elif (defined(BURGER_POWERPC) || defined(BURGER_64BITCPU))
#else
Fixed32 BURGER_API Burger::FixedDivide(Fixed32 fInputNumerator,Fixed32 fInputDenominator)
{
	Int64 iFoo = (static_cast<Int64>(fInputNumerator)<<16)/static_cast<Int64>(fInputDenominator);
	return static_cast<Fixed32>(iFoo>>16);
}
#endif

/*! ************************************

	\fn Fixed32 Burger::FixedReciprocal(Fixed32 fInput)
	\brief Return the reciprocal of a fixed point number.

	Divide a 16.16 fixed point number by \ref FLOATTOFIXED(1.0f) in fixed point.
	If the input value is negative epsilon (0xFFFFFFFF), return 0x8000000. If
	the input is equal to 0 or positive epsilon (0x1), then return 0x7ffffff.
	This has the effect of saturating the output and leaving no output 
	as undefined.

	\param fInput \ref Fixed32 value to convert to a reciprocal.
	\return Result of \ref FLOATTOFIXED(1.0f) / fInput with saturation

	\note It's not recommended to input epsilon or 0 due to
	saturation.

	\sa FixedDivideSaturate(Fixed32,Fixed32) or FixedDivide(Fixed32,Fixed32)

***************************************/

#if defined(BURGER_WATCOM)
#elif (defined(BURGER_X86) && defined(BURGER_METROWERKS))
#elif (defined(BURGER_X86) && defined(BURGER_MSVC))
#elif (defined(BURGER_POWERPC) || defined(BURGER_64BITCPU))
#else
Fixed32 BURGER_API Burger::FixedReciprocal(Fixed32 fInput)
{
	if (fInput!=-1) {
		if (static_cast<Word32>(fInput)>=2) {		// Prevent a divide by zero
			Word32 uFoo = static_cast<Word32>(Abs(fInput));
			uFoo = 0x80000000UL/(uFoo>>1);
			if (fInput<0) {
				return -static_cast<Fixed32>(uFoo);
			}
			return static_cast<Fixed32>(uFoo);
		}
		return MaxFixed32;
	}
	return MinFixed32;
}
#endif

/*! ************************************

	\brief Get the square root of an integer.
	
	Return the square root of an integer.
	The maximum value is 46341 for the square root
	of 0x7FFFFFFF. This routine is 100% accurate.
		
	\param uInput Value to return the square root of.
	\return Return the square root as an integer.

	\note This will perform a round to nearest
	for the fraction so if the calculated square root is 1.6, it will
	return 2.
	
	\sa Burger::Sqrt(Fixed32), Burger::Sqrt(float), and Burger::Sqrt(double)
	
***************************************/

Word32 BURGER_API Burger::Sqrt(Word32 uInput)
{
	// Perform the square root

#if 0
	Word i = 16;
	Word32 TestValue = 0x40000000;
	Word32 Result = 0;
	do {
		Word32 ToTest = Result+TestValue;
		Result>>=1;
		if (uInput>=ToTest) {
			Result = Result+TestValue;
			uInput -= ToTest;
		}
		TestValue>>=2;
	} while (--i);
	if (uInput>Result) {
		Result += 1;
	}
	
#else
	// This version is the function above, but unrolled
	// for performance
	
	Word32 Result = 0;
	if (uInput>=0x40000000) {
		Result = 0x40000000;
		uInput -= 0x40000000;
	}

	// Here is where the fun begins

	Word32 ToTest = Result + 0x10000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x400000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x100000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x40000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x400;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x100;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x40;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1;
		uInput -= ToTest;
	}

	// Big finish!	

	if (uInput>Result) {
		Result += 1;
	}
#endif
	return Result;
}

/*! ************************************

	\brief Get the square root of a \ref Fixed32.
	
	Return the square root of a \ref Fixed32 and
	return the integer result.
	The maximum value is 255.998 (256) for the square root
	of 0x7FFFFFFF. This routine is 100% accurate.
	
	Negative numbers will return zero for the result.
		
	\param fInput Value to return the square root of.
	\return Return the square root as an integer.

	\note This will perform a round to nearest
	for the fraction so if the calculated square root is 1.6, it will
	return 2.
	
	\sa Burger::Sqrt(Word32)

***************************************/

Word32 BURGER_API Burger::SqrtFixedToWord32(Fixed32 fInput)
{
	// Perform the square root
	
	if (fInput<0) {
		return 0;
	}
	Word32 uInput = static_cast<Word32>(fInput);
	
	Word32 Result = 0;
	if (uInput>=0x40000000) {
		Result = 0x40000000;
		uInput -= 0x40000000;
	}

	// Here is where the fun begins

	Word32 ToTest = Result + 0x10000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x400000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x100000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x40000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10000;
		uInput -= ToTest;
	}

	// Get the extra bit 0.5f
	
	ToTest = Result + 0x4000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4000;
		//uInput -= ToTest;
	}

	// Add in 0.5f to round up
	Result+=0x4000;
	Result>>=15;		// Convert to int
	return Result;
}

/*! ************************************

	\brief Get the square root of a \ref Fixed32.
	
	Return the square root of a \ref Fixed32 and
	return the \ref Fixed32 result.
	The maximum value is 255.998 (256) for the square root
	of 0x7FFFFFFF. This routine is 100% accurate.
	
	Negative numbers will return zero for the result.
		
	\param fInput Value to return the square root of.
	\return Return the square root as a \ref Fixed32.
	
	\sa Burger::Sqrt(Word32), Burger::Sqrt(float), and Burger::Sqrt(double)

***************************************/

Fixed32 BURGER_API Burger::Sqrt(Fixed32 fInput)
{
	if (fInput<0) {
		return 0;
	}
	Word32 uInput = static_cast<Word32>(fInput);

	// This is simplified since most are constants

	Word32 Result = 0;
	if (uInput>=0x40000000) {
		Result = 0x40000000;
		uInput -= 0x40000000;
	}

	/* Here is where the fun begins */

	Word32 ToTest = Result + 0x10000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x400000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x100000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x40000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x400;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x100;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x40;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1;
		uInput -= ToTest;
	}

	// At this point, I've got the integer square root
	// I need another 16 bits of precision

	Result <<= 16;
	uInput <<= 16;

	// Now, iterate another 8 times for the final 16 bits

	ToTest = Result + 0x4000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x400;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x100;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x40;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1;
		uInput -= ToTest;
	}

	/* Big finish! */

	if (uInput>Result) {
		Result += 1;
	}
	return static_cast<Fixed32>(Result);
}

/*! ************************************

	\fn Word32 Burger::RotateLeft(Word32 uInput,Word uShiftCount)
	\brief Rotate the bits left
	
	Using a compiler intrinsic where supported, rotate the bits in
	the 32 bit register to the left.
		
	\param uInput Value to rotate left
	\param uShiftCount 0 to 31, number of bits to shift.
	\return The value with the bits rotated
	
	\sa RotateLeft(Word64,Word) or RotateRight(Word32,Word)
	
***************************************/

/*! ************************************

	\fn Word32 Burger::RotateRight(Word32 uInput,Word uShiftCount)
	\brief Rotate the bits right
	
	Using a compiler intrinsic where supported, rotate the bits in
	the 32 bit register to the right.
		
	\param uInput Value to rotate right
	\param uShiftCount 0 to 31, number of bits to shift.
	\return The value with the bits rotated
	
	\sa RotateRight(Word64,Word) or RotateLeft(Word32,Word)
	
***************************************/

/*! ************************************

	\fn Word64 Burger::RotateLeft(Word64 uInput,Word uShiftCount)
	\brief Rotate the bits left
	
	Using a compiler intrinsic where supported, rotate the bits in
	the 64 bit register to the left.
		
	\param uInput Value to rotate left
	\param uShiftCount 0 to 63, number of bits to shift.
	\return The value with the bits rotated
	
	\sa RotateLeft(Word32,Word) or RotateRight(Word64,Word)
	
***************************************/

/*! ************************************

	\fn Word64 Burger::RotateRight(Word64 uInput,Word uShiftCount)
	\brief Rotate the bits right
	
	Using a compiler intrinsic where supported, rotate the bits in
	the 64 bit register to the right.
		
	\param uInput Value to rotate right
	\param uShiftCount 0 to 63, number of bits to shift.
	\return The value with the bits rotated
	
	\sa RotateRight(Word32,Word) or RotateLeft(Word64,Word)
	
***************************************/