/***************************************

	File Manager Class: Windows version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_WINDOWS)
#include "brfile.h"
#include "brstring16.h"
#include "brutf8.h"

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <shlobj.h>

#if !defined(BURGER_METROWERKS)
#include <io.h>
#include <stdlib.h>
#else
#include <wchar.h>
// For some wacky reason, this function is in the Codewarrior libraries, yet
// it isn't exposed in a single header. I know, I searched.
extern "C" FILE* _MSL_CDECL _wfopen(
	const wchar_t* _MSL_RESTRICT name, const wchar_t* _MSL_RESTRICT mode);
#endif

#if defined(BURGER_WATCOM)
#define std
#endif

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
	if (uVolumeNum < 26) {

		// Check if the drive even exists.
		uResult = kErrorVolumeNotFound;
		const DWORD uDriveMask = GetLogicalDrives();
		if ((1U << uVolumeNum) & uDriveMask) {

			// Drive name template ( "C:\\" )
			WCHAR InputName[4];
			InputName[0] = static_cast<WCHAR>('A' + uVolumeNum);
			InputName[1] = ':';
			InputName[2] = '\\';
			InputName[3] = 0;

			// Prevent Windows from showing a dialog asking for a disk to be
			// inserted
			const UINT uOldMode = SetErrorMode(SEM_FAILCRITICALERRORS);

			// Buffer to copy to + two colons and a terminating zero
			WCHAR OutputNames[MAX_PATH];

			// Get the volume name from windows
			if (!GetVolumeInformationW(InputName, OutputNames,
					BURGER_ARRAYSIZE(OutputNames), nullptr, nullptr, nullptr,
					nullptr, 0)) {

				// Process the error code
				GetLastError();
			} else {

				// Was the volume name requested?
				if (pOutput) {

					// Convert from UTF16 to UTF8
					char OutputName[(MAX_PATH * 2) + 3];
					UTF8::FromUTF16(OutputName + 1, sizeof(OutputName) - 3,
						reinterpret_cast<const uint16_t*>(OutputNames));

					// Prefix the volume name with a colon
					OutputName[0] = ':';
					uintptr_t uLength = StringLength(OutputName + 1);

					// No volume name? Make a fake one based on drive letter.
					if (!uLength) {
						// Generic disk name
						StringCopy(OutputName + 1, "C_DRIVE");
						OutputName[1] = static_cast<char>('A' + uVolumeNum);
						uLength = 7;
					}
					// End with a colon
					OutputName[uLength + 1] = ':';
					OutputName[uLength + 2] = 0;
					pOutput->assign(OutputName);
				}

				// No error!
				uResult = kErrorNone;
			}

			// Restore Window's disk dialog mode
			SetErrorMode(uOldMode);
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
	WIN32_FIND_DATAW FindData;
	HANDLE FileHandle;
	{
		String16 WideString(pFileName->get_native());
		FileHandle = FindFirstFileW(
			reinterpret_cast<const WCHAR*>(WideString.GetPtr()), &FindData);
	}
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
	WIN32_FIND_DATAW FindData;
	HANDLE FileHandle;
	{
		String16 WideString(pFileName->get_native());
		FileHandle = FindFirstFileW(
			reinterpret_cast<const WCHAR*>(WideString.GetPtr()), &FindData);
	}
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
	String16 WideString(pFileName->get_native());
	// Get file info
	DWORD uOutput =
		GetFileAttributesW(reinterpret_cast<const WCHAR*>(WideString.GetPtr()));
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

static uint_t BURGER_API DirCreate(const uint16_t* pFileName) BURGER_NOEXCEPT
{
	SECURITY_ATTRIBUTES MySec;
	MySec.nLength = sizeof(MySec);  // Must set the size
	MySec.lpSecurityDescriptor = 0; // No extra data
	MySec.bInheritHandle = FALSE;   // Don't do anything special
	if (!CreateDirectoryW(reinterpret_cast<const WCHAR*>(pFileName),
			&MySec)) {                                // Make the directory
		if (GetLastError() != ERROR_ALREADY_EXISTS) { // Already exist?
			return TRUE;                              // Error
		}
	}
	return FALSE; // Success!
}

Burger::eError BURGER_API Burger::FileManager::create_directory_path(
	Filename* pFileName) BURGER_NOEXCEPT
{
	String16 NewName(pFileName->get_native());

	// Easy way!
	if (!DirCreate(NewName.GetPtr())) {
		return kErrorNone; // No error
	}
	// Ok see if I can create the directory tree
	uint16_t* pNewFilename = NewName.GetPtr();
	if (pNewFilename[0]) { // Is there a filename?
		uint16_t* pWork = pNewFilename;
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
			pWork = reinterpret_cast<uint16_t*>(
				wcschr(reinterpret_cast<wchar_t*>(pWork), '\\'));
			if (!pWork) { // No colon found?
				pWork = reinterpret_cast<uint16_t*>(
					wcschr(reinterpret_cast<wchar_t*>(pNewFilename), 0));
			}
			Old = pWork[0];                        // Get the previous char
			pWork[0] = 0;                          // End the string
			Err = DirCreate(pNewFilename);         // Create the directory
			pWork[0] = static_cast<uint16_t>(Old); // Restore the string
			++pWork;                               // Index past the char
		} while (Old);                             // Still more string?
		if (!Err) {                                // Cool!!
			return kErrorNone;                     // No error
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
	String16 MyName(pFileName->get_native());

	// Assume succeed
	eError uResult = kErrorNone;
	if (!DeleteFileW(reinterpret_cast<const WCHAR*>(MyName.c_str()))) {
		// Try to delete a directory
		if (!RemoveDirectoryW(reinterpret_cast<const WCHAR*>(MyName.c_str()))) {
			// I failed!
			uResult = kErrorFileNotFound;
		}
	}
	// Return the error, if any
	return uResult;
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
	String16 DestName(pNewName->get_native());
	String16 SourceName(pOldName->get_native());
	// Did it fail?
	eError uResult = kErrorFileNotFound; // Assume failure
	if (MoveFileW(reinterpret_cast<const WCHAR*>(SourceName.GetPtr()),
			reinterpret_cast<const WCHAR*>(DestName.GetPtr()))) {
		uResult = kErrorNone; // I failed!
	}
	return uResult; // Return the error, if any
}

/***************************************

	\brief Change the OS working directory using an OS native pathname..

	Given an OS native pathname, set the OS's current working directory to this
	path.

	This function is useful in cases where an OS native file dialog to select a
	file or perform an OS native function where the current working directory
	needs to be preset. This function under most circumstances is not necessary
	to use, however, it exists since there are some OS native functions in MacOS
	and Windows that require a little "help".

	\note On most systems, this is the same as calling cwd().

	\param pDirName Pointer to a \ref Filename of a native OS path.

	\return Zero if successful, non-zero on error.

	\sa change_OS_directory(const char*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::change_OS_directory(
	Filename* pDirName)
{
	String16 MyDirName(pDirName->get_native());
	eError uResult = kErrorIO; // Assume error
	if (SetCurrentDirectoryW(
			reinterpret_cast<const WCHAR*>(MyDirName.GetPtr()))) {
		uResult = kErrorNone; // Success!
	}
	return uResult; // Error returned (If any)
}

/***************************************

	\brief Open a file using stdio.

	Given a BurgerLib pathname, open a file as if fopen() supported BurgerLib
	pathnames,

	This is a cross platform method for opening a standard "C" library file
	stream. As such, this pointer must eventually be closed with a call to
	fclose().

	The second parameter is passed as is to a subsequent call to fopen(). See
	the docs on fopen() on valid input such as "rb" for read binary and "w" for
	write text file.

	\param pFileName Pointer to a "C" string of a BurgerLib path.
	\param pType Pointer to a "C" string that will be passed to a call to
		fopen().

	\return \ref nullptr on error, a valid FILE * compatible with stdio.h file
		calls.

	\sa \ref File

***************************************/

