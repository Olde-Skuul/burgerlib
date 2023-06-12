/***************************************

	Start up code for games based applications

	MacOS Classic / Carbon

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSTARTUP_H__
#define __BRSTARTUP_H__

#ifndef __BURGER__
#include <burger.h>
#endif

//
// Initialize defaults
//

#if !defined(GAMENAME)
#define GAMENAME "Test Application"
#endif

#if !defined(MEMORYSIZE)
#define MEMORYSIZE Burger::MemoryManagerHandle::DEFAULTMEMORYCHUNK
#endif

#if !defined(HANDLECOUNT)
#define HANDLECOUNT Burger::MemoryManagerHandle::DEFAULTHANDLECOUNT
#endif

#if !defined(MINIMUMRESERVE)
#define MINIMUMRESERVE Burger::MemoryManagerHandle::DEFAULTMINIMUMRESERVE
#endif

/***************************************

	This is the main entry point for a Mac OS Carbon version of the game

***************************************/

#ifndef __GESTALT__
#include <Gestalt.h>
#endif
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

/***************************************

	Validate the System Software
	Macintosh specific
	This must be user modified since it may force you to include
	libraries you really don't care about

***************************************/

static int ValidateMacSystem(void)
{
	const char* pErrorText = NULL;
	long gestaltAnswer;
	int iResult = 0;

	// Is Gestalt available?

	if (Gestalt(gestaltVersion, &gestaltAnswer) != noErr) {
		pErrorText = "Gestalt not present, requires system 7.5.3 or later.";
	} else

		// Check system version

		if (Burger::MacOS::get_os_version() < 0x0753) {
			pErrorText = "Requires system 7.5.3 or later to run";
		} else

			// Check for QuickTime

			if (Burger::MacOS::get_quicktime_version() < 0x0201) {
				pErrorText = "Requires Quicktime 2.0 later to run";
			} else

			// Check for Sound Manager 3.0 or later

#if defined(BURGER_POWERPC)
				if (Burger::MacOS::get_draw_sprocket_version() < 0x175) {
				pErrorText = "Requires DrawSprocket 1.7.5 or later to run";
			} else

				// Check for Sound Manager 3.0 or later

				if ((Burger::MacOS::get_os_version() < 0x9FF) &&
					(Burger::MacOS::get_input_sprocket_version() < 0x173)) {
					pErrorText =
						"Requires Input Sprocket 1.7.3 or later to run";
				} else
#endif

					// Check for Sound Manager 3.0 or later

					if (Burger::MacOS::get_sound_manager_version() < 0x300) {
						pErrorText =
							"Requires Sound manager 3.2.1 or later to run";
					} else

					// Check the screen size

#if !TARGET_API_MAC_CARBON
						if ((qd.screenBits.bounds.bottom < 480) ||
							(qd.screenBits.bounds.right < 640)) {
						pErrorText =
							"Requires a video display of 640 by 480 pixels or larger to run";
					} else
#endif

						// Check the free memory amount

						if (TempFreeMem() < 500 * 1024) {
							if (!OkCancelAlertMessage(
									"You are very low on memory, do you wish to continue?",
									GAMENAME)) {
								iResult = 10;
							}
						}

	// Was an error message generated?
	if (pErrorText) {
		// Display the error and quit
		Burger::OkAlertMessage(pErrorText, GAMENAME);
		iResult = 10;
	}
	return iResult;
}

/***************************************

	Validate the System Software
	Macintosh specific
	This must be user modified since it may force you to include
	libraries you really don't care about

***************************************/

extern "C" int main(void);

int main(void)
{
	// Create an application instance

	Burger::MacApp MyApp(MEMORYSIZE, HANDLECOUNT, MINIMUMRESERVE);

	// Error on startup?
	int iResult = MyApp.GetReturnCode();

	if (!iResult) {
		// Verify the tools are the minimum version
		iResult = ValidateMacSystem();
		if (!iResult) {
			iResult = CodeEntry(&MyApp);
		}
	}
	return iResult;
}

#endif
