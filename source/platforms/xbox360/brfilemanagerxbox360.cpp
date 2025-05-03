/***************************************

	File Manager Class: Xbox 360 version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_XBOX360)
#include "brfile.h"
#include "brmemoryfunctions.h"

#define NOD3D
#define NONET
#include <xtl.h>

#include <xbdm.h>

#if !defined(DOXYGEN)

//
// Actual drive volume names
//
static const char* gVolumeNames[] = {"DVD", "GAME", "DEVKIT", "HDD", "MU0",
	"MU1", "MUINT", "INTUSB", "USBMASS0MU", "USBMASS1MU"};

#endif

/***************************************

	\brief Handle platform specific startup code

	Calls system functions to determine the version, state and several platform
	specific variables to allow the FileManager to run better by pre-caching
	relevant data.

	For MS-DOS, it will determine the version and flavor of MS/DOS this
	application is running. It will also detect DosBox.

	For Linux, it will scan all the mounted volumes for quick access to shared
	volumes.

	For Android and consoles, it will check if there are mounted SD Cards or
	other external data storage devices so the application can be aware of them.

	\sa platform_shutdown() or \ref FileManager

***************************************/

void BURGER_API Burger::FileManager::platform_setup(void) BURGER_NOEXCEPT
{

	// Xbox 360 needs a little love for the file cache

#if defined(NDEBUG)
	// Init the file cache to something small for Release
	XSetFileCacheSize(128 * 1024);
#else
	// Since runtime files can be added, and dev kits have more memory,
	// bump up the file cache size a bit.
	XSetFileCacheSize(1024 * 1024);

	// Mount the dev kit drives
	DmMapDevkitDrive();
#endif
}

