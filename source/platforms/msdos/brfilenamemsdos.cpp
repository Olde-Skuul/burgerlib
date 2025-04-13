/***************************************

	Filename Class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_MSDOS)
#include "brfilemanager.h"
#include "brnumberstring.h"
#include "brnumberto.h"

#include "msdos_structs.h"
#include "msdos_memory.h"

#include <direct.h>
#include <dos.h>
#include <stdlib.h>

#if defined(BURGER_WATCOM)
#ifdef __cplusplus
extern "C" {
#endif
// Expose the hidden variable
extern char** _argv;
#ifdef __cplusplus
}
#endif
#endif

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

const char* Burger::Filename::get_native(void) BURGER_NOEXCEPT
{
	// If already parsed, skip the conversion.
	if (!m_bNativeValid) {

		// Resolve prefixes and convert to absolute path
		abs_path();

		// DOS version and Win 95 version
		// I prefer for all paths intended for DOS use a generic drive specifier
		// before the working directory. The problem is that Volume LABEL are
		// very difficult to parse and slow to access.

		// First parse either the volume name of a .DXX device number
		// I hopefully will get a volume number since DOS prefers it

		// Copy to running pointer
		const uint8_t* pPath = reinterpret_cast<uint8_t*>(m_Filename.c_str());

		// Init the default drive number
		uint_t uDeviceNum = UINT32_MAX;

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
			uDeviceNum = FileManager::get_volume_number(VolumeName);

			// Can't find the volume?!?
			if (uDeviceNum == UINT32_MAX) {
				// Ignore the leading colon
				uDeviceNum = UINT32_MAX - 1;
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
		} else {
			// Only device 0-25 are valid (A-Z)
			uDeviceNum = is_drive_number();
			if (uDeviceNum < 26) {
				// Discard accepted input
				pPath = reinterpret_cast<const uint8_t*>(StringCharacter(
							reinterpret_cast<const char*>(pPath), ':')) +
					1;
			} else {
				uDeviceNum = UINT32_MAX;
			}
		}

		// Now that I have the drive number, determine the length
		// of the output buffer and start the conversion
		uintptr_t uPathLength =
			StringLength(reinterpret_cast<const char*>(pPath));
		m_NativeFilename.clear();

		// Insert the prefix, if any, to the output string

		if (uDeviceNum == (UINT32_MAX - 1)) {
			// Since I didn't find the volume name, I'll assume it's
			// a network volume
			m_NativeFilename.push_back('\\');
			m_NativeFilename.push_back('\\');
		} else if (uDeviceNum != UINT32_MAX) {
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

		// It's valid
		m_bNativeValid = TRUE;
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
	char* pWorkingDirectory = getcwd(nullptr, 0);
	String WorkingDir(pWorkingDirectory);
	if (pWorkingDirectory) {
		free(pWorkingDirectory);
	}

	// Convert from 8.3 to long, if supported.
	eError uResult = FileManager::MSDOS_expand_8_3_filename(&WorkingDir);
	if (!uResult) {
		String UTF8;
		UTF8.assign_win437(WorkingDir.c_str());
		uResult = set_native(UTF8.c_str());
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

Burger::eError BURGER_API Burger::Filename::set_application_directory(
	void) BURGER_NOEXCEPT
{
	// Get the pathname to the executable
	String ProgramName(_argv[0]);

	// Remove the executable name. Example "c:\\dos\\foo.exe"
	char* pSlash = StringCharacterReverse(ProgramName.c_str(), '\\');
	if (pSlash) {
		pSlash[0] = 0;
	}

	// Convert from 8.3 to long, if supported.
	eError uResult = FileManager::MSDOS_expand_8_3_filename(&ProgramName);
	if (!uResult) {
		String UTF8;
		UTF8.assign_win437(ProgramName.c_str());
		uResult = set_native(UTF8.c_str());
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

Burger::eError BURGER_API Burger::Filename::set_boot_volume(
	void) BURGER_NOEXCEPT
{
	MSDos::Regs16_t Regs; // Intel registers

	// Get the boot volume DOS 4.0+
	// http://www.ctyme.com/intr/rb-2729.htm
	Regs.ax = 0x3305;
	Regs.dx = 0;
	MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
	uint_t uBootDrive = Regs.dx & 0xFFU;

	// If the drive is zero, the call failed.
	if (!uBootDrive) {
		// If DOS is pre-4.0, get the current drive instead.
		// http://www.ctyme.com/intr/rb-2588.htm
		Regs.ax = 0x1900;
		MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
		// This will assumed to be the boot drive
		uBootDrive = Regs.ax & 0xFFU;
	} else {
		// Convert to 0-25 from 1-26
		--uBootDrive;
	}

	// Convert to .D<DriveNumber>:
	NumberString BootDrive(uBootDrive);
	String Drive(".D", BootDrive.c_str(), ":");
	return assign(Drive.c_str());
}

/***************************************

	\brief Set the filename to the local machine preferences directory

	Determine the directory where the user's preferences that are local to the
	machine is located. The path is converted into UTF8 character encoding and
	stored in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::set_system_prefs_directory(
	void) BURGER_NOEXCEPT
{
	// C:\DOS, perhaps?
	// Start with the boot drive.
	eError uResult = set_boot_volume();
	if (!uResult) {

		// Test if C:\DOS\COMMAND.COM exists
		Filename Temp(*this);
		Temp.join("DOS:COMMAND.COM");
		if (FileManager::does_file_exist(&Temp)) {
			// It exists, append DOS to the directory
			uResult = join("DOS");
		}
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

Burger::eError BURGER_API Burger::Filename::set_user_prefs_directory(
	void) BURGER_NOEXCEPT
{
	return set_application_directory();
}

/***************************************

	Convert an MSDOS path to a Burgerlib path

	There is no volume name support in DOS

	Paths without a leading '\' are prefixed with
	the current working directory

	Paths with a drive letter but no leading \ will use
	the drive's current working directory

	If it's a network path "\\" then dispose of the drive
	letter and use the pathname.

	The DOS version converts these types of paths:

	"C:\TEMP\TEMP2" = ".D2:TEMP:TEMP2:"<br>
	"TEMP" = "(working directory from 8):TEMP:"<br>
	"TEMP\TEMP2" = "(working directory from 8):TEMP:TEMP2:"<br>
	"\TEMP" = ".D(Mounted drive number):TEMP:"

	Only available on MSDOS

***************************************/

