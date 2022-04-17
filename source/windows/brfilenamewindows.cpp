/***************************************

	Filename Class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_WINDOWS)
#include "brfilemanager.h"
#include "brglobals.h"
#include "brnumberto.h"
#include "brstring16.h"
#include "brutf16.h"
#include "brutf8.h"
#include "brnumberstring.h"

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <ObjBase.h>
#include <Windows.h>
#include <shlobj.h>

#if defined(BURGER_WATCOM)
#pragma library("shfolder.lib");
#endif

//
// GUIDs needed for locating folders in Vista or higher
//

#define KF_FLAG_DONT_UNEXPAND 0x00002000
#define KF_FLAG_DONT_VERIFY 0x00004000

EXTERN_C const GUID DECLSPEC_SELECTANY FOLDERID_LocalAppData = {
	0xF1B32785, 0x6FBA, 0x4FCF, 0x9D, 0x55, 0x7B, 0x8E, 0x7F, 0x15, 0x70, 0x91};
EXTERN_C const GUID DECLSPEC_SELECTANY FOLDERID_RoamingAppData = {
	0x3EB685DB, 0x65F9, 0x4CF6, 0xA0, 0x3A, 0xE3, 0xEF, 0x65, 0x72, 0x9F, 0x3D};

/*! ************************************

	\brief Expand a filename into Windows format.

	Using the rules for a Burgerlib type pathname, expand a path
	into a FULL pathname native to the Windows file system.

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

const char* Burger::Filename::GetNative(void) BURGER_NOEXCEPT
{
	// First step, expand myself to a fully qualified pathname
	Expand();
	if (!m_bNativeValid) {

		// DOS version and Win 95 version
		// I prefer for all paths intended for DOS use a generic drive specifier
		// before the working directory. The problem is that Volume LABEL are
		// very difficult to parse and slow to access.

		// First parse either the volume name of a .DXX device number
		// I hopefully will get a volume number since DOS prefers it

		// Copy to running pointer
		const uint8_t* pPath = reinterpret_cast<uint8_t*>(m_Filename.c_str());

		// Init the default drive number
		uint_t uDeviceNum = BURGER_MAXUINT;

		// Fully qualified pathname?
		if (pPath[0] == ':') {

			// Init index to the volume name
			uintptr_t uLength = 0;
			uint8_t uTemp;
			do {
				// Parse to the next colon
				++uLength;
				uTemp = pPath[uLength];
			} while (uTemp != ':' && uTemp);

			// Ensure the name ends with ':' in the case of ":foobar"
			String VolumeName(m_Filename, 0, uLength + 1);

			// Find a volume
			uDeviceNum = FileManager::GetVolumeNumber(VolumeName);

			// Can't find the volume?!?
			if (uDeviceNum == BURGER_MAXUINT) {
				// Ignore the leading colon
				uDeviceNum = BURGER_MAXUINT - 1;
				++pPath;
			} else {
				// Accept the name
				pPath = pPath + uLength;

				// Remove the colon if it had one at the end
				if (uTemp) {
					++pPath;
				}
			}

			// Is this a "drive letter"? Look for ".d2:"
		} else if (pPath[0] == '.') {

			// Get the second char
			uint_t uTemp = pPath[1];

			// Is it a 'D'?
			if ((uTemp & 0xDF) == 'D') {

				// Init numeric index
				uintptr_t uLength = 2;

				// Init drive number
				uDeviceNum = 0;
				do {
					// Get an ASCII char
					uTemp = pPath[uLength];
					++uLength;

					// Proper end of string?
					if (uTemp == ':') {
						// If nothing was parsed, abort
						if (uLength == 3) {
							uLength = 0;
							uDeviceNum = BURGER_MAXUINT;
						}
						break;
					}
					uTemp -= '0';

					// Numeric value?
					if (uTemp >= 10) {
						// Abort
						uLength = 0;
						// Force using the CWD
						uDeviceNum = BURGER_MAXUINT;
						break; // Go to phase 2
					}

					// Adjust previous value
					uDeviceNum = uDeviceNum * 10;

					// Make full decimal result
					uDeviceNum = uDeviceNum + uTemp;
					// Loop until done
				} while (uDeviceNum < 26);

				// Discard accepted input
				pPath = pPath + uLength;
			}
		}

		// Now that I have the drive number, determine the length
		// of the output buffer and start the conversion

		uintptr_t uPathLength =
			StringLength(reinterpret_cast<const char*>(pPath));

		// Reserve 6 extra bytes for the prefix and/or the trailing / and null

		m_NativeFilename.reserve(uPathLength + 6);
		m_NativeFilename.clear();
		
		// Insert the prefix, if any, to the output string

		if (uDeviceNum == (BURGER_MAXUINT - 1)) {
			// Since I didn't find the volume name, I'll assume it's
			// a network volume
			m_NativeFilename.push_back('\\');
			m_NativeFilename.push_back('\\');
		} else if (uDeviceNum != BURGER_MAXUINT) {
			m_NativeFilename.push_back(static_cast<char>(uDeviceNum + 'A'));
			m_NativeFilename.push_back(':');
			m_NativeFilename.push_back('\\');
		}

		// Convert the colons to slashes
		if (uPathLength) {
			uint_t uTemp;
			do {
				uTemp = pPath[0];
				++pPath;
				if (uTemp == ':') {
					uTemp = '\\';
				}
				m_NativeFilename.push_back(static_cast<char>(uTemp));
			} while (--uPathLength);
			// Remove trailing slash
			if (m_NativeFilename.ends_with('\\')) {
				m_NativeFilename.pop_back();
			}
		}
		m_bNativeValid = TRUE;
	}
	return m_NativeFilename.c_str();
}

/***************************************

	\brief Set the filename to the current working directory

	Query the operating system for the current working directory and set the
	filename to that directory. The path is converted into UTF8 character
	encoding and stored in Burgerlib filename format

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::SetSystemWorkingDirectory(
	void) BURGER_NOEXCEPT
{
	eError uResult = kErrorNotADirectory;

	// Get the length of the directory in WCHARs + the terminating null
	const DWORD uLength = GetCurrentDirectoryW(0, nullptr);

	// Should never fail.
	if (uLength) {

		// Reserve space for the incoming string minus the terminating zero
		String16 Temp16;
		uResult = Temp16.resize(uLength - 1);

		if (!uResult) {

			// Read in the directory name and check for failure
			uResult = kErrorNotADirectory;
			if (GetCurrentDirectoryW(uLength,
					reinterpret_cast<LPWSTR>(Temp16.data())) == (uLength - 1)) {

				// Convert from UTF16 to UTF8
				uResult = SetFromNative(Temp16.c_str());
			}
		}
	}

	if (uResult) {
		clear();
	}
	return uResult;
}

/***************************************

	\brief Set the filename to the application's directory

	Determine the directory where the application resides and set the filename
	to that directory. The path is converted into UTF8 character encoding and
	stored in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::SetApplicationDirectory(
	void) BURGER_NOEXCEPT
{
	eError uResult = kErrorNotADirectory;

	// Ask windows what's the folder the app is running in

	uint16_t Buffer[MAX_PATH];

	// Try the easy way
	DWORD uLength = GetModuleFileNameW(
		nullptr, reinterpret_cast<LPWSTR>(Buffer), BURGER_ARRAYSIZE(Buffer));
	if (uLength) {

		// There's a filename
		uint16_t* pWBuffer = Buffer;
		// See if the buffer was filled. If so, it's a deep path so deal with
		// it.

		if (uLength >= BURGER_ARRAYSIZE(Buffer)) {

			//
			// DAMN YOU MICROSOFT!
			// In case the folder is longer than MAX_PATH, do a binary doubling
			// of a pathname buffer until the function succeeds or the buffers
			// just don't make sense anymore
			//

			// Start with this size
			DWORD uTestLength = BURGER_ARRAYSIZE(Buffer);
			pWBuffer = nullptr;
			do {
				// Release the previous buffer, if any
				Free(pWBuffer);

				// Double the size for this pass
				uTestLength <<= 1U;

				// Should NEVER happen
				if (!uTestLength) {
					pWBuffer = nullptr;
					break;
				}

				// Allocate a new buffer
				pWBuffer = static_cast<uint16_t*>(
					Alloc(uTestLength * sizeof(uint16_t)));
				if (!pWBuffer) {
					uResult = kErrorOutOfMemory;
					break;
				}
				// Try to get the pathname with the bigger buffer
				uLength = GetModuleFileNameW(
					nullptr, reinterpret_cast<LPWSTR>(pWBuffer), uTestLength);
			} while (uLength >= uTestLength);
		}

		// Got a valid pathname?
		if (pWBuffer) {

			// Remove the \\foo.exe at the end of the path to just get the
			// directory
			uint16_t* pLastSlash = StringCharacterReverse(pWBuffer, '\\');
			if (pLastSlash) {
				pLastSlash[0] = 0;
			}

			// Convert to UTF8
			SetFromNative(pWBuffer);

			// Release the buffer (If allocated)
			if (pWBuffer != Buffer) {
				Free(pWBuffer);
			}
			uResult = kErrorNone;
		}
	}

	if (uResult) {
		clear();
	}
	return uResult;
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
	eError uResult = kErrorNotADirectory;

	// Get the length of the directory in WCHARs + the terminating null
	const UINT uLength = GetSystemDirectoryW(nullptr, 0);

	// Should never fail.
	if (uLength) {

		// Reserve space for the incoming string minus the terminating zero
		String16 Temp16;
		uResult = Temp16.resize(uLength - 1);

		if (!uResult) {

			// Read in the directory name and check for failure
			uResult = kErrorNotADirectory;
			if (GetSystemDirectoryW(reinterpret_cast<LPWSTR>(Temp16.data()),
					uLength) == (uLength - 1)) {

				// Find the slash after C: so it becomes C:\\.
				uint16_t* pSlash = StringCharacter(Temp16.data(), '\\');
				if (pSlash) {
					// Trim AFTER the slash
					pSlash[1] = 0;
				}

				// Convert from UTF16 to UTF8, using the modified string
				uResult = SetFromNative(Temp16.c_str());
			}
		}
	}

	if (uResult) {
		clear();
	}
	return uResult;
}

/***************************************

	\brief Set the filename to the local machine preferences directory

	Determine the directory where the user's preferences that are local to the
	machine is located. The path is converted into UTF8 character encoding and
	stored in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::SetMachinePrefsDirectory(
	void) BURGER_NOEXCEPT
{
	eError uResult = kErrorNotADirectory;

	// Try the code for Vista or higher
	uint16_t* pResult = nullptr;
	uint_t hResult = Windows::SHGetKnownFolderPath(&FOLDERID_LocalAppData,
		KF_FLAG_DONT_UNEXPAND | KF_FLAG_DONT_VERIFY, nullptr, &pResult);
	if (hResult == S_OK) {
		// All good! Use this pathname!
		uResult = SetFromNative(pResult);
		// Release the pointer
		CoTaskMemFree(pResult);

	} else {

		// Try it for Windows XP instead
		WCHAR NameBuffer[MAX_PATH];
		// Application system data folder (Local for Vista and Win7)
		hResult = static_cast<uint_t>(SHGetFolderPathW(
			nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, NameBuffer));
		if ((hResult == S_OK) || (hResult == E_FAIL)) {
			// Convert to UTF8
			uResult = SetFromNative(reinterpret_cast<const uint16_t*>(NameBuffer));
		}
	}

	if (uResult) {
		clear();
	}
	return uResult;
}

/***************************************

	\brief Set the filename to the user's preferences directory

	Determine the directory where the user's preferences that could be shared
	among all machines the user has an account with is located. The path is
	converted into UTF8 character encoding and stored in Burgerlib filename
	format.

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::SetUserPrefsDirectory(
	void) BURGER_NOEXCEPT
{
	eError uResult = kErrorNotADirectory;

	// Try the code for Vista or higher
	uint16_t* pResult = nullptr;
	uint_t hResult = Windows::SHGetKnownFolderPath(&FOLDERID_RoamingAppData,
		KF_FLAG_DONT_UNEXPAND | KF_FLAG_DONT_VERIFY, nullptr, &pResult);
	if (hResult == S_OK) {
		// All good! Use this pathname!
		uResult = SetFromNative(pResult);
		// Release the pointer
		CoTaskMemFree(pResult);
	} else {

		// Try it for Windows XP instead
		WCHAR NameBuffer[MAX_PATH];
		// Application data folder (Roaming for Vista and Win7)
		hResult = static_cast<uint_t>(
			SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, NameBuffer));
		if ((hResult == S_OK) || (hResult == E_FAIL)) {
			// Convert to UTF8
			uResult = SetFromNative(reinterpret_cast<const uint16_t *>(NameBuffer));
		}
	}
	if (uResult) {
		clear();
	}
	return uResult;
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
	if (!pInput || !pInput[0]) { // No directory at all?
		pInput = ".";            // Just get the current directory
	}

	// First thing, convert it to UTF16

	WCHAR InputPath[512];
	WCHAR* pInputPath;
	uintptr_t uInputLength = UTF16::TranslateFromUTF8(
		reinterpret_cast<uint16_t*>(InputPath), BURGER_ARRAYSIZE(InputPath), pInput);
	if (uInputLength >= BURGER_ARRAYSIZE(InputPath)) {
		pInputPath =
			static_cast<WCHAR*>(Alloc((uInputLength + 2) * sizeof(WCHAR)));
		if (!pInputPath) {
			return kErrorOutOfMemory;
		}
		uInputLength =
			UTF16::TranslateFromUTF8(reinterpret_cast<uint16_t*>(pInputPath),
				uInputLength + 2, pInput);
	} else {
		pInputPath = InputPath;
	}

	// Now that it's UTF16, let's have Windows expand it

	WCHAR ExpandedPath[512];
	WCHAR* pExpanded;
	// Have windows expand it out
	uintptr_t uExpandedLength = GetFullPathNameW(
		pInputPath, BURGER_ARRAYSIZE(ExpandedPath), ExpandedPath, nullptr);

	if (uExpandedLength >= BURGER_ARRAYSIZE(ExpandedPath)) {
		pExpanded =
			static_cast<WCHAR*>(Alloc((uExpandedLength + 2) * sizeof(WCHAR)));
		if (pExpanded) {
			uExpandedLength = GetFullPathNameW(pInputPath,
				static_cast<DWORD>(uExpandedLength + 2), pExpanded, nullptr);
		}
	} else {
		pExpanded = ExpandedPath;
	}

	// I don't need the original anymore
	if (pInputPath != InputPath) {
		Free(pInputPath);
	}

	// Was there a memory error above?
	if (!pExpanded) {
		// Fudge
		return kErrorOutOfMemory;
	}
	m_NativeFilename.assign(reinterpret_cast<const uint16_t *>(pExpanded));

	// How long would the string be if it was UTF8?

	const uintptr_t uOutputLength =
		UTF8::GetUTF16Size(reinterpret_cast<uint16_t*>(pExpanded));
	eError uResult = m_Filename.reserve(uOutputLength + 6);
	if (uResult) {
		return uResult;
	}

	WCHAR* pSrc = pExpanded;

	// Network name?
	if ((uExpandedLength >= 2) && (pSrc[0] == '\\') && (pSrc[1] == '\\')) {
		// Leading colon
		m_Filename.assign(':');
		// Only return 1 colon
		pSrc += 2;                
	} else {
		// Get the drive letter
		uint_t uTemp = static_cast<uint_t>(pSrc[0]);
		// Upper case
		if ((uTemp >= 'a') && (uTemp < ('z' + 1))) { 
			uTemp &= 0xDF;
		}
		uTemp = uTemp - 'A';
		pSrc += 3; // Accept the "C:\"

		// At this point I have the drive number, create the drive number prefix

		m_Filename.assign('.'); // .D2 for C:
		m_Filename.push_back('D');
		NumberString DriveNumber(static_cast<uint32_t>(uTemp));
		m_Filename.append(DriveNumber);

		// Append a colon
		m_Filename.push_back(':');
	}

	// Append the filename to output and convert from UTF16 to UTF8
	m_Filename.append(reinterpret_cast<uint16_t*>(pSrc));

	if (pExpanded != ExpandedPath) {
		Free(pExpanded);
	}

	uint8_t* pSlasher = reinterpret_cast<uint8_t *>(m_Filename.c_str());
	uint_t uTemp2 = pSlasher[0];
	if (uTemp2) {
		do {
			if (uTemp2 == '\\') { // Convert directory holders
				uTemp2 = ':';     // To generic paths
			}
			// Save char
			pSlasher[0] = static_cast<uint8_t>(uTemp2);
			++pSlasher;
			uTemp2 = pSlasher[0]; // Next char
		} while (uTemp2);        // Still more?
	}

	// The wrap up...
	// Make sure it's appended with a colon
	m_bNativeValid = TRUE;
	return end_with_colon();
}

#endif
