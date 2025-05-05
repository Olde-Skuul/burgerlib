/***************************************

	16 bit float manager

	Copyright (c) 2020-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfphalf.h"
#include "brstructs.h"

// These platforms have hardware support
#if ((defined(BURGER_MACOSX) && defined(BURGER_ARM)) || \
	defined(BURGER_XBOXONE) || defined(BURGER_PS4) || defined(BURGER_PS5) || \
	defined(BURGER_SWITCH)) && \
	!defined(DOXYGEN)
#define USE_INTRINSICS
#endif

#if defined(USE_INTRINSICS) && defined(BURGER_INTEL)
#include <immintrin.h>
#endif

#if defined(USE_INTRINSICS) && defined(BURGER_ARM)
#include <arm_neon.h>
#endif

/*! ************************************

	\typedef Burger::float16_t
	\brief 16 bit float

	Typedef that directly maps to a __fp16 or _Float16 type if the compiler
	supports it. Otherwise it is a uint16_t to hold a 16 bit floating point
	number.

	\sa Half, or BURGER_FLOAT16_EMULATED

***************************************/

/*! ************************************

	\def BURGER_FLOAT16_EMULATED
	\brief Define to determine if 16 bit float support is not supported in
		hardware.

	If this define exists, then the code being generated for 16 bit floating
	point is through software. If this define does not exist, then the typedef
	Burger::float16_t maps to a compiler supported native data type that has
	hardware support for the data format.

	\sa BURGER_ARM, BURGER_INTEL, BURGER_AVX, BURGER_NEON, or Burger::Half

***************************************/

/*! ************************************

	\brief Convert a 32 bit float into a 16 bit float

	The conversion will retain sign, Nan, Inf and perform rounding to nearest
	even.

	Small numbers are converted to zero, large numbers to infinity.

	Negative zero returns negative zero.

	\param fInput 32 bit float to convert

	\return 16 bit float as a uint16_t

	\sa Half, or convert_to_float(uint16_t)

***************************************/

uint16_t BURGER_API Burger::convert_to_float16(float fInput) BURGER_NOEXCEPT
{
#if defined(USE_INTRINSICS) && defined(BURGER_INTEL)
	// Convert float to SSE
	__m128 vValue = _mm_set_ss(fInput);
	// Use hardware to convert to FP16
	__m128i iValue = _mm_cvtps_ph(vValue, _MM_FROUND_TO_NEAREST_INT);
	// Return the value
	return static_cast<uint16_t>(_mm_cvtsi128_si32(iValue));

#elif defined(USE_INTRINSICS) && defined(BURGER_ARM)
	// Convert float to Neon
	float32x4_t vValue = vdupq_n_f32(fInput);
	// Use hardware to convert to FP16
	float16x4_t iValue = vcvt_f16_f32(vValue);
	// Return the value
	return vget_lane_u16(vreinterpret_u16_f16(iValue), 0);

#else

	// Convert the float into a binary integer, literally
	uint32_float_t holder;
	holder.set_float(fInput);
	uint32_t uInput = holder.get_uint32();

	// Extract the sign for float16
	const uint32_t uSign = (uInput >> 16U) & 0x8000U;

	// Extract the signed exponent
	// Bias on 32 bit floats is 127, bias on 16 bit floats is 15
	int32_t iExponent =
		static_cast<int32_t>(((uInput >> 23U) & 0xFFU) - (127 - 15));

	// Check if the number is REALLY small
	if (iExponent < -10) {
		// The number is smaller than epsilon, so return a zero
		// with the sign retained.
		return static_cast<uint16_t>(uSign);
	}

	// Extract the 23 bit mantissa
	uint32_t uMantissa = uInput & 0x7FFFFFU;

	// Check if the number is less than BURGER_HALF_NRM_MIN
	if (iExponent <= 0) {

		// The exponent is 0 through -10 for a normalized float
		// Convert it to a denormalized float16 with an exponent of zero

		// Denormalized numbers have the implicit 1, so restore it.
		uMantissa = uMantissa | 0x800000U;

		// Round the value to the nearest 10 bit value with rounding to the
		// nearest even (24-10 = 14)

		// Yield 14 to 24
		const uint32_t uShift = static_cast<uint32_t>(14U - iExponent);

		// Create the 0.999999 mask for rounding up
		const uint32_t uMask = (1U << (uShift - 1U)) - 1U;

		// If odd, force rounding up by 1
		const uint32_t uRound = (uMantissa >> uShift) & 1U;

		// Add in either 1.0 or 0.9999 and then hack off the unused bits
		uMantissa = (uMantissa + uMask + uRound) >> uShift;

		// Update the new mantissa (Exponent = 0)
		return static_cast<uint16_t>(uSign | uMantissa);
	}

	// Was the original exponent 0xFF (NaN / Infinity)?
	if (iExponent == 0xFFU - (127 - 15)) {
		if (!uMantissa) {
			// Infinity!
			return static_cast<uint16_t>(uSign | 0x7C00U);
		}
		// NaN!
		return static_cast<uint16_t>(0x7FFFU | uSign);
	}

	// What remains is a normalized float, perform the actual conversion
	// with rounding to even
	uMantissa = uMantissa + 0xFFFU + ((uMantissa >> 13U) & 1U);

	// Check if the rounding overflowed the mantissa
	if (uMantissa & 0x800000U) {
		// Zark the mantissa and increase the exponent
		uMantissa = 0;
		++iExponent;
	}

	// Is the exponent too darn high?
	if (iExponent > 30) {

		// Convert to infinity
		return static_cast<uint16_t>(uSign | 0x7C00U);
	}

	// Return the final float16
	return static_cast<uint16_t>(
		uSign | (iExponent << 10U) | (uMantissa >> 13U));

#endif
}

