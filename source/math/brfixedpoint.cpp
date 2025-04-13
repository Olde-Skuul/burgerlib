/***************************************

	Fixed point math functions

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfixedpoint.h"
#include "brfloatingpoint.h"
#include "brintrinsics.h"

/*! ************************************

	\def BURGER_FLOAT_TO_FIXED
	\brief Define to convert a float into a \ref fixed16_16_t.

	Macro to convert at compile time a floating point value into 16.16 fixed
	point. Used for compile assignment of \ref fixed16_16_t constants.

	\note Due to the float to integer conversion, use of this macro at runtime
		may be a performance penalty on the Xbox 360 and PS3 on non-constant
		values.

	\code
	// Fast use
	static const fixed16_16_t MyData = BURGER_FLOAT_TO_FIXED(4.503f);
	// Slow use
	void Foo(float fInput)
	{
		fixed16_16_t iInput = BURGER_FLOAT_TO_FIXED(fInput);
	}
	\endcode

	\param x Floating point value to convert to a \ref fixed16_16_t.

***************************************/

/*! ************************************

	\def BURGER_FIXED_TO_FLOAT
	\brief Define to convert a \ref fixed16_16_t into a float.

	Macro to convert at compile time a 16.16 fixed point number into a floating
	point value Used for compile assignment of float constants using
	\ref fixed16_16_t constants..

	\note Due to the integer to float conversion, use of this macro at runtime
		on non constant values may be a performance penalty on the Xbox 360 and
		PS3.

	\code
	// Fast use
	static const float MyData = BURGER_FIXED_TO_FLOAT(Burger::kPiFixed32);
	// Slow use
	void Foo(fixed16_16_t iInput)
	{
		float fInput = BURGER_FIXED_TO_FLOAT(iInput);
	}
	\endcode

	\param x 16.16 fixed point value to convert to a float.

***************************************/

/*! ************************************

	\def BURGER_INT_TO_FIXED
	\brief Define to convert a integer into a \ref fixed16_16_t.

	Macro to convert at compile time an integer value into 16.16 fixed point.
	Used for compile assignment of \ref fixed16_16_t constants.

	\note There is no bounds checking performed by this macro. As a result,
		integers larger than 32767 or less than -32768 will cause undefined
		behavior due to over/underflow.

	\code
	static const fixed16_16_t MyData = BURGER_INT_TO_FIXED(443);
	\endcode

	\param x Integer value to convert to a \ref fixed16_16_t.

***************************************/

/*! ************************************

	\def BURGER_FIXED_TO_INT
	\brief Define to convert a \ref fixed16_16_t value to an integer.

	Macro to convert at compile time a 16.16 fixed point value into an integer.
	Used for compile time assignment of integers using \ref fixed16_16_t constants.

	\note This macro uses round to zero in the removal of the fraction. Negative
		numbers like -0.8 become 0, as well as 0.8 becomes 0.

	\code
	static const int MyData = BURGER_FIXED_TO_INT(Burger::kPiFixed32);
	\endcode

	\param x \ref fixed16_16_t value to convert to an integer.

***************************************/

/*! ************************************

	\fn uint_t Burger::get_low_word(uint32_t uInput)
	\brief Retrieves the low-order word from the specified value.

	Mask off the upper 16 bit of the 32 bit input and set the bits to zero. This
	is an equivalent to the function
	[LOWORD(DWORD)](https://learn.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms632659(v=vs.85))

	\note This is a direct replacement of the MacOS macro LoWord().

	\param uInput The value to be converted.
	\return The low order 16 bits of uInput.

	\sa get_high_word(uint32_t)

***************************************/

/*! ************************************

	\fn uint_t Burger::get_high_word(uint32_t uInput)
	\brief Retrieves the low-order word from the specified value.

	Shift the input 16 bits to the right and set the upper
	bits to zero. This is an equivalent to the function
	[HIWORD(DWORD)](http://msdn.microsoft.com/en-us/library/windows/desktop/ms632657(v=vs.85).aspx)

	\note This is a direct replacement of the MacOS macro HiWord().

	\param uInput The value to be converted.
	\return The high order 16 bits of uInput.

	\sa get_low_word(uint32_t)

***************************************/

/*! ************************************

	\fn fixed16_16_t Burger::int_to_fixed(int32_t iInput)
	\brief Convert an int32_t into a \ref fixed16_16_t value.

	Convert an integer into a fixed point number. No bounds checking is
	performed so values that exceed 32767 or are less than -32768 will yield
	undefined results.

	\param iInput Integer to convert

	\return Result of the conversion without saturation

	\sa int_to_fixed_saturate(int32_t)

***************************************/

/*! ************************************

	\brief Convert an int32_t into a fixed16_16_t value with saturation.

	Convert an integer into a fixed point number. Bounds checking is performed
	so values that exceed 32767 or are less than -32768 will yield max and min
	values.

	Examples of clamping:
	* * 0x8000 -> 0x7FFFFFFF
	* * 0xFEEDBEEF -> 0x80000000

	\param iInput Integer to convert
	\return Result of the conversion with saturation

	\sa int_to_fixed(int32_t)

***************************************/

fixed16_16_t BURGER_API Burger::int_to_fixed_saturate(int32_t iInput) BURGER_NOEXCEPT
{
	fixed16_16_t iResult;

	// Not too big?
	if (iInput >= 0x8000) {
		// Return maximum
		iResult = kMaxFixed32;

		// Not too small?
	} else if (iInput <= -0x7FFF) {
		// Return minimum
		iResult = kMinFixed32;

	} else {
		// Convert to fixed (Signed shift)
		iResult = static_cast<fixed16_16_t>(iInput << 16);
	}
	return iResult;
}

/*! ************************************

	\fn Burger::fixed_to_int_floor(fixed16_16_t fInput)
	\brief Convert a fixed point value to an integer using round down.

	Convert a \ref fixed16_16_t into an integer using the same formula as floor().

	\code
	int iResult;
	iResult = Burger::fixed_to_int_floor(BURGER_FLOAT_TO_FIXED(1.1f));		//1
	iResult = Burger::fixed_to_int_floor(BURGER_FLOAT_TO_FIXED(1.95f));		//1
	iResult = Burger::fixed_to_int_floor(BURGER_FLOAT_TO_FIXED(-1.1f));		//-2
	iResult = Burger::fixed_to_int_floor(BURGER_FLOAT_TO_FIXED(-1.95f));	//-2
	iResult = Burger::fixed_to_int_floor(BURGER_FLOAT_TO_FIXED(0.1f));		//0
	iResult = Burger::fixed_to_int_floor(BURGER_FLOAT_TO_FIXED(0.95f));		//0
	iResult = Burger::fixed_to_int_floor(BURGER_FLOAT_TO_FIXED(-0.1f));		//-1
	iResult = Burger::fixed_to_int_floor(BURGER_FLOAT_TO_FIXED(-0.95f));	//-1
	\endcode

	\param fInput Value to convert to an integer.

	\return The input converted to an integer using the truth table below.

	\sa get_floor(float), get_floor(double), fixed_to_int(fixed16_16_t),
		fixed_to_int_ceil(fixed16_16_t), or fixed_to_int_nearest(fixed16_16_t)

***************************************/

