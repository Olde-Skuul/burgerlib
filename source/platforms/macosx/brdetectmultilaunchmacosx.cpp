/***************************************

    Detect multiple launches class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdetectmultilaunch.h"

#if defined(BURGER_MACOSX)
#include <Foundation/NSConnection.h>
#include <Foundation/NSString.h>

/***************************************

	\brief Initialize the object to a power up state.

	Variables are initialized but no action is taken

***************************************/

Burger::DetectMultiLaunch::DetectMultiLaunch() : m_pConnection(NULL) {}

/***************************************

	\brief Teardown on exit

	If IsMultiLaunched() is called, a global object exists. Once the class is
	destroyed, so is the object.

***************************************/

Burger::DetectMultiLaunch::~DetectMultiLaunch()
{
	if (m_pConnection) {
		[m_pConnection release];
		m_pConnection = NULL;
	}
}

/***************************************

	\brief Test if this is the only instance of this class systemwide.

	An object is created, for Windows, it's a FileMapping object of 32 bytes
	that stays in memory. If executed again, the recreation of the object will
	return a ERROR_ALREADY_EXISTS error that will tell me to go away.

***************************************/

uint_t Burger::DetectMultiLaunch::IsMultiLaunched(const char* pSignature)
{
	// Assume success
	uint_t uResult = FALSE;

	// Did I run already?

	if (!m_pConnection) {

		// Convert the string to something the mac likes
		NSString* pString = [NSString stringWithUTF8String:pSignature];

		// Test if this name was already registered
		m_pConnection = [[NSConnection alloc] init];
		if ([m_pConnection registerName:pString] == NO) {
			[m_pConnection release];
			m_pConnection = NULL;
			uResult = TRUE;
		}
		[pString release];
	}
	// Error! Either the name couldn't be registered or I exist
	return uResult;
}

#endif
