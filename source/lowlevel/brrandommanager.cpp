/***************************************

	Random number generator

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brrandommanager.h"
#include "brtick.h"
#include "brstringfunctions.h"

/*! ************************************

	\class Burger::Random
	\brief A simple random number generator.
	
	This class will create random numbers that
	are spread evenly across a 32 bit range.
	
	Functions exist that will pre-bake numbers
	into specific ranges like Get() and
	GetSigned().

	\note It's permissible to make binary copies of this class.
	
***************************************/

/***************************************

	Default random number array

***************************************/

static const Word32 DefaultArray[17] = {
	1571187604U,2130556662U,2075648113U,1384553414U,
	3758113950U,2350400989U,3768155391U,1438658665U,
	3424562190U,788898928U,107012447U,2497767687U,
	617416951U,3139554167U,2837196932U,224669655U,
	4159075602U
};

/*! ************************************

	\fn Burger::Random::Random()
	\brief Static constructor.
	
	All this does is init the random number generator
	with a call to Init().
	
	\sa Init() and SetSeed().

***************************************/

/*! ************************************

	\fn Burger::Random::Delete()
	\brief Delete memory allocated by New().
	
	If you create an instance via a call to New(), use
	this function to dispose of it.
	
	\sa Random() and New().

***************************************/

/*! ************************************

	\brief Create a new instance of Random.
	
	Create a new random number generator instance
	using the Burgerlib memory manager.
	
	\param uNewSeed A seed value for tighter control. Passed to SetSeed().
	\return Pointer to instance, or \ref NULL if out of memory.
	\sa Delete() and SetSeed().

***************************************/

Burger::Random * Burger::Random::New(Word32 uNewSeed)
{
	Random *pRandom = reinterpret_cast<Random *>(Alloc(sizeof(Random)));
	if (pRandom) {
		pRandom->Init();					// Init the data
		if (uNewSeed) {
			pRandom->SetSeed(uNewSeed);		// Set a new seed 
		}
	}
	return pRandom;							// Return the instance
}

/*! ************************************

	\brief Reset to a default state.
	
	Init the random number generator to a KNOWN state.
	This will allow games to record just the joystick
	movements and have random actions repeat for demo playback.
	
	\sa RandomInit().

***************************************/

void Burger::Random::Init(void)
{
	m_uSeed = 728056387U;		// Set the seed
	m_uIndex = 0;				// Index
	MemoryCopy(m_Array,DefaultArray,sizeof(DefaultArray));
}

/*! ************************************

	\brief Reset to a random state.

	Init the random number generator with an "Anything goes"
	policy so programs will power up in an unknown state.
	Do NOT use this if you wish your title to have recordable demos.

	This function will use a formula that assumes that the tick
	timer runs at a constant time base but the machine in question
	does not. As a result. The number of times Get() is
	called is anyone's guess.
	
	\sa Init().

***************************************/

void Burger::Random::RandomInit(void)
{
	Init();								// Init the structure
	Word32 uTickMark = Tick::Read();		// Get a current tick mark
	do {
		Get();							// Discard a number from the stream
	} while (Tick::Read()==uTickMark);	// Same time?
}

/*! ************************************

	\brief Return a 32 bit random number.
	
	Get a random number. Return a number between 0
	through (Range-1) inclusive.
	
	\param uRange 0 means return 32 bits as is, anything else
		means return 0 through (Range-1) inclusive.
	\return A random number in the specified range.

***************************************/

Word32 Burger::Random::Get(Word32 uRange)
{
	Word32 i = m_uIndex;		/* Cache indexs */
	Word32 j = i+5;
	if (j>=17) {		/* Don't exceed the buffer */
		j-=17;
	}
	Word32 uNewVal = m_Array[i] + m_Array[j];	/* Get the delta seed */
	m_Array[i] = uNewVal;	/* Save in array */
	uNewVal += m_uSeed;		/* Add to the base seed */
	m_uSeed = uNewVal;		/* Save the seed */
	--i;
	if (static_cast<int>(i)<0) {			/* Advance the indexs */
		i = 16;
	}
	m_uIndex = i;		/* Save in statics */
	if (!uRange) {
		return uNewVal;
	}
	if (uRange<=0x10000) {
		uNewVal&=0xFFFFU;		/* Make sure they are shorts! */
		return ((uNewVal*uRange)>>16U);
	}
	uNewVal%=uRange;
	return uNewVal;
}

/*! ************************************

	\brief Seed the random number generator.
	
	Set the random number generator to a specific seed.
	This allows altering the random number flow in a
	controlled manner.
	
	\param uNewSeed 32 bit seed value.
	\sa Init().

***************************************/

void Burger::Random::SetSeed(Word32 uNewSeed)
{
	MemoryCopy(m_Array,DefaultArray,sizeof(DefaultArray));
	m_uSeed = 0-uNewSeed;
	Word i = uNewSeed&0xF;
	m_uIndex = i;
	i = ((uNewSeed>>8)&0x1f)+1;
	do {
		Get();
	} while (--i);
}

/*! ************************************

	\brief Return a signed value in a specific range.
	
	Return a random number between -Range and +Range (Inclusive)
	and it's a SIGNED value.
	If Range = 3, then the value returned is -2 to 2 inclusive.
	0, and numbers higher than 0x7FFFFFFFU are illegal.
	
	\param uRange Range from 1 to MAX_INT-1.
	\return Signed value from -uRange to uRange (Inclusive)
	\sa Get() and GetFloat().

***************************************/

Int32 Burger::Random::GetSigned(Word32 uRange)
{
	return static_cast<Int32>(Get(uRange<<1U)-uRange);		/* Get the random number */
}

/*! ************************************

	\brief Return a float from 0.0f to 0.99999f.
	
	Returns a random number in the range of 0.0f
	to 0.999999f. The numbers are spread evenly.
	
	\return Random float from 0.0 to 0.9999999999f
	\sa GetSigned() and Get().

***************************************/

float Burger::Random::GetFloat(void)
{
	Int32 Value = static_cast<Int32>(Get())&0x7FFFFFFF;		// Max 32 bit int
	// Convert to float
	return static_cast<float>(Value)*
		(1.0f/static_cast<float>(0x80000000U));
}
