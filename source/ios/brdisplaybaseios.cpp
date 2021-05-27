/***************************************

    Display base class

    iOS version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplay.h"

#if defined(BURGER_IOS)
#include "brgameapp.h"
#include "brdebug.h"
#include "brglobals.h"
#include "brglobalmemorymanager.h"
#if !defined(DOXYGEN)
#define GL_GLEXT_PROTOTYPES
#define __BURGER__
#endif

#include "brgl.h"
#include "brglext.h"
#import <UIKit/UIKit.h>

//
// Get the default values
//

void BURGER_API Burger::Display::InitGlobals(void)
{
	if (!g_Globals.m_bInitialized) {

		//
		// Start by getting the screen size in POINTS
		//

		UIScreen *pMainScreen = [UIScreen mainScreen];
		CGRect ScreenRect = pMainScreen.bounds;

		//
		// Get the retina display scale factor
		//

		float fRetinaScale = [pMainScreen scale];
		g_Globals.m_fRetinaScale = fRetinaScale;

		// Get the TRUE display size in pixels
		uint_t uDisplayWidth = static_cast<uint_t>(static_cast<int>(ScreenRect.size.width*fRetinaScale));
		uint_t uDisplayHeight = static_cast<uint_t>(static_cast<int>(ScreenRect.size.height*fRetinaScale));

		// Save the size of the screen
		g_Globals.m_uDefaultWidth = uDisplayWidth;
		g_Globals.m_uDefaultHeight = uDisplayHeight;
		// LCD screens are always 32 bits deep and no scan rate
		g_Globals.m_uDefaultDepth = 32;
		g_Globals.m_uDefaultHertz = 0;

		g_Globals.m_uDefaultTotalWidth = uDisplayWidth;
		g_Globals.m_uDefaultTotalHeight = uDisplayHeight;

		NSArray *pScreens = [UIScreen screens];

		g_Globals.m_uDefaultMonitorCount = static_cast<uint_t>([pScreens count]);
		g_Globals.m_bInitialized = TRUE;
	}
}

/***************************************

	Given a device, iterate over the modes available and add them
	to the pOutput

***************************************/

static void GetResolutions(Burger::Display::VideoCardDescription *pOutput,UIScreen *pScreen)
{
	float fRetinaScale = Burger::Display::GetRetinaScale();
	for (UIScreenMode *pMode in [pScreen availableModes]) {
		Burger::Display::VideoMode_t Entry;
		Entry.m_uFlags = Burger::Display::VideoMode_t::VIDEOMODE_HARDWARE;
		
		// All LCDs
		Entry.m_uHertz = 0;		// Frequency is meaningless
		Entry.m_uDepth = 32;

		CGSize size = [pMode size];
		// If retina, insert the smaller resolution first
		if (fRetinaScale!=1.0f) {
			Entry.m_uWidth = static_cast<uint_t>(static_cast<int_t>(size.width/fRetinaScale));
			Entry.m_uHeight = static_cast<uint_t>(static_cast<int_t>(size.height/fRetinaScale));
			pOutput->m_Array.push_back(Entry);
		}
		// Pass the native resolution
		Entry.m_uWidth = static_cast<uint_t>(static_cast<int_t>(size.width));
		Entry.m_uHeight = static_cast<uint_t>(static_cast<int_t>(size.height));
		pOutput->m_Array.push_back(Entry);
	}
}

/***************************************

	Iterate over the displays and get the modes

***************************************/

uint_t Burger::Display::GetVideoModes(ClassArray<VideoCardDescription> *pOutput)
{
	pOutput->clear();
	uint_t uResult = 0;		// Assume success
	// Get the number of displays attached to this mac.
	// It CAN be zero

	uint_t uDisplayID = 0;
	for (UIScreen *pScreen in [UIScreen screens]) {
		VideoCardDescription Entry;
		Entry.m_uDevNumber = uDisplayID;
		Entry.m_uFlags = VideoCardDescription::VIDEOCARD_HARDWARE|VideoCardDescription::VIDEOCARD_PRIMARY;
		Entry.m_MonitorName = "LCD";
		Entry.m_DeviceName = "OpenGL";

		CGSize CurrentSize = [[pScreen currentMode] size];
		int iWidth = static_cast<int>(CurrentSize.width);
		int iHeight = static_cast<int>(CurrentSize.height);
		
		Entry.m_SystemRect.SetRight(iWidth);
		Entry.m_SystemRect.SetBottom(iHeight);
		Entry.m_CurrentResolution.SetRight(iWidth);
		Entry.m_CurrentResolution.SetBottom(iHeight);

		// Iterate the video modes
		GetResolutions(&Entry,pScreen);
		// Save the monitor
		pOutput->push_back(Entry);
		++uDisplayID;
	}

	return uResult;
}

#endif
