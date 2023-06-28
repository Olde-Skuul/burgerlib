/***************************************

	Random number generator using Mersenne Twist (MT19937)

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMERSENNETWIST_H__
#define __BRMERSENNETWIST_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRRANDOMBASE_H__
#include "brrandombase.h"
#endif

/* BEGIN */
namespace Burger {

class RandomMersenneTwist: public RandomBase {
	BURGER_RTTI_IN_CLASS();

protected:
	/** Number of elements in the polynomial array */
	static const uint_t kElements = 624;

	/** Tap index into the polynomial array */
	static const uint_t kTapIndex = 397;

	/** Array of seed values (Polynomial) */
	uint32_t m_Array[kElements];

	/** First lookup index */
	uint32_t m_uIndex;

public:
	RandomMersenneTwist(uint32_t uNewSeed = 5489U) BURGER_NOEXCEPT;
	static RandomMersenneTwist* BURGER_API New(
		uint32_t uNewSeed = 0) BURGER_NOEXCEPT;

	void set_seed(uint32_t uNewSeed) BURGER_NOEXCEPT BURGER_OVERRIDE;
	uint32_t get(void) BURGER_NOEXCEPT BURGER_OVERRIDE;
};
}

/* END */

#endif
