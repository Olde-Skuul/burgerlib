/***************************************

	File Manager Class: Playstation Vita version

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_VITA) || defined(DOXYGEN)
#include "brfile.h"
#include "brmemoryfunctions.h"
#include "brstring.h"
#include <kernel.h>

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

static const char* g_VolumeNames[] = {
	":app0:", ":ux0:", ":addcont0:", ":addcont1:", ":savedata0:", ":savedata1:",
	":photo0:", ":music0:", ":host0:"};

Burger::eError BURGER_API Burger::FileManager::GetVolumeName(
	Burger::Filename* pOutput, uint_t uVolumeNum) BURGER_NOEXCEPT
{
	eError uResult;
	if (uVolumeNum < BURGER_ARRAYSIZE(g_VolumeNames)) {
		pOutput->Set(g_VolumeNames[uVolumeNum]);
		uResult = kErrorNone;
	} else {
		// Clear on error
		pOutput->Clear();
		uResult = kErrorInvalidParameter;
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

Burger::eError BURGER_API Burger::FileManager::DefaultPrefixes(void)
{
	Filename MyFilename;
	eError uResult = GetVolumeName(&MyFilename, 0); // Get the boot volume name
	if (uResult == kErrorNone) {
		// Set the initial prefix
		SetPrefix(kPrefixBoot, MyFilename.GetPtr());
	}
	SetPrefix(kPrefixCurrent, ":app0:");     // Set the standard work prefix
	SetPrefix(kPrefixApplication, ":app0:"); // Set the standard work prefix
	SetPrefix(kPrefixSystem, ":addcont0:");  // Set the standard work prefix
	SetPrefix(kPrefixPrefs, ":ux0:");        // Set the standard work prefix
	return kErrorNone;
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
	Set(":app0:");
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
	Set(":app0:");
	return kErrorNone;
}

/***************************************

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Burger::eError BURGER_API Burger::FileManager::GetModificationTime(
	Burger::Filename* pFileName, Burger::TimeDate_t* pOutput)
{
	// Buffer to hold the attributes and the filename
	SceIoStat Entry;

	// Initialize the attributes list
	MemoryClear(&Entry, sizeof(Entry));

	// Get the directory entry
	int iError = sceIoGetstat(pFileName->GetNative(), &Entry);

	// No errors?

	eError uResult;
	if (iError < SCE_OK) {
		pOutput->Clear();
		uResult = kErrorFileNotFound;
	} else {
		// Get the file dates
		pOutput->Load(&Entry.st_mtime);
		// It's parsed!
		uResult = kErrorNone;
	}
	return uResult;
}

/***************************************

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Burger::eError BURGER_API Burger::FileManager::GetCreationTime(
	Burger::Filename* pFileName, Burger::TimeDate_t* pOutput)
{
	// Buffer to hold the attributes and the filename
	SceIoStat Entry;

	// Initialize the attributes list
	MemoryClear(&Entry, sizeof(Entry));

	// Get the directory entry
	int iError = sceIoGetstat(pFileName->GetNative(), &Entry);

	// No errors?

	eError uResult;
	if (iError < SCE_OK) {
		pOutput->Clear();
		uResult = kErrorFileNotFound;
	} else {
		// Get the file dates
		pOutput->Load(&Entry.st_ctime);
		// It's parsed!
		uResult = kErrorNone;
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

uint_t BURGER_API Burger::FileManager::DoesFileExist(
	Burger::Filename* pFileName) BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	// Buffer to hold the attributes and the filename
	SceIoStat MyStat;

	// Initialize the attributes list
	MemoryClear(&MyStat, sizeof(MyStat));

	// Get the directory entry
	int eError = sceIoGetstat(pFileName->GetNative(), &MyStat);
	if (eError >= SCE_OK) {
		// If it succeeded, the file must exist
		uResult = TRUE;
	}
	return uResult;
}

/***************************************

	Create a directory path using an operating system native name
	Return FALSE if successful, or TRUE if an error

***************************************/

Burger::eError BURGER_API Burger::FileManager::CreateDirectoryPath(
	Burger::Filename* pFileName)
{
	// Assume an error condition
	eError uResult = kErrorIO;
	// Get the full path
	const char* pPath = pFileName->GetNative();

	// Already here?

	SceIoStat MyStat;
	MemoryClear(&MyStat, sizeof(MyStat));

	int eError = sceIoGetstat(pPath, &MyStat);
	if (eError >= SCE_OK) {
		// Ensure it's a directory for sanity's sake
		if (SCE_STM_ISDIR(MyStat.st_mode)) {
			// There already is a directory here by this name.
			// Exit okay!
			uResult = kErrorNone;
		}

	} else {
		// No folder here...
		// Let's try the easy way
		eError = sceIoMkdir(pPath, SCE_STM_RWU);
		if (eError >= SCE_OK) {
			// That was easy!
			uResult = kErrorNone;

		} else {

			// Check the pathname
			if (pPath[0]) {

				// This is more complex, parse each
				// segment of the folder to see if it
				// either already exists, and if not,
				// create it.

				// Skip the leading '/'
				char* pWork = const_cast<char*>(pPath) + 1;
				// Is there a mid fragment?
				char* pEnd = StringCharacter(pWork, '/');
				if (pEnd) {

					// Let's iterate! Assume success unless
					// an error occurs in this loop.

					uResult = kErrorNone;
					do {
						// Terminate at the fragment
						pEnd[0] = 0;
						// Create the directory (Maybe)
						eError = sceIoMkdir(pPath, SCE_STM_RWU);
						// Restore the pathname
						pEnd[0] = '/';
						// Error and it's not because it's already present
						if (eError < SCE_OK) {
							// Uh, oh... Perhaps not enough permissions?
							uResult = kErrorIO;
							break;
						}
						// Skip past this fragment
						pWork = pEnd + 1;
						// Get to the next fragment
						pEnd = StringCharacter(pWork, '/');
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

Burger::eError BURGER_API Burger::FileManager::DeleteFile(
	Burger::Filename* pFileName) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone; // Success
	if (sceIoRemove(pFileName->GetNative()) < SCE_OK) {
		if (sceIoRmdir(pFileName->GetNative()) < SCE_OK) {
			uResult = kErrorIO; // Error!
		}
	}
	return uResult;
}

/***************************************

	Rename a file using native pathnames

***************************************/

Burger::eError BURGER_API Burger::FileManager::RenameFile(
	Burger::Filename* pNewName, Burger::Filename* pOldName)
{
	if (sceIoRename(pOldName->GetNative(), pNewName->GetNative()) >= SCE_OK) {
		return kErrorNone;
	}
	return kErrorIO; /* Oh oh... */
}

#endif
