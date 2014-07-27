/***************************************

	Random number generator

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRRANDOMMANAGER_H__
#define __BRRANDOMMANAGER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGLOBALMEMORYMANAGER_H__
#include "brglobalmemorymanager.h"
#endif

/* BEGIN */
namespace Burger {
class Random {
public:
	BURGER_INLINE Random() { Init(); }
	static Random* New(Word32 uNewSeed=0);
	BURGER_INLINE void Delete(void) { Burger::Free(this); }
	void Init(void);
	void RandomInit(void);
	Word32 Get(Word32 uRange=0);
	void SetSeed(Word32 uNewSeed);
	Int32 GetSigned(Word32 uRange);
	float GetFloat(void);
private:
	Word32 m_Array[17];	///< Array of seed values (Polynomial)
	Word32 m_uSeed;		///< Random number seed
	Word m_uIndex;		///< First lookup index
};
}

/* END */

#endif

