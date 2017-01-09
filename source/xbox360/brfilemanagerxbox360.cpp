/***************************************

	Xbox 360 version

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "brfilemanager.h"

#if defined(BURGER_XBOX360)
#include "brfile.h"
#define NOD3D
#define NONET
#include <xtl.h>
// Friggin' windows!
#undef DeleteFile
#undef CopyFile

/***************************************

	Given a drive number, return in generic format
	the drive's name.

***************************************/

Word BURGER_API Burger::FileManager::GetVolumeName(Filename *pOutput,Word uVolumeNum)
{
	if (pOutput) {
		pOutput->Clear();
	}

	Word uResult = File::OUTOFRANGE;		// Assume error
	if (uVolumeNum<32) {
		uResult = File::FILENOTFOUND;
		// Only query drives that exist
		char OutputNames[MAX_PATH];	// Buffer to copy to + two colons and a terminating zero
		char InputName[4];				// Drive name template ( "C:\\" )

		InputName[0] = static_cast<char>('A'+uVolumeNum);	// Create "C:\\" 
		InputName[1] = ':';
		InputName[2] = '\\';
		InputName[3] = 0;

		// Get the volume name from windows
		if (GetVolumeInformationA(InputName,OutputNames,(sizeof(OutputNames)/2),
			NULL,NULL,NULL,NULL,0)) {
			uResult = File::OKAY;	// No error!
			// Did I want the output name?
			if (pOutput) {
				char OutputName[(MAX_PATH*2)+3];
				StringCopy(OutputName+1,sizeof(OutputName)-3,OutputNames);
				OutputName[0] = ':';
				WordPtr uLength = StringLength(OutputName+1);
				OutputName[uLength+1] = ':';
				OutputName[uLength+2] = 0;
				pOutput->Set(OutputName);
			}
		}
	}
	return uResult;			// Return the error
}

/***************************************

	Set the initial default prefixs for a power up state
	*: = Boot volume
	$: = System folder
	@: = Prefs folder
	8: = Default directory
	9: = Application directory

***************************************/

void BURGER_API Burger::FileManager::DefaultPrefixes(void)
{
	Filename MyFilename(":D:");

	SetPrefix(8,&MyFilename);		// Set the standard work prefix 
	SetPrefix(9,&MyFilename);		// Set the application prefix
	// Set the system folder
	SetPrefix(FileManager::PREFIXSYSTEM,&MyFilename);
	// Boot volume is the same
	SetPrefix(FileManager::PREFIXBOOT,&MyFilename);
	SetPrefix(FileManager::PREFIXPREFS,&MyFilename);
}

/***************************************

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Word BURGER_API Burger::FileManager::GetModificationTime(Filename *pFileName,TimeDate_t *pOutput)
{
	// Clear out the output
	pOutput->Clear();	
	WIN32_FIND_DATAA FindData;
	HANDLE FileHandle = FindFirstFileA(pFileName->GetNative(),&FindData);
	Word uResult = TRUE;
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

Word BURGER_API Burger::FileManager::GetCreationTime(Filename *pFileName,TimeDate_t *pOutput)
{
	// Clear out the output
	pOutput->Clear();	
	WIN32_FIND_DATAA FindData;
	HANDLE FileHandle = FindFirstFileA(pFileName->GetNative(),&FindData);
	Word uResult = TRUE;
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

Word BURGER_API Burger::FileManager::DoesFileExist(Filename *pFileName)
{
	// Get file info
	DWORD uOutput = GetFileAttributesA(pFileName->GetNative());
	Word uResult = TRUE;		// File exists
	if ((uOutput & FILE_ATTRIBUTE_DIRECTORY) /* || (uOutput == -1) */ ) { // -1 means error
		uResult = FALSE;		// Bad file!
	}
	return uResult;
}

/***************************************

	Create a directory path using an operating system native name
	Return FALSE if successful, or TRUE if an error

***************************************/

static Word BURGER_API DirCreate(const char *pFileName)
{
	if (!CreateDirectoryA(pFileName,NULL)) {	// Make the directory
		if (GetLastError()!=ERROR_ALREADY_EXISTS) {		// Already exist?
			return TRUE;		// Error
		}
	}
	return FALSE;		// Success!
}

Word BURGER_API Burger::FileManager::CreateDirectoryPath(Filename *pFileName)
{
	String NewName(pFileName->GetNative());

	// Easy way!
	if (!DirCreate(NewName.GetPtr())) {	
		return FALSE;				// No error
	}
	// Ok see if I can create the directory tree
	char *pNewFilename = NewName.GetPtr();
	if (pNewFilename[0]) {			// Is there a filename?
		char *pWork = pNewFilename;
		if (pWork[0] && pWork[1]==':') {	// Drive name?
			pWork+=2;				// Skip the drive name
		}
		if (pWork[0] == '\\') {		// Accept the first slash
			++pWork;
		}
		Word Err;					// Error code
		Word Old;
		do {
			// Skip to the next colon
			pWork = StringCharacter(pWork,'\\');
			if (!pWork) {			// No colon found?
				pWork = pNewFilename+StringLength(pNewFilename);
			}
			Old = pWork[0];		// Get the previous char
			pWork[0] = 0;		// End the string
			Err = DirCreate(pNewFilename);			// Create the directory
			if (Err) {
				break;
			}
			pWork[0] = static_cast<char>(Old);		// Restore the string
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

Word BURGER_API Burger::FileManager::DeleteFile(Filename *pFileName)
{
	// Did it fail?
	Word uResult = FALSE;		// Assume succeed
	if (!DeleteFileA(pFileName->GetNative())) {
		// Try to delete a directory
		if (!RemoveDirectoryA(pFileName->GetNative())) {
			uResult = File::FILENOTFOUND;		// I failed!
		}
	}
	return uResult;		// Return the error, if any
}

/***************************************

	Rename a file using full pathnames

***************************************/

Word BURGER_API Burger::FileManager::RenameFile(Filename *pNewName,Filename *pOldName)
{
	// Did it fail?
	Word uResult = File::FILENOTFOUND;		// Assume failure
	if (MoveFileA(pOldName->GetNative(),pNewName->GetNative())) {
		uResult = File::OKAY;		// I failed!
	}
	return uResult;		// Return the error, if any
}

/***************************************

	Copy a file using native pathnames

***************************************/

Word BURGER_API Burger::FileManager::CopyFile(Filename *pDestName,Filename *pSourceName)
{
	if (CopyFileA(pSourceName->GetNative(),pDestName->GetNative(),FALSE)) {
		return FALSE;
	}
	return TRUE;
}

#endif