/*! ************************************

	\fn Burger::fixed_to_int(fixed16_16_t fInput)
	\brief Convert a fixed point value to an integer using round to zero.

	Convert a \ref fixed16_16_t into an integer using round to zero.

	\code
	floorint = Burger::fixed_to_int(BURGER_FLOAT_TO_FIXED(1.1f));	//1
	floorint = Burger::fixed_to_int(BURGER_FLOAT_TO_FIXED(1.95f));	//1
	floorint = Burger::fixed_to_int(BURGER_FLOAT_TO_FIXED(-1.1f));	//-1
	floorint = Burger::fixed_to_int(BURGER_FLOAT_TO_FIXED(-1.95f));	//-1
	floorint = Burger::fixed_to_int(BURGER_FLOAT_TO_FIXED(0.1f));	//0
	floorint = Burger::fixed_to_int(BURGER_FLOAT_TO_FIXED(0.95f));	//0
	floorint = Burger::fixed_to_int(BURGER_FLOAT_TO_FIXED(-0.1f));	//0
	floorint = Burger::fixed_to_int(BURGER_FLOAT_TO_FIXED(-0.95f));	//0
	\endcode

	\param fInput Value to convert to an integer.
	\return The input converted to an integer using the truth table below.

	\sa fixed_to_int_floor(fixed16_16_t), fixed_to_int_ceil(fixed16_16_t), or
		fixed_to_int_nearest(fixed16_16_t)

***************************************/

/*! ************************************

	\fn Burger::fixed_to_int_ceil(fixed16_16_t fInput)
	\brief Convert a fixed point value to an integer using round up.

	Convert a \ref fixed16_16_t into an integer using the same formula as ceil().

	\code
	int iResult;
	iResult = Burger::fixed_to_int_ceil(BURGER_FLOAT_TO_FIXED(1.1f));	//2
	iResult = Burger::fixed_to_int_ceil(BURGER_FLOAT_TO_FIXED(1.95f));	//2
	iResult = Burger::fixed_to_int_ceil(BURGER_FLOAT_TO_FIXED(-1.1f));	//-1
	iResult = Burger::fixed_to_int_ceil(BURGER_FLOAT_TO_FIXED(-1.95f));	//-1
	iResult = Burger::fixed_to_int_ceil(BURGER_FLOAT_TO_FIXED(0.1f));	//1
	iResult = Burger::fixed_to_int_ceil(BURGER_FLOAT_TO_FIXED(0.95f));	//1
	iResult = Burger::fixed_to_int_ceil(BURGER_FLOAT_TO_FIXED(-0.1f));	//0
	iResult = Burger::fixed_to_int_ceil(BURGER_FLOAT_TO_FIXED(-0.95f));	//0
	\endcode

	\param fInput Value to convert to an integer.

	\return The input converted to an integer using the truth table below.

	\sa get_ceiling(float), get_ceiling(double), fixed_to_int_floor(fixed16_16_t),
		fixed_to_int(fixed16_16_t), or fixed_to_int_nearest(fixed16_16_t)

***************************************/

/*! ************************************

	\fn Burger::fixed_to_int_nearest(fixed16_16_t fInput)
	\brief Convert a fixed point value to an integer using round to nearest.

	Convert a \ref fixed16_16_t into an integer using round to nearest.

	\code
	int iResult;
	iResult = Burger::fixed_to_int_nearest(BURGER_FLOAT_TO_FIXED(1.1f));	//1
	iResult = Burger::fixed_to_int_nearest(BURGER_FLOAT_TO_FIXED(1.95f));	//2
	iResult = Burger::fixed_to_int_nearest(BURGER_FLOAT_TO_FIXED(-1.1f));	//-1
	iResult = Burger::fixed_to_int_nearest(BURGER_FLOAT_TO_FIXED(-1.95f));	//-2
	iResult = Burger::fixed_to_int_nearest(BURGER_FLOAT_TO_FIXED(0.1f));	//0
	iResult = Burger::fixed_to_int_nearest(BURGER_FLOAT_TO_FIXED(0.95f));	//1
	iResult = Burger::fixed_to_int_nearest(BURGER_FLOAT_TO_FIXED(-0.1f));	//0
	iResult = Burger::fixed_to_int_nearest(BURGER_FLOAT_TO_FIXED(-0.95f));	//-1
	\endcode

	\note This can be used to replace FixRound() from MacOS.

	\param fInput Value to convert to an integer.

	\return The input converted to an integer using the truth table below.

	\sa get_round(float), get_round(double), fixed_to_int_floor(fixed16_16_t),
		fixed_to_int(fixed16_16_t), or fixed_to_int_ceil(fixed16_16_t)

***************************************/

/*! ************************************

	\brief Convert a 32 bit float to an integer using floor().

	Convert a single precision floating point number to an integer
	using the floor() form of fractional truncation (Round down)

	\code
	floorint = Burger::float_to_int_floor(1.1f);	//1
	floorint = Burger::float_to_int_floor(1.95f);	//1
	floorint = Burger::float_to_int_floor(-1.1f);	//-2
	floorint = Burger::float_to_int_floor(-1.95f);	//-2
	floorint = Burger::float_to_int_floor(0.1f);	//0
	floorint = Burger::float_to_int_floor(0.95f);	//0
	floorint = Burger::float_to_int_floor(-0.1f);	//-1
	floorint = Burger::float_to_int_floor(-0.95f);	//-1
	\endcode

	\param fInput A valid single precision floating point number.
	\return Signed integer equivalent value after applying floor() on the
		floating point number.

	\sa float_to_int_floor(int32_t *,float), float_to_int_round_to_zero(float),
		float_to_int_ceil(float), or float_to_int_round(float)

***************************************/

#if ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_X86) || defined(BURGER_PPC))) || \
	(defined(BURGER_MSVC) && (defined(BURGER_X86) || defined(BURGER_PPC))) || \
	((defined(BURGER_CLANG) || defined(BURGER_GNUC)) && \
		(defined(BURGER_INTEL) || defined(BURGER_PPC))) || \
	(defined(BURGER_GHS) && defined(BURGER_PPC)) || \
	(defined(BURGER_SNSYSTEMS) && defined(BURGER_PPC))) && \
	!defined(DOXYGEN)

// Done in assembly

#else

