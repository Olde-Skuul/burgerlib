/***************************************

    Code library (DLL) manager

    Windows version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brcodelibrary.h"

#if defined(BURGER_WINDOWS)
#include "brfilename.h"
#include "brglobals.h"
#include "brstring16.h"
#if !defined(WIN32_LEAN_AND_MEAN) && !defined(DOXYGEN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

/***************************************

	Attempt to load in a shared library or DLL using the standard paths. Return
	NULL if it fails Please note, in Win95, passing in just the DLL name without
	a full path will allow the SYSTEM DLL's to be loaded, so, to see if you want
	to load a system DLL, I check the pathname if it has a ':' in it.

***************************************/

Word Burger::CodeLibrary::Init(const char* pFilename)
{
	// If there was a previous library, release it
	Shutdown();

	Filename Pathname;
	// Is this a burgerlib pathname?
	// Test by checking for a colon
	if (StringCharacter(pFilename, ':')) {
		// Convert to a native windows pathname
		Pathname.Set(pFilename);
		pFilename = Pathname.GetNative();
	}
	Word uResult = FALSE;
	// Convert from UTF8 to UTF16
	String16 FinalName(pFilename);
	// Load the library from Windows
	HMODULE hModule = Windows::LoadLibraryW(FinalName.GetPtr());
	// On success!
	if (hModule != NULL) {
		m_pLibInstance = hModule;
		uResult = TRUE;
	}
	return uResult;
}

/***************************************

	Release a shared library

***************************************/

void Burger::CodeLibrary::Shutdown(void)
{
	// Release the windows lib
	if (m_pLibInstance) {
		FreeLibrary(static_cast<HINSTANCE>(m_pLibInstance));
		m_pLibInstance = NULL;
	}
}

/***************************************

	Return a function pointer to a procedure or data contained within a shared
	library

***************************************/

void* Burger::CodeLibrary::GetFunction(const char* pFunctionName)
{
	void* pFunction = NULL;
	if (pFunctionName && m_pLibInstance) {
		pFunction = GetProcAddress(
			static_cast<HINSTANCE>(m_pLibInstance), pFunctionName);
	}
	return pFunction;
}

#endif
