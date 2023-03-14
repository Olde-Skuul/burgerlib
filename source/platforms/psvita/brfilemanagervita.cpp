/***************************************

	File Manager Class: Playstation Vita version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

	\param pOutput A \ref Filename structure to contain the filename (Can be
		\ref nullptr )

	\param uVolumeNum A valid drive number from 0-?? with ?? being the
		maximum number of drives in the system

	\return Zero if no error, non-zero if an error occurred

	\sa get_volume_number(const char *)

***************************************/

static const char* g_VolumeNames[] = {
	":app0:", ":ux0:", ":addcont0:", ":addcont1:", ":savedata0:", ":savedata1:",
	":photo0:", ":music0:", ":host0:"};

Burger::eError BURGER_API Burger::FileManager::get_volume_name(
	Filename* pOutput, uint_t uVolumeNum) BURGER_NOEXCEPT
{
	eError uResult;
	if (uVolumeNum < BURGER_ARRAYSIZE(g_VolumeNames)) {
		pOutput->assign(g_VolumeNames[uVolumeNum]);
		uResult = kErrorNone;
	} else {
		// Clear on error
		pOutput->clear();
		uResult = kErrorInvalidParameter;
	}
	return uResult;
}

/***************************************

	\brief Set the initial default prefixes for a power up state

	Sets these prefixes based on the current setup of the machine the
	application is running on.

	- "*:" = Boot volume
	- "$:" = System folder
	- "@:" = Preferences folder
	- "8:" = Default directory
	- "9:" = Application directory

	\sa initialize(void), get_prefix(Filename*, uint_t) or
		set_prefix(uint_t, const char*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::set_default_prefixes(void)
{
	Filename MyFilename;
	eError uResult =
		get_volume_name(&MyFilename, 0); // Get the boot volume name
	if (uResult == kErrorNone) {
		// Set the initial prefix
		set_prefix(kPrefixBoot, MyFilename.c_str());
	}
	set_prefix(kPrefixCurrent, ":app0:");     // Set the standard work prefix
	set_prefix(kPrefixApplication, ":app0:"); // Set the standard work prefix
	set_prefix(kPrefixSystem, ":addcont0:");  // Set the standard work prefix
	set_prefix(kPrefixPrefs, ":ux0:");        // Set the standard work prefix
	return kErrorNone;
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
	return assign(":app0:");
}

/***************************************

	\brief Set the filename to the application's directory

	Determine the directory where the application resides and set the filename
	to that directory. The path is converted into UTF8 character encoding and
	stored in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like an
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::set_application_directory(
	void) BURGER_NOEXCEPT
{
	return assign(":app0:");
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
	// Buffer to hold the attributes and the filename
	SceIoStat Entry;

	// Initialize the attributes list
	MemoryClear(&Entry, sizeof(Entry));

	// Get the directory entry
	int iError = sceIoGetstat(pFileName->get_native(), &Entry);

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
	// Buffer to hold the attributes and the filename
	SceIoStat Entry;

	// Initialize the attributes list
	MemoryClear(&Entry, sizeof(Entry));

	// Get the directory entry
	int iError = sceIoGetstat(pFileName->get_native(), &Entry);

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
	uint_t uResult = FALSE;
	// Buffer to hold the attributes and the filename
	SceIoStat MyStat;

	// Initialize the attributes list
	MemoryClear(&MyStat, sizeof(MyStat));

	// Get the directory entry
	int eError = sceIoGetstat(pFileName->get_native(), &MyStat);
	if (eError >= SCE_OK) {
		// If it succeeded, the file must exist
		uResult = TRUE;
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

Burger::eError BURGER_API Burger::FileManager::create_directory_path(
	Filename* pFileName) BURGER_NOEXCEPT
{
	// Assume an error condition
	eError uResult = kErrorIO;
	// Get the full path
	const char* pPath = pFileName->get_native();

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
	eError uResult = kErrorNone; // Success
	if (sceIoRemove(pFileName->get_native()) < SCE_OK) {
		if (sceIoRmdir(pFileName->get_native()) < SCE_OK) {
			uResult = kErrorIO; // Error!
		}
	}
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
	if (sceIoRename(pOldName->get_native(), pNewName->get_native()) >= SCE_OK) {
		return kErrorNone;
	}
	return kErrorIO; /* Oh oh... */
}

#endif
