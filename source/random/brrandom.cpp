/***************************************

	Random number generator

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brrandom.h"
#include "brglobalmemorymanager.h"

/*! ************************************

	\class Burger::Random
	\brief A simple random number generator.
	
	This class will create random numbers that
	are spread evenly across a 32 bit range.

	This algorithm uses a simple table and iterates over it
	using an index and bit rotation.

	\note It's permissible to make binary copies of this class.
	
***************************************/

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::Random,Burger::RandomBase);
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::Random::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/


/***************************************

	Default random number array

***************************************/

#if !defined(DOXYGEN)
static const Word32 g_DefaultArray[17] = {
	1571187604U,2130556662U,2075648113U,1384553414U,
	3758113950U,2350400989U,3768155391U,1438658665U,
	3424562190U,788898928U,107012447U,2497767687U,
	617416951U,3139554167U,2837196932U,224669655U,
	4159075602U
};
#endif


/*! ************************************

	\brief Static constructor.
	
	Initialize the random number generator
	with a call to SetSeed(Word32).
	
	\param uNewSeed New seed value
	\sa SetSeed(Word32).

***************************************/

Burger::Random::Random(Word32 uNewSeed)
{
	SetSeed(uNewSeed);
}

/*! ************************************

	\brief Create a new instance of Random.
	
	Create a new random number generator instance
	using the Burgerlib memory manager.
	
	\param uNewSeed Seed value for the random number generator
	\return Pointer to instance, or \ref NULL if out of memory.
	\sa SetSeed(Word32).

***************************************/

Burger::Random * BURGER_API Burger::Random::New(Word32 uNewSeed)
{
	Random *pResult = static_cast<Random *>(Alloc(sizeof(Random)));
	if (pResult) {
		new (pResult) Random(uNewSeed);
	}
	return pResult;
}

/***************************************

	\brief Return a 32 bit random number.
	
	Get a random number. Return a number between 0
	through (Range-1) inclusive.
	
	\return A random number in the specified range.

***************************************/

Word32 Burger::Random::Get(void)
{
	Word32 i = m_uIndex;	// Cache indexes
	Word32 j = i+5;
	if (j>=17U) {			// Don't exceed the buffer/
		j-=17U;
	}
	Word32 uNewVal = m_Array[i] + m_Array[j];	// Get the delta seed
	m_Array[i] = uNewVal;	// Save in array
	uNewVal += m_uState;		// Add to the base seed
	m_uState = uNewVal;		// Save the seed
	--i;
	if (static_cast<Int32>(i)<0) {	// Advance the indexes
		i = 16;
	}
	m_uIndex = i;		// Save in statics
	return uNewVal;
}

/***************************************

	\brief Seed the random number generator.
	
	Set the random number generator to a specific seed.
	This allows altering the random number flow in a
	controlled manner.
	
	\param uNewSeed 32 bit seed value.
	\sa Get(void).

***************************************/

void Burger::Random::SetSeed(Word32 uNewSeed)
{
	m_uSeed = uNewSeed;
	WordPtr i = 0;
	do {
		m_Array[i] = g_DefaultArray[i];
	} while (++i<BURGER_ARRAYSIZE(m_Array));

	m_uState = 0-uNewSeed;
	Word32 uCount = uNewSeed&0xF;
	m_uIndex = uCount;
	uCount = ((uNewSeed>>8)&0x1f)+1;
	do {
		Get();
	} while (--uCount);
}

