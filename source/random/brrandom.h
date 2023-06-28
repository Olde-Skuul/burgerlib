/***************************************

	Random number generator

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRRANDOM_H__
#define __BRRANDOM_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRRANDOMBASE_H__
#include "brrandombase.h"
#endif

/* BEGIN */
namespace Burger {

class Random: public RandomBase {
	BURGER_RTTI_IN_CLASS();

protected:
	/** Array of seed values (Polynomial) */
	uint32_t m_Array[17];

	/** Last random number generated */
	uint32_t m_uState;

	/** First lookup index */
	uint32_t m_uIndex;

public:
	Random(uint32_t uNewSeed = 728056387U) BURGER_NOEXCEPT;
	static Random* BURGER_API New(uint32_t uNewSeed = 0) BURGER_NOEXCEPT;

	void set_seed(uint32_t uNewSeed) BURGER_NOEXCEPT BURGER_OVERRIDE;
	uint32_t get(void) BURGER_NOEXCEPT BURGER_OVERRIDE;
};
}

/* END */

#endif
