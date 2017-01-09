/***************************************

	Unit tests for the Date/Time manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "testbrdisplay.h"
#include "common.h"
#include "brdisplay.h"
#include "brarray.h"
#include "brmemoryansi.h"
#include "brsound.h"
#include "brguid.h"

/***************************************

	Print the list of video modes

***************************************/

static Word TestGetVideoModes(void)
{
	Burger::ClassArray<Burger::Display::VideoCardDescription> Modes;
	Word uFailure = Burger::Display::GetVideoModes(&Modes);

	ReportFailure("Burger::Display::GetVideoModes() didn't return zero.",uFailure);
	WordPtr uCount = Modes.size();
	if (uCount) {
		const Burger::Display::VideoCardDescription *pWork1 = Modes.GetPtr();
		do {
			///< (Windows only) Device GUID
#if defined(BURGER_WINDOWS)
			char GUIDString[64];
			Burger::GUIDToString(GUIDString,&pWork1->m_GUID);
			Message("GUID = %s",GUIDString);
#endif
			Message("Device = %s",pWork1->m_DeviceName.GetPtr());
			Message("Monitor = %s",pWork1->m_MonitorName.GetPtr());
			Message("Device number = %u, Flags = %u",pWork1->m_uDevNumber,pWork1->m_uFlags);
			Message("SystemRect = %d,%d,%d,%d",pWork1->m_SystemRect.m_iLeft,pWork1->m_SystemRect.m_iTop,pWork1->m_SystemRect.m_iRight,pWork1->m_SystemRect.m_iBottom);
			Message("CurrentRect = %d,%d,%d,%d",pWork1->m_CurrentResolution.m_iLeft,pWork1->m_CurrentResolution.m_iTop,pWork1->m_CurrentResolution.m_iRight,pWork1->m_CurrentResolution.m_iBottom);
			WordPtr uCount2 = pWork1->m_Array.size();
			if (uCount2) {
				const Burger::Display::VideoMode_t *pMode = pWork1->m_Array.GetPtr();
				do {
					Message("Width %u, Height %u, Depth %u, Hertz %u, Flags 0x%08X",pMode->m_uWidth,pMode->m_uHeight,
						pMode->m_uDepth,pMode->m_uHertz,pMode->m_uFlags);
					++pMode;
				} while (--uCount2);
			}
			++pWork1;
		} while (--uCount);
	}
	return uFailure;
}

/***************************************

	Print the list of audio modes

***************************************/

static Word TestGetAudioModes(void)
{
	Burger::ClassArray<Burger::SoundManager::SoundCardDescription> SoundModes;
	Word uFailure = Burger::SoundManager::GetAudioModes(&SoundModes);

	ReportFailure("Burger::SoundManager::GetAudioModes() didn't return zero.",uFailure);
	WordPtr uCount = SoundModes.size();
	if (uCount) {
		const Burger::SoundManager::SoundCardDescription *pWork1 = SoundModes.GetPtr();
		do {
			Message("Device = %s",pWork1->m_DeviceName.GetPtr());
			Message("Device number = %u, Hardware = %u, Min Rate %u, Max Rate %u,\n"
				"8 Bit = %u, 16 bit = %u, Stereo = %u",
				pWork1->m_uDevNumber,pWork1->m_bHardwareAccelerated,pWork1->m_uMinimumSampleRate,pWork1->m_uMaximumSampleRate,
				pWork1->m_b8Bit,pWork1->m_b16Bit,pWork1->m_bStereo);
			++pWork1;
		} while (--uCount);
	}
	return uFailure;
}

//
// Perform all the tests for the Burgerlib Endian Manager
//

int BURGER_API TestBrDisplay(void)
{	
	Burger::MemoryManagerGlobalANSI Memory;

	Word uResult;	// Assume no failures
	Message("Running Display tests");
	// Test compiler switches
	uResult = TestGetVideoModes();

	uResult |= TestGetAudioModes();

	return static_cast<int>(uResult);
}
