/***************************************

	Linux version

	Copyright (c) 2021-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_DARWIN) || defined(DOXYGEN)
#include "brfilemanager.h"
#include "brglobalmemorymanager.h"
#include "brmemoryfunctions.h"
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

	Expand a filename into MacOSX format.

	Using the rules for a Burgerlib type pathname, expand a path into a FULL
	pathname native to the MacOSX file system.

	Directory delimiters are colons only.
	If the path starts with a colon, then it is a full pathname starting with a
	volume name. If the path starts with ".D2:" then it is a full pathname
	starting with a volume name enumated by volumes. If the path starts with a
	"$:","*:" or "@:" then use special prefix numbers 32-34 If the path starts
	with 0: through 31: then use prefix 0-31. Otherwise prepend the pathname
	with the contents of prefix 8 ("Default")

	If the path after the prefix is removed is a period then POP the number of
	directories from the pathname for each period present after the first.
	Example "..:PrevDir:File:" will go down one directory and up the directory
	PrevDir

	All returned pathnames will NOT have a trailing "/", they will take the form
	of /foo/bar/file.txt or similar

	Examples:<br>
	If home drive is named "boot" then ":boot:foo:bar.txt" = "/foo/bar.txt"<br>
	If the home drive is not named "boot" then ":boot:foo:bar.txt" =
		"/Volumes/boot/foo/bar.txt"<br>
	"@:game:data.dat" =
	"/Users/<Current user>/Library/Preferences/game/data.dat"

***************************************/

const char* Burger::Filename::get_native(void) BURGER_NOEXCEPT
{
	// If already parsed, skip the conversion.
	if (!m_bNativeValid) {

		// Resolve prefixes and convert to absolute path
		abs_path();

		// Now, is this a fully qualified name?
		const uint8_t* pPath =
			reinterpret_cast<const uint8_t*>(m_Filename.c_str());
		m_NativeFilename.clear();

		// First char is ':' for a qualified pathname
		if (pPath[0] == ':') {

			// Look for the volume name by scanning for the ending colon
			const uint8_t* pFileParsed = reinterpret_cast<uint8_t*>(
				StringCharacter(reinterpret_cast<const char*>(pPath) + 1, ':'));
			if (pFileParsed) {
				// Is this on the boot volume?
				// Also test for the special case of :Foo vs :FooBar

				uintptr_t uIndex = FileManager::get_boot_name_length();

				// Test for boot name match
				if (MemoryCaseCompare(
						FileManager::get_boot_name(), pPath, uIndex)) {
					// Look in the mounted volumes folder
					m_NativeFilename.assign("/Volumes");

				} else {
					// If the volume requested is the boot volume, remove
					// the name and use the leading "/" for the root prefix.
					pPath += (uIndex - 1);
				}
			}
		}

		// Convert the rest of the path
		// Colons to slashes

		uint_t uTemp = pPath[0];
		if (uTemp) {
			do {
				++pPath;
				if (uTemp == ':') {
					uTemp = '/'; // Unix style
				}

				m_NativeFilename.push_back(static_cast<char>(uTemp));
				uTemp = pPath[0];
			} while (uTemp);

			// A trailing slash assumes more to follow, get rid of it
			if (m_NativeFilename.ends_with('/')) {
				m_NativeFilename.pop_back();
			}
		}

		// It's valid
		m_bNativeValid = TRUE;
	}

	// Return the translated path
	return m_NativeFilename.c_str();
}

/***************************************

	Convert a MacOSX filename into BurgerLib format.

	Using the rules for a Burgerlib type pathname, expand a path from a MacOSX
	filename into BurgerLib.

	The pathname will have an ending colon.

	Examples:<br>
	If home drive is named "boot" then "/foo/bar.txt" = ":boot:foo:bar.txt"<br>
	If the first name is "/Volumes" then "/Volumes/boot/foo/bar.txt" =
	":boot:foo:bar.txt"<br>

***************************************/

Burger::eError BURGER_API Burger::Filename::set_native(
	const char* pInput) BURGER_NOEXCEPT
{
	if (!pInput || !pInput[0]) { // No directory at all?
		pInput = "./";           // Just get the current directory
	}

	m_NativeFilename.assign(pInput);

	// Determine the length of the prefix
	uintptr_t uInputLength = string_length(pInput);
	const char* pBaseName;
	uintptr_t uBaseNameLength;

	// Must I prefix with the current directory?
	if (reinterpret_cast<const uint8_t*>(pInput)[0] != '/') {
		// Dispose of "current directory"
		if ((uInputLength >= 2) && !MemoryCompare("./", pInput, 2)) {
			pInput += 2;
			uInputLength -= 2;
		}
		pBaseName = "8:";
		uBaseNameLength = 2;
	} else {

		// Place a leading colon in the output
		if ((uInputLength > 9) && !MemoryCompare(pInput, "/Volumes/", 9)) {
			pBaseName = ":";
			uBaseNameLength = 1;
			pInput += 9;
			uInputLength -= 9;
		} else {
			pBaseName = FileManager::get_boot_name();
			uBaseNameLength = FileManager::get_boot_name_length();
			++pInput;
			--uInputLength;
		}
	}

	// Allocate the space
	eError uResult = m_Filename.reserve(uBaseNameLength + uInputLength + 6);
	if (uResult) {
		return uResult;
	}

	m_Filename.assign(pBaseName, uBaseNameLength);

	// Now, just copy the rest of the path

	uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
	if (uTemp) { // Any more?
		do {
			// Accept char
			++pInput;
			if (uTemp == '/') {
				uTemp = ':';
			}
			// Save char
			m_Filename.push_back(static_cast<char>(uTemp));
			// Next char
			uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// Still more?
		} while (uTemp);
	}

	// The wrap up...
	// Make sure it's appended with a colon
	m_bNativeValid = TRUE;
	return end_with_colon();
}

/***************************************

	\brief Set the filename to the application's directory

	Determine the directory where the application resides and set the filename
	to that directory. The path is converted into UTF8 character encoding and
	stored in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like an
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::set_application_directory(
	void) BURGER_NOEXCEPT
{
	clear();
	// Get the size of the path to the application
	uint32_t uSize = 0;
	int iTest = _NSGetExecutablePath(nullptr, &uSize);
	// Got the length?
	if (iTest == -1) {
		// Allocate the buffer
		char* pBuffer = static_cast<char*>(allocate_memory(uSize + 1));
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
				char* pRealPath = static_cast<char*>(allocate_memory(path_max * 2));

				// Did it parse?
				if (realpath(pDirBuffer, pRealPath)) {
					pDirBuffer = pRealPath;
				}
				// Convert to burgerlib
				set_native(pDirBuffer);

				// Release the expanded buffer
				free_memory(pRealPath);
			}
			// Clean up
			free_memory(pBuffer);
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

Burger::eError BURGER_API Burger::Filename::set_user_prefs_directory(
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
		string_copy(&Temp, pString);
		if (!Temp.empty()) {
			// Convert to path
			set_native(Temp.c_str());

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

Burger::eError BURGER_API Burger::Filename::set_system_prefs_directory(
	void) BURGER_NOEXCEPT
{
	clear();
#if 0
	glob_t globbuf;
	if (!glob("~/Library/Application Support", GLOB_TILDE, nullptr, &globbuf)) {
		// Convert the string
		set_native(globbuf.gl_pathv[0]);
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
		string_copy(&Temp, pString);
		if (!Temp.empty()) {
			// Convert to path
			set_native(Temp.c_str());
		}
	}

	// Release all the temp memory
	[pPool release];
	return kErrorNone;
}
#endif
