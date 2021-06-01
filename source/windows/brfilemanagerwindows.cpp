/***************************************

    Win95 version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_WINDOWS)
#include "brfile.h"
#include "brutf8.h"
#include "brstring16.h"

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
extern "C" FILE * _MSL_CDECL _wfopen(const wchar_t * _MSL_RESTRICT name, const wchar_t * _MSL_RESTRICT mode);
#endif
#if defined(BURGER_WATCOM)
#define std
#endif

// Friggin' windows!
#undef DeleteFile
#undef CopyFile

/***************************************

	Given a drive number, return in generic format
	the drive's name.

***************************************/

uint_t BURGER_API Burger::FileManager::GetVolumeName(Filename *pOutput,uint_t uVolumeNum) BURGER_NOEXCEPT
{
	if (pOutput) {
		pOutput->Clear();
	}

	uint_t uResult = kErrorInvalidParameter;		// Assume error
	if (uVolumeNum<32) {
		uResult = kErrorFileNotFound;
		// Only query drives that exist
		DWORD uDriveMask = GetLogicalDrives();
		if ((1U<<uVolumeNum)&uDriveMask) {
			WCHAR OutputNames[MAX_PATH];	// Buffer to copy to + two colons and a terminating zero
			WCHAR InputName[4];				// Drive name template ( "C:\\" )

			InputName[0] = static_cast<WCHAR>('A'+uVolumeNum);	// Create "C:\\" 
			InputName[1] = ':';
			InputName[2] = '\\';
			InputName[3] = 0;

			// Prevent Windows from showing a dialog asking for a disk to be inserted
			UINT uOldMode = SetErrorMode(SEM_FAILCRITICALERRORS);
			// Get the volume name from windows
			if (GetVolumeInformationW(InputName,OutputNames,(sizeof(OutputNames)/2),
				NULL,NULL,NULL,NULL,0)) {
				uResult = kErrorNone;	// No error!
				// Did I want the output name?
				if (pOutput) {
					char OutputName[(MAX_PATH*2)+3];
					UTF8::FromUTF16(OutputName+1,sizeof(OutputName)-3,reinterpret_cast<const uint16_t*>(OutputNames));
					OutputName[0] = ':';
					uintptr_t uLength = StringLength(OutputName+1);
					OutputName[uLength+1] = ':';
					OutputName[uLength+2] = 0;
					pOutput->Set(OutputName);
				}
			}
			// Restore Window's disk dialog mode
			SetErrorMode(uOldMode);
		}
	}
	return uResult;			// Return the error
}

/***************************************

	Set the initial default prefixes for a power up state
	*: = Boot volume
	$: = System folder
	@: = Prefs folder
	8: = Default directory
	9: = Application directory

***************************************/

#define _argv __argv
#if defined(__MWERKS__)
#ifdef __cplusplus
extern "C" {
#endif
extern char **__argv;
#ifdef __cplusplus
}
#endif
#endif

void BURGER_API Burger::FileManager::DefaultPrefixes(void)
{
	WCHAR NameBuffer[MAX_PATH];
	Filename MyFilename;

	MyFilename.SetFromNative("");	// Get the current working directory
	SetPrefix(8,MyFilename.GetPtr());		// Set the standard work prefix 

	// Ask windows what's my app's file ID.

	if (GetModuleFileNameW(NULL,reinterpret_cast<LPWSTR>(NameBuffer),MAX_PATH)<MAX_PATH) {
		String ExePath(reinterpret_cast<uint16_t *>(NameBuffer));
		MyFilename.SetFromNative(ExePath.GetPtr());
	} else {
		// Fall back, use the argument list
		MyFilename.SetFromNative(_argv[0]);	// Extract the application's directory
	}
	MyFilename.DirName();							// Get the application's name
	SetPrefix(9,MyFilename.GetPtr());				// Set the application prefix

	// Get the system folder as well as the boot drive

	// System folder in Unicode
	HRESULT uResult = SHGetFolderPathW(NULL,CSIDL_SYSTEM,NULL,0,NameBuffer);
	if ((uResult==S_OK) || (uResult==E_FAIL)) {
		// Convert to UTF8
		String MyName1(reinterpret_cast<const uint16_t*>(NameBuffer));
		MyFilename.SetFromNative(MyName1);
		char *pFilename = MyFilename.GetPtr();
		char *pEndColon = StringCharacter(pFilename,':');
		if (pEndColon) {
			pEndColon[1] = 0;								// ".D3:xxxx" is now ".D3"
		}
		SetPrefix(FileManager::PREFIXBOOT,pFilename);	// Set the boot volume
	}

	// Application system data folder (Local for Vista and Win7)
	uResult = SHGetFolderPathW(NULL,CSIDL_LOCAL_APPDATA,NULL,0,NameBuffer);
	if ((uResult==S_OK) || (uResult==E_FAIL)) {
		// Convert to UTF8
		String MyName2(reinterpret_cast<const uint16_t*>(NameBuffer));
		MyFilename.SetFromNative(MyName2);
		SetPrefix(FileManager::PREFIXSYSTEM,MyFilename.GetPtr());	// Set the system folder
	}

	// Application data folder (Roaming for Vista and Win7)
	uResult = SHGetFolderPathW(NULL,CSIDL_APPDATA,NULL,0,NameBuffer);
	if ((uResult==S_OK) || (uResult==E_FAIL)) {
		// Convert to UTF8
		String MyName3(reinterpret_cast<const uint16_t*>(NameBuffer));
		MyFilename.SetFromNative(MyName3);
		SetPrefix(FileManager::PREFIXPREFS,MyFilename.GetPtr());	// Set the system folder
	}
}

