/***************************************

	Sound manager class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brsound.h"

/*! ************************************

	\struct Burger::WAVEFORMATEX

	Structure describing the header of a *.WAV file

***************************************/

/*! ************************************

	\class Burger::Sound

	This class manages sound output to the application's speakers
	
	All of the underlying code is system specific

***************************************/

/*! ************************************

	\brief Release all resources

	Shut down the Sound class by calling Shutdown() to
	release all of the resources.

	\sa Shutdown()

***************************************/

Burger::Sound::~Sound()
{
	Shutdown();
}

#if !defined(BURGER_WINDOWS) && !defined(BURGER_XBOX360)
void Burger::Sound::Shutdown(void)
{
}

Burger::Sound::Voice::~Voice()
{
}

#endif