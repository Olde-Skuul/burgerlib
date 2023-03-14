/***************************************

	Burgerlib Filename class, unix version

	Copyright (c) 2021-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_UNIX) || defined(DOXYGEN)
#include <stdlib.h>
#include <unistd.h>

#include "brosstringfunctions.h"

#if !defined(BURGER_DARWIN)
#include "brfilemanager.h"
#include "brmemoryfunctions.h"
#endif

#if defined(BURGER_IOS)
#include <CoreFoundation/CFString.h>
#include <Foundation/NSPathUtilities.h>
#endif

/***************************************

	Expand a filename into Unix format.

	Using the rules for a Burgerlib type pathname, expand a path into a FULL
	pathname native to the Unix file system.

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

#if !defined(BURGER_DARWIN)
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

				uintptr_t uIndex = 13;

				// Test for boot name match
				if (MemoryCompare(":boot_volume:", pPath, uIndex)) {
					// Look in the mounted volumes folder
					m_NativeFilename.assign(*FileManager::get_volume_prefix());

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

	Convert a Unix filename into BurgerLib format.

	Using the rules for a Burgerlib type pathname, expand a path from a Unix
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
	// No directory at all?
	if (!pInput || !pInput[0]) {

		// Just get the current directory
		pInput = "./";
	}

	// Normalize the path
	eError uResult = get_abspath(&m_NativeFilename, pInput);
	if (uResult) {
		return uResult;
	}

	// Determine the length of the prefix
	uintptr_t uInputLength = m_NativeFilename.length();
	pInput = m_NativeFilename.c_str();
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

		const String* pVolumePrefix = FileManager::get_volume_prefix();
		uintptr_t uVolumeNameLength = pVolumePrefix->length() + 1U;
		if ((uInputLength > uVolumeNameLength) &&
			!MemoryCompare(
				pInput, pVolumePrefix->c_str(), pVolumePrefix->length()) &&
			(pInput[uVolumeNameLength - 1U] == '/')) {
			pBaseName = ":";
			uBaseNameLength = 1;
			pInput += uVolumeNameLength;
			uInputLength -= uVolumeNameLength;
		} else {
			pBaseName = ":boot_volume:";
			uBaseNameLength = 13;
			++pInput;
			--uInputLength;
		}
	}

	// Allocate the space
	uResult = m_Filename.reserve(uBaseNameLength + uInputLength + 6);
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

	// Make sure it's appended with a colon
	m_bNativeValid = TRUE;
	return end_with_colon();
}
#endif

/***************************************

	\brief Set the filename to the current working directory

	Query the operating system for the current working directory and set the
	filename to that directory. The path is converted into UTF8 character
	encoding and stored in Burgerlib filename format

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

	\return \ref kErrorNone if successful, \ref kErrorPathNotFound if invalid,
		or \ref kErrorNotSupportedOnThisPlatform if not implemented on the
		platform.

***************************************/

Burger::eError BURGER_API Burger::Filename::set_system_working_directory(
	void) BURGER_NOEXCEPT
{
	clear();

	// iOS has no "Current directory", so pull the directory the *.app file
	// resides in
#if defined(BURGER_IOS)
	// NSHomeDirectory() requires a scoped autorelease
	String Temp;
	BURGER_SCOPED_AUTORELEASE;
	StringCopy(&Temp, reinterpret_cast<CFStringRef>(NSHomeDirectory()));
	if (!Temp.empty()) {
		set_native(Temp.c_str());
	}
#else
	// Get the current working directory from posix
	char* pTemp = getcwd(nullptr, 0);
	if (pTemp) {
		// Got it!
		set_native(pTemp);

		// Release the memory passed by getcwd()
		free(pTemp);
	}
#endif
	return kErrorNone;
}

#endif