int32_t BURGER_API Burger::float_to_int_floor(float fInput) BURGER_NOEXCEPT
{
	// Let's SSE this shit
#if defined(BURGER_AMD64)
	// Convert to SSE register
	__m128 vInput = _mm_set_ss(fInput);
	// Convert to int with round to zero
	__m128i iVar = _mm_cvttps_epi32(vInput);
	// Convert back to float
	__m128 fVar = _mm_cvtepi32_ps(iVar);
	// Did I round up?
	fVar = _mm_cmpgt_ss(fVar, vInput);
	// Add 0 or -1
	iVar = _mm_add_epi32(iVar, _mm_castps_si128(fVar));
	return _mm_cvtsi128_si32(iVar);

#else
	int iVar = static_cast<int>(fInput); // Convert to int but rounded!
	float fVar = static_cast<float>(iVar);
	if (fVar > fInput) { // Did I round up?
		--iVar;          // Fix it
	}
	return iVar;
#endif
}
#endif

/*! ************************************

	\brief Convert a 32 bit float to an integer using floor().

	Convert a single precision floating point number to an integer
	using the floor() form of fractional truncation and store it to memory

	\code
	Burger::float_to_int_floor(&floorint,1.1f);	//1
	Burger::float_to_int_floor(&floorint,1.95f);	//1
	Burger::float_to_int_floor(&floorint,-1.1f);	//-2
	Burger::float_to_int_floor(&floorint,-1.95f);	//-2
	Burger::float_to_int_floor(&floorint,0.1f);	//0
	Burger::float_to_int_floor(&floorint,0.95f);	//0
	Burger::float_to_int_floor(&floorint,-0.1f);	//-1
	Burger::float_to_int_floor(&floorint,-0.95f);	//-1
	\endcode

	\param pOutput A valid pointer to a 32-bit integer to receive the result.
	\param fInput A valid single precision floating point number.

	\sa float_to_int_floor(float), float_to_int_round_to_zero(int32_t *,float),
		float_to_int_ceil(int32_t *,float), or float_to_int_round(int32_t
*,float)

***************************************/

#if ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_X86) || defined(BURGER_PPC))) || \
	(defined(BURGER_MSVC) && (defined(BURGER_X86) || defined(BURGER_PPC))) || \
	((defined(BURGER_CLANG) || defined(BURGER_GNUC)) && \
		(defined(BURGER_INTEL) || defined(BURGER_PPC))) || \
	(defined(BURGER_GHS) && defined(BURGER_PPC)) || \
	(defined(BURGER_SNSYSTEMS) && defined(BURGER_PPC))) && \
	!defined(DOXYGEN)

#else
// Done in assembly

void BURGER_API Burger::float_to_int_floor(
	int32_t* pOutput, float fInput) BURGER_NOEXCEPT
{
	// Let's SSE this shit
#if defined(BURGER_AMD64)
	// Convert to SSE register
	__m128 vInput = _mm_set_ss(fInput);
	// Convert to int with round to zero
	__m128i iVar = _mm_cvttps_epi32(vInput);
	// Convert back to float
	__m128 fVar = _mm_cvtepi32_ps(iVar);
	// Did I round up?
	fVar = _mm_cmpgt_ss(fVar, vInput);
	// Add 0 or -1
	iVar = _mm_add_epi32(iVar, _mm_castps_si128(fVar));
	// Store the result
	_mm_store_ss(static_cast<float*>(static_cast<void*>(pOutput)),
		_mm_castsi128_ps(iVar));
#else
	int iVar = static_cast<int>(fInput); // Convert to int but rounded!
	float fVar = static_cast<float>(iVar);
	if (fVar > fInput) { // Did I round up?
		--iVar;          // Fix it
	}
	pOutput[0] = iVar;
#endif
}

#endif

/*! ************************************

	\fn int32_t Burger::float_to_int_ceil(float fInput)
	\brief Convert a 32 bit float to an integer using ceil().

	Convert a single precision floating point number to an integer
	using the ceil() form of fractional truncation

	\code
	floorint = Burger::float_to_int_ceil(1.1f);	//2
	floorint = Burger::float_to_int_ceil(1.95f);	//2
	floorint = Burger::float_to_int_ceil(-1.1f);	//-1
	floorint = Burger::float_to_int_ceil(-1.95f);	//-1
	floorint = Burger::float_to_int_ceil(0.1f);	//1
	floorint = Burger::float_to_int_ceil(0.95f);	//1
	floorint = Burger::float_to_int_ceil(-0.1f);	//0
	floorint = Burger::float_to_int_ceil(-0.95f);	//0
	\endcode

	\param fInput A valid single precision floating point number.
	\return Signed integer equivalent value after applying ceil() on the
		floating point number.

	\sa float_to_int_ceil(int32_t *,float), float_to_int_floor(float),
		float_to_int_round_to_zero(float), or float_to_int_round(float)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_X86) || defined(BURGER_PPC))) || \
	(defined(BURGER_MSVC) && (defined(BURGER_X86) || defined(BURGER_PPC))) || \
	((defined(BURGER_CLANG) || defined(BURGER_GNUC)) && \
		(defined(BURGER_INTEL) || defined(BURGER_PPC))) || \
	(defined(BURGER_GHS) && defined(BURGER_PPC)) || \
	(defined(BURGER_SNSYSTEMS) && defined(BURGER_PPC))

#else

int32_t BURGER_API Burger::float_to_int_ceil(float fInput) BURGER_NOEXCEPT
{

#if defined(BURGER_AMD64)

	// Convert to SSE register
	__m128 vInput = _mm_set_ss(fInput);
	// Convert to int with round to zero
	__m128i iVar = _mm_cvttps_epi32(vInput);
	// Convert back to float
	__m128 fVar = _mm_cvtepi32_ps(iVar);
	// Did I round up?
	fVar = _mm_cmplt_ss(fVar, vInput);
	// Subtract 0 or -1
	iVar = _mm_sub_epi32(iVar, _mm_castps_si128(fVar));
	return _mm_cvtsi128_si32(iVar);
#else

	int iVar = static_cast<int>(fInput); // Convert to an int
	float fVar = static_cast<float>(iVar);
	if (fVar < fInput) { // Was there a change?
		++iVar;          // Round up
	}
	return iVar;

#endif
}
#endif

/*! ************************************

	\brief Convert a 32 bit float to an integer using ceil().

	Convert a single precision floating point number to an integer
	using the ceil() form of fractional truncation and store it to memory

	\code
	Burger::float_to_int_ceil(&floorint,1.1f);		//2
	Burger::float_to_int_ceil(&floorint,1.95f);	//2
	Burger::float_to_int_ceil(&floorint,-1.1f);	//-1
	Burger::float_to_int_ceil(&floorint,-1.95f);	//-1
	Burger::float_to_int_ceil(&floorint,0.1f);		//1
	Burger::float_to_int_ceil(&floorint,0.95f);	//1
	Burger::float_to_int_ceil(&floorint,-0.1f);	//0
	Burger::float_to_int_ceil(&floorint,-0.95f);	//0
	\endcode

	\param pOutput A valid pointer to a 32-bit integer to receive the result.
	\param fInput A valid single precision floating point number.

	\sa float_to_int_ceil(float),
		float_to_int_floor(int32_t* ,float),
		float_to_int_round_to_zero(int32_t* ,float), or
		float_to_int_round(int32_t* ,float)

***************************************/

