/***************************************

	Console manager, macOS classic version

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brconsolemanager.h"

#if defined(BURGER_MAC)
#include "brfilemanager.h"
#include "brglobals.h"
#include "brmactypes.h"
#include "brstringfunctions.h"

#if defined(__MSL__)
#include <SIOUX.h>
#endif

#include <AppleEvents.h>
#include <Gestalt.h>
#include <MacTypes.h>

#if !TARGET_API_MAC_CARBON
#include <Dialogs.h>
#include <Fonts.h>
#include <MacWindows.h>
#include <TextEdit.h>
#endif

/***************************************

	\brief Base constructor.

	When a console app is spawned using ANSI C++ main(),
	it is given two parameters argc and argv. These parms
	are cached inside of this base class so the member
	functions can handle automatic parameter passing
	and decoding. The return code is initialized to zero.

***************************************/

Burger::ConsoleApp::ConsoleApp(int iArgc, const char** ppArgv, uint_t uFlags) BURGER_NOEXCEPT:
	m_ANSIMemoryManager(), m_bLaunchedFromDesktop(FALSE)
{
	BURGER_UNUSED(uFlags);

	// Assume the input is UTF8 for all other platforms
	m_ppArgv = ppArgv;
	m_iArgc = iArgc;

	// For MacOS X, initialize the graphics so the tool can create
	// a console window

#if !TARGET_API_MAC_CARBON
	InitGraf(&qd.thePort);       // Init the graphics system
	InitFonts();                 // Init the font manager
	InitWindows();               // Init the window manager
	InitMenus();                 // Init the menu manager
	TEInit();                    // Init text edit
	InitDialogs(NULL);           // Init the dialog manager
	FlushEvents(everyEvent, 0L); // Clear pending keyboard/mouse events
	MaxApplZone(); // Expand the heap so code segments load at the top
#endif

	// Ensure I have some extra handles available
	uint_t i = 5;
	do {
		MoreMasters();
	} while (--i);

	// Only on Metrowerks standard libraries
#if defined(__MSL__)
	SIOUXSettings.initializeTB = FALSE;     // Don't init the toolbox
	SIOUXSettings.asktosaveonclose = FALSE; // Don't ask to save on close
#endif

	// Init the file system
	FileManager::initialize();
}

/***************************************

	\brief Pause console output if the return code is not zero.

	If the return code is not set to zero, force the text output to remain on
	the screen if the application was launched from double clicking. If the
	application was launched from a console, this function does nothing.

***************************************/

void Burger::ConsoleApp::PauseOnError(Burger::eError uError)
{
	if (WasDesktopLaunched()) {
		if (uError == kErrorNone) {
#if defined(__MSL__)
			SIOUXSettings.autocloseonquit = TRUE;
			SIOUXSettings.asktosaveonclose = FALSE;
#endif
		}
	}
}

/***************************************

	\fn Burger::ConsoleApp::ProcessFilenames(Burger::ConsoleApp::CallbackProc
		pCallback)

	\brief Handle drag and drop for console apps

	Detect if the application was launched from the Finder
	or from Explorer. If so, detect if it was because data
	files were "dropped" on the application for processing.
	If both cases are true, then call the user supplied function
	pointer for each file to be processed. The filenames are
	in Burgerlib format.

	\note This function will set the console return code to
	1 on entry, so if the processing doesn't take place, it
	will assume an error has occurred. The processing procedure
	can set the return code to zero or any other value at will
	and that's the return code that will be retained.

	\param pCallback Function pointer to a call that accepts a Burgerlib
		filename.

	\return TRUE if the function pointer was called. FALSE if normal
		processing should occur.

***************************************/

typedef struct Foo_t {
	Burger::ConsoleApp* m_pThis;             // This pointer for callback
	Burger::ConsoleApp::CallbackProc m_Proc; // Function to call
	uint_t m_bAbort;                         // Return value
	uint_t m_bProcessed;                     // TRUE if something was processed
} Foo_t;

/***************************************

	This is my Apple Event handler that's monitoring the Open Doc events and
	sending them back to the console application for processing

***************************************/

