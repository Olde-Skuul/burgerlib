/***************************************

    Random number generator using Mersenne Twist (MT19937)

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brmersennetwist.h"
#include "brglobalmemorymanager.h"
#include "brwatcom.h"		// turn off some compiler warnings

/*! ************************************

	\class Burger::RandomMersenneTwist
	\brief Random number generator using Mersenne Twist (MT19937)
	
	This class will create random numbers that
	are spread evenly across a 32 bit range.

	https://en.wikipedia.org/wiki/Mersenne_Twister

	\note It's permissible to make binary copies of this class.
	
***************************************/

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::RandomMersenneTwist,Burger::RandomBase);

static const Word32 g_MagTable[2]= {0,0x9908B0DFU};

#endif


/*! ************************************

	\brief Static constructor.
	
	Initialize the random number generator
	with a call to SetSeed(Word32).
	
	\param uNewSeed New seed value
	\sa SetSeed(Word32).

***************************************/

Burger::RandomMersenneTwist::RandomMersenneTwist(Word32 uNewSeed)
{
	SetSeed(uNewSeed);
}

/*! ************************************

	\brief Create a new instance of RandomMersenneTwist.
	
	Create a new random number generator instance
	using the Burgerlib memory manager.
	
	\param uNewSeed Seed value for the random number generator
	\return Pointer to instance, or \ref NULL if out of memory.
	\sa SetSeed(Word32).

***************************************/

Burger::RandomMersenneTwist * BURGER_API Burger::RandomMersenneTwist::New(Word32 uNewSeed)
{
	RandomMersenneTwist *pResult = static_cast<RandomMersenneTwist *>(Alloc(sizeof(RandomMersenneTwist)));
	if (pResult) {
		new (pResult) RandomMersenneTwist(uNewSeed);
	}
	return pResult;
}




/*! ************************************

	\var const Burger::StaticRTTI Burger::RandomMersenneTwist::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/




/***************************************

	\brief Return a 32 bit random number.
	
	Get a random number. Return a number between 0
	through (Range-1) inclusive.
	
	\return A random number in the specified range.

***************************************/

Word32 Burger::RandomMersenneTwist::Get(void)
{
	Word32 uResult;

	// Are the numbers exhausted?
	if (m_uIndex >= cElements) {
		
		// Iterate until the tap
		Word32 uArrayIndex=0;
		do {
			uResult = (m_Array[uArrayIndex]&0x80000000U)|(m_Array[uArrayIndex+1]&0x7FFFFFFFU);
			m_Array[uArrayIndex] = m_Array[uArrayIndex+cTapIndex] ^ (uResult >> 1U) ^ g_MagTable[uResult & 1];
			++uArrayIndex;
		} while (uArrayIndex<(cElements-cTapIndex));

		// Iterate with negative tap
		do {
			uResult = (m_Array[uArrayIndex]&0x80000000U)|(m_Array[uArrayIndex+1]&0x7FFFFFFFU);
			m_Array[uArrayIndex] = m_Array[uArrayIndex+(static_cast<Int>(cTapIndex)-cElements)] ^ (uResult >> 1U) ^ g_MagTable[uResult & 1];
			++uArrayIndex;
		} while (uArrayIndex<(cElements-1));

		// Handle wrap around
		uResult = (m_Array[cElements-1]&0x80000000U)|(m_Array[0]&0x7FFFFFFFU);
		m_Array[cElements-1] = m_Array[cTapIndex-1] ^ (uResult >> 1U) ^ g_MagTable[uResult & 1];

		m_uIndex = 0;
	}

	// Fetch a value from the random array
	uResult = m_Array[m_uIndex++];

	// Temper the value
	uResult ^= (uResult >> 11U);
	uResult ^= (uResult << 7U) & 0x9D2C5680U;
	uResult ^= (uResult << 15U) & 0xEFC60000U;
	uResult ^= (uResult >> 18U);
	return uResult;
}

/***************************************

	\brief Seed the random number generator.
	
	Set the random number generator to a specific seed.
	This allows altering the random number flow in a
	controlled manner.
	
	\param uNewSeed 32 bit seed value.
	\sa Get(void).

***************************************/

void Burger::RandomMersenneTwist::SetSeed(Word32 uNewSeed)
{
	// Save the seed
	m_uSeed = uNewSeed;
	m_uIndex = cElements;

	// The seed can't be zero
	if (!uNewSeed) {
		uNewSeed = 1;
	}

	m_Array[0] = uNewSeed;
	Word32 i = 1;
	do {
		uNewSeed = (1812433253U * (uNewSeed ^ (uNewSeed >> 30U)) + i);
		m_Array[i] = uNewSeed;
	} while (++i<cElements);
}
