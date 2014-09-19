/***************************************

	Start up code for games based applications
	
	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BURGER__
#include <burger.h>
#endif

//
// Define this function in your application
//

extern int BURGER_API CodeEntry(Burger::GameApp *pApp);

using namespace Burger;

//
// Initialize defaults
//

#if !defined(GAMENAME)
#define GAMENAME "Test Application"
#endif

#if !defined(WINDOWS_CALLBACK)
#define WINDOWS_CALLBACK NULL
#endif

#if !defined(MEMORYSIZE)
#define MEMORYSIZE MemoryManagerHandle::DEFAULTMEMORYCHUNK
#endif

#if !defined(HANDLECOUNT)
#define HANDLECOUNT MemoryManagerHandle::DEFAULTHANDLECOUNT
#endif

#if !defined(MINIMUMRESERVE)
#define MINIMUMRESERVE MemoryManagerHandle::DEFAULTMINIMUMRESERVE
#endif

/***************************************

	This is the main entry point for a windows version of the game

***************************************/

#if defined(BURGER_WINDOWS)
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE /* hPrevInstance */,char * /* lpCmdLine */,int /* nCmdShow */);

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE /* hPrevInstance */,char * /* lpCmdLine */,int /* nCmdShow */)
{
	int iResult = 0;

// Singular instance enabled?
#if defined(GAMELOCK)
	DetectMultiLaunch OneShot;
	if (!OneShot.IsMultiLaunched(GAMELOCK)) {
#endif

		// Create the window for the game

		WindowsApp MyApp(hInstance,GAMENAME,WINDOWS_CALLBACK,MEMORYSIZE,HANDLECOUNT,MINIMUMRESERVE);
		iResult = Globals::GetErrorCode();
		if (!iResult) {
		
		// Run the app
			iResult = CodeEntry(&MyApp);
		}

// Needed to properly close for the lock
#if defined(GAMELOCK)
	}
#endif
	return iResult;
}

/***************************************

	This is the main entry point for a Mac OS Carbon version of the game

***************************************/

#elif defined(BURGER_MAC)

#include <Gestalt.h>
#include <MacMemory.h>
#include <Quickdraw.h>

/***************************************

	Validate the System Software
	Macintosh specific
	This must be user modified since it may force you to include
	libraries you really don't care about

***************************************/

static int ValidateMacSystem(void) 
{
	const char *pErrorText=NULL;
	long gestaltAnswer;
	int iResult = 0;

	// Is Gestalt available?

	if (Gestalt(gestaltVersion,&gestaltAnswer)!=noErr) {
		pErrorText = "Gestalt not present, requires system 7.5.3 or later.";
	} else

	// Check system version

	if (Mac::GetOSVersion() < 0x0753) {
		pErrorText = "Requires system 7.5.3 or later to run";
	} else

	// Check for QuickTime

	if (Mac::GetQuickTimeVersion() < 0x0201) {
		pErrorText = "Requires Quicktime 2.0 later to run";
	} else

	// Check for Sound Manager 3.0 or later

#if defined(BURGER_POWERPC)
	if (Mac::GetDrawSprocketVersion() < 0x175) {
		pErrorText = "Requires DrawSprocket 1.7.5 or later to run";
	} else

	// Check for Sound Manager 3.0 or later

	if ((Mac::GetOSVersion()<0x9FF) && (Mac::GetInputSprocketVersion() < 0x173)) {
		pErrorText = "Requires Input Sprocket 1.7.3 or later to run";
	} else
#endif

	// Check for Sound Manager 3.0 or later

	if (Mac::GetSoundManagerVersion() < 0x300) {
		pErrorText = "Requires Sound manager 3.2.1 or later to run";
	} else

	// Check the screen size

#if !TARGET_API_MAC_CARBON
	if ((qd.screenBits.bounds.bottom < 480) || (qd.screenBits.bounds.right < 640)) {
		pErrorText = "Requires a video display of 640 by 480 pixels or larger to run";
	} else
#endif

	// Check the free memory amount

	if (TempFreeMem() < 500*1024) {
		if (!OkCancelAlertMessage("You are very low on memory, do you wish to continue?",GAMENAME)) {
			iResult = 10;
		}
	}

	// Was an error message generated?
	if (pErrorText) {
		// Display the error and quit
		OkAlertMessage(pErrorText,GAMENAME);
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

int main(void);

int main(void)
{
	// Create an application instance

	MacApp MyApp(MEMORYSIZE,HANDLECOUNT,MINIMUMRESERVE);

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

#elif defined(BURGER_XBOX360)

/***************************************

	This is the main entry point for an XBox 360 version of the game

***************************************/

void BURGER_ANSIAPI main(void);

void BURGER_ANSIAPI main(void)
{
	// Create an application instance

	Xbox360App MyApp(MEMORYSIZE,HANDLECOUNT,MINIMUMRESERVE);

	// Error on startup?
	int iResult = MyApp.GetReturnCode();

	if (!iResult) {
		iResult = CodeEntry(&MyApp);
	}
}

#elif defined(BURGER_MACOSX)

/***************************************

	This is the main entry point for a MacOSX version of the game

***************************************/

int BURGER_ANSIAPI main(void);

int BURGER_ANSIAPI main(void)
{
	// Create an application instance

	MacOSXApp MyApp(MEMORYSIZE,HANDLECOUNT,MINIMUMRESERVE);

	// Error on startup?
	int iResult = Globals::GetErrorCode();

	if (!iResult) {
#if !defined(BURGER_NOMENUS)
		Globals::CreateDefaultMenus();
#endif
		iResult = CodeEntry(&MyApp);
	}
	return iResult;
}

#elif defined(BURGER_IOS)

/***************************************

	This is the main entry point for a iOS version of the game

***************************************/

int BURGER_ANSIAPI main(void);

int BURGER_ANSIAPI main(void)
{
	// Create an application instance

	iOSApp MyApp(GAMENAME,MEMORYSIZE,HANDLECOUNT,MINIMUMRESERVE);

	// Error on startup?
	int iResult = Globals::GetErrorCode();

	if (!iResult) {
		iResult = CodeEntry(&MyApp);
	}
	return iResult;
}

#endif

