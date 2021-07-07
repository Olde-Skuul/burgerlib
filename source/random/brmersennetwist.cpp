/***************************************

	Random number generator using Mersenne Twist (MT19937)

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brmersennetwist.h"
#include "brglobalmemorymanager.h"

#if defined(BURGER_WATCOM) && !defined(DOXYGEN)
// Disable 'sizeof' operand contains compiler generated information
#pragma disable_message(549)
#endif

/*! ************************************

	\class Burger::RandomMersenneTwist
	\brief Random number generator using Mersenne Twist (MT19937)

	This class will create random numbers that are spread evenly across a 32 bit
	range.

	https://en.wikipedia.org/wiki/Mersenne_Twister

	\note It's permissible to make binary copies of this class.

***************************************/

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(
	Burger::RandomMersenneTwist, Burger::RandomBase);

static const uint32_t g_MagTable[2] = {0, 0x9908B0DFU};

#endif

/*! ************************************

	\brief Static constructor.

	Initialize the random number generator with a call to SetSeed(uint32_t).

	\param uNewSeed New seed value
	\sa SetSeed(uint32_t).

***************************************/

Burger::RandomMersenneTwist::RandomMersenneTwist(
	uint32_t uNewSeed) BURGER_NOEXCEPT
{
	SetSeed(uNewSeed);
}

/*! ************************************

	\brief Create a new instance of RandomMersenneTwist.

	Create a new random number generator instance using the Burgerlib memory
	manager.

	\param uNewSeed Seed value for the random number generator
	\return Pointer to instance, or \ref NULL if out of memory.
	\sa SetSeed(uint32_t).

***************************************/

Burger::RandomMersenneTwist* BURGER_API Burger::RandomMersenneTwist::New(
	uint32_t uNewSeed) BURGER_NOEXCEPT
{
	RandomMersenneTwist* pResult =
		static_cast<RandomMersenneTwist*>(Alloc(sizeof(RandomMersenneTwist)));
	if (pResult) {
		new (pResult) RandomMersenneTwist(uNewSeed);
	}
	return pResult;
}

/*! ************************************

	\var const Burger::StaticRTTI Burger::RandomMersenneTwist::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a reference to the parent

***************************************/

/***************************************

	\brief Return a 32 bit random number.

	Get a random number. Return a number between 0 through (Range-1) inclusive.

	\return A random number in the specified range.

***************************************/

uint32_t Burger::RandomMersenneTwist::Get(void) BURGER_NOEXCEPT
{
	uint32_t uResult;

	// Are the numbers exhausted?
	if (m_uIndex >= kElements) {

		// Iterate until the tap
		uint32_t uArrayIndex = 0;
		do {
			uResult = (m_Array[uArrayIndex] & 0x80000000U) |
				(m_Array[uArrayIndex + 1] & 0x7FFFFFFFU);
			m_Array[uArrayIndex] = m_Array[uArrayIndex + kTapIndex] ^
				(uResult >> 1U) ^ g_MagTable[uResult & 1];
			++uArrayIndex;
		} while (uArrayIndex < (kElements - kTapIndex));

		// Iterate with negative tap
		do {
			uResult = (m_Array[uArrayIndex] & 0x80000000U) |
				(m_Array[uArrayIndex + 1] & 0x7FFFFFFFU);
			m_Array[uArrayIndex] = m_Array[static_cast<int32_t>(uArrayIndex) +
									   (static_cast<int_t>(kTapIndex) -
										   static_cast<int_t>(kElements))] ^
				(uResult >> 1U) ^ g_MagTable[uResult & 1];
			++uArrayIndex;
		} while (uArrayIndex < (kElements - 1));

		// Handle wrap around
		uResult =
			(m_Array[kElements - 1] & 0x80000000U) | (m_Array[0] & 0x7FFFFFFFU);
		m_Array[kElements - 1] =
			m_Array[kTapIndex - 1] ^ (uResult >> 1U) ^ g_MagTable[uResult & 1];

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
	This allows altering the random number flow in a controlled manner.

	\param uNewSeed 32 bit seed value.
	\sa Get(void).

***************************************/

void Burger::RandomMersenneTwist::SetSeed(uint32_t uNewSeed) BURGER_NOEXCEPT
{
	// Save the seed
	m_uSeed = uNewSeed;
	m_uIndex = kElements;

	// The seed can't be zero
	if (!uNewSeed) {
		uNewSeed = 1;
	}

	m_Array[0] = uNewSeed;
	uint32_t i = 1;
	do {
		uNewSeed = (1812433253U * (uNewSeed ^ (uNewSeed >> 30U)) + i);
		m_Array[i] = uNewSeed;
	} while (++i < kElements);
}
