/***************************************

	Linux version

	Copyright (c) 2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_DARWIN) || defined(DOXYGEN)
#include "brglobalmemorymanager.h"
#include "brosstringfunctions.h"

#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFString.h>
#include <Foundation/NSAutoreleasePool.h>
#include <Foundation/NSPathUtilities.h>
#include <libgen.h>
#include <mach-o/dyld.h>
#include <stdlib.h>
#include <unistd.h>

/***************************************

	\brief Set the filename to the application's directory

	Determine the directory where the application resides and set the filename
	to that directory. The path is converted into UTF8 character encoding and
	stored in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like an
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::SetApplicationDirectory(
	void) BURGER_NOEXCEPT
{
	clear();
	// Get the size of the path to the application
	uint32_t uSize = 0;
	int iTest = _NSGetExecutablePath(nullptr, &uSize);
	// Got the length?
	if (iTest == -1) {
		// Allocate the buffer
		char* pBuffer = static_cast<char*>(Alloc(uSize + 1));
		if (pBuffer) {
			// Try again
			iTest = _NSGetExecutablePath(pBuffer, &uSize);
			if (!iTest) {

				// Pop the executable name
				char* pDirBuffer = ::dirname(pBuffer);

				// realpath() on older versions of OSX before 10.6
				// cannot have the 2nd parameter set to NULL. This
				// bites.

				// Get the maximum length of the path
				long path_max = pathconf(pDirBuffer, _PC_PATH_MAX);
				if (path_max <= 0) {
					path_max = 4096; // Failsafe
				}

				// Allocate a generous buffer
				char* pRealPath = static_cast<char*>(Alloc(path_max * 2));

				// Did it parse?
				if (realpath(pDirBuffer, pRealPath)) {
					pDirBuffer = pRealPath;
				}
				// Convert to burgerlib
				SetFromNative(pDirBuffer);

				// Release the expanded buffer
				Free(pRealPath);
			}
			// Clean up
			Free(pBuffer);
		}
	}
	return kErrorNone;
}

/***************************************

	\brief Set the filename to the user's preferences directory

	Determine the directory where the user's preferences that could be shared
	among all machines the user has an account with is located. The path is
	converted into UTF8 character encoding and stored in Burgerlib filename
	format.

	On platforms where a current working directory doesn't make sense, like an
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::SetUserPrefsDirectory(
	void) BURGER_NOEXCEPT
{
	clear();

	// Need the pool for NSSearchPathForDirectoriesInDomains()

	NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];

	// Get the directory for ~/Library
	CFArrayRef pArray =
		reinterpret_cast<CFArrayRef>(NSSearchPathForDirectoriesInDomains(
			NSLibraryDirectory, NSUserDomainMask, YES));

	// Was a library found?
	if (CFArrayGetCount(pArray) >= 1) {
		CFStringRef pString =
			static_cast<CFStringRef>(CFArrayGetValueAtIndex(pArray, 0));

		// Convert to Burgerlib String
		String Temp;
		StringCopy(&Temp, pString);
		if (!Temp.empty()) {
			// Convert to path
			SetFromNative(Temp.c_str());

			// Add "Preferences"
			join("Preferences");
		}
	}

	// Release all the temp memory
	[pPool release];
	return kErrorNone;
}

/***************************************

	\brief Set the filename to the local machine preferences directory

	Determine the directory where the user's preferences that are local to the
	machine is located. The path is converted into UTF8 character encoding and
	stored in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like an
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::SetMachinePrefsDirectory(
	void) BURGER_NOEXCEPT
{
	clear();
#if 0
	glob_t globbuf;
	if (!glob("~/Library/Application Support", GLOB_TILDE, nullptr, &globbuf)) {
		// Convert the string
		SetFromNative(globbuf.gl_pathv[0]);
		globfree(&globbuf);
	}
#endif
	// Need the pool for NSSearchPathForDirectoriesInDomains()

	NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];

	// Get the directory for ~/Library
	CFArrayRef pArray =
		reinterpret_cast<CFArrayRef>(NSSearchPathForDirectoriesInDomains(
			NSApplicationSupportDirectory, NSUserDomainMask, YES));

	// Was a library found?
	if (CFArrayGetCount(pArray) >= 1) {
		CFStringRef pString =
			static_cast<CFStringRef>(CFArrayGetValueAtIndex(pArray, 0));

		// Convert to Burgerlib String
		String Temp;
		StringCopy(&Temp, pString);
		if (!Temp.empty()) {
			// Convert to path
			SetFromNative(Temp.c_str());
		}
	}

	// Release all the temp memory
	[pPool release];
	return kErrorNone;
}
#endif