/***************************************

	\brief Return the name of a drive

	Given a drive number (0-?), return the name of the volume in the format of
	":Volume name:". The function will guarantee the existence of the colons.

	\note This function should be used with caution. Only mounted drives would
	return immediately and if the drive has ejectable media may take a while for
	it to respond to a volume name query.

	\param pOutput A \ref Filename structure to contain the filename (Can be
		\ref nullptr )

	\param uVolumeNum A valid drive number from 0-?? with ?? being the
		maximum number of drives in the system

	\return Zero if no error, non-zero if an error occurred

	\sa get_volume_number(const char *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::get_volume_name(
	Filename* pOutput, uint_t uVolumeNum) BURGER_NOEXCEPT
{
	// Assume invalid drive A-Z
	eError uResult = kErrorInvalidParameter;
	if (uVolumeNum < BURGER_ARRAYSIZE(gVolumeNames)) {

		// Check if the drive even exists.
		uResult = kErrorVolumeNotFound;

		// Drive name template ( "DVD:\\" )
		uintptr_t uLength = string_length(gVolumeNames[uVolumeNum]);
		char InputName[16];
		memory_copy(InputName, gVolumeNames[uVolumeNum], uLength);
		InputName[uLength] = ':';
		InputName[uLength + 1] = '\\';
		InputName[uLength + 2] = 0;

		// Buffer to copy to + two colons and a terminating zero
		char OutputNames[MAX_PATH];

		// Get the volume name from windows
		if (GetVolumeInformationA(InputName, OutputNames,
				BURGER_ARRAYSIZE(OutputNames), nullptr, nullptr, nullptr,
				nullptr, 0)) {
			uResult = kErrorNone; // No error!
			// Did I want the output name?
			if (pOutput) {

				// Name is ASCII, not UTF8
				char OutputName[(MAX_PATH * 2) + 3];

				// Prefix the volume name with a colon
				OutputName[0] = ':';

				// Device disk name
				memory_copy(OutputName + 1, gVolumeNames[uVolumeNum], uLength);

				// End with a colon
				OutputName[uLength + 1] = ':';
				OutputName[uLength + 2] = 0;
				pOutput->assign(OutputName);
			}

			// No error!
			uResult = kErrorNone;
		}
	}

	// If there was an error, generate a fake drive name anyways.
	if (uResult && pOutput) {
		pOutput->clear();
	}

	// Return the error
	return uResult;
}

/***************************************

	\brief Get a file's last modification time.

	Given a BurgerLib path, access the file and return the time/date that it was
	last modified.

	\param pFileName Pointer to a "C" string to a BurgerLib pathname
	\param pOutput Pointer to an uninitialized TimeDate_t structure that
		will receive the time/date.

	\return Zero on success, non-zero in the event of an error (Usually file not
		found)

	\sa get_modification_time(Filename*, TimeDate_t*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::get_modification_time(
	Filename* pFileName, TimeDate_t* pOutput) BURGER_NOEXCEPT
{
	// Clear out the output
	pOutput->Clear();
	WIN32_FIND_DATAA FindData;
	HANDLE FileHandle = FindFirstFileA(pFileName->get_native(), &FindData);
	eError uResult = kErrorFileNotFound;
	if (FileHandle != INVALID_HANDLE_VALUE) {
		FindClose(FileHandle);
		uResult = pOutput->Load(&FindData.ftLastWriteTime);
	}
	return uResult;
}

/***************************************

	\brief Get a file's creation time.

	Given a native operating system path, access the file and return the
	time/date that it was created.

	\param pFileName Pointer to a \ref Filename class that is properly
		initialized to the native file system.
	\param pOutput Pointer to an uninitialized \ref TimeDate_t structure that
		will receive the time/date.

	\return Zero on success, non-zero in the event of an error (Usually file not
		found)

	\sa get_modification_time(Filename*, TimeDate_t*) or get_creation_time(
		const char*, TimeDate_t*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::get_creation_time(
	Filename* pFileName, TimeDate_t* pOutput) BURGER_NOEXCEPT
{
	// Clear out the output
	pOutput->Clear();
	WIN32_FIND_DATAA FindData;
	HANDLE FileHandle = FindFirstFileA(pFileName->get_native(), &FindData);
	eError uResult = kErrorFileNotFound;
	if (FileHandle != INVALID_HANDLE_VALUE) {
		FindClose(FileHandle);
		uResult = pOutput->Load(&FindData.ftCreationTime);
	}
	return uResult;
}

/***************************************

	\brief Detect for a file's existence using a native pathname.

	Given a OS native pathname, return \ref TRUE if the file exists. \ref FALSE
	if there is an disk error or the file does not exist.

	\note This does not guarantee that the file can be opened due to security
		permissions or the file being on a locked folder. It only guarantees its
		existence.

	\param pFileName Pointer to a \ref Filename which contains a native
		version of the path to the file.

	\return \ref FALSE if the file does not exist, \ref TRUE if the file exists.

	\sa does_file_exist(const char*)

***************************************/

uint_t BURGER_API Burger::FileManager::does_file_exist(
	Filename* pFileName) BURGER_NOEXCEPT
{
	// Get file info
	DWORD uOutput = GetFileAttributesA(pFileName->get_native());
	uint_t uResult = TRUE; // File exists
	if ((uOutput &
			FILE_ATTRIBUTE_DIRECTORY) /* || (uOutput == -1) */) { // -1 means
																  // error
		uResult = FALSE;                                          // Bad file!
	}
	return uResult;
}

/***************************************

	\brief Create a directory path.

	Given an Burgerlib directory, create each and every part of the directory.
	If the directory already exists, or is successfully created, the call is
	considered successful.

	Example:

	If the path c:\\Foo\\Bar exists, a call with "c:\Foo\Bar\Fooey\Temp\" will
	create both "Fooey" and "Temp" in this single call.

	\param pFileName Pointer to a \ref Filename that contains a Burgerlib
		pathname.

	\return Zero if successful, non-zero on error.

	\sa create_directory_path(const char*) or
		create_directory_path_dirname(const char*)

***************************************/

static uint_t BURGER_API DirCreate(const char* pFileName) BURGER_NOEXCEPT
{
	if (!CreateDirectoryA(pFileName, NULL)) {         // Make the directory
		if (GetLastError() != ERROR_ALREADY_EXISTS) { // Already exist?
			return TRUE;                              // Error
		}
	}
	return FALSE; // Success!
}

