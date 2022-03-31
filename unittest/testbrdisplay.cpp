/***************************************

	Unit tests for the display manager

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrdisplay.h"
#include "brarray.h"
#include "brdebug.h"
#include "brdisplay.h"
#include "brguid.h"
#include "brmemoryansi.h"
#include "brsound.h"
#include "common.h"

/***************************************

	Print the list of video modes

***************************************/

static uint_t BURGER_ANSIAPI TestGetVideoModes(void) BURGER_NOEXCEPT
{
	Burger::ClassArray<Burger::Display::VideoCardDescription> Modes;
	uint_t uFailure = Burger::Display::GetVideoModes(&Modes);

	ReportFailure(
		"Burger::Display::GetVideoModes() didn't return zero.", uFailure);
	uintptr_t uCount = Modes.size();
	if (uCount) {
		const Burger::Display::VideoCardDescription* pWork1 = Modes.GetPtr();
		do {
			///< (Windows only) Device GUID
#if defined(BURGER_WINDOWS)
			char GUIDString[64];
			Burger::GUIDToString(GUIDString, &pWork1->m_GUID);
			Message("GUID = %s", GUIDString);
#endif
			Message("Device = %s", pWork1->m_DeviceName.c_str());
			Message("Monitor = %s", pWork1->m_MonitorName.c_str());
			Message("Device number = %u, Flags = %u", pWork1->m_uDevNumber,
				pWork1->m_uFlags);
			Message("SystemRect = %d,%d,%d,%d", pWork1->m_SystemRect.m_iLeft,
				pWork1->m_SystemRect.m_iTop, pWork1->m_SystemRect.m_iRight,
				pWork1->m_SystemRect.m_iBottom);
			Message("CurrentRect = %d,%d,%d,%d",
				pWork1->m_CurrentResolution.m_iLeft,
				pWork1->m_CurrentResolution.m_iTop,
				pWork1->m_CurrentResolution.m_iRight,
				pWork1->m_CurrentResolution.m_iBottom);
			uintptr_t uCount2 = pWork1->m_Array.size();
			if (uCount2) {
				const Burger::Display::VideoMode_t* pMode =
					pWork1->m_Array.GetPtr();
				do {
					Message(
						"Width %u, Height %u, Depth %u, Hertz %u, Flags 0x%08X",
						pMode->m_uWidth, pMode->m_uHeight, pMode->m_uDepth,
						pMode->m_uHertz, pMode->m_uFlags);
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

static uint_t BURGER_ANSIAPI TestGetAudioModes(void) BURGER_NOEXCEPT
{
	Burger::ClassArray<Burger::SoundManager::SoundCardDescription> SoundModes;
	uint_t uFailure = Burger::SoundManager::GetAudioModes(&SoundModes);

	ReportFailure(
		"Burger::SoundManager::GetAudioModes() didn't return zero.", uFailure);
	uintptr_t uCount = SoundModes.size();
	if (uCount) {
		const Burger::SoundManager::SoundCardDescription* pWork1 =
			SoundModes.GetPtr();
		do {
			Message("Device = %s", pWork1->m_DeviceName.c_str());
			Message(
				"Device number = %u, Hardware = %u, Min Rate %u, Max Rate %u,\n"
				"8 Bit = %u, 16 bit = %u, Stereo = %u",
				pWork1->m_uDevNumber, pWork1->m_bHardwareAccelerated,
				pWork1->m_uMinimumSampleRate, pWork1->m_uMaximumSampleRate,
				pWork1->m_b8Bit, pWork1->m_b16Bit, pWork1->m_bStereo);
			++pWork1;
		} while (--uCount);
	}
	return uFailure;
}

//
// Perform all the tests for the Burgerlib Endian Manager
//

int BURGER_API TestBrDisplay(void) BURGER_NOEXCEPT
{
	uint_t uResult; // Assume no failures
	Message("Running Display tests");
	// Test compiler switches
	uResult = TestGetVideoModes();

	uResult |= TestGetAudioModes();

	return static_cast<int>(uResult);
}

/***************************************

	Test quick dialogs (And test for UTF-8 text)

***************************************/

#define SAILORMOON \
	"\xE7\xBE\x8E\xE5\xB0\x91\xE5\xA5\xB3\xE6\x88" \
	"\xA6\xE5\xA3\xAB\xE3\x82\xBB\xE3\x83\xBC\xE3\x83\xA9\xE3\x83\xBC" \
	"\xE3\x83\xA0\xE3\x83\xBC\xE3\x83\xB3"

int BURGER_API TestBrDialogs(void) BURGER_NOEXCEPT
{
	Burger::OkAlertMessage("OkAlertMessage test", SAILORMOON);
	Burger::OkCancelAlertMessage(
		"OkCancelAlertMessage test" SAILORMOON, "Fake Title");
	return 0;
}