/*! ************************************

	\brief Convert a 16 bit float into a 32 bit float

	The conversion will retain sign, Nan, Inf and perform rounding to nearest
	even.

	Negative zero returns negative zero.

	\param uInput 16 bit float to convert

	\return 32 bit float

	\sa Half, or convert_to_float16(float)

***************************************/

float BURGER_API Burger::convert_to_float(uint16_t uInput) BURGER_NOEXCEPT
{
#if defined(USE_INTRINSICS) && defined(BURGER_INTEL)
	// Convert short to SSE
	__m128i iValue = _mm_cvtsi32_si128(static_cast<int>(uInput));
	// Use hardware to convert to 32 bit float
	__m128 vValue = _mm_cvtph_ps(iValue);
	// Return the value
	return _mm_cvtss_f32(vValue);

#elif defined(USE_INTRINSICS) && defined(BURGER_ARM)
	// Convert short to Neon
	uint16x4_t iValue = vdup_n_u16(uInput);
	// Use hardware to convert to 32 bit float
	float32x4_t vValue = vcvt_f32_f16(vreinterpret_f16_u16(iValue));
	// Return the value
	return vgetq_lane_f32(vValue, 0);

#else

	// Used for int -> float binary conversion
	uint32_float_t holder;

	// Convert the sign to the final form
	const uint32_t uSign = ((uInput & 0x8000U) << 16U);

	// Extract the exponent
	uint32_t uExponent = (uInput >> 10U) & 0x1FU;

	// Extract the mantissa
	uint32_t uMantissa = uInput & 0x000003ffU;

	// Zero or denormalized?
	if (!uExponent) {

		// Zero?
		if (!uMantissa) {

			// Return 0.0f or -0.0f
			holder.set_uint32(uSign);
			return holder.get_float();
		}

		// Convert the denormalized number to normalized
		while (!(uMantissa & 0x400U)) {
			uMantissa <<= 1U;
			--uExponent;
		}

		// Fix the exponent
		++uExponent;

		// Hack off the implicit 1
		uMantissa &= static_cast<uint32_t>(~0x400U);

		// Check for special exponent
	} else if (uExponent == 31U) {
		if (!uMantissa) {
			// Positive or negative infinity
			return uSign ? g_fNegInf : g_fInf;
		}
		return uSign ? g_fNegNan : g_fNan;
	}

	// Perform the conversion
	uExponent = uExponent + (127U - 15U);
	uMantissa = uMantissa << 13U;

	// Assemble the 32 bit float and return
	holder.set_uint32(uSign | (uExponent << 23U) | uMantissa);
	return holder.get_float();
#endif
}

