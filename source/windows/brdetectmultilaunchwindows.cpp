/***************************************

	Detect multiple launches class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdetectmultilaunch.h"

#if defined(BURGER_WINDOWS)
#include "brstring16.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

/*! ************************************

	\brief Initialize the object to a power up state.
	
	Variables are initialized but no action is taken
	
***************************************/

Burger::DetectMultiLaunch::DetectMultiLaunch() :
	m_hInstanceLock(INVALID_HANDLE_VALUE)
{
}

/*! ************************************

	\brief Tear down on exit

	If IsMultiLaunched() is called, a global object exists.
	Once the class is destroyed, so is the object.

***************************************/

Burger::DetectMultiLaunch::~DetectMultiLaunch()
{
	if (m_hInstanceLock!=INVALID_HANDLE_VALUE) {
		CloseHandle(m_hInstanceLock);
		// Clean up
		m_hInstanceLock = INVALID_HANDLE_VALUE;
	}
}

/*! ************************************

	\brief Test if this is the only instance of this class systemwide.
	
	An object is created, for Windows, it's a FileMapping
	object of 32 bytes that stays in memory. If executed again,
	the recreation of the object will return a ERROR_ALREADY_EXISTS
	error that will tell me to go away.

***************************************/

Word Burger::DetectMultiLaunch::IsMultiLaunched(const char *pSignature)
{
	Word uResult = FALSE;
	// Did I run already?
	if (m_hInstanceLock==INVALID_HANDLE_VALUE) {
		String16 NewName(pSignature);
		// Create a global instance of a file mapper that's 32 bytes in size
		HANDLE hLock=CreateFileMappingW(INVALID_HANDLE_VALUE,NULL,PAGE_READONLY,0,32,reinterpret_cast<LPCWSTR>(NewName.GetPtr()));
		uResult = TRUE;
		if (hLock!=NULL) {
			// Got the mapping object?
			// Create and not already present?
			if (GetLastError()!=ERROR_ALREADY_EXISTS) {
				m_hInstanceLock = hLock;
				// No error
				uResult = FALSE;
			}
		}
	}
	// Error! Either the file couldn't be made or I exist
	return uResult;
}

#endif
