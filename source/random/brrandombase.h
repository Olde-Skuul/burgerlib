/***************************************

	Random number generator base class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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

class RandomBase : public Base {
	BURGER_RTTI_IN_CLASS();
protected:
	uint32_t m_uSeed;		///< Random number seed
public:
	virtual void SetSeed(uint32_t uNewSeed) = 0;
	virtual uint32_t Get(void) = 0;
	BURGER_INLINE uint32_t GetSeed(void) const { return m_uSeed; }
	void BURGER_API RandomSeed(void);
    uint32_t BURGER_API GetWord(uint32_t uRange);
    uint32_t BURGER_API GetDice(uint32_t uDiceCount, uint32_t uDiceSize);
    int32_t BURGER_API GetSigned(uint32_t uRange);
	float BURGER_API GetFloat(void);
	float BURGER_API GetFloat(float fRange);
	float BURGER_API GetSymmetricFloat(void);
	float BURGER_API GetSymmetricFloat(float fRange);
	double BURGER_API GetDouble(void);
};

}

/* END */

#endif