/*! ************************************

	\class Burger::Half
	\brief Container for a 16 bit float

	This class will allow math functions to be applied to a 16 bit float since
	most "C" compilers don't have native support for the data type.

	While this class exists, it is recommended to perform math operations with
	float or double until a final result is created and then the result is
	converted to the \ref float16_t data type.

	\sa convert_to_float16(float), or convert_to_float(uint16_t)

***************************************/

/*! ************************************

	\fn Burger::Half::Half()
	\brief Constructor

	This constructor does NOT initialize this class.

	\sa Half(uint16_t), or Half(float)

***************************************/

/*! ************************************

	\fn Burger::Half::Half(uint16_t)
	\brief Constructor with 16 bit float.

	Initialize with a 16 bit float.

	\sa Half(float), or Half()

***************************************/

/*! ************************************

	\brief Constructor with 32 bit float.

	Initialize with a 32 bit float, which is converted into a 16 bit float.

	\sa Half(uint16_t), or Half()

***************************************/

Burger::Half::Half(float fInput) BURGER_NOEXCEPT
{
	u = convert_to_float16(fInput);
}

/*! ************************************

	\brief Convert to a 32 bit float.

	Convert the float 16 into a 32 bit float and return the float.

	\sa \ref operator uint16_t()

***************************************/

Burger::Half::operator float() const BURGER_NOEXCEPT
{
	return convert_to_float(u);
}

/*! ************************************

	\fn Burger::Half::operator uint16_t() const
	\brief Return the 16 bit float.

	\sa \ref operator float()

***************************************/

/*! ************************************

	\fn Burger::Half::is_finite() const
	\brief Test if the value is finite.

	If the contained value is a finite value, return \ref TRUE

	\return \ref TRUE or \ref FALSE

	\sa is_infinity() const, or is_nan() const

***************************************/

/*! ************************************

	\fn Burger::Half::is_infinity() const
	\brief Test if the value is infinite.

	If the contained value is a infinite value, return \ref TRUE

	\return \ref TRUE or \ref FALSE

	\sa is_finite() const, or is_nan() const

***************************************/

/*! ************************************

	\fn Burger::Half::is_nan() const
	\brief Test if the value is NaN.

	If the contained value is a NaN (Not a Number) value, return \ref TRUE

	\return \ref TRUE or \ref FALSE

	\sa is_finite() const, or is_infinity() const

***************************************/

/*! ************************************

	\fn Burger::Half::is_normalized() const
	\brief Test if the value is normalzed.

	If the contained value is normalized, return \ref TRUE

	\return \ref TRUE or \ref FALSE

	\sa is_denormalized() const

***************************************/

/*! ************************************

	\fn Burger::Half::is_denormalized() const
	\brief Test if the value is denormalzed.

	If the contained value is denormalized, return \ref TRUE

	\return \ref TRUE or \ref FALSE

	\sa is_normalized() const

***************************************/

/*! ************************************

	\fn Burger::Half::is_zero() const
	\brief Test if the value is zero.

	If the contained value is zero, return \ref TRUE

	\note It will check for both positive and negative zero.

	\return \ref TRUE or \ref FALSE

	\sa is_negative() const

***************************************/

/*! ************************************

	\fn Burger::Half::is_negative() const
	\brief Test if the value is negative.

	If the contained value has the sign bit set, return \ref TRUE

	\return \ref TRUE or \ref FALSE

	\sa is_zero() const

***************************************/

/*! ************************************

	\brief Return a negated Half

	Negate the float16 and return that result as a newly created Half class.

	\return Half class with the value negated from this value

***************************************/

Burger::Half Burger::Half::operator-() const BURGER_NOEXCEPT
{
	return Half(static_cast<uint16_t>(u ^ 0x8000U));
}

/*! ************************************

	\brief Copy a Half class

	Copy the contents of the supplied Half into this instance.

	\param rInput Half to copy to this instance.

	\return *this

***************************************/

Burger::Half& Burger::Half::operator=(const Half& rInput) BURGER_NOEXCEPT
{
	u = rInput.u;
	return *this;
}

