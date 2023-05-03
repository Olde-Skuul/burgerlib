/***************************************

	16 bit float manager

	Copyright (c) 2020-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

typedef uint16_t float16_t;

extern float16_t BURGER_API convert_to_float16(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API convert_to_float(float16_t uInput) BURGER_NOEXCEPT;

class Half {
	/** Binary representation of a 16 bit float */
	float16_t m_uData;

public:
	Half() BURGER_NOEXCEPT {}
	Half(float16_t uData) BURGER_NOEXCEPT: m_uData(uData) {}
	Half(float fInput) BURGER_NOEXCEPT;

	operator float() const BURGER_NOEXCEPT;

	BURGER_INLINE operator uint16_t() const BURGER_NOEXCEPT
	{
		return m_uData;
	}

	BURGER_INLINE uint_t is_finite() const BURGER_NOEXCEPT
	{
		return (static_cast<uint_t>(m_uData) & (0x1FU << 10U)) < (31U << 10U);
	}

	BURGER_INLINE uint_t is_infinity() const BURGER_NOEXCEPT
	{
		const uint_t uTemp = m_uData;
		return ((uTemp & (0x1FU << 10U)) == (31U << 10U)) &&
			(!(uTemp & 0x3FFU));
	}

	BURGER_INLINE uint_t is_nan() const BURGER_NOEXCEPT
	{
		const uint_t uTemp = m_uData;
		return ((uTemp & (0x1FU << 10U)) == (31U << 10U)) && (uTemp & 0x3FFU);
	}

	BURGER_INLINE uint_t is_normalized() const BURGER_NOEXCEPT
	{
		const uint_t uTemp = static_cast<uint_t>(m_uData) & (31U << 10U);
		return uTemp && (uTemp < (31U << 10U));
	}

	BURGER_INLINE uint_t is_denormalized() const BURGER_NOEXCEPT
	{
		const uint_t uTemp = m_uData;

		// Zero exponent and a non-zero mantissa
		return !(uTemp & (31U << 10U)) && (uTemp & 0x3FFU);
	}

	BURGER_INLINE uint_t is_zero() const BURGER_NOEXCEPT
	{
		return (m_uData & 0x7FFFU) != 0;
	}

	BURGER_INLINE uint_t is_negative() const BURGER_NOEXCEPT
	{
		return (m_uData & 0x8000U) != 0;
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
