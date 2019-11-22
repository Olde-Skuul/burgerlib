/***************************************

	Random number generator using Mersenne Twist (MT19937)

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMERSENNETWIST_H__
#define __BRMERSENNETWIST_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BRRANDOMBASE_H__
#include "brrandombase.h"
#endif

/* BEGIN */
namespace Burger {

class RandomMersenneTwist : public RandomBase {
	BURGER_RTTI_IN_CLASS();
protected:
	static const Word cElements = 624;	///< Number of elements in the polynomial array
	static const Word cTapIndex = 397;	///< Tap index into the polynomial array
	Word32 m_Array[cElements];			///< Array of seed values (Polynomial)
	Word32 m_uIndex;					///< First lookup index
public:
	RandomMersenneTwist(Word32 uNewSeed=5489U);
	static RandomMersenneTwist* BURGER_API New(Word32 uNewSeed=0);
	void SetSeed(Word32 uNewSeed) BURGER_OVERRIDE;
	Word32 Get(void) BURGER_OVERRIDE;
};
}

/* END */

#endif