/*! ************************************

	\brief Assign a 32 bit float to this class.

	Convert the 32 bit float into a 16 bit float and store the 16 bit value in
	this instance of this class.

	\param fInput 32 bit float to convert to a 16 bit float.

	\return *this

***************************************/

Burger::Half& Burger::Half::operator=(float fInput) BURGER_NOEXCEPT
{
	u = convert_to_float16(fInput);
	return *this;
}

/*! ************************************

	\brief Add a Half to this Half.

	Convert both inputs into floats, perform an addition, and then store the
	result in this class.

	\note Due to conversions, this function is not as fast as performing
		hardware float operations.

	\param rInput Half to add to this instance.

	\return *this

***************************************/

Burger::Half& Burger::Half::operator+=(const Half& rInput) BURGER_NOEXCEPT
{
	u = convert_to_float16(convert_to_float(u) + convert_to_float(rInput.u));
	return *this;
}

/*! ************************************

	\brief Add a float to this Half.

	Convert this class into a float, perform an addition, and then store the
	result in this class.

	\note Due to conversions, this function is not as fast as performing
		hardware float operations.

	\param fInput 32 bit float to add to this Half

	\return *this

***************************************/

Burger::Half& Burger::Half::operator+=(float fInput) BURGER_NOEXCEPT
{
	u = convert_to_float16(convert_to_float(u) + fInput);
	return *this;
}

/*! ************************************

	\brief Subtract a Half from this Half.

	Convert both inputs into floats, perform a subtraction, and then store the
	result in this class.

	\note Due to conversions, this function is not as fast as performing
		hardware float operations.

	\param rInput Half to subtract from this instance.

	\return *this

***************************************/

Burger::Half& Burger::Half::operator-=(const Half& rInput) BURGER_NOEXCEPT
{
	u = convert_to_float16(convert_to_float(u) - convert_to_float(rInput.u));
	return *this;
}

/*! ************************************

	\brief Subtract a float to this Half.

	Convert this class into a float, perform a subtraction, and then store the
	result in this class.

	\note Due to conversions, this function is not as fast as performing
		hardware float operations.

	\param fInput 32 bit float to subtract from this Half

	\return *this

***************************************/

Burger::Half& Burger::Half::operator-=(float fInput) BURGER_NOEXCEPT
{
	u = convert_to_float16(convert_to_float(u) - fInput);
	return *this;
}

/*! ************************************

	\brief Multiply a Half with this Half.

	Convert both inputs into floats, perform a multiplication, and then store
	the result in this class.

	\note Due to conversions, this function is not as fast as performing
		hardware float operations.

	\param rInput Half to multiply to this instance.

	\return *this

***************************************/

Burger::Half& Burger::Half::operator*=(const Half& rInput) BURGER_NOEXCEPT
{
	u = convert_to_float16(convert_to_float(u) * convert_to_float(rInput.u));
	return *this;
}

/*! ************************************

	\brief Multiply a float to this Half.

	Convert this class into a float, perform a multiplication, and then store
	the result in this class.

	\note Due to conversions, this function is not as fast as performing
		hardware float operations.

	\param fInput 32 bit float to multiply with this Half

	\return *this

***************************************/

Burger::Half& Burger::Half::operator*=(float fInput) BURGER_NOEXCEPT
{
	u = convert_to_float16(convert_to_float(u) * fInput);
	return *this;
}

/*! ************************************

	\brief Divide a Half from this Half.

	Convert both inputs into floats, perform a division, and then store
	the result in this class.

	\note Due to conversions, this function is not as fast as performing
		hardware float operations.

	\param rInput Half to divide from this instance.

	\return *this

***************************************/

Burger::Half& Burger::Half::operator/=(const Half& rInput) BURGER_NOEXCEPT
{
	u = convert_to_float16(convert_to_float(u) / convert_to_float(rInput.u));
	return *this;
}

/*! ************************************

	\brief Divide a float from this Half.

	Convert this class into a float, perform a division, and then store
	the result in this class.

	\note Due to conversions, this function is not as fast as performing
		hardware float operations.

	\param fInput 32 bit float to divide from this Half

	\return *this

***************************************/

Burger::Half& Burger::Half::operator/=(float fInput) BURGER_NOEXCEPT
{
	u = convert_to_float16(convert_to_float(u) / fInput);
	return *this;
}
