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
// Friggin' windows!
#undef DeleteFile
#undef CopyFile

#if !defined(DOXYGEN)

//
// Actual drive volume names
//
static const char* gVolumeNames[] = {"DVD", "GAME", "DEVKIT", "HDD", "MU0",
	"MU1", "MUINT", "INTUSB", "USBMASS0MU", "USBMASS1MU"};

#endif

/***************************************

	\brief Return the name of a drive

	Given a drive number (0-?), return the name of the volume in the format of
	":Volume name:". The function will guarantee the existence of the colons.

	\note This function should be used with caution. Only mounted drives would
	return immediately and if the drive has ejectable media may take a while for
	it to respond to a volume name query.

	\param pOutput A Burger::Filename structure to contain the filename (Can be
		nullptr)

	\param uVolumeNum A valid drive number from 0-?? with ?? being the
		maximum number of drives in the system

	\return Zero if no error, non-zero if an error occurred

	\sa Burger::FileManager::GetVolumeNumber(const char *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::GetVolumeName(
	Filename* pOutput, uint_t uVolumeNum) BURGER_NOEXCEPT
{
	// Assume invalid drive A-Z
	eError uResult = kErrorInvalidParameter;
	if (uVolumeNum < BURGER_ARRAYSIZE(gVolumeNames)) {

		// Check if the drive even exists.
		uResult = kErrorVolumeNotFound;

		// Drive name template ( "DVD:\\" )
		uintptr_t uLength = StringLength(gVolumeNames[uVolumeNum]);
		char InputName[16];
		MemoryCopy(InputName, gVolumeNames[uVolumeNum], uLength);
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
				MemoryCopy(OutputName + 1, gVolumeNames[uVolumeNum], uLength);

				// End with a colon
				OutputName[uLength + 1] = ':';
				OutputName[uLength + 2] = 0;
				pOutput->Set(OutputName);
			}

			// No error!
			uResult = kErrorNone;
		}
	}

	// If there was an error, generate a fake drive name anyways.
	if (uResult && pOutput) {
		pOutput->Clear();
	}

	// Return the error
	return uResult;
}

/***************************************

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Burger::eError BURGER_API Burger::FileManager::GetModificationTime(
	Filename* pFileName, TimeDate_t* pOutput)
{
	// Clear out the output
	pOutput->Clear();
	WIN32_FIND_DATAA FindData;
	HANDLE FileHandle = FindFirstFileA(pFileName->GetNative(), &FindData);
	eError uResult = kErrorFileNotFound;
	if (FileHandle != INVALID_HANDLE_VALUE) {
		FindClose(FileHandle);
		uResult = pOutput->Load(&FindData.ftLastWriteTime);
	}
	return uResult;
}

/***************************************

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Burger::eError BURGER_API Burger::FileManager::GetCreationTime(
	Filename* pFileName, TimeDate_t* pOutput)
{
	// Clear out the output
	pOutput->Clear();
	WIN32_FIND_DATAA FindData;
	HANDLE FileHandle = FindFirstFileA(pFileName->GetNative(), &FindData);
	eError uResult = kErrorFileNotFound;
	if (FileHandle != INVALID_HANDLE_VALUE) {
		FindClose(FileHandle);
		uResult = pOutput->Load(&FindData.ftCreationTime);
	}
	return uResult;
}

/***************************************

	Determine if a file exists.
	I will return TRUE if the specified path
	is a path to a file that exists, if it doesn't exist
	or it's a directory, I return FALSE.
	Note : I do not check if the file has any data in it.
	Just the existence of the file.

***************************************/

uint_t BURGER_API Burger::FileManager::DoesFileExist(
	Filename* pFileName) BURGER_NOEXCEPT
{
	// Get file info
	DWORD uOutput = GetFileAttributesA(pFileName->GetNative());
	uint_t uResult = TRUE; // File exists
	if ((uOutput &
			FILE_ATTRIBUTE_DIRECTORY) /* || (uOutput == -1) */) { // -1 means
																  // error
		uResult = FALSE;                                          // Bad file!
	}
	return uResult;
}

/***************************************

	Create a directory path using an operating system native name
	Return FALSE if successful, or TRUE if an error

***************************************/

static uint_t BURGER_API DirCreate(const char* pFileName)
{
	if (!CreateDirectoryA(pFileName, NULL)) {         // Make the directory
		if (GetLastError() != ERROR_ALREADY_EXISTS) { // Already exist?
			return TRUE;                              // Error
		}
	}
	return FALSE; // Success!
}

Burger::eError BURGER_API Burger::FileManager::CreateDirectoryPath(
	Filename* pFileName)
{
	String NewName(pFileName->GetNative());

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
				pWork = pNewFilename + StringLength(pNewFilename);
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

	Delete a file using native file system

***************************************/

Burger::eError BURGER_API Burger::FileManager::DeleteFile(
	Filename* pFileName) BURGER_NOEXCEPT
{
	// Did it fail?
	eError uResult = kErrorNone; // Assume succeed
	if (!DeleteFileA(pFileName->GetNative())) {
		// Try to delete a directory
		if (!RemoveDirectoryA(pFileName->GetNative())) {
			uResult = kErrorFileNotFound; // I failed!
		}
	}
	return uResult; // Return the error, if any
}

/***************************************

	Rename a file using full pathnames

***************************************/

Burger::eError BURGER_API Burger::FileManager::RenameFile(
	Filename* pNewName, Filename* pOldName)
{
	// Did it fail?
	eError uResult = kErrorFileNotFound; // Assume failure
	if (MoveFileA(pOldName->GetNative(), pNewName->GetNative())) {
		uResult = kErrorNone; // I failed!
	}
	return uResult; // Return the error, if any
}

/***************************************

	Copy a file using native pathnames

***************************************/

Burger::eError BURGER_API Burger::FileManager::CopyFile(
	Filename* pDestName, Filename* pSourceName)
{
	if (CopyFileA(pSourceName->GetNative(), pDestName->GetNative(), FALSE)) {
		return kErrorNone;
	}
	return kErrorIO;
}

#endif
