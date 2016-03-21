/***************************************

	Playstation Vita version

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "brfilemanager.h"

#if defined(BURGER_VITA) || defined(DOXYGEN)
#include "brfile.h"
#include "brstringfunctions.h"
#include "brstring.h"
#include <kernel.h>

/***************************************

	Given a drive number, return in generic format
	the drive's name.

***************************************/

static const char *g_VolumeNames[] = {
	":app0:",
	":ux0:",
	":addcont0:",
	":addcont1:",
	":savedata0:",
	":savedata1:",
	":photo0:",
	":music0:",
	":host0:"
};

Word BURGER_API Burger::FileManager::GetVolumeName(Burger::Filename *pOutput,Word uVolumeNum)
{
	Word uResult;
	if (uVolumeNum<BURGER_ARRAYSIZE(g_VolumeNames)) {
		pOutput->Set(g_VolumeNames[uVolumeNum]);
		uResult = File::OKAY;
	} else {
		// Clear on error
		pOutput->Clear();
		uResult = File::OUTOFRANGE;
	}
	return uResult;
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
	Filename MyFilename;
	Word uResult = GetVolumeName(&MyFilename,0);		// Get the boot volume name
	if (uResult==File::OKAY) {
		// Set the initial prefix
		SetPrefix(PREFIXBOOT,MyFilename.GetPtr());
	}
	SetPrefix(PREFIXCURRENT,":app0:");		// Set the standard work prefix
	SetPrefix(PREFIXAPPLICATION,":app0:");	// Set the standard work prefix
	SetPrefix(PREFIXSYSTEM,":addcont0:");	// Set the standard work prefix
	SetPrefix(PREFIXPREFS,":ux0:");			// Set the standard work prefix
}

/***************************************

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Word BURGER_API Burger::FileManager::GetModificationTime(Burger::Filename *pFileName,Burger::TimeDate_t *pOutput)
{
	// Buffer to hold the attributes and the filename
	SceIoStat Entry;

	// Initialize the attributes list
	MemoryClear(&Entry,sizeof(Entry));

	// Get the directory entry
	int eError = sceIoGetstat(pFileName->GetNative(),&Entry);

	// No errors?

	Word uResult;
	if (eError<SCE_OK) {
		pOutput->Clear();
		uResult = File::FILENOTFOUND;
	} else {
		// Get the file dates
		pOutput->Load(&Entry.st_mtime);
		// It's parsed!
		uResult = File::OKAY;
	}
	return uResult;
}

/***************************************

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Word BURGER_API Burger::FileManager::GetCreationTime(Burger::Filename *pFileName,Burger::TimeDate_t *pOutput)
{
	// Buffer to hold the attributes and the filename
	SceIoStat Entry;

	// Initialize the attributes list
	MemoryClear(&Entry,sizeof(Entry));

	// Get the directory entry
	int eError = sceIoGetstat(pFileName->GetNative(),&Entry);

	// No errors?

	Word uResult;
	if (eError<SCE_OK) {
		pOutput->Clear();
		uResult = File::FILENOTFOUND;
	} else {
		// Get the file dates
		pOutput->Load(&Entry.st_ctime);
		// It's parsed!
		uResult = File::OKAY;
	}
	return uResult;
}

/***************************************

	Determine if a file exists.
	I will return TRUE if the specified path
	is a path to a file that exists, if it doesn't exist
	or it's a directory, I return FALSE.
	Note : I do not check if the file havs any data in it.
	Just the existence of the file.

***************************************/

Word BURGER_API Burger::FileManager::DoesFileExist(Burger::Filename *pFileName)
{
	Word uResult = FALSE;
	// Buffer to hold the attributes and the filename
	SceIoStat MyStat;

	// Initialize the attributes list
	MemoryClear(&MyStat,sizeof(MyStat));

	// Get the directory entry
	int eError = sceIoGetstat(pFileName->GetNative(),&MyStat);
	if (eError>=SCE_OK) {
		// If it succeeded, the file must exist
		uResult = TRUE;
	}
	return uResult;
}

/***************************************

	Create a directory path using an operating system native name
	Return FALSE if successful, or TRUE if an error

***************************************/

Word BURGER_API Burger::FileManager::CreateDirectoryPath(Burger::Filename *pFileName)
{
	// Assume an eror condition
	Word uResult = File::IOERROR;
	// Get the full path
	const char *pPath = pFileName->GetNative();

	// Already here?

	SceIoStat MyStat;
	MemoryClear(&MyStat,sizeof(MyStat));

	int eError = sceIoGetstat(pPath,&MyStat);
	if (eError>=SCE_OK) {
		// Ensure it's a directory for sanity's sake
		if (SCE_STM_ISDIR(MyStat.st_mode)) {
			// There already is a directory here by this name.
			// Exit okay!
			uResult = File::OKAY;
		}

	} else {
		// No folder here...
		// Let's try the easy way
		eError = sceIoMkdir(pPath,SCE_STM_RWU);
		if (eError>=SCE_OK) {
			// That was easy!
			uResult = File::OKAY;

		} else {

			// Check the pathname
			if (pPath[0]) {

				// This is more complex, parse each
				// segment of the folder to see if it
				// either already exists, and if not,
				// create it.

				// Skip the leading '/'
				char *pWork = const_cast<char *>(pPath)+1;
				// Is there a mid fragment?
				char *pEnd = StringCharacter(pWork,'/');
				if (pEnd) {

					// Let's iterate! Assume success unless 
					// an error occurs in this loop.

					uResult = File::OKAY;
					do {
						// Terminate at the fragment
						pEnd[0] = 0;
						// Create the directory (Maybe)
						eError = sceIoMkdir(pPath,SCE_STM_RWU);
						// Restore the pathname
						pEnd[0] = '/';
						// Error and it's not because it's already present
						if (eError<SCE_OK) {
							// Uh, oh... Perhaps not enough permissions?
							uResult = File::IOERROR;
							break;
						}
						// Skip past this fragment
						pWork = pEnd+1;
						// Get to the next fragment
						pEnd = StringCharacter(pWork,'/');
						// All done?
					} while (pEnd);
				}
			}		
		}
	}
	return uResult;
}

/***************************************

	Delete a file using native file system

***************************************/

Word BURGER_API Burger::FileManager::DeleteFile(Burger::Filename *pFileName)
{
	Word uResult = FALSE;		// Success
	if (sceIoRemove(pFileName->GetNative())<SCE_OK) {
		if (sceIoRmdir(pFileName->GetNative())<SCE_OK) {
			uResult = TRUE;		// Error!
		}
	}
	return uResult;
}

/***************************************

	Rename a file using native pathnames

***************************************/

Word BURGER_API Burger::FileManager::RenameFile(Burger::Filename *pNewName,Burger::Filename *pOldName)
{
	if (sceIoRename(pOldName->GetNative(),pNewName->GetNative())>=SCE_OK) {
		return FALSE;
	}
	return TRUE;		/* Oh oh... */
}

#endif
