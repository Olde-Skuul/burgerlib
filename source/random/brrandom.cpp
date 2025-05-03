/***************************************

	Random number generator

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brrandom.h"
#include "brglobalmemorymanager.h"

#if defined(BURGER_WATCOM)
// Disable 'sizeof' operand contains compiler generated information
#pragma disable_message(549)
#endif

/*! ************************************

	\class Burger::Random
	\brief A simple random number generator.

	This class will create random numbers that are spread evenly across a 32 bit
	range.

	This algorithm uses a simple table and iterates over it using an index and
	bit rotation.

	\note It's permissible to make binary copies of this class.

***************************************/

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::Random, Burger::RandomBase);
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::Random::g_StaticRTTI

	\brief The global description of the class

	This record contains the name of this class and a reference to the parent

***************************************/

/***************************************

	Default random number array

***************************************/

#if !defined(DOXYGEN)
static const uint32_t g_DefaultArray[17] = {1571187604U, 2130556662U,
	2075648113U, 1384553414U, 3758113950U, 2350400989U, 3768155391U,
	1438658665U, 3424562190U, 788898928U, 107012447U, 2497767687U, 617416951U,
	3139554167U, 2837196932U, 224669655U, 4159075602U};
#endif

/*! ************************************

	\brief Static constructor.

	Initialize the random number generator with a call to set_seed(uint32_t).

	\param uNewSeed New seed value

	\sa set_seed(uint32_t).

***************************************/

Burger::Random::Random(uint32_t uNewSeed) BURGER_NOEXCEPT
{
	set_seed(uNewSeed);
}

/*! ************************************

	\brief Create a new instance of Random.

	Create a new random number generator instance using the Burgerlib memory
	manager.

	\param uNewSeed Seed value for the random number generator

	\return Pointer to instance, or \ref nullptr if out of memory.

	\sa set_seed(uint32_t).

***************************************/

Burger::Random* BURGER_API Burger::Random::new_object(
	uint32_t uNewSeed) BURGER_NOEXCEPT
{
	Random* pResult = static_cast<Random*>(allocate_memory(sizeof(Random)));
	if (pResult) {
		new (pResult) Random(uNewSeed);
	}
	return pResult;
}

/***************************************

	\brief Return a 32 bit random number.

	Get a random number. Return a number between 0 through (Range-1) inclusive.

	\return A random number in the specified range.

***************************************/

uint32_t Burger::Random::get(void) BURGER_NOEXCEPT
{
	// Cache indexes
	uint32_t i = m_uIndex;
	uint32_t j = i + 5;

	// Don't exceed the buffer
	if (j >= BURGER_ARRAYSIZE(m_Array)) {
		j -= static_cast<uint32_t>(BURGER_ARRAYSIZE(m_Array));
	}

	// Get the delta seed
	uint32_t uNewVal = m_Array[i] + m_Array[j];

	// Save in array
	m_Array[i] = uNewVal;

	// Add to the base seed
	uNewVal += m_uState;

	// Save the seed
	m_uState = uNewVal;
	--i;

	// Advance the indexes
	if (static_cast<int32_t>(i) < 0) {
		i = static_cast<uint32_t>(BURGER_ARRAYSIZE(m_Array) - 1);
	}

	// Save in statics
	m_uIndex = i;
	return uNewVal;
}

/***************************************

	\brief Seed the random number generator.

	Set the random number generator to a specific seed. This allows altering the
	random number flow in a controlled manner.

	\param uNewSeed 32 bit seed value.

	\sa get(void).

***************************************/

void Burger::Random::set_seed(uint32_t uNewSeed) BURGER_NOEXCEPT
{
	m_uSeed = uNewSeed;
	uintptr_t i = 0;
	do {
		m_Array[i] = g_DefaultArray[i];
	} while (++i < BURGER_ARRAYSIZE(m_Array));

	m_uState = 0 - uNewSeed;

	// Blend starting from 0-15
	uint32_t uCount = uNewSeed & 0xFU;
	m_uIndex = uCount;

	// 1-32 blends
	uCount = ((uNewSeed >> 8U) & 0x1FU) + 1U;
	do {
		get();
	} while (--uCount);
}