static pascal OSErr OpenDocMacProcessFilenames(
	const AEDescList* pAppleEventDescList, AEDescList* /* reply */,
	long iRefCon)
{
	char TempBuffer[2048];
#if TARGET_API_MAC_CARBON
	char TempBuffer2[2048]; // Used for URL translations on OSX
#endif
	AEDesc FileListDescription;

	// First, I see if any files are present
	// by checking the apple event that was given to me to process

	FileListDescription.descriptorType = typeWildCard; // Any file
	FileListDescription.dataHandle = nil;              // No handle (Yet)

	if (!AEGetKeyDesc(pAppleEventDescList, keyDirectObject, typeAEList,
			&FileListDescription)) {

		// Now load each and every file

		Foo_t* FooPtr = reinterpret_cast<Foo_t*>(iRefCon);
		if (!FooPtr->m_bAbort) {
			AEKeyword MyKeyword;
			DescType MyFileType;
			long lDataSize;  // Size of the data returned from the Apple Event
			long iIndex = 1; // Start at the first entry
			do {

#if TARGET_API_MAC_CARBON
				// First, check if I'm running under OSX. If so, use OSX
				// compatible filenames

				if (!AEGetNthPtr(&FileListDescription, iIndex, typeFileURL,
						&MyKeyword, &MyFileType, TempBuffer,
						sizeof(TempBuffer) - 1, &lDataSize)) {

					// Convert to Burgerlib path
					TempBuffer[lDataSize] = 0; // Convert to "C" string
#if 0 // BECKY FIX THIS
					char *pBuffer = Burger::Mac::GetPathFromFileURL(TempBuffer2,sizeof(TempBuffer2),TempBuffer);
#else
					char* pBuffer = NULL;
#endif
					if (pBuffer) {
						FooPtr->m_bProcessed = TRUE;

						// Process the file
						int iResult =
							FooPtr->m_Proc(FooPtr->m_pThis, pBuffer, NULL);
						if (pBuffer != TempBuffer2) {
							// Release the buffer if needed
							Burger::Free(pBuffer);
						}
						if (iResult) {
							Burger::Globals::SetErrorCode(
								static_cast<Burger::eError>(iResult));
							FooPtr->m_bAbort = TRUE;
							break;
						}
						++iIndex;
						continue;
					}
				}
				// I fell through, probably I'm Carbon, running on OS9
#endif

				FSSpec TheFileSpec; // FSSpec returned
				// I must be in OS9, do it the old fashioned way
				if (!AEGetNthPtr(&FileListDescription, iIndex, typeFSS,
						&MyKeyword, &MyFileType, &TheFileSpec, sizeof(FSSpec),
						&lDataSize)) {
					// Convert the FSSpec to a Burgerlib path
#if 0 // BECKY FIX THIS
					char *pBuffer = Burger::Filename::GetPathFromFSSpec(TempBuffer,sizeof(TempBuffer),&TheFileSpec);
#else
					char* pBuffer = NULL;
#endif
					if (pBuffer) {
						FooPtr->m_bProcessed = TRUE;

						// Process the file
						int iResult =
							FooPtr->m_Proc(FooPtr->m_pThis, pBuffer, NULL);
						if (pBuffer != TempBuffer) {
							// Release the buffer if needed
							Burger::Free(pBuffer);
						}
						if (iResult) {
							Burger::Globals::SetErrorCode(
								static_cast<Burger::eError>(iResult));
							FooPtr->m_bAbort = TRUE;
							break;
						}
					}
				} else {
					break; // No more events
				}
				++iIndex; // Next one
			} while (1);
		}
	}

	// I'm all done processing.
	AEDisposeDesc(&FileListDescription); // Release this
	return noErr;
}

/***************************************

	By installing an OpenDoc AppleEvent, see if I can capture incoming open file
	requests so I can pass them to the tool for processing

***************************************/

uint_t BURGER_API Burger::ConsoleApp::ProcessFilenames(
	Burger::ConsoleApp::CallbackProc pCallback)
{
	long lGestalt; // Gestalt temp
	Foo_t FooData; // Data state

	FooData.m_pThis = this;       // Initialize my callback data
	FooData.m_Proc = pCallback;   // Callback
	FooData.m_bAbort = FALSE;     // Not canceled
	FooData.m_bProcessed = FALSE; // Nothing processed yet

	if (pCallback &&
		!Gestalt(gestaltAppleEventsAttr,
			&lGestalt)) { // Do I have apple events and a valid callback?

		AEEventHandlerUPP OpenFileProc = NewAEEventHandlerUPP(
			OpenDocMacProcessFilenames); // Create a function pointer
		if (OpenFileProc) {

			EventRecord MyEvent;
			AEEventHandlerUPP PrevFileProc; // Previous proc pointer
			long lPrevRefCon;               // Previous proc refcon

			// Get the old event handler
			OSErr iPrevErr = AEGetEventHandler(kCoreEventClass,
				kAEOpenDocuments, &PrevFileProc, &lPrevRefCon, FALSE);
			// Set it to my event handler

			AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments,
				OpenFileProc, reinterpret_cast<long>(&FooData), FALSE);
			// 50 times should be enough attempts to poll for my events of
			// interest
			uint_t i = 50;
			do {
				// I only care about Apple Events
				if (!GetNextEvent(highLevelEventMask, &MyEvent)) {
					MyEvent.what = nullEvent;
				}
				// Did I get an Apple event?

				if (MyEvent.what == kHighLevelEvent) {
					m_bLaunchedFromDesktop = TRUE; // I was directly launched
					AEProcessAppleEvent(
						&MyEvent); // Let the OS invoke my callback
				}
			} while (--i);

			// I'm done. Remove my callback
			AERemoveEventHandler(
				kCoreEventClass, kAEOpenDocuments, OpenFileProc, FALSE);
			// Clean up
			DisposeAEEventHandlerUPP(OpenFileProc);

			// If there was a previous callback, restore it
			if (!iPrevErr) {
				AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments,
					PrevFileProc, lPrevRefCon, FALSE);
			}
		}
	}
	return FooData.m_bProcessed; // Did I process anything?
}

/***************************************

	\brief Set the title of the console window.

	On platforms that present console text output in a desktop window, this
	function will set the title of that window to the specified UTF-8 string.
	On platforms that don't support such a string, this function does nothing
	and returns no error.

	\param pTitle UTF-8 "C" string for new terminal title string.

	\return The current error code. Can be returned to the operating system.
	\sa WasDesktopLaunched()

***************************************/

Burger::eError BURGER_API Burger::ConsoleApp::SetWindowTitle(const char* pTitle)
{
	// Metrowerks uses the SIOUX console
#if defined(__MSL__)
	// Convert to PASCAL string
	uint8_t Buffer[256];
	CStringToPString(Buffer, pTitle);
	SIOUXSetTitle(Buffer);
#else
	// MPW and non-SIOUX consoles don't support title changes
	BURGER_UNUSED(pTitle);
#endif
	return kErrorNone;
}

#endif
