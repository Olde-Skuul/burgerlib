/***************************************

	MacOS version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
#include "brfilemanager.h"
#include "brglobalmemorymanager.h"
#include "brglobals.h"
#include "brmemoryfunctions.h"
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFString.h>
#include <Foundation/NSAutoreleasePool.h>
#include <Foundation/NSPathUtilities.h>
#include <glob.h>
#include <libgen.h>
#include <mach-o/dyld.h>
#include <stdlib.h>
#include <sys/param.h>
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

const char* Burger::Filename::GetNative(void) BURGER_NOEXCEPT
{
	Expand(); // Resolve prefixes
	if (!m_bNativeValid) {

		m_NativeFilename.reserve(m_Filename.length() + 10);
		m_NativeFilename.clear();

		// Now, is this a fully qualified name?
		const uint8_t* pFullPathName =
			reinterpret_cast<const uint8_t*>(m_Filename.c_str());

		// First char is ':' for a qualified pathname
		if (pFullPathName[0] == ':') {

			// Look for the volume name by scanning for the ending colon
			const uint8_t* pFileParsed =
				reinterpret_cast<uint8_t*>(StringCharacter(
					reinterpret_cast<const char*>(pFullPathName) + 1, ':'));
			if (pFileParsed) {
				// Is this on the boot volume?
				// Also test for the special case of :Foo vs :FooBar

				uintptr_t uIndex = FileManager::GetBootNameSize();

				// Test for boot name match
				if (MemoryCaseCompare(
						FileManager::GetBootName(), pFullPathName, uIndex)) {
					// Look in the mounted volumes folder
					m_NativeFilename.assign("/Volumes");

				} else {
					// If the volume requested is the boot volume, remove
					// the name and use the leading "/" for the root prefix.
					pFullPathName += (uIndex - 1);
				}
			}
		}

		// Convert the rest of the path
		// Colons to slashes

		uint_t uTemp = pFullPathName[0];
		if (uTemp) {
			do {
				++pFullPathName;
				if (uTemp == ':') {
					uTemp = '/'; // Unix style
				}

				m_NativeFilename.push_back(static_cast<char>(uTemp));
				uTemp = pFullPathName[0];
			} while (uTemp);

			// A trailing slash assumes more to follow, get rid of it
			if (m_NativeFilename.ends_with('/')) {
				m_NativeFilename.pop_back();
			}
		}

		m_bNativeValid = TRUE;
	}
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

Burger::eError BURGER_API Burger::Filename::SetFromNative(
	const char* pInput) BURGER_NOEXCEPT
{
	if (!pInput || !pInput[0]) { // No directory at all?
		pInput = "./";           // Just get the current directory
	}

	m_NativeFilename.assign(pInput);

	// Determine the length of the prefix
	uintptr_t uInputLength = StringLength(pInput);
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
			pBaseName = FileManager::GetBootName();
			uBaseNameLength = FileManager::GetBootNameSize();
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

#endif
