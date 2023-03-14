/***************************************

	Code library (DLL) manager

	Windows version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

Burger::eError Burger::CodeLibrary::init(const char* pFilename) BURGER_NOEXCEPT
{
	// If there was a previous library, release it
	shutdown();

	Filename Pathname;
	// Is this a burgerlib pathname?
	// Test by checking for a colon
	if (StringCharacter(pFilename, ':')) {
		// Convert to a native windows pathname
		Pathname.assign(pFilename);
		pFilename = Pathname.get_native();
	}
	eError uResult = kErrorFileNotFound;
	// Convert from UTF8 to UTF16
	String16 FinalName(pFilename);
	// Load the library from Windows
	HMODULE hModule = Windows::LoadLibraryW(FinalName.c_str());
	// On success!
	if (hModule != nullptr) {
		m_pLibInstance = hModule;
		uResult = kErrorNone;
	}
	return uResult;
}

/***************************************

	Release a shared library

***************************************/

void Burger::CodeLibrary::shutdown(void) BURGER_NOEXCEPT
{
	// Release the windows lib
	if (m_pLibInstance) {
		FreeLibrary(static_cast<HINSTANCE>(m_pLibInstance));
		m_pLibInstance = nullptr;
	}
}

/***************************************

	Return a function pointer to a procedure or data contained within a shared
	library

***************************************/

void* Burger::CodeLibrary::get_function(
	const char* pFunctionName) BURGER_NOEXCEPT
{
	void* pFunction = nullptr;
	if (pFunctionName && m_pLibInstance) {
		pFunction = GetProcAddress(
			static_cast<HINSTANCE>(m_pLibInstance), pFunctionName);
	}
	return pFunction;
}

#endif
