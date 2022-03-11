/***************************************

	Code library (DLL) manager

	iOS version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brcodelibrary.h"

#if defined(BURGER_IOS)
#include "brfilename.h"
#include "brstringfunctions.h"
#include <dlfcn.h>

/***************************************

	Attempt to load in a shared library or DLL using the standard paths. Return
	nullptr if it fails Please note, in posix, passing in just the DLL name
	without a full path will allow the SYSTEM DLL's to be loaded, so, to see if
	you want to load a system DLL, I check the pathname if it has a ':' in it.

***************************************/

uint_t Burger::CodeLibrary::Init(const char* pFilename) BURGER_NOEXCEPT
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
	// Load the library from Linux
	void* pModule = dlopen(pFilename, RTLD_LAZY);
	// On success!
	uint_t uResult = FALSE;
	if (pModule != nullptr) {
		m_pLibInstance = pModule;
		uResult = TRUE; // Error!
	}
	return uResult;
}

/***************************************

	Release a shared library

***************************************/

void Burger::CodeLibrary::Shutdown(void) BURGER_NOEXCEPT
{
	// Release the Android lib
	if (m_pLibInstance) {
		dlclose(m_pLibInstance);
		m_pLibInstance = nullptr;
	}
}

/***************************************

	Return a function pointer to a procedure or data
	contained within a shared library

***************************************/

void* Burger::CodeLibrary::GetFunction(
	const char* pFunctionName) BURGER_NOEXCEPT
{
	void* pFunction = nullptr;
	if (pFunctionName && m_pLibInstance) {
		pFunction = dlsym(m_pLibInstance, pFunctionName);
	}
	return pFunction;
}

#endif