/***************************************

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

uint_t BURGER_API Burger::FileManager::GetModificationTime(Filename *pFileName,TimeDate_t *pOutput)
{
	// Clear out the output
	pOutput->Clear();	
	WIN32_FIND_DATAW FindData;
	HANDLE FileHandle;
	{
		String16 WideString(pFileName->GetNative());
		FileHandle = FindFirstFileW(reinterpret_cast<const WCHAR*>(WideString.GetPtr()),&FindData);
	}
	uint_t uResult = TRUE;
	if (FileHandle!=INVALID_HANDLE_VALUE) {
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

uint_t BURGER_API Burger::FileManager::GetCreationTime(Filename *pFileName,TimeDate_t *pOutput)
{
	// Clear out the output
	pOutput->Clear();	
	WIN32_FIND_DATAW FindData;
	HANDLE FileHandle;
	{
		String16 WideString(pFileName->GetNative());
		FileHandle = FindFirstFileW(reinterpret_cast<const WCHAR*>(WideString.GetPtr()),&FindData);
	}
	uint_t uResult = TRUE;
	if (FileHandle!=INVALID_HANDLE_VALUE) {
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

uint_t BURGER_API Burger::FileManager::DoesFileExist(Filename *pFileName) BURGER_NOEXCEPT
{
	String16 WideString(pFileName->GetNative());
	// Get file info
	DWORD uOutput = GetFileAttributesW(reinterpret_cast<const WCHAR *>(WideString.GetPtr()));
	uint_t uResult = TRUE;		// File exists
	if ((uOutput & FILE_ATTRIBUTE_DIRECTORY) /* || (uOutput == -1) */ ) { // -1 means error
		uResult = FALSE;		// Bad file!
	}
	return uResult;
}

/***************************************

	Create a directory path using an operating system native name
	Return FALSE if successful, or TRUE if an error

***************************************/

static uint_t BURGER_API DirCreate(const uint16_t *pFileName)
{
	SECURITY_ATTRIBUTES MySec;
	MySec.nLength = sizeof(MySec);		// Must set the size
	MySec.lpSecurityDescriptor = 0;		// No extra data
	MySec.bInheritHandle = FALSE;		// Don't do anything special
	if (!CreateDirectoryW(reinterpret_cast<const WCHAR *>(pFileName),&MySec)) {	// Make the directory
		if (GetLastError()!=ERROR_ALREADY_EXISTS) {		// Already exist?
			return TRUE;		// Error
		}
	}
	return FALSE;		// Success!
}