#if ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_X86) || defined(BURGER_PPC))) || \
	(defined(BURGER_MSVC) && (defined(BURGER_X86) || defined(BURGER_PPC))) || \
	((defined(BURGER_CLANG) || defined(BURGER_GNUC)) && \
		(defined(BURGER_INTEL) || defined(BURGER_PPC))) || \
	(defined(BURGER_GHS) && defined(BURGER_PPC)) || \
	(defined(BURGER_SNSYSTEMS) && defined(BURGER_PPC))) \
	&& !defined(DOXYGEN)

#else

void BURGER_API Burger::float_to_int_ceil(
	int32_t* pOutput, float fInput) BURGER_NOEXCEPT
{

#if defined(BURGER_AMD64)
	// Convert to SSE register
	__m128 vInput = _mm_set_ss(fInput);
	// Convert to int with round to zero
	__m128i iVar = _mm_cvttps_epi32(vInput);
	// Convert back to float
	__m128 fVar = _mm_cvtepi32_ps(iVar);
	// Did I round up?
	fVar = _mm_cmplt_ss(fVar, vInput);
	// Add 0 or -1
	iVar = _mm_sub_epi32(iVar, _mm_castps_si128(fVar));
	// Store the result
	_mm_store_ss(static_cast<float*>(static_cast<void*>(pOutput)),
		_mm_castsi128_ps(iVar));

#else

	int iVar = static_cast<int>(fInput); // Convert to an int
	float fVar = static_cast<float>(iVar);
	if (fVar < fInput) { // Was there a change?
		++iVar;          // Round up
	}
	pOutput[0] = iVar;

#endif
}

#endif

/*! ************************************

	\brief Convert a 32 bit float to an integer using round to nearest.

	Convert a single precision floating point number to an integer
	using the round to nearest fractional truncation

	\code
	floorint = Burger::float_to_int_round(1.1f);		//1
	floorint = Burger::float_to_int_round(1.95f);		//2
	floorint = Burger::float_to_int_round(-1.1f);		//-1
	floorint = Burger::float_to_int_round(-1.95f);		//-2
	floorint = Burger::float_to_int_round(0.1f);		//0
	floorint = Burger::float_to_int_round(0.95f);		//1
	floorint = Burger::float_to_int_round(-0.1f);		//0
	floorint = Burger::float_to_int_round(-0.95f);		//-1
	\endcode

	\param fInput A valid single precision floating point number.
	\return Signed integer equivalent value after applying round to nearest on
		the floating point number.

	\sa float_to_int_round(int32_t *,float), float_to_int_floor(float),
		float_to_int_round_to_zero(float), or float_to_int_ceil(float)

***************************************/

#if ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_X86) || defined(BURGER_PPC))) || \
	(defined(BURGER_MSVC) && (defined(BURGER_X86) || defined(BURGER_PPC))) || \
	((defined(BURGER_CLANG) || defined(BURGER_GNUC)) && \
		(defined(BURGER_INTEL) || defined(BURGER_PPC))) || \
	(defined(BURGER_GHS) && defined(BURGER_PPC)) || \
	(defined(BURGER_SNSYSTEMS) && defined(BURGER_PPC))) && \
	!defined(DOXYGEN)

#else

int32_t BURGER_API Burger::float_to_int_round(float fInput) BURGER_NOEXCEPT
{
#if defined(BURGER_AMD64)
	// Convert to SSE register
	__m128 vInput = _mm_set_ss(fInput);

	// Convert to int with round to zero
	__m128i iVar = _mm_cvttps_epi32(vInput);

	// Convert back to float
	__m128 fVar = _mm_cvtepi32_ps(iVar);

	// Get the difference
	__m128 fDiff = _mm_sub_ps(vInput, fVar);

	// Constant 0x80000000
	__m128 vZero = _mm_set_ss(-0.0f);

	// Absolute value if fDiff (-0.0 becomes 0x7FFFFFFF)
	fDiff = _mm_andnot_ps(vZero, fDiff);

	// If vInput is negative, return 0xFFFFFFFF or return 0x00000001
	vInput = _mm_cmplt_ss(vInput, vZero);

	// Convert vZero to hex 0x000000001
	vZero = _mm_castsi128_ps(_mm_srli_epi32(_mm_castps_si128(vZero), 31));

	// 0xFFFFFFFF if greater than or equal 0.5f
	fDiff = _mm_cmpge_ss(fDiff, _mm_set_ss(g_fHalf));

	// Turn zero to 0x00000001
	vInput = _mm_or_ps(vInput, vZero);

	// Turn the -1/1 to 0 if < 0.5f
	fVar = _mm_and_ps(vInput, fDiff);

	// Add -1, 0 or 1 for final adjustment
	iVar = _mm_add_epi32(iVar, _mm_castps_si128(fVar));
	return _mm_cvtsi128_si32(iVar);

#else
	int32_t iVal = static_cast<int32_t>(fInput);
	float fVal = static_cast<float>(iVal);
	float fDiff = absolute(fInput - fVal);
	if (fDiff >= 0.5f) {
		if (fInput >= 0.0f) {
			iVal += 1;
		} else {
			iVal -= 1;
		}
	}
	return iVal; // Round to zero
#endif
}

#endif

/*! ************************************

	\brief Convert a 32 bit float to an integer using round to nearest.

	Convert a single precision floating point number to an integer
	using the round to nearest fractional truncation and store it to memory

	\code
	Burger::float_to_int_round(&floorint,1.1f);		//1
	Burger::float_to_int_round(&floorint,1.95f);		//2
	Burger::float_to_int_round(&floorint,-1.1f);		//-1
	Burger::float_to_int_round(&floorint,-1.95f);		//-2
	Burger::float_to_int_round(&floorint,0.1f);		//0
	Burger::float_to_int_round(&floorint,0.95f);		//1
	Burger::float_to_int_round(&floorint,-0.1f);		//0
	Burger::float_to_int_round(&floorint,-0.95f);		//-1
	\endcode

	\param pOutput A valid pointer to a 32-bit integer to receive the result.
	\param fInput A valid single precision floating point number.

	\sa float_to_int_round(float), float_to_int_floor(int32_t *,float),
		float_to_int_round_to_zero(int32_t *,float), or
		float_to_int_ceil(int32_t *,float)

***************************************/

#if ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_X86) || defined(BURGER_PPC))) || \
	(defined(BURGER_MSVC) && (defined(BURGER_X86) || defined(BURGER_PPC))) || \
	((defined(BURGER_CLANG) || defined(BURGER_GNUC)) && \
		(defined(BURGER_INTEL) || defined(BURGER_PPC))) || \
	(defined(BURGER_GHS) && defined(BURGER_PPC)) || \
	(defined(BURGER_SNSYSTEMS) && defined(BURGER_PPC))) && \
	!defined(DOXYGEN)

#else