Burger::eError BURGER_API Burger::FileManager::create_directory_path(
	Filename* pFileName) BURGER_NOEXCEPT
{
	String NewName(pFileName->get_native());

	// Easy way!
	if (!DirCreate(NewName.c_str())) {
		return kErrorNone; // No error
	}
	// Ok see if I can create the directory tree
	char* pNewFilename = NewName.c_str();
	if (pNewFilename[0]) { // Is there a filename?
		char* pWork = pNewFilename;
		if (pWork[0] && pWork[1] == ':') { // Drive name?
			pWork += 2;                    // Skip the drive name
		}
		if (pWork[0] == '\\') { // Accept the first slash
			++pWork;
		}
		uint_t Err; // Error code
		uint_t Old;
		do {
			// Skip to the next colon
			pWork = StringCharacter(pWork, '\\');
			if (!pWork) { // No colon found?
				pWork = pNewFilename + string_length(pNewFilename);
			}
			Old = pWork[0];                // Get the previous char
			pWork[0] = 0;                  // End the string
			Err = DirCreate(pNewFilename); // Create the directory
			if (Err) {
				break;
			}
			pWork[0] = static_cast<char>(Old); // Restore the string
			++pWork;                           // Index past the char
		} while (Old);                         // Still more string?
		if (!Err) {                            // Cool!!
			return kErrorNone;                 // No error
		}
	}
	return kErrorIO; /* Didn't do it! */
}

/***************************************

	\brief Delete a file or empty directory using a \ref Filename.

	Given an OS native pathname, delete the file at the end of the pathname.

	Example:

	If a call with "c:\Foo\Bar\Fooey\Temp.txt" is issued, only the file
	"Temp.txt" is deleted. The rest of the path is left intact. This call will
	not delete non-empty directories, only files. If the file is already
	deleted, an error is returned.

	\param pFileName Pointer to a \ref Filename of a Burgerlib path.

	\return Zero if successful, non-zero on error.

	\sa delete_file(const char*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::delete_file(
	Filename* pFileName) BURGER_NOEXCEPT
{
	// Did it fail?
	eError uResult = kErrorNone; // Assume succeed
	if (!DeleteFileA(pFileName->get_native())) {
		// Try to delete a directory
		if (!RemoveDirectoryA(pFileName->get_native())) {
			uResult = kErrorFileNotFound; // I failed!
		}
	}
	return uResult; // Return the error, if any
}

/***************************************

	\brief Rename a file using \ref Filename.

	Given a \ref Filename formatted old pathname and new pathname, rename the
	file at the end of the pathname.

	\note The source and destination directories must be the same.

	\param pNewName Pointer to a \ref Filename for the new name.
	\param pOldName Pointer to a \ref Filename for the current file name.

	\return Zero if successful, non-zero on error.

	\sa rename_file(const cha *, const char*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::rename_file(
	Filename* pNewName, Filename* pOldName) BURGER_NOEXCEPT
{
	// Did it fail?
	eError uResult = kErrorFileNotFound; // Assume failure
	if (MoveFileA(pOldName->get_native(), pNewName->get_native())) {
		uResult = kErrorNone; // I failed!
	}
	return uResult; // Return the error, if any
}

/***************************************

	\brief Copy a file using OS native pathnames.

	Given an OS native formatted old pathname and new pathname, make a duplicate
	of the file at the end of the pathname.

	\param pDestName Pointer to a \ref Filename of an OS native path for the
		new name.
	\param pSourceName Pointer to a \ref Filename of an OS native path
		for the current file name.

	\return Zero if successful, non-zero on error.

	\sa copy_file(const char*, const char*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::copy_file(
	Filename* pDestName, Filename* pSourceName)
{
	if (CopyFileA(pSourceName->get_native(), pDestName->get_native(), FALSE)) {
		return kErrorNone;
	}
	return kErrorIO;
}

#endif