uint_t BURGER_API Burger::FileManager::CreateDirectoryPath(Filename *pFileName)
{
	String16 NewName(pFileName->GetNative());

	// Easy way!
	if (!DirCreate(NewName.GetPtr())) {	
		return FALSE;				// No error
	}
    // Ok see if I can create the directory tree
    uint16_t *pNewFilename = NewName.GetPtr();
	if (pNewFilename[0]) {			// Is there a filename?
		uint16_t *pWork = pNewFilename;
		if (pWork[0] && pWork[1]==':') {	// Drive name?
			pWork+=2;				// Skip the drive name
		}
		if (pWork[0] == '\\') {		// Accept the first slash
			++pWork;
		}
		uint_t Err;					// Error code
		uint_t Old;
		do {
			// Skip to the next colon
			pWork = reinterpret_cast<uint16_t *>(wcschr(reinterpret_cast<wchar_t *>(pWork),'\\'));
			if (!pWork) {			// No colon found?
				pWork = reinterpret_cast<uint16_t *>(wcschr(reinterpret_cast<wchar_t *>(pNewFilename),0));
			}
			Old = pWork[0];		// Get the previous char
			pWork[0] = 0;		// End the string
			Err = DirCreate(pNewFilename);			// Create the directory
			pWork[0] = static_cast<uint16_t>(Old);		// Restore the string
			++pWork;			// Index past the char
		} while (Old);			// Still more string?
		if (!Err) {				// Cool!!
			return FALSE;		// No error
		}
	}
	return TRUE;		/* Didn't do it! */
}

/***************************************

	Delete a file using native file system

***************************************/

uint_t BURGER_API Burger::FileManager::DeleteFile(Filename *pFileName) BURGER_NOEXCEPT
{
	String16 MyName(pFileName->GetNative());
	// Did it fail?
	uint_t uResult = FALSE;		// Assume succeed
	if (!DeleteFileW(reinterpret_cast<const WCHAR *>(MyName.GetPtr()))) {
		// Try to delete a directory
		if (!RemoveDirectoryW(reinterpret_cast<const WCHAR *>(MyName.GetPtr()))) {
			uResult = kErrorFileNotFound;		// I failed!
		}
	}
	return uResult;		// Return the error, if any
}

/***************************************

	Rename a file using full pathnames

***************************************/

uint_t BURGER_API Burger::FileManager::RenameFile(Filename *pNewName,Filename *pOldName)
{
	String16 DestName(pNewName->GetNative());
	String16 SourceName(pOldName->GetNative());
	// Did it fail?
	uint_t uResult = kErrorFileNotFound;		// Assume failure
	if (MoveFileW(reinterpret_cast<const WCHAR *>(SourceName.GetPtr()),
		reinterpret_cast<const WCHAR *>(DestName.GetPtr()))) {
		uResult = kErrorNone;		// I failed!
	}
	return uResult;		// Return the error, if any
}

/***************************************

	Change a directory using long filenames
	This only accepts Native OS filenames

***************************************/

uint_t BURGER_API Burger::FileManager::ChangeOSDirectory(Filename *pDirName)
{
	String16 MyDirName(pDirName->GetNative());
	uint_t uResult = TRUE;	// Assume error
	if (SetCurrentDirectoryW(reinterpret_cast<const WCHAR *>(MyDirName.GetPtr()))) {
		uResult = FALSE;		// Success!
	}
	return uResult;			// Error returned (If any)
}

/***************************************

	Open a file using a native path

***************************************/

FILE * BURGER_API Burger::FileManager::OpenFile(Filename *pFileName,const char *pType)
{
	String16 FinalName(pFileName->GetNative());
	String16 FinalType(pType);
	return _wfopen(reinterpret_cast<const wchar_t *>(FinalName.GetPtr()),reinterpret_cast<const wchar_t *>(FinalType.GetPtr()));		/* Open using standard fopen */
}

/***************************************

	Copy a file using native pathnames

***************************************/

uint_t BURGER_API Burger::FileManager::CopyFile(Filename *pDestName,Filename *pSourceName)
{
	String16 DestName(pDestName->GetNative());
	String16 SrcName(pSourceName->GetNative());

	if (CopyFileW(reinterpret_cast<const WCHAR *>(SrcName.GetPtr()),reinterpret_cast<const WCHAR *>(DestName.GetPtr()),FALSE)) {
		return FALSE;
	}
	return TRUE;
}

/***************************************

	Worker thread for handling file commands

***************************************/