void BURGER_API Burger::float_to_int_round(
	int32_t* pOutput, float fInput) BURGER_NOEXCEPT
{
#if defined(BURGER_AMD64)
	// Convert to SSE register
	__m128 vInput = _mm_set_ss(fInput);

	// Convert to int with round to zero
	__m128i iVar = _mm_cvttps_epi32(vInput);

	// Convert back to float
	__m128 fVar = _mm_cvtepi32_ps(iVar);

	// Get the difference
	__m128 fDiff = _mm_sub_ps(vInput, fVar);

	// Constant 0x80000000
	__m128 vZero = _mm_set_ss(-0.0f);

	// Absolute value if fDiff (-0.0 becomes 0x7FFFFFFF)
	fDiff = _mm_andnot_ps(vZero, fDiff);

	// If vInput is negative, return 0xFFFFFFFF or return 0x00000001
	vInput = _mm_cmplt_ss(vInput, vZero);

	// Convert vZero to hex 0x000000001
	vZero = _mm_castsi128_ps(_mm_srli_epi32(_mm_castps_si128(vZero), 31));

	// 0xFFFFFFFF if greater than or equal 0.5f
	fDiff = _mm_cmpge_ss(fDiff, _mm_set_ss(g_fHalf));

	// Turn zero to 0x00000001
	vInput = _mm_or_ps(vInput, vZero);

	// Turn the -1/1 to 0 if < 0.5f
	fVar = _mm_and_ps(vInput, fDiff);

	// Add -1, 0 or 1 for final adjustment
	iVar = _mm_add_epi32(iVar, _mm_castps_si128(fVar));
	// Store the result
	_mm_store_ss(static_cast<float*>(static_cast<void*>(pOutput)),
		_mm_castsi128_ps(iVar));

#else
	int32_t iVal = static_cast<int32_t>(fInput);
	float fVal = static_cast<float>(iVal);
	float fDiff = absolute(fInput - fVal);
	if (fDiff >= 0.5f) {
		if (fInput >= 0.0f) {
			iVal += 1;
		} else {
			iVal -= 1;
		}
	}
	pOutput[0] = iVal; // Round to zero
#endif
}

#endif

/*! ************************************

	\fn int32_t Burger::float_to_int_round_to_zero(float fInput)
	\brief Convert a 32 bit float to an integer using round to zero.

	Convert a single precision floating point number to an integer
	using the round to zero fractional truncation

	\code
	floorint = Burger::float_to_int_round_to_zero(1.1f);	//1
	floorint = Burger::float_to_int_round_to_zero(1.95f);	//1
	floorint = Burger::float_to_int_round_to_zero(-1.1f);	//-1
	floorint = Burger::float_to_int_round_to_zero(-1.95f);	//-1
	floorint = Burger::float_to_int_round_to_zero(0.1f);	//0
	floorint = Burger::float_to_int_round_to_zero(0.95f);	//0
	floorint = Burger::float_to_int_round_to_zero(-0.1f);	//0
	floorint = Burger::float_to_int_round_to_zero(-0.95f);	//0
	\endcode

	\param fInput A valid single precision floating point number.
	\return Signed integer equivalent value after applying round to zero on the
		floating point number.

	\sa float_to_int_round_to_zero(int32_t *,float), float_to_int_floor(float),
		float_to_int_ceil(float), or float_to_int_round(float)

***************************************/

int32_t BURGER_API Burger::float_to_int_round_to_zero(
	float fInput) BURGER_NOEXCEPT
{
	return static_cast<int32_t>(fInput); // Round to zero
}

/*! ************************************

	\fn void Burger::float_to_int_round_to_zero(int32_t *pOutput,float fInput)
	\brief Convert a 32 bit float to an integer using round to zero.

	Convert a single precision floating point number to an integer
	using the round to zero fractional truncation and store it to memory

	\code
	Burger::float_to_int_round_to_zero(&floorint,1.1f);		//1
	Burger::float_to_int_round_to_zero(&floorint,1.95f);	//1
	Burger::float_to_int_round_to_zero(&floorint,-1.1f);	//-1
	Burger::float_to_int_round_to_zero(&floorint,-1.95f);	//-1
	Burger::float_to_int_round_to_zero(&floorint,0.1f);		//0
	Burger::float_to_int_round_to_zero(&floorint,0.95f);	//0
	Burger::float_to_int_round_to_zero(&floorint,-0.1f);	//0
	Burger::float_to_int_round_to_zero(&floorint,-0.95f);	//0
	\endcode

	\param pOutput A valid pointer to a 32-bit integer to receive the result.
	\param fInput A valid single precision floating point number.

	\sa float_to_int_round_to_zero(float), float_to_int_floor(int32_t *,float),
		float_to_int_ceil(int32_t *,float), or float_to_int_round(int32_t
*,float)

***************************************/

void BURGER_API Burger::float_to_int_round_to_zero(
	int32_t* pOutput, float fInput) BURGER_NOEXCEPT
{
	pOutput[0] = static_cast<int32_t>(fInput); // Round to zero
}

/*! ************************************

	\brief Convert a 32 bit float to a \ref fixed16_16_t using floor().

	Convert a single precision floating point number to a \ref fixed16_16_t
	using the floor() form of fractional truncation

	\param fInput A valid single precision floating point number.

	\return a \ref fixed16_16_t equivalent value after applying floor() on the
		floating point number.

	\sa float_to_fixed_floor(fixed16_16_t *,float),
		float_to_fixed_round_to_zero(float),
		float_to_fixed_ceil(float), or
		float_to_fixed_round(float)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && defined(BURGER_X86)) || \
	(defined(BURGER_MSVC) && defined(BURGER_X86))
// Assembly
#else
fixed16_16_t BURGER_API Burger::float_to_fixed_floor(float fInput) BURGER_NOEXCEPT
{
	return static_cast<fixed16_16_t>(float_to_int_floor(fInput * 65536.0f));
}
#endif

/*! ************************************

	\brief Convert a 32 bit float to a \ref fixed16_16_t using round to zero.

	Convert a single precision floating point number to a \ref fixed16_16_t
	using the round to zero fractional truncation

	\param fInput A valid single precision floating point number.

	\return a \ref fixed16_16_t equivalent value after applying round to zero on the
		floating point number.

	\sa float_to_fixed_round_to_zero(fixed16_16_t *,float),
		float_to_fixed_floor(float), float_to_fixed_ceil(float), or
		float_to_fixed_round(float)

***************************************/

fixed16_16_t BURGER_API Burger::float_to_fixed_round_to_zero(
	float fInput) BURGER_NOEXCEPT
{
	return static_cast<fixed16_16_t>(
		float_to_int_round_to_zero(fInput * g_f65536.f));
}

