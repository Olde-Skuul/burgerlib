/***************************************

	Burgerlib Filename class, unix version

	Copyright (c) 2021-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_UNIX) || defined(DOXYGEN)
#include <stdlib.h>
#include <unistd.h>

#if defined(BURGER_IOS)
#include <Foundation/NSPathUtilities.h>
#include <CoreFoundation/CFString.h>
#endif

/***************************************

	\brief Set the filename to the current working directory

	Query the operating system for the current working directory and set the
	filename to that directory. The path is converted into UTF8 character
	encoding and stored in Burgerlib filename format

	On platforms where a current working directory doesn't make sense, like an
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::SetSystemWorkingDirectory(
	void) BURGER_NOEXCEPT
{
	clear();

	// iOS has no "Current directory", so pull the directory the *.app file
	// resides in
#if defined(BURGER_IOS)
	char NameBuffer[2048];
	if (CFStringGetCString(reinterpret_cast<CFStringRef>(NSHomeDirectory()),
			NameBuffer, sizeof(NameBuffer), kCFStringEncodingUTF8)) {
		if (NameBuffer[0]) {
			SetFromNative(NameBuffer);
		}
	}

#else
	// Get the current working directory from posix
	char* pTemp = getcwd(nullptr, 0);
	if (pTemp) {
		// Got it!
		SetFromNative(pTemp);

		// Release the memory passed by getcwd()
		free(pTemp);
	}
#endif
	return kErrorNone;
}

#endif