uintptr_t BURGER_API Burger::FileManager::QueueHandler(void *pData)
{
	// Read,Write,Append,Read/Write
	static const DWORD g_Access[4] = { GENERIC_READ,GENERIC_WRITE,GENERIC_WRITE,GENERIC_READ|GENERIC_WRITE };
	static const DWORD g_Share[4] = { FILE_SHARE_READ,FILE_SHARE_WRITE,FILE_SHARE_WRITE,FILE_SHARE_WRITE };
	static const DWORD g_Creation[4] = { OPEN_EXISTING,CREATE_ALWAYS,OPEN_ALWAYS, OPEN_ALWAYS };

	char Temp[128];
	OutputDebugStringA("Start filemanager thread\n");

	// No error at this time
	uint_t uError = 0;
	FileManager *pThis = static_cast<FileManager *>(pData);
	for (;;) {
		// Wait until there's a command in the queue
		pThis->m_PingIOThread.Acquire();
		// Get the command
		Queue_t *pQueue = &pThis->m_IOQueue[pThis->m_uQueueStart];
		File *pFile;

		sprintf(Temp,"Command %u\n",pQueue->m_uIOCommand);
		OutputDebugStringA(Temp);

		// Dispatch the command
		switch (pQueue->m_uIOCommand) {

		// Was the thread requested to shut down?
		case IOCOMMAND_ENDTHREAD:
			return 0;

		// Issue a callback at this location
		case IOCOMMAND_CALLBACK:
			pQueue->m_uLength = uError;
			static_cast<ProcCallback>(pQueue->m_pBuffer)(pQueue);
			uError = 0;		// Release error
			break;

		// Open a file
		case IOCOMMAND_OPEN:
			{
				pFile = pQueue->m_pFile;

				// Convert from UTF 8 to UTF16
				String16 FinalName(pFile->m_Filename.GetNative());

				// Open the file
				File::eFileAccess uAccess = static_cast<File::eFileAccess>(pQueue->m_uLength&3);
				HANDLE hfp = CreateFileW(reinterpret_cast<const WCHAR *>(FinalName.GetPtr()),g_Access[uAccess],g_Share[uAccess],NULL,g_Creation[uAccess],/* FILE_ATTRIBUTE_NORMAL| */ FILE_FLAG_SEQUENTIAL_SCAN,NULL);

				// Success?
				if (hfp==INVALID_HANDLE_VALUE) {
					uError = GetLastError();
				} else {

					// Save the opened file reference
					pFile->m_pFile = hfp;
					uError = 0;

					// Was it an append command?
					if (uAccess==File::kAppend) {

						// Set the file mark to the end of the file
						LARGE_INTEGER uNewPointer;
						uNewPointer.QuadPart = 0;
						if (!SetFilePointerEx(hfp,uNewPointer,NULL,FILE_END)) {
							uError = GetLastError();
						}
					}
				}
				break;
			}

		// Close the file
		case IOCOMMAND_CLOSE:
			{
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
			}
			break;

		// Read in data
		case IOCOMMAND_READ:
			{
				DWORD uRead;
				pFile = pQueue->m_pFile;
				uError = 0;
				HANDLE hfp = pFile->m_pFile;
				if (hfp) {
					if (ReadFile(hfp,pQueue->m_pBuffer,static_cast<DWORD>(pQueue->m_uLength),&uRead,NULL)) {
						uError = uRead;
					}
				}
				break;
			}

		// Write out data
		case IOCOMMAND_WRITE:
			{
				DWORD uWritten;
				pFile = pQueue->m_pFile;
				uError = 0;
				HANDLE hfp = pFile->m_pFile;
				if (hfp) {
					if (WriteFile(hfp,pQueue->m_pBuffer,static_cast<DWORD>(pQueue->m_uLength),&uWritten,NULL)) {
						uError = uWritten;
					}
				}
				break;
			}

		// Seek the file
		case IOCOMMAND_SEEK:
			{
				pFile = pQueue->m_pFile;
				uError = 0;
				HANDLE hfp = pFile->m_pFile;
				LARGE_INTEGER uNewPointer;
				uNewPointer.QuadPart = pQueue->m_uLength;
				if (!SetFilePointerEx(hfp,uNewPointer,NULL,FILE_BEGIN)) {
					uError = GetLastError();
				}
			}
			break;

		// Issue a sync command to signal that
		// this command token was reached

		case IOCOMMAND_SYNC:
			pThis->m_IOThreadSync.Release();
			break;

		default:;
		}

		// Acknowledge that the command was consumed
		pThis->m_uQueueStart = (pThis->m_uQueueStart+1)&(kMaxQueue-1);
	}
}
#endif
