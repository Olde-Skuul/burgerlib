/***************************************

	Random number generator

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRRANDOM_H__
#define __BRRANDOM_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BRRANDOMBASE_H__
#include "brrandombase.h"
#endif

#ifndef __BRWATCOM_H__
#include "brwatcom.h"
#endif

/* BEGIN */
namespace Burger {

class Random : public RandomBase {
	BURGER_RTTI_IN_CLASS();
protected:
	Word32 m_Array[17];	///< Array of seed values (Polynomial)
	Word32 m_uState;	///< Last random number generated
	Word m_uIndex;		///< First lookup index
public:
	Random(Word32 uNewSeed=728056387U);
	static Random* BURGER_API New(Word32 uNewSeed=0);
	virtual void SetSeed(Word32 uNewSeed);
	virtual Word32 Get(void);
};
}

/* END */

#endif

