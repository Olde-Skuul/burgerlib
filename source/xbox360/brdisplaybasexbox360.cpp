/***************************************

	Display base class

	Xbox 360 version

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplay.h"

#if defined(BURGER_XBOX360)
#include "brdebug.h"
#include "brglobals.h"
#include "brglobalmemorymanager.h"
#include "brfixedpoint.h"
#define NONET
#include <xtl.h>

/***************************************

	Iterate over the displays and get the modes

***************************************/

Word Burger::Display::GetVideoModes(ClassArray<VideoCardDescription> *pOutput)
{
	pOutput->resize(1);

	// The XBox 360 is set to one mode by the user. Return the
	// single mode

	// Get the current mode
	XVIDEO_MODE VideoMode;
	MemoryClear(&VideoMode,sizeof(VideoMode));
	XGetVideoMode(&VideoMode);

	VideoCardDescription *pEntry = pOutput->GetPtr();
	// Use generic names for the details
	pEntry->m_DeviceName = "XBox 360";
	pEntry->m_MonitorName = "Monitor";
	pEntry->m_uDevNumber = 0;
	pEntry->m_bHardwareAccelerated = TRUE;
	// Append the single mode
	pEntry->m_Array.resize(1);

	// Get the mode the console is set to
	VideoMode_t *pMode = pEntry->m_Array.GetPtr();
	pMode->m_uWidth = VideoMode.dwDisplayWidth;
	pMode->m_uHeight = VideoMode.dwDisplayHeight;

	// Use FloatToInt to avoid LHS
	__stfiwx(__fctiw(VideoMode.RefreshRate),0,&pMode->m_uHertz);
	pMode->m_uDepth = 32;			// Always 32 bits
	pMode->m_uFlags = VIDEOMODE_HARDWARE;		// Nothing special

	// All done!
	return 0;
}

//
// Initialize the xbox 360 video defaults
//

void BURGER_API Burger::Display::InitGlobals(void)
{
	Globals_t *pGlobals = &g_Globals;
	if (!pGlobals->m_bInitialized) {
		// Read in the default display modes that the user has set.
		XVIDEO_MODE VideoMode;
		XGetVideoMode(&VideoMode);
		pGlobals->m_uDefaultWidth = VideoMode.dwDisplayWidth;
		pGlobals->m_uDefaultHeight = VideoMode.dwDisplayHeight;
		pGlobals->m_uDefaultDepth = 32;		// Always 32 bit
		pGlobals->m_uDefaultTotalWidth = VideoMode.dwDisplayWidth;
		pGlobals->m_uDefaultTotalHeight = VideoMode.dwDisplayHeight;
		pGlobals->m_uDefaultMonitorCount = 1;	// Only one monitor
		pGlobals->m_bIsWidescreen = VideoMode.fIsWideScreen;
		pGlobals->m_bIsHiDef = VideoMode.fIsHiDef;
		pGlobals->m_bIsInterlaced = VideoMode.fIsInterlaced;

		__stfiwx(__fctiw(VideoMode.RefreshRate),0,&pGlobals->m_uDefaultHertz);
		pGlobals->m_bInitialized = TRUE;
	}
}

#endif
