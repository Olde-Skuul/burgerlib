/***************************************

	Console manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brconsolemanager.h"

#if defined(BURGER_WINDOWS)
#include "brfilemanager.h"
#include "brutf8.h"
#include "brglobals.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <conio.h>
#include <shellapi.h>
//#include <winbase.h>
//#include <wincon.h>

// Needed for code that manually grabs the parm list
extern "C" char **__argv;

/*! ************************************

	\brief Base constructor.

	When a console app is spawned using ANSI C++ main(),
	it is given two parameters argc and argv. These parms
	are cached inside of this base class so the member
	functions can handle automatic parameter passing
	and decoding. The return code is initialized to zero.

***************************************/

Burger::ConsoleApp::ConsoleApp(int iArgc,const char **ppArgv) :
	m_ANSIMemoryManager(),
	m_bLaunchedFromDesktop(FALSE),
	m_ppOldArgv(NULL)
{
	// In order to support unicode command lines under windows,
	// the command line needs to be re-processed by calling the
	// shellapi and manually extracting the commands and 
	// convert them to UTF-8

	// Get the UTF-16 command line string from windows
	LPWSTR *pWideArgv = CommandLineToArgvW(GetCommandLineW(),&m_iArgc);
	// Get the parameter count
	iArgc = m_iArgc;
	// Do I bother?
	if (iArgc>0) {
		// Allocate a buffer to contain the string pointers and all of
		// the strings. This allows the pointers to be
		// manipulated within the list without
		// the strings being leaked on shutdown
		// since when the pointer array is released, it
		// also releases the strings as well

		// Determine how much space the strings will take as
		// UTF8
		Word16 **ppWork = reinterpret_cast<Word16 **>(pWideArgv);
		WordPtr uDataSize = 0;
		do {
			uDataSize += UTF8::FromUTF16(NULL,0,ppWork[0])+1;
			++ppWork;
		} while (--iArgc);

		// Allocate the buffer for a pointer array and the strings
		iArgc = m_iArgc;
		ppArgv = static_cast<const char **>(Alloc((sizeof(char *)*iArgc)+uDataSize));
		
		// Store the pointer for later use and disposal
		m_ppArgv = ppArgv;
		// Get the pointer beyond the pointer array
		char *pDest = reinterpret_cast<char *>(ppArgv)+(sizeof(char *)*iArgc);
		ppWork = reinterpret_cast<Word16 **>(pWideArgv);

		// This "hack" is to allow any other code that manually grabs the 
		// parameter list to inherit the UTF8 support
		m_ppOldArgv = const_cast<const char **>(__argv);
		__argv = const_cast<char **>(ppArgv);

		// Convert all the strings
		do {
			// Store the pointer in the array
			ppArgv[0] = pDest;
			++ppArgv;
			// Convert a string
			WordPtr uLength = UTF8::FromUTF16(pDest,uDataSize,ppWork[0])+1;
			// Adjust the pointers
			uDataSize -= uLength;
			pDest+=uLength;
			++ppWork;
		} while (--iArgc);
	} else {
		// No parameters?!?!
		m_ppArgv = NULL;
	}
	// Release the data Windows gave me for the parsed parameters
	LocalFree(pWideArgv);

	// Init the file system
	FileManager::Init();	
}

/*! ************************************

	\brief Dispose of any allocated resources

	When a console app is shut down, dispose of 
	everything here.

***************************************/

Burger::ConsoleApp::~ConsoleApp()
{
	// Release the file system
	FileManager::Shutdown();
	Free(m_ppArgv);
	if (m_ppOldArgv) {
		__argv = const_cast<char **>(m_ppOldArgv);
		m_ppOldArgv = NULL;
	}
}

/*! ************************************

	\brief Pause console output if the return code is not zero.
	
	If the return code is not set to zero, force the text output
	to remain on the screen if the application was launched
	from double clicking. If the application was launched
	from a console, this function does nothing.
	
***************************************/

void Burger::ConsoleApp::PauseOnError(void) const
{
	if (m_bLaunchedFromDesktop) {
		if (Globals::GetErrorCode()) {
			getch();		// Wait for a keypress before closing the console window
		}
	}
}

/*! ************************************

	\fn Burger::ConsoleApp::ProcessFilenames(Burger::ConsoleApp::CallbackProc pCallback) 
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
	
	\param pCallback Function pointer to a call that accepts a Burgerlib filename.
	\return TRUE if the function pointer was called. FALSE if normal processing should occur.
	
***************************************/

Word BURGER_API Burger::ConsoleApp::ProcessFilenames(Burger::ConsoleApp::CallbackProc pCallback) 
{

	Word uResult = FALSE;

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
        // Check the startup information. If a window is present, it's a GUI interface
        STARTUPINFOW StartupInfo;
        GetStartupInfoW(&StartupInfo);
        if (StartupInfo.dwFlags & STARTF_USESHOWWINDOW) {
			m_bLaunchedFromDesktop = TRUE;			// I was directly launched
			
			// Okay, the command line is all about the files
			// that were dropped on me
			
			if (m_iArgc>=2) {
				Filename MyFilename;
				int iIndex = 1;
				do {
					// Convert the pathname
					MyFilename.SetFromNative(m_ppArgv[iIndex]);
					const char *pNewName = MyFilename.GetPtr();
					if (pNewName[0]) {
						uResult = TRUE;		// Mark that I called something
						// Call the function
						int iCode = pCallback(this,pNewName,NULL);
						if (iCode) {
							Globals::SetErrorCode(iCode);
							break;
						}
					}
				} while (++iIndex<m_iArgc);
			}
		}
	}
	return uResult;
}

#endif
