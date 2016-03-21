/***************************************

	Sound manager class

	Windows version

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brsound.h"

#if defined(BURGER_XBOX360)
#define NOD3D
#define NONET
#include <xtl.h>

#include "brglobals.h"
#include "brrunqueue.h"
#include "brendian.h"

/***************************************

	Init

***************************************/

Burger::SoundManager::SoundManager(GameApp *pGameApp) :
	m_pGameApp(pGameApp),
	m_uMaxVoices(cMaxVoiceCount),
	m_uVolume(cMaxVolume),
	m_bStereoAvailable(FALSE),
	m_uOutputSamplesPerSecond(22050)
{
}

/***************************************

	Init the sound manager

***************************************/

Word Burger::SoundManager::Init(void)
{
	Word uResult = 0;
	return uResult;
}

/***************************************

	Shut down the main sound manager

***************************************/

void Burger::SoundManager::Shutdown(void)
{
}



void Burger::SoundManager::SetVolume(Word uVolume)
{
}

Burger::SoundManager::Buffer::Buffer() :
	m_uPan(cPanCenter),
	m_uVolume(cMaxVolume)
{
}


void Burger::SoundManager::Buffer::Shutdown(void)
{
}

//
// Upload to DirectSound
//

Word Burger::SoundManager::Buffer::Upload(SoundManager *pSoundManager)
{
	return 10;
}

Burger::SoundManager::Voice::Voice()
{
}

#endif
