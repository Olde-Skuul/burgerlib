/***************************************

	Playstation Vita version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_VITA) || defined(DOXYGEN)
#include "brfilemanager.h"
#include "brglobalmemorymanager.h"
#include "brmemoryfunctions.h"

/***************************************

	Expand a filename into Playstation Vita format.

	Using the rules for a Burgerlib type pathname, expand a path
	into a FULL pathname native to the MacOSX file system.

	Directory delimiters are colons only.
	If the path starts with a colon, then it is a full pathname starting with a
	volume name. If the path starts with ".D2:" then it is a full pathname
	starting with a volume name enumerated by volumes. If the path starts with a
	"$:","*:" or "@:" then use special prefix numbers 32-34 If the path starts
	with 0: through 31: then use prefix 0-31. Otherwise prepend the pathname
	with the contents of prefix 8 ("Default")

	If the path after the prefix is removed is a period then POP the number of
	directories from the pathname for each period present after the first.
	Example "..:PrevDir:File:" will go down one directory and up the directory
	PrevDir

	All returned pathnames will NOT have a trailing "/", they will
	take the form of /foo/bar/file.txt or similar

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

		// Determine the final length
		const uint8_t* pPath =
			reinterpret_cast<const uint8_t*>(m_Filename.c_str());

		m_NativeFilename.clear();

		// First char is ':' for a qualified pathname
		if (pPath[0] == ':') {
			// Discard the leading colon
			++pPath;
			// Look for the mount name by scanning for the ending colon
			const uint8_t* pFileParsed = reinterpret_cast<uint8_t*>(
				StringCharacter(reinterpret_cast<const char*>(pPath), ':'));
			if (pFileParsed) {
				// Length of the mount name
				uintptr_t uLength =
					static_cast<uintptr_t>(pFileParsed - pPath) + 1;

				// Copy :app0: to app0:/
				m_NativeFilename.assign(
					reinterpret_cast<const char*>(pPath), uLength);
				m_NativeFilename.push_back('/');

				// Accept the input
				pPath = pFileParsed + 1;
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

				m_NativeFilename.push_back(uTemp);
				uTemp = pPath[0];
			} while (uTemp);

			// A trailing slash assumes more to follow, get rid of it
			if (m_NativeFilename.length() >= 2U) {
				if (m_NativeFilename.ends_with('/')) {
					m_NativeFilename.pop_back();
				}
			}
		}

		// It's valid
		m_bNativeValid = TRUE;
	}

	// Return the translated path
	return m_NativeFilename.c_str();
}

/***************************************

	Convert a Vita filename into BurgerLib format.

	Using the rules for a Burgerlib type pathname, expand a path
	from a Vita filename into BurgerLib.

	The pathname will have an ending colon.

	Examples:<br>
	app0:/foo.txt becomes :app0:foo.txt:<br>
	tempfile.txt becomes 8:tempfile.txt:<br>

***************************************/

Burger::eError BURGER_API Burger::Filename::set_native(
	const char* pInput) BURGER_NOEXCEPT
{
	clear(); // Clear out the previous string

	// Determine the length of the string buffer
	uintptr_t uInputLength = string_length(pInput);
	uintptr_t uOutputLength =
		uInputLength + 6; // Could prepend :app0:, 6 characters
	char* pOutput = m_Filename;
	if (uOutputLength >= sizeof(m_Filename)) {
		pOutput = static_cast<char*>(Alloc(uOutputLength));
		if (!pOutput) {
			// We're boned
			return kErrorOutOfMemory;
		}
	}
	m_Filename.assign(pOutput);

	// Convert mount names to drive names

	const char* pMountName = StringCharacter(pInput, ':');
	if (!pMountName) { // Must I prefix with the current directory?
		// Use the default directory
		if (pInput[0] == '/') {
			MemoryCopy(pOutput, ":app0:", 6);
			pOutput += 6;
		} else {
			pOutput[0] = '8';
			pOutput[1] = ':';
			pOutput += 2;
		}
	} else {
		// Convert app0:/ to :app0:
		uintptr_t uMountNameSize =
			static_cast<uintptr_t>((pMountName - pInput) + 1);
		pOutput[0] = ':';
		MemoryCopy(pOutput + 1, pInput, uMountNameSize);
		pOutput += uMountNameSize + 1;
		pInput += uMountNameSize;
	}

	// Now, just copy the rest of the path

	uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
	if (uTemp == '/') {
		++pInput;
		uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
	}
	if (uTemp) { // Any more?
		do {
			++pInput; // Accept char
			if (uTemp == '/') {
				uTemp = ':';
			}
			pOutput[0] = uTemp; // Save char
			++pOutput;
			uTemp = reinterpret_cast<const uint8_t*>(pInput)[0]; // Next char
		} while (uTemp);                                         // Still more?
	}

	// The wrap up...
	// Make sure it's appended with a colon

	if (reinterpret_cast<const uint8_t*>(pOutput)[-1] != ':') {
		pOutput[0] = ':';
		++pOutput;
	}
	pOutput[0] = 0; // End the string with zero
	return kErrorNone;
}

#endif
