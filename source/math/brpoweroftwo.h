/***************************************

	Power of two and other simple math functions

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRPOWEROFTWO_H__
#define __BRPOWEROFTWO_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRTEMPLATES_H__
#include "brtemplates.h"
#endif

/* BEGIN */
namespace Burger {

extern uint32_t BURGER_API power_of_two(uint32_t uInput BURGER_68K_D0) BURGER_NOEXCEPT;
extern uint64_t BURGER_API power_of_two(uint64_t uInput) BURGER_NOEXCEPT;
extern float BURGER_API power_of_two(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API power_of_two(double dInput) BURGER_NOEXCEPT;

template<class T>
BURGER_INLINE T power_of_two(T input) BURGER_NOEXCEPT
{
	BURGER_STATIC_ASSERT(is_unsigned<T>::value);
	return static_cast<T>(power_of_two(static_cast<
		typename conditional<sizeof(T) <= 4, uint32_t, uint64_t>::type>(
		input)));
}

extern uint32_t BURGER_API convert_from_BCD(uint32_t uInput) BURGER_NOEXCEPT;
extern uint32_t BURGER_API convert_to_BCD(uint32_t uInput) BURGER_NOEXCEPT;
extern uint32_t BURGER_API bit_reverse(
	uint32_t uInput, uint_t uBitLength) BURGER_NOEXCEPT;
extern uint64_t BURGER_API bit_reverse(
	uint64_t uInput, uint_t uBitLength) BURGER_NOEXCEPT;
extern uint_t BURGER_API count_set_bits(uint32_t uInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API count_set_bits(uint64_t uInput) BURGER_NOEXCEPT;

}
/* END */

#endif
