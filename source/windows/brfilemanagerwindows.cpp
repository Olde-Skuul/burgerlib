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

// Friggin' windows!
#undef DeleteFile
#undef CopyFile

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
					pOutput->Set(OutputName);
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
	WIN32_FIND_DATAW FindData;
	HANDLE FileHandle;
	{
		String16 WideString(pFileName->GetNative());
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

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Burger::eError BURGER_API Burger::FileManager::GetCreationTime(
	Filename* pFileName, TimeDate_t* pOutput)
{
	// Clear out the output
	pOutput->Clear();
	WIN32_FIND_DATAW FindData;
	HANDLE FileHandle;
	{
		String16 WideString(pFileName->GetNative());
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
	String16 WideString(pFileName->GetNative());
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

	Create a directory path using an operating system native name
	Return FALSE if successful, or TRUE if an error

***************************************/

static uint_t BURGER_API DirCreate(const uint16_t* pFileName)
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

Burger::eError BURGER_API Burger::FileManager::CreateDirectoryPath(
	Filename* pFileName)
{
	String16 NewName(pFileName->GetNative());

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

	Delete a file using native file system

***************************************/

Burger::eError BURGER_API Burger::FileManager::DeleteFile(
	Filename* pFileName) BURGER_NOEXCEPT
{
	String16 MyName(pFileName->GetNative());
	// Did it fail?
	eError uResult = kErrorNone; // Assume succeed
	if (!DeleteFileW(reinterpret_cast<const WCHAR*>(MyName.GetPtr()))) {
		// Try to delete a directory
		if (!RemoveDirectoryW(
				reinterpret_cast<const WCHAR*>(MyName.GetPtr()))) {
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
	String16 DestName(pNewName->GetNative());
	String16 SourceName(pOldName->GetNative());
	// Did it fail?
	eError uResult = kErrorFileNotFound; // Assume failure
	if (MoveFileW(reinterpret_cast<const WCHAR*>(SourceName.GetPtr()),
			reinterpret_cast<const WCHAR*>(DestName.GetPtr()))) {
		uResult = kErrorNone; // I failed!
	}
	return uResult; // Return the error, if any
}

/***************************************

	Change a directory using long filenames
	This only accepts Native OS filenames

***************************************/

Burger::eError BURGER_API Burger::FileManager::ChangeOSDirectory(
	Filename* pDirName)
{
	String16 MyDirName(pDirName->GetNative());
	eError uResult = kErrorIO; // Assume error
	if (SetCurrentDirectoryW(
			reinterpret_cast<const WCHAR*>(MyDirName.GetPtr()))) {
		uResult = kErrorNone; // Success!
	}
	return uResult; // Error returned (If any)
}

/***************************************

	Open a file using a native path

***************************************/

FILE* BURGER_API Burger::FileManager::OpenFile(
	Filename* pFileName, const char* pType)
{
	String16 FinalName(pFileName->GetNative());
	String16 FinalType(pType);
	return _wfopen(reinterpret_cast<const wchar_t*>(FinalName.GetPtr()),
		reinterpret_cast<const wchar_t*>(
			FinalType.GetPtr())); /* Open using standard fopen */
}

/***************************************

	Copy a file using native pathnames

***************************************/

Burger::eError BURGER_API Burger::FileManager::CopyFile(
	Filename* pDestName, Filename* pSourceName)
{
	String16 DestName(pDestName->GetNative());
	String16 SrcName(pSourceName->GetNative());

	if (CopyFileW(reinterpret_cast<const WCHAR*>(SrcName.GetPtr()),
			reinterpret_cast<const WCHAR*>(DestName.GetPtr()), FALSE)) {
		return kErrorNone;
	}
	return kErrorIO;
}

/***************************************

	Worker thread for handling file commands

***************************************/

uintptr_t BURGER_API Burger::FileManager::QueueHandler(
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
			String16 FinalName(pFile->m_Filename.GetNative());

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
				pFile->m_pFile = hfp;
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
			HANDLE hfp = pFile->m_pFile;
			// Only call if the handle is valid
			if (hfp) {
				if (!CloseHandle(hfp)) {
					uError = GetLastError();
				}
			}
			// Zap the handle
			pFile->m_pFile = NULL;
		} break;

		// Read in data
		case kIOCommandRead: {
			DWORD uRead;
			pFile = pQueue->m_pFile;
			uError = 0;
			HANDLE hfp = pFile->m_pFile;
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
			HANDLE hfp = pFile->m_pFile;
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
			HANDLE hfp = pFile->m_pFile;
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
