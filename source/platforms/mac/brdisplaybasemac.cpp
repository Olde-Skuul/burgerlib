/***************************************

	Display base class

	Mac version

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplay.h"

#if defined(BURGER_MAC)
#include "brdebug.h"
#include "brgameapp.h"
#include "brglobalmemorymanager.h"
#include "brglobals.h"
#include "brmemoryfunctions.h"
#include "mac_sprockets.h"
#include <DrawSprocket.h>

/***************************************

	Given a device, iterate over the modes available and add them
	to the pOutput

***************************************/

static void BURGER_API GetModes(
	Burger::Display::VideoCardDescription* pOutput, DisplayIDType MyDevID)
{
	// Only available with DrawSprocket
#if defined(BURGER_CFM)
	DSpContextReference MyRef;

	// Get the first context
	DSpGetFirstContext(MyDevID, &MyRef);

	// Valid reference
	if (MyRef) {
		do {
			// Space for the attributes
			DSpContextAttributes MyAttr;
			Burger::MemoryClear(&MyAttr, sizeof(MyAttr));
			if (!DSpContext_GetAttributes(MyRef, &MyAttr)) {

				// If 0x10 is set, it's a stretched context, skip it!
				if (!(MyAttr.contextOptions & 0x10U)) {

					// Video mode found
					Burger::Display::VideoMode_t MyEntry;

					// Save the pixel width, height, depth
					MyEntry.m_uWidth = MyAttr.displayWidth;
					MyEntry.m_uHeight = MyAttr.displayHeight;
					uint_t uDepth = MyAttr.displayBestDepth;
					MyEntry.m_uDepth = uDepth;

					uint_t uFlags = 0;
					uint_t uHertz =
						static_cast<uint_t>(MyAttr.frequency) >> 16U;
					if (uHertz) {
						uFlags |= Burger::Display::VideoMode_t::
							VIDEOMODE_REFRESHVALID;
					}
					MyEntry.m_uHertz = uHertz;

#if defined(BURGER_POWERPC)
					if (uDepth == 16) {
						uFlags |=
							Burger::Display::VideoMode_t::VIDEOMODE_HARDWARE;
					} else if (uDepth == 32) {
						uFlags |=
							Burger::Display::VideoMode_t::VIDEOMODE_HARDWARE;
					}
#endif
					MyEntry.m_uFlags = uFlags;
					pOutput->m_Array.push_back(MyEntry);
				}
			}
			// Next context
			if (DSpGetNextContext(MyRef, &MyRef)) {
				break; // Error??
			}
			// All done?
		} while (MyRef);
	}
#endif
}

/***************************************

	Iterate over the displays and get the modes

***************************************/

uint_t BURGER_API Burger::Display::GetVideoModes(
	ClassArray<VideoCardDescription>* pOutput)
{
	pOutput->clear();

	uint_t uResult = 0; // Assume success

						// Only available with DrawSprocket
#if defined(BURGER_CFM)

	// Draw sprocket linked in?
	if (MacOS::start_draw_sprocket()) {

		// Get the first active device
		GDHandle ppDevice = GetDeviceList();
		if (ppDevice) {
			uint_t uDevNumber = 0;
			do {
				// Get the device ID
				DisplayIDType MyDevID;
				if (!DMGetDisplayIDByGDevice(ppDevice, &MyDevID, TRUE)) {

					VideoCardDescription Entry;
					Entry.m_uDevNumber = uDevNumber;
					Entry.m_DeviceName = "OpenGL";
					Entry.m_MonitorName = "Monitor";

					// Get the location of the monitor
					Entry.m_SystemRect.Set(&(*ppDevice)->gdRect);
					Entry.m_CurrentResolution.SetRight(
						Entry.m_SystemRect.GetWidth());
					Entry.m_CurrentResolution.SetBottom(
						Entry.m_SystemRect.GetHeight());

					// Get the modes available
					GetModes(&Entry, MyDevID);

					// Save the monitor
					pOutput->push_back(Entry);
				}

				// Next device in the chain
				++uDevNumber;
				ppDevice = GetNextDevice(ppDevice);

				// More?
			} while (ppDevice);
		}
	}
#endif
	return uResult;
}

#endif
