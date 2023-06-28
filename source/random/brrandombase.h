/***************************************

	Random number generator base class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRRANDOMBASE_H__
#define __BRRANDOMBASE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

/* BEGIN */
namespace Burger {

class RandomBase: public Base {
	BURGER_RTTI_IN_CLASS();

protected:
	/** Random number seed */
	uint32_t m_uSeed;

public:
	virtual void set_seed(uint32_t uNewSeed) BURGER_NOEXCEPT = 0;
	virtual uint32_t get(void) BURGER_NOEXCEPT = 0;

	BURGER_INLINE uint32_t get_seed(void) const BURGER_NOEXCEPT
	{
		return m_uSeed;
	}

	void BURGER_API random_seed(void) BURGER_NOEXCEPT;
	uint32_t BURGER_API get_uint32(uint32_t uRange) BURGER_NOEXCEPT;
	uint32_t BURGER_API get_dice(
		uint32_t uDiceCount, uint32_t uDiceSize) BURGER_NOEXCEPT;
	int32_t BURGER_API get_int32(uint32_t uRange) BURGER_NOEXCEPT;
	float BURGER_API get_float(void) BURGER_NOEXCEPT;
	float BURGER_API get_float(float fRange) BURGER_NOEXCEPT;
	float BURGER_API get_symmetric_float(void) BURGER_NOEXCEPT;
	float BURGER_API get_symmetric_float(float fRange) BURGER_NOEXCEPT;
	double BURGER_API get_double(void) BURGER_NOEXCEPT;
};

}

/* END */

#endif