Burger::eError BURGER_API Burger::Filename::set_native(
	const char* pInput) BURGER_NOEXCEPT
{
	// Used for DOS calls
	MSDos::Regs16_t Regs;

	if (!pInput || !pInput[0]) { // No directory at all?
		// Just get the current directory
		pInput = "";
	}

	// Convert to UTF8 and use it instead
	m_NativeFilename.assign_win437(pInput);
	pInput = m_NativeFilename.c_str();
	uintptr_t uInputLength = m_NativeFilename.length();

	// Adjust the native path to a full path

	// Assume no drive is found
	uint_t uDriveNum = UINT32_MAX;

	// Check if a network name, (\\\\), query MSDOS for the current drive

	if ((uInputLength >= 2) &&
		(reinterpret_cast<const uint8_t*>(pInput)[0] == '\\') &&
		(reinterpret_cast<const uint8_t*>(pInput)[1] == '\\')) {
		// Parse past the volume slashes
		pInput += 1;
		uInputLength -= 1;
	} else {
		// Extract a drive letter
		if (uInputLength >= 2) {
			uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];

			// Upper case
			uTemp = (uTemp & 0xDF) - 'A';

			// Could this be a drive letter?
			if ((uTemp < 26) &&
				(reinterpret_cast<const uint8_t*>(pInput)[1] == ':')) {
				uDriveNum = uTemp;
				// Accept the drive letter
				pInput += 2;
				// Shrink the input string
				uInputLength -= 2;
			}
		}

		// No drive letter found? Find and insert it.
		if (uDriveNum == UINT32_MAX) {
			// Get the default drive number
			_dos_getdrive(&uDriveNum);
			--uDriveNum;
			char DriveLetterPrefix[2] = {
				static_cast<char>('A' + uDriveNum), ':'};
			m_NativeFilename.insert(0, DriveLetterPrefix, 2);
			pInput = m_NativeFilename.c_str() + 2;
		}

		// Check if there is working directory
		// Obtain the current working directory for the current drive

		if (!uInputLength ||
			(reinterpret_cast<const uint8_t*>(pInput)[0] != '\\')) {

			// Requested drive
			Regs.dx = static_cast<uint16_t>(uDriveNum + 1);

			// Get real memory buffer
			uint32_t DosBuffer = MSDos::get_temp_real_buffer();
			// Pass to Dos call
			Regs.ds = static_cast<uint16_t>(DosBuffer >> 16U);
			Regs.si = static_cast<uint16_t>(DosBuffer & 0xFFFFU);

			// First try long version
			Regs.ax = 0x7147;
			// Call DOS
			MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
			// Carry set??
			if (Regs.flags & 1) {
				// Try DOS 2.0 version
				Regs.ax = 0x4700;
				// Get the working directory
				MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
			}

			// If carry is clear, then one of the preceding calls succeeded
			if (!(Regs.flags & 1)) {
				// Insert the delimiting directory slash
				m_NativeFilename.insert(2, "\\", 1);

				// Get the pointer to the current directory
				const char* pWorkingDirectory =
					static_cast<char*>(MSDos::real_to_protected(DosBuffer));
				uintptr_t uWorkingDirectoryLength =
					StringLength(pWorkingDirectory);

				// Is there a directory?
				if (uWorkingDirectoryLength) {
					// Insert it, and add another slash.
					m_NativeFilename.insert(
						2, pWorkingDirectory, uWorkingDirectoryLength);
					m_NativeFilename.insert(2, "\\", 1);
				}

				// Skip past the drive letter and colon
				pInput = m_NativeFilename.c_str() + 2;
				uInputLength = m_NativeFilename.length() - 2;
			}
		}
	}

	// At this point, uInputLength contains the approximate length of the final
	// string. Let's make a buffer to store it! 7 for .D26: and a trailing colon
	// and ending null

	eError uResult = m_Filename.reserve(uInputLength + 7);
	if (uResult) {
		return uResult;
	}

	// Now if the path starts with a '\' then I assume it's fully qualified
	// otherwise, I must insert the working directory for the drive
	// I assume that DriveNum has the current requested drive number

	m_Filename.clear();
	if (uDriveNum != UINT32_MAX) {
		// .D2 for C:
		m_Filename.push_back('.');
		m_Filename.push_back('D');
		NumberString DriveNumber(static_cast<uint32_t>(uDriveNum));
		m_Filename.append(DriveNumber);
	}

	// Append the rest of the filename
	if (uInputLength) {
		do {
			uint_t uTemp = reinterpret_cast<const char*>(pInput)[0];
			++pInput;
			if (uTemp == '\\') { // Convert directory holders
				uTemp = ':';     // To generic paths
			}
			// Save char
			m_Filename.push_back(static_cast<char>(uTemp));
		} while (--uInputLength);
	}
	// The wrap up...
	// Make sure it's appended with a colon
	m_bNativeValid = TRUE;
	return end_with_colon();
}
#endif
