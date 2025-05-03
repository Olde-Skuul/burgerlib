/***************************************

	Console manager, Windows XP and higher version

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brconsolemanager.h"

#if defined(BURGER_WINDOWS)
#include "brfilemanager.h"
#include "brglobals.h"
#include "brstring16.h"
#include "brutf8.h"
#include "brtick.h"

#include "win_windows.h"

#include <conio.h>
#include <mmsystem.h>
#include <shellapi.h>

#if defined(BURGER_WATCOM)
// Needed for timeBeginPeriod
#pragma library("Winmm.lib")
#else
#pragma comment(lib, "Winmm.lib")
#endif

// Needed for code that manually grabs the parm list
extern "C" char** __argv;

/***************************************

	\brief Base constructor.

	When a console app is spawned using ANSI C++ main(),
	it is given two parameters argc and argv. These parms
	are cached inside of this base class so the member
	functions can handle automatic parameter passing
	and decoding. The return code is initialized to zero.

***************************************/

Burger::ConsoleApp::ConsoleApp(int iArgc, const char** ppArgv,
	uint_t uFlags) BURGER_NOEXCEPT: m_ANSIMemoryManager(),
									m_bLaunchedFromDesktop(FALSE),
									m_ppOldArgv(NULL)
{
	BURGER_UNUSED(uFlags);

	// Increase the speed of the timer from 15.6 ticks per second to
	// 1000 ticks per second so sleep_ms() actually has millisecond
	// accuracy
	timeBeginPeriod(1);

	// In order to support unicode command lines under windows,
	// the command line needs to be re-processed by calling the
	// shellapi and manually extracting the commands and
	// convert them to UTF-8

	// Get the UTF-16 command line string from windows
	LPWSTR* pWideArgv = CommandLineToArgvW(GetCommandLineW(), &m_iArgc);
	// Get the parameter count
	iArgc = m_iArgc;
	// Do I bother?
	if (iArgc > 0) {
		// Allocate a buffer to contain the string pointers and all of
		// the strings. This allows the pointers to be
		// manipulated within the list without
		// the strings being leaked on shutdown
		// since when the pointer array is released, it
		// also releases the strings as well

		// Determine how much space the strings will take as
		// UTF8
		uint16_t** ppWork = reinterpret_cast<uint16_t**>(pWideArgv);
		uintptr_t uDataSize = 0;
		do {
			uDataSize += UTF8::GetUTF16Size(ppWork[0]) + 1;
			++ppWork;
		} while (--iArgc);

		// Allocate the buffer for a pointer array and the strings
		iArgc = m_iArgc;
		ppArgv = static_cast<const char**>(
			allocate_memory((sizeof(char*) * iArgc) + uDataSize));

		// Store the pointer for later use and disposal
		m_ppArgv = ppArgv;
		// Get the pointer beyond the pointer array
		char* pDest = reinterpret_cast<char*>(ppArgv) + (sizeof(char*) * iArgc);
		ppWork = reinterpret_cast<uint16_t**>(pWideArgv);

		// This "hack" is to allow any other code that manually grabs the
		// parameter list to inherit the UTF8 support
		m_ppOldArgv = const_cast<const char**>(__argv);
		//__argv = const_cast<char**>(ppArgv);

		// Convert all the strings
		do {
			// Store the pointer in the array
			ppArgv[0] = pDest;
			++ppArgv;
			// Convert a string
			uintptr_t uLength =
				UTF8::from_UTF16(pDest, uDataSize, ppWork[0]) + 1;
			// Adjust the pointers
			uDataSize -= uLength;
			pDest += uLength;
			++ppWork;
		} while (--iArgc);
	} else {
		// No parameters?!?!
		m_ppArgv = NULL;
	}
	// Release the data Windows gave me for the parsed parameters
	LocalFree(pWideArgv);

	Tick::init();
	// Init the file system
	FileManager::initialize();
}

/***************************************

	\brief Dispose of any allocated resources

	When a console app is shut down, dispose of
	everything here.

***************************************/

Burger::ConsoleApp::~ConsoleApp()
{
	// Release the file system
	FileManager::shut_down();
	Tick::shutdown();

	free_memory(m_ppArgv);
	if (m_ppOldArgv) {
		__argv = const_cast<char**>(m_ppOldArgv);
		m_ppOldArgv = nullptr;
	}

	// Release the Windows high speed timer
	timeEndPeriod(1);
}

/***************************************

	\brief Was the application launched from the desktop

	Returns \ref TRUE if the console application was launched by double clicking
	on the icon or \ref FALSE if the application was launched from a command
	line shell.

	\return \ref TRUE if desktop launched, \ref FALSE if from a shell.
	\sa Burger::ConsoleApp::GetArgv() const or
		Burger::ConsoleApp::GetArgc() const

***************************************/