/*! ************************************

	\fn fixed16_16_t Burger::float_to_fixed_ceil(float fInput)
	\brief Convert a 32 bit float to a \ref fixed16_16_t using ceil().

	Convert a single precision floating point number to a \ref fixed16_16_t
	using the ceil() form of fractional truncation

	\param fInput A valid single precision floating point number.

	\return a \ref fixed16_16_t equivalent value after applying ceil() on the
		floating point number.

	\sa float_to_fixed_ceil(fixed16_16_t *,float), float_to_fixed_floor(float),
		float_to_fixed_round_to_zero(float), or float_to_fixed_round(float)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && defined(BURGER_X86)) || \
	(defined(BURGER_MSVC) && defined(BURGER_X86))
// Assembly
#else
fixed16_16_t BURGER_API Burger::float_to_fixed_ceil(float fInput) BURGER_NOEXCEPT
{
	return static_cast<fixed16_16_t>(float_to_int_ceil(fInput * 65536.0f));
}
#endif

/*! ************************************

	\brief Convert a 32 bit float to a \ref fixed16_16_t using round to nearest.

	Convert a single precision floating point number to a \ref fixed16_16_t
	using the round to nearest fractional truncation

	\param fInput A valid single precision floating point number.
	\return a \ref fixed16_16_t equivalent value after applying round to nearest on
		the floating point number.

	\sa float_to_fixed_round(fixed16_16_t *,float), float_to_fixed_floor(float),
		float_to_fixed_round_to_zero(float), or float_to_fixed_ceil(float)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && defined(BURGER_X86)) || \
	(defined(BURGER_MSVC) && defined(BURGER_X86))
// Assembly
#else
fixed16_16_t BURGER_API Burger::float_to_fixed_round(float fInput) BURGER_NOEXCEPT
{
	return static_cast<fixed16_16_t>(float_to_int_round(fInput * 65536.0f));
}
#endif

/*! ************************************

	\brief Convert a 32 bit float to a \ref fixed16_16_t using floor().

	Convert a single precision floating point number to a \ref fixed16_16_t
	using the floor() form of fractional truncation and store it to memory

	\param pOutput A valid pointer to a \ref fixed16_16_t to receive the result.
	\param fInput A valid single precision floating point number.

	\sa float_to_fixed_floor(float),
		float_to_fixed_round_to_zero(fixed16_16_t* ,float),
		float_to_fixed_ceil(fixed16_16_t* ,float), or
		float_to_fixed_round(fixed16_16_t* ,float)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && defined(BURGER_X86)) || \
	(defined(BURGER_MSVC) && defined(BURGER_X86))
// Assembly
#else
void BURGER_API Burger::float_to_fixed_floor(
	fixed16_16_t* pOutput, float fInput) BURGER_NOEXCEPT
{
	float_to_int_floor(reinterpret_cast<int32_t*>(pOutput), fInput * 65536.0f);
}
#endif

/*! ************************************

	\brief Convert a 32 bit float to a \ref fixed16_16_t using round to zero.

	Convert a single precision floating point number to a \ref fixed16_16_t
	using the round to zero fractional truncation and store it to memory

	\param pOutput A valid pointer to a \ref fixed16_16_t to receive the result.
	\param fInput A valid single precision floating point number.
	\sa float_to_fixed_round_to_zero(float),
		float_to_fixed_floor(fixed16_16_t*, float),
		float_to_fixed_ceil(fixed16_16_t*, float), or
		float_to_fixed_round(fixed16_16_t*, float)

***************************************/

void BURGER_API Burger::float_to_fixed_round_to_zero(
	fixed16_16_t* pOutput, float fInput) BURGER_NOEXCEPT
{
	float_to_int_round_to_zero(
		reinterpret_cast<int32_t*>(pOutput), fInput * g_f65536.f);
}

/*! ************************************

	\brief Convert a 32 bit float to a \ref fixed16_16_t using ceil().

	Convert a single precision floating point number to a \ref fixed16_16_t
	using the ceil() form of fractional truncation and store it to memory

	\param pOutput A valid pointer to a \ref fixed16_16_t to receive the result.
	\param fInput A valid single precision floating point number.

	\sa float_to_fixed_ceil(float), float_to_fixed_floor(fixed16_16_t *,float),
		float_to_fixed_round_to_zero(fixed16_16_t *,float),
		or float_to_fixed_round(fixed16_16_t *,float)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && defined(BURGER_X86)) || \
	(defined(BURGER_MSVC) && defined(BURGER_X86))
// Assembly
#else
void BURGER_API Burger::float_to_fixed_ceil(
	fixed16_16_t* pOutput, float fInput) BURGER_NOEXCEPT
{
	float_to_int_ceil(reinterpret_cast<int32_t*>(pOutput), fInput * 65536.0f);
}
#endif

/*! ************************************

	\brief Convert a 32 bit float to a \ref fixed16_16_t using round to nearest.

	Convert a single precision floating point number to a \ref fixed16_16_t
	using the round to nearest fractional truncation and store it to memory

	\param pOutput A valid pointer to a \ref fixed16_16_t to receive the result.
	\param fInput A valid single precision floating point number.

	\sa float_to_fixed_round(float), float_to_fixed_floor(fixed16_16_t *,float),
		float_to_fixed_round_to_zero(fixed16_16_t *,float),
		or float_to_fixed_ceil(fixed16_16_t *,float)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && defined(BURGER_X86)) || \
	(defined(BURGER_MSVC) && defined(BURGER_X86))
// Assembly
#else
void BURGER_API Burger::float_to_fixed_round(
	fixed16_16_t* pOutput, float fInput) BURGER_NOEXCEPT
{
	float_to_int_round(reinterpret_cast<int32_t*>(pOutput), fInput * 65536.0f);
}
#endif

/*! ************************************

	\fn int8_t Burger::absolute(int8_t iInput)
	\brief Get the absolute value of an integer.

	Without branching, calculate the absolute value of an integer.

	\param iInput Value to get the absolute value of.
	\return The absolute value of iInput.

	\sa absolute(int16_t), absolute(int32_t), absolute(int64_t), absolute(float)
		and absolute(double)

***************************************/

/*! ************************************

	\fn int16_t Burger::absolute(int16_t iInput)
	\brief Get the absolute value of an integer.

	Without branching, calculate the absolute value of an integer.

	\param iInput Value to get the absolute value of.
	\return The absolute value of iInput.

	\sa absolute(int8_t), absolute(int32_t), absolute(int64_t), absolute(float)
		and absolute(double)

***************************************/

/*! ************************************

	\fn int32_t Burger::absolute(int32_t iInput)
	\brief Get the absolute value of an integer.

	Without branching, calculate the absolute value of an integer.

	\param iInput Value to get the absolute value of.
	\return The absolute value of iInput.

	\sa absolute(int8_t), absolute(int16_t), absolute(int64_t), absolute(float)
		and absolute(double)

***************************************/

/*! ************************************

	\fn int64_t Burger::absolute(int64_t iInput)
	\brief Get the absolute value of a 64 bit integer.

	Without branching, calculate the absolute value of an integer.

	\param iInput Value to get the absolute value of.
	\return The absolute value of iInput.

	\sa absolute(int8_t), absolute(int16_t), absolute(int32_t), absolute(float)
		and absolute(double)

***************************************/

