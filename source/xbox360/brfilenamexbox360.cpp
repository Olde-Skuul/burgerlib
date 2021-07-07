/***************************************

	Filename Class
	Xbox 360 specific code

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
volume name. If the path starts with ".D2:" then it is a full pathname starting
with a drive number. If the path starts with a "$:","*:" or "@:" then use
special prefix numbers 32-34 If the path starts with 0: through 31: then use
prefix 0-31. Otherwise prepend the pathname with the contents of prefix 8
("Default")

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

const char* Burger::Filename::GetNative(void) BURGER_NOEXCEPT
{
	// First step, expand myself to a fully qualified pathname
	Expand();

	/***************************************

		DOS version and Win 95 version
		I prefer for all paths intended for DOS use
		a generic drive specifier before the working directory.
		The problem is that Volume LABEL are very difficult to parse
		and slow to access.

	***************************************/

	// First parse either the volume name of a .DXX device number
	// I hopefully will get a volume number since DOS prefers it

	const uint8_t* pPath =
		reinterpret_cast<uint8_t*>(m_pFilename); // Copy to running pointer

	// Now that I have the drive number, determine the length
	// of the output buffer and start the conversion

	uintptr_t uPathLength = StringLength(reinterpret_cast<const char*>(pPath));
	// Reserve 6 extra bytes for the prefix and/or the trailing / and null
	char* pOutput = m_NativeFilename;

	if (uPathLength >= (sizeof(m_NativeFilename) - 6)) {
		pOutput = static_cast<char*>(Alloc(uPathLength + 6));
		if (!pOutput) {
			pOutput = m_NativeFilename;
			uPathLength = 0; // I'm so boned
		} else {
			m_pNativeFilename = pOutput; // This is my buffer
		}
	}

	// Ignore the first colon, it's to the device name
	if (pPath[0] == ':') {
		++pPath;
		--uPathLength;
	}

	if (uPathLength) {
		uint_t uTemp;
		uint_t uFirst = TRUE;
		do {
			uTemp = pPath[0];
			++pPath;
			if (uTemp == ':') {
				if (uFirst) {
					pOutput[0] = ':';
					++pOutput;
					uFirst = FALSE;
				}
				uTemp = '\\';
			}
			pOutput[0] = static_cast<char>(uTemp);
			++pOutput;
		} while (--uPathLength);
		// Remove trailing slash
		if (uTemp == '\\') {
			--pOutput;
		}
	}
	pOutput[0] = 0; // Terminate the "C" string
	return m_pNativeFilename;
}

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
	Set(":GAME:");
	return kErrorNone;
}

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
	Set(":GAME:");
	return kErrorNone;
}

/***************************************

	\brief Set the filename to the boot volume directory

	Determine the directory of the drive volume that the operating system was
	loaded from. The path is converted into UTF8 character encoding and stored
	in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::SetBootVolumeDirectory(
	void) BURGER_NOEXCEPT
{
	Set(":GAME:");
	return kErrorNone;
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

Burger::eError BURGER_API Burger::Filename::SetMachinePrefsDirectory(
	void) BURGER_NOEXCEPT
{
	Set(":GAME:");
	return kErrorNone;
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

Burger::eError BURGER_API Burger::Filename::SetUserPrefsDirectory(
	void) BURGER_NOEXCEPT
{
	Set(":GAME:");
	return kErrorNone;
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

Burger::eError BURGER_API Burger::Filename::SetFromNative(
	const char* pInput) BURGER_NOEXCEPT
{
	Clear();

	if (!pInput || !pInput[0]) { // No directory at all?
		pInput = "D:\\";         // Just get the current directory
	}

	// How long would the string be if it was UTF8?

	uintptr_t uExpandedLength = StringLength(pInput);
	uintptr_t uOutputLength = uExpandedLength + 6;
	char* pOutput = m_Filename;
	if (uOutputLength >= sizeof(m_Filename)) {
		pOutput = static_cast<char*>(Alloc(uOutputLength));
		if (!pOutput) {
			return kErrorOutOfMemory;
		}
	}
	m_pFilename = pOutput;
	const char* pSrc = pInput;

	pOutput[0] = ':';
	++pOutput;

	// Turn the drive name into the root segment

	const char* pColon = StringCharacter(pSrc, ':');
	if (pColon) {
		uintptr_t uVolumeNameSize = pColon - pInput;
		MemoryCopy(pOutput, pSrc, uVolumeNameSize);
		pOutput += uVolumeNameSize;
		pSrc = pColon + 1;
	}

	// Append the rest of the path, and change slashes to colons

	uint_t uTemp2 = reinterpret_cast<const uint8_t*>(pSrc)[0];
	++pSrc;
	if (uTemp2) {
		do {
			if (uTemp2 == '\\') { // Convert directory holders
				uTemp2 = ':';     // To generic paths
			}
			pOutput[0] = static_cast<char>(uTemp2); // Save char
			++pOutput;
			uTemp2 = pSrc[0]; // Next char
			++pSrc;
		} while (uTemp2); // Still more?
	}

	// The wrap up...
	// Make sure it's appended with a colon

	if (pOutput[-1] != ':') { // Last char a colon?
		pOutput[0] = ':';     // End with a colon!
		pOutput[1] = 0;
	}
	return kErrorNone;
}

#endif