uint_t BURGER_API Burger::ConsoleApp::WasDesktopLaunched(void) BURGER_NOEXCEPT
{
	uint_t uResult = m_bLaunchedFromDesktop;
	if (!uResult) {
		// By checking the console process list, if only this application was
		// attached, then it was double clicked from the desktop.
		DWORD Buffer[2];
		const DWORD uPIDCount =
			GetConsoleProcessList(Buffer, BURGER_ARRAYSIZE(Buffer));
		uResult = uPIDCount < 2;
		m_bLaunchedFromDesktop = uResult | 0x80U;
	}
	return uResult & 1;
}

/***************************************

	\brief Pause console output if the return code is not zero.

	If the return code is not set to zero, force the text output
	to remain on the screen if the application was launched
	from double clicking. If the application was launched
	from a console, this function does nothing.

***************************************/

void Burger::ConsoleApp::PauseOnError(Burger::eError uError)
{
	if (WasDesktopLaunched()) {
		if (uError != kErrorNone) {
			// Wait for a key press before closing the console window
			getch();
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

uint_t BURGER_API Burger::ConsoleApp::ProcessFilenames(
	Burger::ConsoleApp::CallbackProc pCallback)
{

	uint_t uResult = FALSE;

	//
	// To determine if I am running a user console, or
	// if I was double clicked, I check the console's
	// title. If the title is a match to the
	// first parameter, then I was invoked
	// by explorer.
	//

	// At least 2 parms and the console's title is
	// a match for the filename?

	if (pCallback) {
		// Check the startup information. If a window is present, it's a GUI
		// interface
		if (WasDesktopLaunched()) {

			// Okay, the command line is all about the files
			// that were dropped on me

			if (m_iArgc >= 2) {
				Filename MyFilename;
				int iIndex = 1;
				do {
					// Convert the pathname
					MyFilename.set_native(m_ppArgv[iIndex]);
					const char* pNewName = MyFilename.c_str();
					if (pNewName[0]) {
						uResult = TRUE; // Mark that I called something
						// Call the function
						const int iCode = pCallback(this, pNewName, nullptr);
						if (iCode) {
							Globals::SetErrorCode(
								static_cast<Burger::eError>(iCode));
							break;
						}
					}
				} while (++iIndex < m_iArgc);
			}
		}
	}
	return uResult;
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
	String16 Converter(pTitle);
	if (!SetConsoleTitleW(reinterpret_cast<LPCWSTR>(Converter.c_str()))) {
		GetLastError();
		return kErrorInvalidParameter;
	}
	return kErrorNone;
}

/***************************************

	\brief Set the size of the console window.

	On platforms that present console text output in a desktop window, this
	function will resize the window to the specified width and height in text
	cells. On platforms that don't text window resizing, this function does
	nothing and returns no error.

	\param uWidth Number of text cells wide of the new window
	\param uHeight Number of text cells high of the new window

	\return The current error code. Can be returned to the operating system.
	\sa WasDesktopLaunched()

***************************************/

Burger::eError BURGER_API Burger::ConsoleApp::SetWindowSize(
	uint_t uWidth, uint_t uHeight)
{
	// Sanity check
	if ((uWidth < 8) || (uHeight < 8)) {
		return kErrorInvalidParameter;
	}

	// Windows only supports values up to a signed 16 bit short.
	// Clamp it.
	if (uWidth >= 0x7FFF) {
		uWidth = 0x7FFF;
	}
	if (uHeight >= 0x7FFF) {
		uHeight = 0x7FFF;
	}

	// Get the handle to the standard output
	const HANDLE hStdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdoutHandle != INVALID_HANDLE_VALUE) {

		// Set the scroll back buffer to something massive
		COORD NewWindowSize;
		NewWindowSize.X = static_cast<SHORT>(uWidth);
		NewWindowSize.Y = 20000;
		if (SetConsoleScreenBufferSize(hStdoutHandle, NewWindowSize)) {

			// Set up the bounds rect for the window
			SMALL_RECT WinRect;
			WinRect.Left = 0;
			WinRect.Top = 0;
			WinRect.Right = static_cast<SHORT>(uWidth - 1);
			WinRect.Bottom = static_cast<SHORT>(uHeight);
			if (SetConsoleWindowInfo(hStdoutHandle, TRUE, &WinRect)) {
				// Success
				return kErrorNone;
			}
		}
	}
	// Clear out the error and exit
	GetLastError();
	return kErrorInvalidParameter;
}

#endif
