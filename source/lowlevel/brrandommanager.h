/***************************************

	Random number generator

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
protected:
	Word32 m_Array[17];	///< Array of seed values (Polynomial)
	Word32 m_uSeed;		///< Random number seed
	Word m_uIndex;		///< First lookup index
public:
	BURGER_INLINE Random() { Init(); }
	static Random* BURGER_API New(Word32 uNewSeed=0);
	void BURGER_API Init(void);
	void BURGER_API RandomInit(void);
	Word32 BURGER_API Get(Word32 uRange=0);
	void BURGER_API SetSeed(Word32 uNewSeed);
	Int32 BURGER_API GetSigned(Word32 uRange);
	float BURGER_API GetFloat(void);
	float BURGER_API GetFloat(float fRange);
	float BURGER_API GetSymmetricFloat(void);
	float BURGER_API GetSymmetricFloat(float fRange);
};
}

/* END */

#endif