/*! ************************************

	\fn int8_t Burger::ClampZero(int8_t iInput)
	\brief Clamp an integer to 0

	Without branching, if the integer is less than zero, set it to zero.

	\param iInput Value to test for less than zero.
	\return The zero clamped value of iInput.

	\sa ClampZero(int16_t), ClampZero(int32_t) or ClampZero(int64_t)

***************************************/

/*! ************************************

	\fn int16_t Burger::ClampZero(int16_t iInput)
	\brief Clamp an integer to 0

	Without branching, if the integer is less than zero, set it to zero.

	\param iInput Value to test for less than zero.
	\return The zero clamped value of iInput.

	\sa ClampZero(int8_t), ClampZero(int32_t) or ClampZero(int64_t)

***************************************/

/*! ************************************

	\fn int32_t Burger::ClampZero(int32_t iInput)
	\brief Clamp an integer to 0

	Without branching, if the integer is less than zero, set it to zero.

	\param iInput Value to test for less than zero.
	\return The zero clamped value of iInput.

	\sa ClampZero(int8_t), ClampZero(int16_t) or ClampZero(int64_t)

***************************************/

/*! ************************************

	\fn int64_t Burger::ClampZero(int64_t iInput)
	\brief Clamp an integer to 0

	Without branching, if the integer is less than zero, set it to zero.

	\param iInput Value to test for less than zero.
	\return The zero clamped value of iInput.

	\sa ClampZero(int8_t), ClampZero(int16_t) or ClampZero(int32_t)

***************************************/

/*! ************************************

	\fn int8_t Burger::get_sign(int8_t iInput)
	\brief Get the sign value of an integer.

	Without branching, calculate the sign value of an integer. If
	the input was zero, return zero, if it's positive, return 1 and
	if negative, return -1.

	\param iInput Value to get the sign value of.
	\return 0, 1 or -1.

	\sa get_sign(int16_t), get_sign(int32_t), get_sign(int64_t), get_sign(float)
		and get_sign(double)

***************************************/

/*! ************************************

	\fn int16_t Burger::get_sign(int16_t iInput)
	\brief Get the sign value of an integer.

	Without branching, calculate the sign value of an integer. If
	the input was zero, return zero, if it's positive, return 1 and
	if negative, return -1.

	\param iInput Value to get the sign value of.
	\return 0, 1 or -1.

	\sa get_sign(int8_t), get_sign(int32_t), get_sign(int64_t), get_sign(float)
		and get_sign(double)

***************************************/

/*! ************************************

	\fn int32_t Burger::get_sign(int32_t iInput)
	\brief Get the sign value of an integer.

	Without branching, calculate the sign value of an integer. If
	the input was zero, return zero, if it's positive, return 1 and
	if negative, return -1.

	\param iInput Value to get the sign value of.
	\return 0, 1 or -1.

	\sa get_sign(int8_t), get_sign(int16_t), get_sign(int64_t), get_sign(float)
		and get_sign(double)

***************************************/

/*! ************************************

	\fn int64_t Burger::get_sign(int64_t iInput)
	\brief Get the sign value of a 64 bit integer.

	Without branching, calculate the sign value of an integer. If
	the input was zero, return zero, if it's positive, return 1 and
	if negative, return -1.

	\param iInput Value to get the sign value of.
	\return 0, 1 or -1.

	\sa get_sign(int8_t), get_sign(int16_t), get_sign(int32_t), get_sign(float)
		and get_sign(double)

***************************************/

/*! ************************************

	\fn fixed16_16_t Burger::FixedMultiply(fixed16_16_t fInput1,fixed16_16_t fInput2)
	\brief Multiply two 16.16 fixed point numbers.

	Perform a signed multiplication of two 32-bit fixed point
	numbers and return the fixed point result. No
	error checking is performed, so overflow or underflow
	conditions yield undefined results.

	\param fInput1 First signed value to multiply
	\param fInput2 Second signed value to multiply
	\return Result of fInput1 * fInput2 without bounds checking

	\sa FixedMultiplySaturate(fixed16_16_t,fixed16_16_t) or FixedDivide(fixed16_16_t,fixed16_16_t)

***************************************/

#if defined(BURGER_WATCOM)
#elif (defined(BURGER_X86) && defined(BURGER_METROWERKS))
#elif (defined(BURGER_X86) && defined(BURGER_MSVC))
#elif (defined(BURGER_PPC) || defined(BURGER_64BITCPU))
#else
fixed16_16_t BURGER_API Burger::FixedMultiply(fixed16_16_t fInput1, fixed16_16_t fInput2)
{
	int64_t iFoo =
		static_cast<int64_t>(fInput1) * static_cast<int64_t>(fInput2);
	return static_cast<fixed16_16_t>(iFoo >> 16);
}
#endif

/*! ************************************

	\fn fixed16_16_t Burger::FixedDivide(fixed16_16_t fInputNumerator,
		fixed16_16_t fInputDenominator)

	\brief Divide two 16.16 fixed point numbers.

	Divide a 16.16 fixed point number by another 16.16 fixed point number
	without saturation or checking for division by zero..

	\param fInputNumerator Signed value to divide
	\param fInputDenominator Signed value to divide with
	\return Result of fInputNumerator / fInputDenominator without saturation

	\note A divide by zero or an overflow can cause an exception error!

	\sa FixedDivideSaturate(fixed16_16_t,fixed16_16_t), FixedReciprocal(fixed16_16_t) or
		FixedMultiply(fixed16_16_t,fixed16_16_t)

***************************************/

#if defined(BURGER_WATCOM)
#elif (defined(BURGER_X86) && defined(BURGER_METROWERKS))
#elif (defined(BURGER_X86) && defined(BURGER_MSVC))
#elif (defined(BURGER_PPC) || defined(BURGER_64BITCPU))
#else
fixed16_16_t BURGER_API Burger::FixedDivide(
	fixed16_16_t fInputNumerator, fixed16_16_t fInputDenominator)
{
	int64_t iFoo = (static_cast<int64_t>(fInputNumerator) << 16) /
		static_cast<int64_t>(fInputDenominator);
	return static_cast<fixed16_16_t>(iFoo >> 16);
}
#endif

/*! ************************************

	\fn fixed16_16_t Burger::FixedReciprocal(fixed16_16_t fInput)
	\brief Return the reciprocal of a fixed point number.

	Divide a 16.16 fixed point number by \ref BURGER_FLOAT_TO_FIXED(1.0f) in
fixed point. If the input value is negative epsilon (0xFFFFFFFF), return
0x8000000. If the input is equal to 0 or positive epsilon (0x1), then return
0x7ffffff. This has the effect of saturating the output and leaving no output as
undefined.

	\param fInput \ref fixed16_16_t value to convert to a reciprocal.
	\return Result of \ref BURGER_FLOAT_TO_FIXED(1.0f) / fInput with saturation

	\note It's not recommended to input epsilon or 0 due to
	saturation.

	\sa FixedDivideSaturate(fixed16_16_t,fixed16_16_t) or FixedDivide(fixed16_16_t,fixed16_16_t)

***************************************/

