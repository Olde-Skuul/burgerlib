/***************************************

	Filename Class
	Xbox 360 specific code

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_XBOX360)
#include "brfilemanager.h"
#include "brmemoryfunctions.h"
#define NOD3D
#define NONET
#include <xtl.h>

/*! ************************************

	\brief Expand a filename into XBox 360 format.

	Using the rules for a Burgerlib type pathname, expand a path
	into a FULL pathname native to the Xbox 360 file system.

	Directory delimiters are colons only.
	If the path starts with a colon, then it is a full pathname starting with a
	volume name. If the path starts with ".D2:" then it is a full pathname
	starting with a drive number. If the path starts with a "$:","*:" or "@:"
	then use special prefix numbers 32-34 If the path starts with 0: through 31:
	then use prefix 0-31. Otherwise prepend the pathname with the contents of
	prefix 8 ("Default")

	If the path after the prefix is removed is a period then POP the number of
	directories from the pathname for each period present after the first.
	Example "..:PrevDir:File:" will go down one directory and up the directory
	PrevDir

	All returned pathnames will NOT have a trailing "\", they will
	take the form of c:\\foo\\bar\\file.txt or similar

	Examples:<br>
	If drive C: is named "boot" then ":boot:foo:bar.txt" = "c:\foo\bar.txt"<br>
	If there is no drive named "boot" then ":boot:foo:bar.txt" =
		"\\boot\foo\bar.txt"<br>
	".D2:foo:bar.txt" = "c:\foo\bar.txt"<br>
	".D4:foo:bar.txt" = "e:\foo\bar.txt"<br>
	"@:game:data.dat" = "c:\users\<Current user>\appdata\roaming\game\data.dat"

	\param pInput Pointer to a pathname string
	\sa Burger::Filename::Set(const char *)

***************************************/

const char* Burger::Filename::get_native(void) BURGER_NOEXCEPT
{
	// Xbox 360 doesn't support device numbers.

	// If already parsed, skip the conversion.
	if (!m_bNativeValid) {

		// Resolve prefixes and convert to absolute path
		abs_path();

		// Get the path pointer and length
		const uint8_t* pPath = reinterpret_cast<uint8_t*>(m_Filename.c_str());
		uintptr_t uPathLength =
			StringLength(reinterpret_cast<const char*>(pPath));

		m_NativeFilename.clear();

		// Ignore the first colon, it's to the device name
		if (pPath[0] == ':') {
			++pPath;
			--uPathLength;
		}

		if (uPathLength) {

			// Iterate over the path, converting all colons
			// to slashes, except the first on, since that's needed for
			// drive name delimiting
			uint_t uTemp;
			uint_t uFirst = TRUE;
			do {
				uTemp = pPath[0];
				++pPath;

				// Path delimiter?
				if (uTemp == ':') {

					// It's the delimiter from the volume name
					if (uFirst) {
						// Insert a colon
						m_NativeFilename.push_back(':');
						uFirst = FALSE;
					}

					// Convert to slash
					uTemp = '\\';
				}

				// Save the character
				m_NativeFilename.push_back(static_cast<char>(uTemp));
			} while (--uPathLength);

			// Remove trailing slash
			if (uTemp == '\\') {
				m_NativeFilename.pop_back();
			}

			// It's valid
			m_bNativeValid = TRUE;
		}
	}

	// Return the translated path
	return m_NativeFilename.c_str();
}

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
	return assign(":d:");
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
	return assign(":d:");
}

/***************************************

	\brief Set the filename to the boot volume directory

	Determine the directory of the drive volume that the operating system was
	loaded from. The path is converted into UTF8 character encoding and stored
	in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::set_boot_volume(
	void) BURGER_NOEXCEPT
{
	return assign(":d:");
}

/***************************************

	\brief Set the filename to the local machine preferences directory

	Determine the directory where the user's preferences that are
	local to the machine is located. The path is converted
	into UTF8 character encoding and stored in Burgerlib
	filename format.

	On platforms where a current working directory doesn't make sense,
	like an ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::set_system_prefs_directory(
	void) BURGER_NOEXCEPT
{
	return assign(":d:");
}

/***************************************

	\brief Set the filename to the user's preferences directory

	Determine the directory where the user's preferences that
	could be shared among all machines the user has an account
	with is located. The path is converted
	into UTF8 character encoding and stored in Burgerlib
	filename format.

	On platforms where a current working directory doesn't make sense,
	like an ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::set_user_prefs_directory(
	void) BURGER_NOEXCEPT
{
	return assign(":d:");
}

/***************************************

	Convert a Windows path to a Burgerlib path

	Paths without a leading '\' are prefixed with
	the current working directory

	Paths with a drive letter but no leading \ will use
	the drive's current working directory

	If it's a network path "\\" then use that as the volume name.

	The Windows version converts these types of paths:

	"C:\foo\bar2" = ".D2:foo:bar2:"<br>
	"foo" = "(working directory from 8):foo:"<br>
	"foo\bar2" = "(working directory from 8):foo:bar2:"<br>
	"\foo" = ".D(Mounted drive number):foo:"<br>
	"\\foo\bar\file.txt" = ":foo:bar:file.txt:"

***************************************/

Burger::eError BURGER_API Burger::Filename::set_native(
	const char* pInput) BURGER_NOEXCEPT
{
	// No directory at all?
	if (!pInput || !pInput[0]) {
		// Just get the current directory
		pInput = "d:\\";
	}

	// Store the native path as is.
	m_NativeFilename.assign(pInput);
	m_bNativeValid = TRUE;

	// Init the converted string
	m_Filename.assign(":");

	const char* pPath = pInput;

	// Turn the drive name into the root segment
	const char* pColon = StringCharacter(pPath, ':');
	if (pColon) {
		uintptr_t uVolumeNameSize = pColon - pInput;
		m_Filename.append(pPath, uVolumeNameSize);
		pPath = pColon + 1;
	}

	// Append the rest of the path, and change slashes to colons

	uint_t uTemp = reinterpret_cast<const uint8_t*>(pPath)[0];
	++pPath;
	if (uTemp) {
		do {
			// Convert directory holders
			if (uTemp == '\\') {
				// To generic paths
				uTemp = ':';
			}

			// Save char
			m_Filename.push_back(static_cast<char>(uTemp));

			// Next char
			uTemp = pPath[0];
			++pPath;

			// Still more?
		} while (uTemp);
	}

	// Make sure it's appended with a colon
	end_with_colon();
	return kErrorNone;
}

#endif
