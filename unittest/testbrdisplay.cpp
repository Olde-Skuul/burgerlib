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

	Test the aspect ratio calculator

***************************************/

struct AspectRatioTest_t {
	uint32_t m_uWidth;
	uint32_t m_uHeight;
	Burger::Display::eAspectRatio m_uRatio;
	uint_t m_bExact;
};

static const AspectRatioTest_t g_AspectRatioTests[] = {
	{320, 240, Burger::Display::kAspectRatio4x3, TRUE},
	{640, 480, Burger::Display::kAspectRatio4x3, TRUE},
	{1400, 1050, Burger::Display::kAspectRatio4x3, TRUE},
	{1440, 1080, Burger::Display::kAspectRatio4x3, TRUE},
	{1600, 1200, Burger::Display::kAspectRatio4x3, TRUE},
	{1920, 1440, Burger::Display::kAspectRatio4x3, TRUE},
	{2048, 1536, Burger::Display::kAspectRatio4x3, TRUE},

	{1280, 720, Burger::Display::kAspectRatio16x9, TRUE},
	{1366, 768, Burger::Display::kAspectRatio16x9, FALSE},
	{1600, 900, Burger::Display::kAspectRatio16x9, TRUE},
	{1920, 1080, Burger::Display::kAspectRatio16x9, TRUE},
	{2560, 1440, Burger::Display::kAspectRatio16x9, TRUE},
	{3840, 2160, Burger::Display::kAspectRatio16x9, TRUE},
	{5120, 2880, Burger::Display::kAspectRatio16x9, TRUE},
	{7680, 4320, Burger::Display::kAspectRatio16x9, TRUE},

	{320, 200, Burger::Display::kAspectRatio16x10, TRUE},
	{640, 400, Burger::Display::kAspectRatio16x10, TRUE},
	{1280, 800, Burger::Display::kAspectRatio16x10, TRUE},
	{1920, 1200, Burger::Display::kAspectRatio16x10, TRUE},
	{2560, 1600, Burger::Display::kAspectRatio16x10, TRUE},

	{2560, 1080, Burger::Display::kAspectRatio21x9, FALSE},
	{3440, 1440, Burger::Display::kAspectRatio21x9, FALSE},
	{5120, 2160, Burger::Display::kAspectRatio21x9, FALSE},

	{3840, 1080, Burger::Display::kAspectRatio32x9, TRUE},
	{5120, 1440, Burger::Display::kAspectRatio32x9, TRUE}};

static uint_t BURGER_API TestAspectRatio(void) BURGER_NOEXCEPT
{
	// Test for exact matches
	uint_t uFailure = FALSE;
	const AspectRatioTest_t* pWork = g_AspectRatioTests;
	uintptr_t uCount = BURGER_ARRAYSIZE(g_AspectRatioTests);
	do {
		const Burger::Display::eAspectRatio uReturn =
			Burger::Display::get_aspect_ratio(
				pWork->m_uWidth, pWork->m_uHeight, TRUE);
		uint_t uTest = uReturn != pWork->m_uRatio;
		if (!pWork->m_bExact) {
			uTest = uTest == 0;
		}
		uFailure |= uTest;
		ReportFailure("Display::get_aspect_ratio(%u, %u, TRUE) = %u, expected %u",
			uTest, pWork->m_uWidth, pWork->m_uHeight,
			static_cast<uint_t>(uReturn), static_cast<uint_t>(pWork->m_uRatio));
		++pWork;
	} while (--uCount);

	// Test for inexact matches
	pWork = g_AspectRatioTests;
	uCount = BURGER_ARRAYSIZE(g_AspectRatioTests);
	do {
		const Burger::Display::eAspectRatio uReturn =
			Burger::Display::get_aspect_ratio(
				pWork->m_uWidth, pWork->m_uHeight, FALSE);
		const uint_t uTest = uReturn != pWork->m_uRatio;
		uFailure |= uTest;
		ReportFailure(
			"Display::get_aspect_ratio(%u, %u, FALSE) = %u, expected %u", uTest,
			pWork->m_uWidth, pWork->m_uHeight, static_cast<uint_t>(uReturn),
			static_cast<uint_t>(pWork->m_uRatio));
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Print the list of video modes

***************************************/

static uint_t BURGER_API TestGetVideoModes(void) BURGER_NOEXCEPT
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
			Burger::GUID_to_string(GUIDString, &pWork1->m_GUID);
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

static uint_t BURGER_API TestGetAudioModes(void) BURGER_NOEXCEPT
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

int BURGER_API TestBrDisplay(uint_t uVerbose) BURGER_NOEXCEPT
{
	// Assume no failures
	uint_t uResult = 0;

	if (uVerbose & VERBOSE_MSG) {
		Message("Running Display tests");
	}

	uResult = TestAspectRatio();

	if (uVerbose & VERBOSE_DISPLAY) {
		// List video modes
		uResult |= TestGetVideoModes();

		uResult |= TestGetAudioModes();
	}

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
