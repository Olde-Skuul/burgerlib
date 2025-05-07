/***************************************

	16 bit float manager

	Copyright (c) 2020-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFPHALF_H__
#define __BRFPHALF_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

// If the compiler supports float16, declare it
#if __has_keyword(__fp16) || \
	(defined(BURGER_ARM) && \
		(defined(__ARM_FP16_FORMAT_IEEE) || (__ARM_FEATURE_MVE & 2)))
typedef __fp16 float16_t;

#elif (__has_keyword(_Float16) || \
	(!defined(BURGER_MSVC) && defined(BURGER_INTEL) && \
		(defined(__F16C__) || \
			(defined(BURGER_SSE2) && defined(BURGER_AVX))))) && \
	!defined(DOXYGEN)
typedef _Float16 float16_t;

#else
#define BURGER_FLOAT16_EMULATED
typedef uint16_t float16_t;
#endif

extern uint16_t BURGER_API convert_to_float16(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API convert_to_float(uint16_t uInput) BURGER_NOEXCEPT;

class Half {
	union {
		/** Binary representation of a 16 bit float */
		uint16_t u;
		/** The native format of a 16 bit float on platforms that support it */
		float16_t f;
	};

public:
	Half() BURGER_NOEXCEPT {}
	Half(uint16_t uData) BURGER_NOEXCEPT: u(uData) {}
	Half(float fInput) BURGER_NOEXCEPT;

	operator float() const BURGER_NOEXCEPT;

	BURGER_INLINE operator uint16_t() const BURGER_NOEXCEPT
	{
		return u;
	}

	uint_t BURGER_API is_finite(void) const BURGER_NOEXCEPT;
	uint_t BURGER_API is_infinite(void) const BURGER_NOEXCEPT;
	uint_t BURGER_API is_NaN(void) const BURGER_NOEXCEPT;
	uint_t BURGER_API is_normal(void) const BURGER_NOEXCEPT;
	uint_t BURGER_API is_denormalized(void) const BURGER_NOEXCEPT;

	BURGER_INLINE uint_t is_zero(void) const BURGER_NOEXCEPT
	{
		return (u & 0x7FFFU) != 0;
	}

	BURGER_INLINE uint_t is_negative(void) const BURGER_NOEXCEPT
	{
		return (u & 0x8000U) != 0;
	}

	Half operator-() const BURGER_NOEXCEPT;
	Half& operator=(const Half& rInput) BURGER_NOEXCEPT;
	Half& operator=(float fInput) BURGER_NOEXCEPT;
	Half& operator+=(const Half& rInput) BURGER_NOEXCEPT;
	Half& operator+=(float fInput) BURGER_NOEXCEPT;
	Half& operator-=(const Half& rInput) BURGER_NOEXCEPT;
	Half& operator-=(float fInput) BURGER_NOEXCEPT;
	Half& operator*=(const Half& rInput) BURGER_NOEXCEPT;
	Half& operator*=(float fInput) BURGER_NOEXCEPT;
	Half& operator/=(const Half& rInput) BURGER_NOEXCEPT;
	Half& operator/=(float fInput) BURGER_NOEXCEPT;
};

}
/* END */

#endif