#if defined(BURGER_WATCOM)
#elif (defined(BURGER_X86) && defined(BURGER_METROWERKS))
#elif (defined(BURGER_X86) && defined(BURGER_MSVC))
#elif (defined(BURGER_PPC) || defined(BURGER_64BITCPU))
#else
fixed16_16_t BURGER_API Burger::FixedReciprocal(fixed16_16_t fInput)
{
	if (fInput != -1) {
		if (static_cast<uint32_t>(fInput) >= 2) { // Prevent a divide by zero
			uint32_t uFoo = static_cast<uint32_t>(absolute(fInput));
			uFoo = 0x80000000UL / (uFoo >> 1);
			if (fInput < 0) {
				return -static_cast<fixed16_16_t>(uFoo);
			}
			return static_cast<fixed16_16_t>(uFoo);
		}
		return kMaxFixed32;
	}
	return kMinFixed32;
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

	\sa Burger::square_root(fixed16_16_t), Burger::square_root(float),
		and Burger::square_root(double)

***************************************/

uint32_t BURGER_API Burger::square_root(uint32_t uInput)
{
	// Perform the square root

#if 0
	uint_t i = 16;
	uint32_t TestValue = 0x40000000;
	uint32_t Result = 0;
	do {
		uint32_t ToTest = Result+TestValue;
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

	uint32_t Result = 0;
	if (uInput >= 0x40000000) {
		Result = 0x40000000;
		uInput -= 0x40000000;
	}

	// Here is where the fun begins

	uint32_t ToTest = Result + 0x10000000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x10000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4000000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x4000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x1000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x400000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x100000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x40000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x10000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x4000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x1000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x400;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x100;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x40;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x10;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x4;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x1;
		uInput -= ToTest;
	}

	// Big finish!

	if (uInput > Result) {
		Result += 1;
	}
#endif
	return Result;
}

/*! ************************************

	\brief Get the square root of a \ref fixed16_16_t.

	Return the square root of a \ref fixed16_16_t and
	return the integer result.
	The maximum value is 255.998 (256) for the square root
	of 0x7FFFFFFF. This routine is 100% accurate.

	Negative numbers will return zero for the result.

	\param fInput Value to return the square root of.
	\return Return the square root as an integer.

	\note This will perform a round to nearest
	for the fraction so if the calculated square root is 1.6, it will
	return 2.

	\sa Burger::square_root(uint32_t)

***************************************/

uint32_t BURGER_API Burger::SqrtFixedToWord32(fixed16_16_t fInput)
{
	// Perform the square root

	if (fInput < 0) {
		return 0;
	}
	uint32_t uInput = static_cast<uint32_t>(fInput);

	uint32_t Result = 0;
	if (uInput >= 0x40000000) {
		Result = 0x40000000;
		uInput -= 0x40000000;
	}

	// Here is where the fun begins

	uint32_t ToTest = Result + 0x10000000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x10000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4000000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x4000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x1000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x400000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x100000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x40000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x10000;
		uInput -= ToTest;
	}

	// Get the extra bit 0.5f

	ToTest = Result + 0x4000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x4000;
		// uInput -= ToTest;
	}

	// Add in 0.5f to round up
	Result += 0x4000;
	Result >>= 15; // Convert to int
	return Result;
}

/*! ************************************

	\brief Get the square root of a \ref fixed16_16_t.

	Return the square root of a \ref fixed16_16_t and
	return the \ref fixed16_16_t result.
	The maximum value is 255.998 (256) for the square root
	of 0x7FFFFFFF. This routine is 100% accurate.

	Negative numbers will return zero for the result.

	\param fInput Value to return the square root of.
	\return Return the square root as a \ref fixed16_16_t.

	\sa Burger::square_root(uint32_t), Burger::square_root(float),
		and Burger::square_root(double)

***************************************/

fixed16_16_t BURGER_API Burger::square_root(fixed16_16_t fInput)
{
	if (fInput < 0) {
		return 0;
	}
	uint32_t uInput = static_cast<uint32_t>(fInput);

	// This is simplified since most are constants

	uint32_t Result = 0;
	if (uInput >= 0x40000000) {
		Result = 0x40000000;
		uInput -= 0x40000000;
	}

	/* Here is where the fun begins */

	uint32_t ToTest = Result + 0x10000000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x10000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4000000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x4000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x1000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x400000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x100000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x40000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x10000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x4000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x1000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x400;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x100;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x40;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x10;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x4;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x1;
		uInput -= ToTest;
	}

	// At this point, I've got the integer square root
	// I need another 16 bits of precision

	Result <<= 16;
	uInput <<= 16;

	// Now, iterate another 8 times for the final 16 bits

	ToTest = Result + 0x4000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x4000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x1000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x400;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x100;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x40;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x10;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x4;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1;
	Result >>= 1;
	if (uInput >= ToTest) {
		Result = Result + 0x1;
		uInput -= ToTest;
	}

	/* Big finish! */

	if (uInput > Result) {
		Result += 1;
	}
	return static_cast<fixed16_16_t>(Result);
}

/*! ************************************

	\fn uint32_t Burger::RotateLeft(uint32_t uInput,uint_t uShiftCount)
	\brief Rotate the bits left

	Using a compiler intrinsic where supported, rotate the bits in
	the 32 bit register to the left.

	\param uInput Value to rotate left
	\param uShiftCount 0 to 31, number of bits to shift.
	\return The value with the bits rotated

	\sa RotateLeft(uint64_t,uint_t) or RotateRight(uint32_t,uint_t)

***************************************/

/*! ************************************

	\fn uint32_t Burger::RotateRight(uint32_t uInput,uint_t uShiftCount)
	\brief Rotate the bits right

	Using a compiler intrinsic where supported, rotate the bits in
	the 32 bit register to the right.

	\param uInput Value to rotate right
	\param uShiftCount 0 to 31, number of bits to shift.
	\return The value with the bits rotated

	\sa RotateRight(uint64_t,uint_t) or RotateLeft(uint32_t,uint_t)

***************************************/

/*! ************************************

	\fn uint64_t Burger::RotateLeft(uint64_t uInput,uint_t uShiftCount)
	\brief Rotate the bits left

	Using a compiler intrinsic where supported, rotate the bits in
	the 64 bit register to the left.

	\param uInput Value to rotate left
	\param uShiftCount 0 to 63, number of bits to shift.
	\return The value with the bits rotated

	\sa RotateLeft(uint32_t,uint_t) or RotateRight(uint64_t,uint_t)

***************************************/

/*! ************************************

	\fn uint64_t Burger::RotateRight(uint64_t uInput,uint_t uShiftCount)
	\brief Rotate the bits right

	Using a compiler intrinsic where supported, rotate the bits in
	the 64 bit register to the right.

	\param uInput Value to rotate right
	\param uShiftCount 0 to 63, number of bits to shift.
	\return The value with the bits rotated

	\sa RotateRight(uint32_t,uint_t) or RotateLeft(uint64_t,uint_t)

***************************************/