FILE* BURGER_API Burger::FileManager::open_file(
	Filename* pFileName, const char* pType) BURGER_NOEXCEPT
{
	String16 FinalName(pFileName->get_native());
	String16 FinalType(pType);

	// Open using standard fopen
	return _wfopen(reinterpret_cast<const wchar_t*>(FinalName.c_str()),
		reinterpret_cast<const wchar_t*>(FinalType.c_str()));
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
	Filename* pDestName, Filename* pSourceName) BURGER_NOEXCEPT
{
	String16 DestName(pDestName->get_native());
	String16 SrcName(pSourceName->get_native());

	if (CopyFileW(reinterpret_cast<const WCHAR*>(SrcName.c_str()),
			reinterpret_cast<const WCHAR*>(DestName.c_str()), FALSE)) {
		return kErrorNone;
	}
	return kErrorIO;
}

/***************************************

	Worker thread for handling file commands

***************************************/

uintptr_t BURGER_API Burger::FileManager::queue_handler(
	void* pData) BURGER_NOEXCEPT
{
	// Read,Write,Append,Read/Write
	static const DWORD g_Access[4] = {GENERIC_READ, GENERIC_WRITE,
		GENERIC_WRITE, GENERIC_READ | GENERIC_WRITE};
	static const DWORD g_Share[4] = {
		FILE_SHARE_READ, FILE_SHARE_WRITE, FILE_SHARE_WRITE, FILE_SHARE_WRITE};
	static const DWORD g_Creation[4] = {
		OPEN_EXISTING, CREATE_ALWAYS, OPEN_ALWAYS, OPEN_ALWAYS};

	char Temp[128];
	OutputDebugStringA("Start filemanager thread\n");

	// No error at this time
	uint_t uError = 0;
	FileManager* pThis = static_cast<FileManager*>(pData);
	for (;;) {
		// Wait until there's a command in the queue
		pThis->m_PingIOThread.Acquire();
		// Get the command
		Queue_t* pQueue = &pThis->m_IOQueue[pThis->m_uQueueStart];
		File* pFile;

		sprintf(Temp, "Command %u\n", pQueue->m_uIOCommand);
		OutputDebugStringA(Temp);

		// Dispatch the command
		switch (pQueue->m_uIOCommand) {

		// Was the thread requested to shut down?
		case kIOCommandEndThread:
			return 0;

		// Issue a callback at this location
		case kIOCommandCallback:
			pQueue->m_uLength = uError;
			static_cast<ProcCallback>(pQueue->m_pBuffer)(pQueue);
			uError = 0; // Release error
			break;

		// Open a file
		case kIOCommandOpen: {
			pFile = pQueue->m_pFile;

			// Convert from UTF 8 to UTF16
			String16 FinalName(pFile->get_filename()->get_native());

			// Open the file
			File::eFileAccess uAccess =
				static_cast<File::eFileAccess>(pQueue->m_uLength & 3);
			HANDLE hfp = CreateFileW(
				reinterpret_cast<const WCHAR*>(FinalName.GetPtr()),
				g_Access[uAccess], g_Share[uAccess], NULL, g_Creation[uAccess],
				/* FILE_ATTRIBUTE_NORMAL| */ FILE_FLAG_SEQUENTIAL_SCAN, NULL);

			// Success?
			if (hfp == INVALID_HANDLE_VALUE) {
				uError = GetLastError();
			} else {

				// Save the opened file reference
				pFile->set_file_pointer(hfp);
				uError = 0;

				// Was it an append command?
				if (uAccess == File::kAppend) {

					// Set the file mark to the end of the file
					LARGE_INTEGER uNewPointer;
					uNewPointer.QuadPart = 0;
					if (!SetFilePointerEx(hfp, uNewPointer, NULL, FILE_END)) {
						uError = GetLastError();
					}
				}
			}
			break;
		}

		// Close the file
		case kIOCommandClose: {
			pFile = pQueue->m_pFile;
			// Close the file
			uError = 0;
			HANDLE hfp = pFile->get_file_pointer();
			// Only call if the handle is valid
			if (hfp) {
				if (!CloseHandle(hfp)) {
					uError = GetLastError();
				}
			}
			// Zap the handle
			pFile->set_file_pointer(nullptr);
		} break;

		// Read in data
		case kIOCommandRead: {
			DWORD uRead;
			pFile = pQueue->m_pFile;
			uError = 0;
			HANDLE hfp = pFile->get_file_pointer();
			if (hfp) {
				if (ReadFile(hfp, pQueue->m_pBuffer,
						static_cast<DWORD>(pQueue->m_uLength), &uRead, NULL)) {
					uError = uRead;
				}
			}
			break;
		}

		// Write out data
		case kIOCommandWrite: {
			DWORD uWritten;
			pFile = pQueue->m_pFile;
			uError = 0;
			HANDLE hfp = pFile->get_file_pointer();
			if (hfp) {
				if (WriteFile(hfp, pQueue->m_pBuffer,
						static_cast<DWORD>(pQueue->m_uLength), &uWritten,
						NULL)) {
					uError = uWritten;
				}
			}
			break;
		}

		// Seek the file
		case kIOCommandSeek: {
			pFile = pQueue->m_pFile;
			uError = 0;
			HANDLE hfp = pFile->get_file_pointer();
			LARGE_INTEGER uNewPointer;
			uNewPointer.QuadPart = pQueue->m_uLength;
			if (!SetFilePointerEx(hfp, uNewPointer, NULL, FILE_BEGIN)) {
				uError = GetLastError();
			}
		} break;

			// Issue a sync command to signal that
			// this command token was reached

		case kIOCommandSync:
			pThis->m_IOThreadSync.Release();
			break;

		default:;
		}

		// Acknowledge that the command was consumed
		pThis->m_uQueueStart = (pThis->m_uQueueStart + 1) & (kMaxQueue - 1);
	}
}
#endif
