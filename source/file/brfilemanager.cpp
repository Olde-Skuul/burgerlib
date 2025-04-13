/***************************************

	File Manager Class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"
#include "brdebug.h"
#include "brfile.h"
#include "brfileansihelpers.h"
#include "brglobals.h"
#include "brmemoryfunctions.h"
#include <stdio.h>

/*! ************************************

	\class Burger::FileManager
	\brief Global file system manager

	Since file systems are all tied to a single device such as a hard drive or
	CD/DVD/Blu-Ray, all file calls are routed through a singleton class to
	enable Burgerlib to prioritize file streaming to reduce seeks and read/write
	operations for peak performance.

	To ensure cross platform compatibility, all file/pathnames use a generic
	universal format that BurgerLib will accept and the library will perform any
	translations needed to the exact same behavior is consistent across
	operating systems.

	The strings are "C" format, UTF8 with a zero termination byte. Colons are
	used at filename delimiters.

	The concept of a BurgerLib path is simple. The first part is either
	a drive specifier, a volume name or a directory prefix. Drive specifiers
	act much like "C:" does on Windows machines. Volume names allow you to
	ask for a CD or DVD by name and query if it is mounted regardless of
	which drive it physically resides. Prefixes act like current working
	directories except that there are over 30 of them active at the same
	time and with a numeric or special character code you can quickly select
	the desired one at runtime.

	Some prefixes are preset after a call to \ref set_default_prefixes().

	Default prefixes:
	- "8:" = Current working directory at application launch
	- "9:" = Directory where the application resides
	- "*:" = Boot drive volume
	- "@:" = Directory for the user's data. OS specific as to the location
		(Usually the Users' folder)
	- "$:" = System folder (System on MacOS or Windows on Windows)
	- "14:" = User prefix #14, the number can be 0 through 31.

	Example pathnames:
	- "9:GameData.dat" = Data file sitting next to the application
	- "@:Contraband Entertainment:CoolGame:MyPrefs.bin" = Preferences file
	- "*:Root.txt" = File at the root of the boot drive
	- "20:LevelData.bin" = Application defined prefix and file within the
		expected directory structure

	\sa \ref UTF8

***************************************/

Burger::FileManager* Burger::FileManager::g_pFileManager;

/*! ************************************

	\enum Burger::FileManager::ePrefix
	\brief Predefined pathname prefixes

	Enumerations for all of the built-in prefixes that are outside the bounds of
	prefixes 0-31

	\sa set_default_prefixes()

***************************************/

/*! ************************************

	\enum Burger::FileManager::eIOCommand
	\brief Asynchronous file I/O commands

	These are the commands for the file event queue to perform file operations
	in the background.

	\sa add_queue()

***************************************/

/*! ************************************

	\brief Construct the file manager

	Guaranteed to construct without errors.

***************************************/

Burger::FileManager::FileManager() BURGER_NOEXCEPT: m_uQueueStart(0),
													m_uQueueEnd(0)
#if defined(BURGER_MSDOS)
	,
													m_pDOSName(nullptr),
													m_uMSDOSVersion(0),
													m_uMSDOSTrueVersion(0),
													m_bLongNamesAllowed(FALSE),
													m_uOEMFlavor(0)
#endif
#if defined(BURGER_DARWIN)
	,
													m_uBootNameSize(0),
													m_pBootName(nullptr)
#endif
{
	MemoryClear(m_IOQueue, sizeof(m_IOQueue));

#if 0
	// Start up the worker thread
	m_Thread.Start(queue_handler,this);
#endif
}

/*! ************************************

	\brief Destroy the file manager

***************************************/

Burger::FileManager::~FileManager()
{
	// Send a message to kill the thread
	add_queue(nullptr, kIOCommandEndThread, nullptr, 0);

#if 0
	// Wait until the thread dies
	m_Thread.Wait();
#endif

	platform_shutdown();
}

/*! ************************************

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

#if !(defined(BURGER_MSDOS) || defined(BURGER_MAC) || defined(BURGER_LINUX) || \
	defined(BURGER_ANDROID) || defined(BURGER_DARWIN) || \
	defined(BURGER_XBOX360)) || \
	defined(DOXYGEN)
void BURGER_API Burger::FileManager::platform_setup(void) BURGER_NOEXCEPT {}
#endif

/*! ************************************

	\brief Handle platform specific shutdown code

	Calls system functions to release cached values obtained from the platform's
	operating system.

	\sa platform_setup() or \ref FileManager

***************************************/

#if !(defined(BURGER_LINUX) || defined(BURGER_ANDROID) || \
	defined(BURGER_DARWIN) || defined(BURGER_MAC)) || \
	defined(DOXYGEN)
void BURGER_API Burger::FileManager::platform_shutdown(void) BURGER_NOEXCEPT {}
#endif

/*! ************************************

	\brief Initialize the global file system manager

	Call this function once on startup to start up the Burgerlib file manager.
	This function's primary purpose is to initialize the default prefixes.

	\sa set_default_prefixes(void) or shut_down(void)

***************************************/

Burger::eError BURGER_API Burger::FileManager::initialize(void) BURGER_NOEXCEPT
{
	eError uError = kErrorNone;
	FileManager* pThis = g_pFileManager;
	if (!pThis) {
		pThis = new (Alloc(sizeof(FileManager))) FileManager;
		g_pFileManager = pThis;

		// Set the platform specific variables
		pThis->platform_setup();

		// Load the default prefixes
		uError = set_default_prefixes();
	}
	return uError;
}

/*! ************************************

	\brief Shut down the global file system manager

	Call this function once on shutdown to the Burgerlib file manager. This
	function's primary purpose is to release all allocated memory and send out a
	report in case any files are still open on debug builds. The application is
	responsible for shutting down file usage before application shutdown.

	\sa initialize(void)

***************************************/

void BURGER_API Burger::FileManager::shut_down(void) BURGER_NOEXCEPT
{
	// Dispose of the global file manager instance
	if (g_pFileManager) {
		g_pFileManager->~FileManager();
		Free(g_pFileManager);
		g_pFileManager = nullptr;
	}
}

/*! ************************************

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

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_UNIX) || defined(BURGER_MACOS) || defined(BURGER_IOS) || \
	defined(BURGER_XBOX360) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)

Burger::eError BURGER_API Burger::FileManager::get_volume_name(
	Filename* pOutput, uint_t /* uVolumeNum */) BURGER_NOEXCEPT
{
	// Clear the output on error
	if (pOutput) {
		pOutput->clear();
	}
	// Error!
	return kErrorNotSupportedOnThisPlatform;
}

#endif

/*! ************************************

	\brief Given a volume name, return the volume number it's mounted on.

	Scanning all the disk volumes, search for a drive that is labeled by the
	input name. The input is in the form of ":home:". The search is case
	insensitive.

	\param pVolumeName Valid pointer to "C" string in the form of ":name:"

	\return Volume number from 0-max drives and -1 in case of the volume was not
		found.

	\note This function will NOT search floppy drives A: and B: on MSDOS
		and Windows platforms. This is to prevent an annoying error for bad
		media.

	\sa get_volume_name(Filename *,uint_t)

***************************************/

uint_t BURGER_API Burger::FileManager::get_volume_number(
	const char* pVolumeName) BURGER_NOEXCEPT
{
#if defined(BURGER_MSDOS) || defined(BURGER_WINDOWS)
	uint_t uDriveNum = 2; // Start at drive C:
	BURGER_CONSTEXPR const uint_t LASTDRIVE = 26;
#else
	uint_t uDriveNum = 0; // Start at drive A:
	BURGER_CONSTEXPR const uint_t LASTDRIVE = 32;
#endif

	// Assume failure
	uint_t uResult = UINT32_MAX;
	Filename TempFilename;
	do {
		// Convert to name
		const eError uError =
			FileManager::get_volume_name(&TempFilename, uDriveNum);
		if (uError == kErrorNone) {
			// Compare
			const int iResult =
				StringCaseCompare(TempFilename.c_str(), pVolumeName);
			// Match?
			if (!iResult) {
				// Return the drive number
				uResult = uDriveNum;
				break;
			}

			// Reached the end of volumes?
		} else if (uError == kErrorInvalidParameter) {
			break;
		}
		// All drives checked?
	} while (++uDriveNum < LASTDRIVE);

	// Return error or drive number
	return uResult;
}

/*! ************************************

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

#if !defined(BURGER_VITA) || defined(DOXYGEN)

Burger::eError BURGER_API Burger::FileManager::set_default_prefixes(
	void) BURGER_NOEXCEPT
{
	Filename MyFilename;

	// Set the standard work prefix
	eError uResult = MyFilename.set_system_working_directory();
	set_prefix(kPrefixCurrent, &MyFilename);

	// Set the application directory
	eError uTempResult = MyFilename.set_application_directory();
	set_prefix(kPrefixApplication, &MyFilename);
	if (uTempResult) {
		uResult = uTempResult;
	}

	// Set the boot volume
	uTempResult = MyFilename.set_boot_volume();
	set_prefix(kPrefixBoot, &MyFilename);
	if (uTempResult) {
		uResult = uTempResult;
	}

	// Set the system preferences location
	uTempResult = MyFilename.set_system_prefs_directory();
	set_prefix(kPrefixSystem, &MyFilename);
	if (uTempResult) {
		uResult = uTempResult;
	}

	// Set the application's preferences location
	uTempResult = MyFilename.set_user_prefs_directory();
	set_prefix(kPrefixPrefs, &MyFilename);
	if (uTempResult) {
		uResult = uTempResult;
	}

	return uResult;
}

#endif

/*! ************************************

	\brief Return the contents of a prefix

	Given a prefix number, fill in a \ref Filename structure with a copy
	of the prefix pathname. The string could be an empty string in the case of
	an unused or nullptr prefix.

	\param pOutput Pointer to a \ref Filename class to store the string
	\param uPrefixNum Index to the requested prefix to obtain

	\return Zero if no error, non-zero if error

	\sa set_prefix(uint_t, const char*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::get_prefix(
	Filename* pOutput, uint_t uPrefixNum) BURGER_NOEXCEPT
{
	eError uResult;
	// Is the prefix number valid?
	if (uPrefixNum >= kPrefixCount) {

		// No, clear out the output and return an error
		pOutput->clear();
		uResult = kErrorInvalidParameter;

	} else {

		// Get the prefix string
		uResult =
			pOutput->assign(g_pFileManager->m_Prefixes[uPrefixNum].c_str());
	}

	// Return the error code
	return uResult;
}

/*! ************************************

	\brief Return the contents of a prefix

	Given a prefix number, fill in a \ref String structure with a copy
	of the prefix pathname. The string could be an empty string in the case of
	an unused or nullptr prefix.

	\param pOutput Pointer to a String class to store the string
	\param uPrefixNum Index to the requested prefix to obtain

	\return Zero if no error, non-zero if error

	\sa get_prefix(Filename*, uint_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::get_prefix(
	String* pOutput, uint_t uPrefixNum) BURGER_NOEXCEPT
{
	eError uResult;
	// Is the prefix number valid?
	if (uPrefixNum >= kPrefixCount) {

		// No, clear out the output and return an error
		pOutput->clear();
		uResult = kErrorInvalidParameter;

	} else {

		// Get the prefix string
		uResult = pOutput->assign(g_pFileManager->m_Prefixes[uPrefixNum]);
	}

	// Return the error code
	return uResult;
}

/*! ************************************

	\brief Set the contents of a prefix

	Given a prefix number and a "C" string to a new pathname, set that prefix to
	the new value. The prefix is expanded BEFORE it's applied, so if you set
	prefix 10 with "10:foo" and prefix 10 was already ":Work:Temp:", then prefix
	10 will result in ":Work:Temp:foo:"

	Passing an empty string or a nullptr will cause the prefix to be released
	and considered empty.

	\param uPrefixNum Index to the requested prefix to obtain
	\param pPrefixName Pointer to a "C" string of a new BurgerLib pathname

	\return Zero if no error, non-zero if error (Usually out of memory or out of
		bounds)

	\sa get_prefix(Filename*, uint_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::set_prefix(
	uint_t uPrefixNum, const char* pPrefixName) BURGER_NOEXCEPT
{
	eError uResult;
	// Is the prefix valid?
	if (uPrefixNum >= kPrefixCount) {
		uResult = kErrorInvalidParameter;
	} else {

		// Assume success, since only out of memory could fail this function.
		uResult = kErrorNone;
		Filename Temp;

		// Assume the new prefix is nothing
		const char* pNewPrefix = nullptr;

		// Valid input prefix?
		if (pPrefixName && pPrefixName[0]) {

			// Convert to full pathname using a temporary filename record
			uResult = Temp.abs_path(pPrefixName);
			if (!uResult) {
				pPrefixName = Temp.c_str();

				// Blank string?
				if (pPrefixName[0]) {
					// Allocate memory for new prefix
					pNewPrefix = pPrefixName;
				}
			}
		}

		// Release the previous prefix and replace it with the new one
		g_pFileManager->m_Prefixes[uPrefixNum] = pNewPrefix;
	}

	// Return the result
	return uResult;
}

/*! ************************************

	\brief Set the contents of a prefix with a filename

	Given a prefix number and a \ref Filename that has a new pathname, set
	that prefix to the new value. The prefix is expanded BEFORE it's applied, so
	if you set prefix 10 with "10:foo" and prefix 10 was already ":Work:Temp:",
	then prefix 10 will result in ":Work:Temp:foo:"

	Passing an empty string or a ``nullptr`` will cause the prefix to be
	released and considered empty.

	\param uPrefixNum Index to the requested prefix to obtain
	\param pPrefixName Pointer to a \ref Filename of a new BurgerLib pathname

	\return Zero if no error, non-zero if error (Usually out of memory or out of
		bounds)

	\sa get_prefix(Filename*, uint_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::set_prefix(
	uint_t uPrefixNum, const Filename* pPrefixName) BURGER_NOEXCEPT
{
	eError uResult;

	// Is the prefix valid?
	if (uPrefixNum >= kPrefixCount) {
		uResult = kErrorInvalidParameter;
	} else {
		uResult = kErrorNone;

		// Zap the prefix
		Filename Temp;
		const char* pTemp = nullptr;

		// Valid prefix?
		if (pPrefixName) {
			const char* pPath = pPrefixName->c_str();
			if (pPath[0]) {

				// Convert to full pathname
				Temp.abs_path(pPrefixName->c_str());
				pPath = Temp.c_str();

				// Blank string?
				if (pPath[0]) {
					// Allocate memory for new prefix
					pTemp = pPath;
				}
			}
		}
		// Was there a path already?
		g_pFileManager->m_Prefixes[uPrefixNum] = pTemp;
	}

	// Return the result
	return uResult;
}

/*! ************************************

	\brief Remove the last entry of a prefix

	Given a prefix number, pop off the last entry so that it effectively goes up
	one entry in a directory hierarchy. Imagine performing the operation "cd .."
	on the prefix.

	This can force the prefix to become an empty string if the prefix is
	pointing to a root folder.

	Examples:

	- ":foo:bar:temp:" = ":foo:bar:"
	- ":foo:bar:" = ":foo:"
	- ":foo:" = ""

	\param uPrefixNum Index to the prefix to modify.

	\sa get_prefix(Filename*, uint_t) or set_prefix(uint_t, const char*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::pop_prefix(
	uint_t uPrefixNum) BURGER_NOEXCEPT
{
	Filename TempName;

	// Get the current prefix
	get_prefix(&TempName, uPrefixNum);

	// Remove a directory
	TempName.dirname();

	// Store the prefix
	return set_prefix(uPrefixNum, &TempName);
}

/*! ************************************

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
	const char* pFileName, TimeDate_t* pOutput) BURGER_NOEXCEPT
{
	Filename PathName(pFileName);
	return get_modification_time(&PathName, pOutput);
}

/*! ************************************

	\brief Get a file's last modification time.

	Given a native operating system path, access the file and return the
	time/date that it was last modified.

	\param pFileName Pointer to a \ref Filename class that is properly
		initialized to the native file system.
	\param pOutput Pointer to an uninitialized TimeDate_t structure that
		will receive the time/date.

	\return Zero on success, non-zero in the event of an error (Usually file not
		found)

	\sa get_modification_time(const char*, TimeDate_t*) or
		get_creation_time(Filename*, TimeDate_t*)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_UNIX) || \
	defined(BURGER_XBOX360) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::get_modification_time(
	Filename* /* pFileName */, TimeDate_t* /* pOutput */) BURGER_NOEXCEPT
{
	return kErrorNotSupportedOnThisPlatform; // Error!
}
#endif

/*! ************************************

	\brief Get a file's creation time.

	Given a BurgerLib path, access the file and return the time/date that it was
	created.

	\param pFileName Pointer to a "C" string to a BurgerLib pathname
	\param pOutput Pointer to an uninitialized \ref TimeDate_t structure that
		will receive the time/date.

	\return Zero on success, non-zero in the event of an error (Usually file not
		found)

	\sa get_creation_time(Filename*, TimeDate_t*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::get_creation_time(
	const char* pFileName, TimeDate_t* pOutput) BURGER_NOEXCEPT
{
	Filename PathName(pFileName);
	return get_creation_time(&PathName, pOutput);
}

/*! ************************************

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

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_UNIX) || \
	defined(BURGER_XBOX360) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::get_creation_time(
	Filename* /* pFileName */, TimeDate_t* /* pOutput */) BURGER_NOEXCEPT
{
	return kErrorNotSupportedOnThisPlatform; // Error!
}
#endif

/*! ************************************

	\brief Detect for a file's existence.

	Given a BurgerLib pathname, return \ref TRUE if the file exists. \ref FALSE
	if there is an disk error or the file does not exist.

	\note This does not guarantee that the file can be opened due to security
		permissions or the file being on a locked folder. It only guarantees its
		existence.

	\param pFileName Pointer to a "C" string of a BurgerLib pathname,

	\return \ref FALSE if the file does not exist, \ref TRUE if the file exists.

	\sa does_file_exist(Filename *)

***************************************/

uint_t BURGER_API Burger::FileManager::does_file_exist(
	const char* pFileName) BURGER_NOEXCEPT
{
	Filename PathName(pFileName);
	return does_file_exist(&PathName);
}

/*! ************************************

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

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_UNIX) || \
	defined(BURGER_XBOX360) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)
uint_t BURGER_API Burger::FileManager::does_file_exist(
	Filename* pFileName) BURGER_NOEXCEPT
{
#if defined(BURGER_DS)
	pFileName = NULL;
	return FALSE;
#else
	FILE* fp = fopen(pFileName->get_native(), "rb"); // Get file info
	if (!fp) {
		return FALSE; // Bad file!
	}
	fclose(fp);
	return TRUE; // File exists
#endif
}
#endif

/*! ************************************

	\brief Return a file's creator code.

	On MacOS, a file has extended data which includes the application code that
	owns the file. This function will retrieve the 4 byte code from the file.

	On non MacOS platforms, this function only return 0 (An error condition) and
	perform nothing.

	\param pFileName Pointer to a "C" string of a BurgerLib pathname

	\return 0 if the file doesn't exist, the function isn't implemented of if
		the file doesn't have extended information. A 32 bit value if the data
		can be retrieved.

	\sa get_creator_type(Filename*), get_file_type(const char*) or
		get_creator_and_file_type(const char*, uint32_t*, uint32_t*)

***************************************/

uint32_t BURGER_API Burger::FileManager::get_creator_type(const char* pFileName)
{
	Filename TempName(pFileName);       // Get the true path
	return get_creator_type(&TempName); // Call the function
}

/*! ************************************

	\brief Return a file's type code using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename. This function will retrieve the 4 byte code
	from the file.

	On non MacOS platforms, this function only return 0 (An error condition) and
	perform nothing.

	\param pFileName Pointer to a \ref Filename of a native OS pathname

	\return 0 if the file doesn't exist, the function isn't implemented of if
		the file doesn't have extended information. A 32 bit value if the data
		can be retrieved.

	\sa get_file_type(const char*), get_creator_type(const char*) or
		get_creator_and_file_type(const char*, uint32_t*, uint32_t*)

***************************************/

#if !(defined(BURGER_MACOS) || defined(BURGER_DARWIN)) || defined(DOXYGEN)
uint32_t BURGER_API Burger::FileManager::get_creator_type(
	Filename* /*pFileName */)
{
	// Don't do anything!
	return 0;
}
#endif

/*! ************************************

	\brief Return a file's type code.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename. This function will retrieve the 4 byte code
	from the file.

	On non MacOS platforms, this function only return 0 (An error condition) and
	perform nothing..

	\param pFileName Pointer to a "C" string of a BurgerLib pathname

	\return 0 if the file doesn't exist, the function isn't implemented of if
		the file doesn't have extended information. A 32 bit value if the data
		can be retrieved.

	\sa get_file_type(Filename*), get_creator_type(const char*) or
		get_creator_and_file_type(const char*, uint32_t*, uint32_t*)

***************************************/

uint32_t BURGER_API Burger::FileManager::get_file_type(const char* pFileName)
{
	// Get the true path
	Filename TempName(pFileName);

	// Call the function
	return get_file_type(&TempName);
}

/*! ************************************

	\brief Return a file's creator code using a native filename.

	On MacOS, a file has extended data which includes the application code that
	owns the file. This function will retrieve the 4 byte code from the file.

	On non MacOS platforms, this function only return 0 (An error condition) and
	perform nothing..

	\param pFileName Pointer to a \ref Filename of a native OS pathname

	\return 0 if the file doesn't exist, the function isn't implemented of if
		the file doesn't have extended information. A 32 bit value if the data
		can be retrieved.

	\sa get_creator_type(const char*), get_file_type(const char*) or
		get_creator_and_file_type(const char*, uint32_t*, uint32_t*)

***************************************/

#if !(defined(BURGER_MACOS) || defined(BURGER_DARWIN)) || defined(DOXYGEN)
uint32_t BURGER_API Burger::FileManager::get_file_type(Filename* /*pFileName*/)
{
	// Don't do anything!
	return 0;
}
#endif

/*! ************************************

	\brief Return a file's type and creator codes.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename, and the creator code of the application that
	owns this file. This function will retrieve the 4 byte codes from the file.

	On non MacOS platforms, this function only return non-zero (An error
	condition) and perform store zeros in the result values..

	\param pFileName Pointer to a "C" string of a BurgerLib pathname
	\param pCreatorType Pointer to a uint32_t that will receive the file creator
		code.
	\param pFileType Pointer to a uint32_t that will receive the file type code.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero is returned on
		successful completion.

	\sa get_creator_and_file_type(Filename*, uint32_t*, uint32_t *),
		get_file_type(const char*) or get_creator_type(const char*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::get_creator_and_file_type(
	const char* pFileName, uint32_t* pCreatorType, uint32_t* pFileType)
{
	// Get the true path
	Filename TempName(pFileName);

	// Call the function
	return get_creator_and_file_type(&TempName, pCreatorType, pFileType);
}

/*! ************************************

	\brief Return a file's type and creator codes using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename, and the creator code of the application that
	owns this file. This function will retrieve the 4 byte codes from the file.

	On non MacOS platforms, this function only return non-zero (An error
	condition) and perform store zeros in the result values..

	\param pFileName Pointer to a \ref Filename of a native OS pathname
	\param pCreatorType Pointer to a uint32_t that will receive the file creator
		code.
	\param pFileType Pointer to a uint32_t that will receive the file type code.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero is returned on
		successful completion.

	\sa get_creator_and_file_type(Filename*, uint32_t*, uint32_t*),
		get_file_type(const char*) or get_creator_type(const char*)

***************************************/

#if !(defined(BURGER_MACOS) || defined(BURGER_DARWIN)) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::get_creator_and_file_type(
	Filename* /* pFileName */, uint32_t* pCreatorType, uint32_t* pFileType)
{
	pCreatorType[0] = 0;
	pFileType[0] = 0;
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Set a file's creator code.

	On MacOS, a file has extended data which includes the application code that
	owns the file. This function will set the 4 byte code for the file.

	On non MacOS platforms, this function will perform nothing.

	\param pFileName Pointer to a "C" string of a BurgerLib pathname
	\param uCreatorType Four byte character code to set.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero if successful.

	\sa set_creator_type(Filename* ,uint32_t),
		set_file_type(const char* ,uint32_t) or
		set_creator_and_file_type(const char* ,uint32_t,uint32_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::set_creator_type(
	const char* pFileName, uint32_t uCreatorType)
{
	Filename TempName(pFileName); // Get the true path
	return set_creator_type(&TempName, uCreatorType);
}

/*! ************************************

	\brief Set a file's type code using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename. This function will set the 4 byte code for the
	file.

	On non MacOS platforms, this function will perform nothing.

	\param pFileName Pointer to a \ref Filename of a native OS pathname
	\param uCreatorType Four byte character code to set.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero if successful.

	\sa set_file_type(const char*, uint32_t), set_creator_type(
		const char*, uint32_t) or set_creator_and_file_type(
		const char*, uint32_t, uint32_t)

***************************************/

#if !(defined(BURGER_MACOS) || defined(BURGER_DARWIN)) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::set_creator_type(
	Filename* /*pFileName*/, uint32_t /* uCreatorType */)
{
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Set a file's type code.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename. This function will set the 4 byte code for the
	file.

	On non MacOS platforms, this function will perform nothing.

	\param pFileName Pointer to a "C" string of a BurgerLib pathname
	\param uFileType Four byte character code to set.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero if successful.

	\sa set_file_type(Filename*, uint32_t), set_creator_type(
		const char*, uint32_t) or set_creator_and_file_type(
		const char*, uint32_t, uint32_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::set_file_type(
	const char* pFileName, uint32_t uFileType)
{
	// Get the true path
	Filename TempName(pFileName);
	return set_file_type(&TempName, uFileType);
}

/*! ************************************

	\brief Set a file's type code using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename. This function will set the 4 byte code for the
	file.

	On non MacOS platforms, this function will perform nothing.

	\param pFileName Pointer to a \ref Filename of a native OS pathname,
	\param uFileType Four byte character code to set.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero if successful.

	\sa set_file_type(const char* ,uint32_t),
		set_creator_type(const char*, uint32_t) or
		set_creator_and_file_type(const char* ,uint32_t,uint32_t)

***************************************/

#if !(defined(BURGER_MACOS) || defined(BURGER_DARWIN)) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::set_file_type(
	Filename* /*pFileName*/, uint32_t /*uFileType */)
{
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Set a file's type and creator codes.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename, and the creator code of the application that
	owns this file. This function will set the 4 byte codes for the file.

	On non MacOS platforms, this function will perform nothing.

	\param pFileName Pointer to a "C" string of a BurgerLib pathname
	\param uCreatorType A uint32_t of the new file creator code.
	\param uFileType A uint32_t of the new file type code.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero is returned on
		successful completion.

	\sa get_creator_and_file_type(Filename*, uint32_t*, uint32_t*),
		get_file_type(const char*) or get_creator_type(const char*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::set_creator_and_file_type(
	const char* pFileName, uint32_t uCreatorType, uint32_t uFileType)
{
	// Get the true path
	Filename TempName(pFileName);
	return set_creator_and_file_type(&TempName, uCreatorType, uFileType);
}

/*! ************************************

	\brief Set a file's type and creator codes using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename, and the creator code of the application that
	owns this file. This function will set the 4 byte codes for the file.

	On non MacOS platforms, this function will perform nothing.

	\param pFileName Pointer to a \ref Filename of a native OS pathname
	\param uCreatorType A uint32_t of the new file creator code.
	\param uFileType A uint32_t of the new file type code.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero is returned on
		successful completion.

	\sa get_creator_and_file_type(const char*, uint32_t*, uint32_t*),
		get_file_type(const char*) or get_creator_type(const char*)

***************************************/

#if !(defined(BURGER_MACOS) || defined(BURGER_IOS)) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::set_creator_and_file_type(
	Filename* /*pFileName*/, uint32_t /* uCreatorType */,
	uint32_t /* uFileType */)
{
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Create a directory path

	Given a BurgerLib directory, create each and every part of the directory. If
	the directory already exists, or is successfully created, the call is
	considered successful.

	Example:

	If the path :Foo:Bar: exists, a call with ":Foo:Bar:Fooey:Temp:" will
	create both "Fooey" and "Temp" in this single call.

	\param pFileName Pointer to a "C" string of a BurgerLib path.
	\return Zero if successful, non-zero on error.

	\sa create_directory_path_dirname(const char *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::create_directory_path(
	const char* pFileName) BURGER_NOEXCEPT
{
	// Convert to native path
	Filename PathName(pFileName);

	// Create the path
	return create_directory_path(&PathName);
}

/*! ************************************

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

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_UNIX) || \
	defined(BURGER_XBOX360) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::create_directory_path(
	Filename* /* pFileName */) BURGER_NOEXCEPT
{
	return kErrorNotSupportedOnThisPlatform; // Always error out
}
#endif

/*! ************************************

	\brief Create a directory path sans the last filename token.

	Given a BurgerLib directory, create each and every part of the directory
	from a path that includes a filename. If the directory already exists, or is
	successfully created, the call is considered successful.

	Example:

	If the path :Foo:Bar: exists, a call with ":Foo:Bar:Fooey:Temp:File.txt"
	will create both "Fooey" and "Temp" in this single call. The last token of
	"File.txt" is ignored.

	\param pFileName Pointer to a "C" string of a BurgerLib path.

	\return Zero if successful, non-zero on error.

	\sa create_directory_path(const char*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::create_directory_path_dirname(
	const char* pFileName)
{
	Filename FileNameCopy(pFileName);
	FileNameCopy.dirname();
	return create_directory_path(&FileNameCopy);
}

/*! ************************************

	\brief Create a directory path sans the last filename token.

	Given a BurgerLib directory, create each and every part of the directory
	from a path that includes a filename. If the directory already exists, or is
	successfully created, the call is considered successful.

	Example:

	If the path :Foo:Bar: exists, a call with ":Foo:Bar:Fooey:Temp:File.txt"
	will create both "Fooey" and "Temp" in this single call. The last token of
	"File.txt" is ignored.

	\param pFileName Pointer to a Filename class

	\return Zero if successful, non-zero on error.

	\sa create_directory_path(Filename*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::create_directory_path_dirname(
	const Filename* pFileName)
{
	Filename FileNameCopy(*pFileName);
	FileNameCopy.dirname();
	return create_directory_path(&FileNameCopy);
}

/*! ************************************

	\brief Delete a file or empty directory.

	Given a BurgerLib pathname, delete the file at the end of the pathname.

	Example:

	If a call with ":Foo:Bar:Fooey:Temp.txt" is issued, only the file "Temp.txt"
	is deleted. The rest of the path is left intact. This call will not delete
	non-empty directories, only files. If the file is already deleted, an error
	is returned.

	\param pFileName Pointer to a "C" string of a BurgerLib path.

	\return Zero if successful, non-zero on error.

	\sa delete_file(Filename*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::delete_file(
	const char* pFileName) BURGER_NOEXCEPT
{
	// Expand the path to a full filename
	Filename Dest(pFileName);

	// Copy the file
	return delete_file(&Dest);
}

/*! ************************************

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

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_UNIX) || \
	defined(BURGER_XBOX360) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::delete_file(
	Filename* pFileName) BURGER_NOEXCEPT
{
#if defined(BURGER_DS)
	// Always error out
	return kErrorNotSupportedOnThisPlatform;
#else

	// Assume error
	eError uResult = kErrorIO;
	if (!remove(pFileName->get_native())) {
		// No error
		uResult = kErrorNone;
	}
	return uResult;
#endif
}
#endif

/*! ************************************

	\brief Rename a file.

	Given a BurgerLib formatted old pathname and new pathname, rename the file
	at the end of the pathname.

	\note The source and destination directories must be the same.

	\param pNewName Pointer to a "C" string of a BurgerLib path for the new
		name.
	\param pOldName Pointer to a "C" string of a BurgerLib path for the current
		file name.

	\return Zero if successful, non-zero on error.

	\sa rename_file(Filename*, Filename*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::rename_file(
	const char* pNewName, const char* pOldName) BURGER_NOEXCEPT
{
	// Expand the path to a full filename
	Filename Dest(pNewName);

	// Expand the source path
	Filename Src(pOldName);

	// Rename or move the file
	return rename_file(&Dest, &Src);
}

/*! ************************************

	\brief Rename a file using \ref Filename.

	Given a \ref Filename formatted old pathname and new pathname, rename the
	file at the end of the pathname.

	\note The source and destination directories must be the same.

	\param pNewName Pointer to a \ref Filename for the new name.
	\param pOldName Pointer to a \ref Filename for the current file name.

	\return Zero if successful, non-zero on error.

	\sa rename_file(const cha *, const char*)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOS) || \
	defined(BURGER_UNIX) || defined(BURGER_XBOX360) || \
	defined(BURGER_VITA)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::rename_file(
	Filename* pNewName, Filename* pOldName) BURGER_NOEXCEPT
{
#if defined(BURGER_DS) || defined(BURGER_PS5)
	// Always error out
	return kErrorNotSupportedOnThisPlatform;
#else

	// Assume error
	eError uResult = kErrorIO;
	if (!rename(pOldName->get_native(), pNewName->get_native())) {
		uResult = kErrorNone;
	}
	return uResult;
#endif
}
#endif

/*! ************************************

	\brief Change the OS working directory.

	Given a BurgerLib pathname, set the OS's current working directory to this
	path.

	This function is useful in cases where an OS native file dialog to select a
	file or perform an OS native function where the current working directory
	needs to be preset. This function under most circumstances is not necessary
	to use, however, it exists since there are some OS native functions in MacOS
	and Windows that require a little "help".

	\note On most systems, this is the same as calling cwd().

	\param pDirName Pointer to a "C" string of a BurgerLib path.

	\return Zero if successful, non-zero on error.

	\sa change_OS_directory(Filename*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::change_OS_directory(
	const char* pDirName)
{
	// Expand the path to a full filename
	Filename DirName(pDirName);

	// Set the directory here
	return change_OS_directory(&DirName);
}

/*! ************************************

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

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MSDOS) && \
	!defined(BURGER_MACOS) && !defined(BURGER_UNIX)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::change_OS_directory(
	Filename* /* pDirName */)
{
	// Error!
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

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
	const char* pFileName, const char* pType) BURGER_NOEXCEPT
{
	// Expand the path to a full filename
	Filename TempPath(pFileName);
	return open_file(&TempPath, pType);
}

/*! ************************************

	\brief Open a file using stdio.

	Given a BurgerLib pathname, open a file as if fopen() supported BurgerLib
	pathnames.

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

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MACOS) && \
	!defined(BURGER_UNIX)) || \
	defined(DOXYGEN)
FILE* BURGER_API Burger::FileManager::open_file(
	Filename* pFileName, const char* pType) BURGER_NOEXCEPT
{
#if defined(BURGER_DS)
	return nullptr;
#else
	// Open using standard fopen
	return fopen(pFileName->get_native(), pType);
#endif
}
#endif

/*! ************************************

	\brief Copy a file.

	Given a BurgerLib formatted old pathname and new pathname, make a duplicate
	of the file at the end of the pathname.

	\param pDestName Pointer to a "C" string of a BurgerLib path for the new
		name.
	\param pSrcName Pointer to a "C" string of a BurgerLib path for the current
		file name.

	\return Zero if successful, non-zero on error.

	\sa copy_file(Filename*, Filename*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::copy_file(
	const char* pDestName, const char* pSrcName) BURGER_NOEXCEPT
{
	// Expand the path to a full filename
	Filename Dest(pDestName);

	// Expand the source path
	Filename Src(pSrcName);

	// Copy the file
	return copy_file(&Dest, &Src);
}

/*! ************************************

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

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MACOS) && \
	!defined(BURGER_UNIX) && !defined(BURGER_XBOX360)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::copy_file(
	Filename* pDestName, Filename* pSourceName) BURGER_NOEXCEPT
{
#if defined(BURGER_DS)
	return kErrorNotSupportedOnThisPlatform;
#else

	// Assume error
	eError uResult = kErrorIO;

	// Open the source file
	File fpsrc(pSourceName, File::kReadOnly);

	// Get the size of the source file
	uint64_t uLength = fpsrc.get_file_size();

	// Shall I copy anything?
	if (uLength) {

		// Make sure the chunk fits in memory
		uintptr_t uMaxChunk =
			static_cast<uintptr_t>((uLength < 0x100000U) ? uLength : 0x100000U);

		uint8_t* pBuffer = static_cast<uint8_t*>(Alloc(uMaxChunk));
		if (pBuffer) {

			// Open the dest file
			File fpdst(pDestName, File::kWriteOnly);
			do {
				// Base chunk
				uint64_t uChunk = uLength;
				if (uChunk > uMaxChunk) {
					// Only copy the chunk
					uChunk = uMaxChunk;
				}

				// Read data
				uintptr_t uReadSize =
					fpsrc.read(pBuffer, static_cast<uintptr_t>(uChunk));
				if (uReadSize != uChunk) {
					break;
				}

				// Write data
				uReadSize = fpdst.write(pBuffer, uReadSize);
				if (uReadSize != uChunk) {
					break;
				}
				uLength -= uChunk;

				// Any data left?
			} while (uLength);

			// All data copied?
			if (!uLength) {

				// No error (So far)
				uResult = kErrorNone;
			}

			// Did the file have an error in closing?
			if (fpdst.close()) {
				// Crap.
				uResult = kErrorIO;
			}
		}

		// Release the copy buffer
		Free(pBuffer);
	}

	// Close the source file
	fpsrc.close();
	return uResult;
#endif
}
#endif

/*! ************************************

	\brief Save a file.

	Given a BurgerLib pathname and a buffer, save the contents of the buffer
	into a file and close the file.

	This function is a quick and easy way to write a buffer from memory directly
	to disk (Or any other storage medium).

	\param pFileName Pointer to a "C" string of a BurgerLib path.
	\param pInput Pointer to an array of bytes to save
	\param uLength Number of bytes in the buffer

	\return \ref TRUE if successful, \ref FALSE on error.

	\sa save_file(Filename*, const void*, uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::save_file(const char* pFileName,
	const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	// Convert to a filename object
	Filename MyName(pFileName);

	// Perform the action
	return save_file(&MyName, pInput, uLength);
}

/*! ************************************

	\brief Save a file using an OS native pathname.

	Given an OS native pathname and a buffer, save the contents of the buffer
	into a file and close the file.

	This function is a quick and easy way to write a buffer from memory directly
	to disk (Or any other storage medium).

	\param pFileName Pointer to a \ref Filename object.
	\param pInput Pointer to an array of bytes to save
	\param uLength Number of bytes in the buffer

	\return \ref TRUE if successful, \ref FALSE on error.

	\sa save_file(const char*, const void*, uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::save_file(
	Filename* pFileName, const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	// Open the file
	File FileRef;
	eError uResult = FileRef.open(pFileName, File::kWriteOnly);
	if (uResult != kErrorNone) {

		// Try creating the directory for the file
		create_directory_path_dirname(pFileName);

		// Now open the file
		uResult = FileRef.open(pFileName, File::kWriteOnly);
	}

	// File opened?
	if (uResult == kErrorNone) {

		// Save the file
		const uintptr_t uWritten = FileRef.write(pInput, uLength);
		uResult = FileRef.close();

		if (uResult == kErrorNone) {
			if (uWritten != uLength) {
				uResult = kErrorWriteFailure;
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Save a text file.

	Given a BurgerLib pathname and a buffer, save the contents of the buffer
	into a file and close the file.

	This function is a quick and easy way to write a buffer from memory directly
	to disk (Or any other storage medium).

	\note This differs from \ref save_file(const char*, const void*, uintptr_t)
		in that all "\n" values will be translated to the proper line feeds for
		the target operating system. Unix and Linux, no change, Windows and
		MSDos, it's converted to a "\n\r", MacOS and MacOSX it's converted to
		"\r".

	\param pFileName Pointer to a "C" string of a BurgerLib path.
	\param pInput Pointer to an array of bytes to save as a text file.
	\param uLength Number of bytes in the buffer

	\return \ref TRUE if successful, \ref FALSE on error.

	\sa save_text_file(Filename*, const void*, uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::save_text_file(
	const char* pFileName, const void* pInput,
	uintptr_t uLength) BURGER_NOEXCEPT
{
	// Convert to burgerlib filename
	Filename MyName(pFileName);

	// Perform the action
	return save_text_file(&MyName, pInput, uLength);
}

/*! ************************************

	\brief Save a text file using a \ref Filename.

	Given a \ref Filename and a buffer, save the contents of the buffer
	into a file and close the file.

	This function is a quick and easy way to write a buffer from memory directly
	to disk (Or any other storage medium).

	\note This differs from save_file(Filename*, const void *, uintptr_t) in
		that all "\n" values will be translated to the proper line feeds for the
		target operating system. Unix and Linux, no change, Windows and MSDos,
		it's converted to a "\n\r", MacOS and MacOSX it's converted to "\r".

	\param pFileName Pointer to a \ref Filename object.
	\param pInput Pointer to an array of bytes to save as a text file.
	\param uLength Number of bytes in the buffer

	\return \ref TRUE if successful, \ref FALSE on error.

	\sa save_text_file(const char*, const void*, uintptr_t),
		save_file(const char*, const void*, uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::save_text_file(
	Filename* pFileName, const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	// Open the file
	FILE* fp = open_file(pFileName, "w");
	if (fp) {
		return Burger::SaveFile(fp, pInput, uLength);
	}
	return kErrorWriteProtected;
}

/*! ************************************

	\brief Load a file into memory.

	Given a BurgerLib pathname, open the file, allocate a buffer for it, load it
	into memory and return the pointer to the allocated buffer and the optional
	buffer size.

	This function is a quick and easy way to read the contents of a file into a
	buffer. The buffer must be deleted with a call to \ref Burger::Free(
	const void*)

	\note This function does no data translation. The buffer contains an exact
		byte for byte copy of the contents of the file on disk.

	\param pFileName Pointer to a "C" string of a BurgerLib path.
	\param pLength Pointer to a uintptr_t to receive the size of the buffer.

	\return \ref nullptr if failure, a valid pointer to the buffer on success.

	\sa load_file(Filename*, uintptr_t *) or save_file(Filename*,
		const void*, uintptr_t)

***************************************/

void* BURGER_API Burger::FileManager::load_file(
	const char* pFileName, uintptr_t* pLength) BURGER_NOEXCEPT
{
	Filename MyName(pFileName);

	// Perform the action
	return load_file(&MyName, pLength);
}

/*! ************************************

	\brief Load a file into memory using a \ref Filename.

	Given an OS native pathname, open the file, allocate a buffer for it, load
	it into memory and return the pointer to the allocated buffer and the
	optional buffer size.

	This function is a quick and easy way to read the contents of a file into a
	buffer. The buffer must be deleted with a call to \ref Burger::Free(
	const void *)

	\note This function does no data translation. The buffer contains an exact
		byte for byte copy of the contents of the file on disk.

	\param pFileName Pointer to a \ref Filename object.
	\param pLength Pointer to a uintptr_t to receive the size of the buffer.

	\return \ref nullptr if failure, a valid pointer to the buffer on success.

	\sa load_file(const char*, uintptr_t*), or save_file(Filename*, const void*,
		uintptr_t)

***************************************/

void* BURGER_API Burger::FileManager::load_file(
	Filename* pFileName, uintptr_t* pLength) BURGER_NOEXCEPT
{
#if defined(BURGER_DS)
	if (pLength) {
		pLength[0] = 0;
	}
	return nullptr;
#else

	File FileRef(pFileName, File::kReadOnly);
	uint64_t uNewSize = FileRef.get_file_size();

	// Is the file too big for memory?
	if (uNewSize > UINTPTR_MAX) {
		return nullptr;
	}

	void* pResult = nullptr;
	if (uNewSize) {
#if defined(_DEBUG)
		if (get_traceflags() & kTraceFlagFileLoad) {
			Debug::PrintString("Loading native file ");
			Debug::PrintString(pFileName->get_native());
			Debug::PrintString(".\n");
		}
#endif
		pResult = Alloc(static_cast<uintptr_t>(uNewSize));
		if (pResult) {
			if (FileRef.read(pResult, static_cast<uintptr_t>(uNewSize)) !=
				uNewSize) {
				Free(pResult);
				pResult = nullptr;
				uNewSize = 0;
			}
		}
	}
	FileRef.close();

	// Return the file length and allocated memory pointer
	if (pLength) {
		pLength[0] = static_cast<uintptr_t>(uNewSize);
	}
	return pResult;
#endif
}

/*! ************************************

	\brief Clear out pending events

	Clear out the I/O file queue. All events with callbacks will return a cancel
	error.

	\sa sync() or wait_until_queue_has_space()

***************************************/

void BURGER_API Burger::FileManager::flush_IO(void) BURGER_NOEXCEPT {}

/*! ************************************

	\brief Test if there is space in the queue.

	If there is not room in the queue for a new entry, wait until a file
	operation completes so a new entry can be added to the end of the queue.

	If there is room, return immediately.

	\sa sync() or flush_IO()

***************************************/

void BURGER_API Burger::FileManager::wait_until_queue_has_space(
	void) BURGER_NOEXCEPT
{
}

/*! ************************************

	\brief Add a file event into the queue

	If the queue is full, wait until there is room and then insert the command
	into the queue.

	The data buffer must exist until after the async event occurs.

	\param pFile Pointer to the File object to perform the action on
	\param uIOCommand Command to execute on the File object
	\param pBuffer Optional pointer to a buffer for a read/write command
	\param uLength Optional buffer size or file marker for read/write/seek
		command

***************************************/

void BURGER_API Burger::FileManager::add_queue(File* pFile,
	eIOCommand uIOCommand, void* pBuffer, uintptr_t uLength) BURGER_NOEXCEPT
{
	wait_until_queue_has_space();

	// Get the pointer to the end of the queue

	uint32_t uEnd = m_uQueueEnd;

	// Get the pointer to the next entry
	Queue_t* pQueue = &m_IOQueue[uEnd];
	// Fill it in
	pQueue->m_pFile = pFile;
	pQueue->m_uIOCommand = uIOCommand;
	pQueue->m_pBuffer = pBuffer;
	pQueue->m_uLength = uLength;
	m_uQueueEnd = (uEnd + 1) & (kMaxQueue - 1);
#if defined(BURGER_WINDOWS)
	// Send a message to the thread to execute
	m_PingIOThread.signal();
#endif
}

/*! ************************************

	\brief Wait until all pending file calls are processed.

	Test all pending file calls and if any remain that affect the File object,
	then wait until they are processed.

	\param pFile Pointer to the File object to test

	\sa flush_IO() or wait_until_queue_has_space()

***************************************/

void BURGER_API Burger::FileManager::sync(File* pFile) BURGER_NOEXCEPT
{
	BURGER_UNUSED(pFile);
}

/*! ************************************

	\brief Queue thread.

	Internal function to handle async file operations.

	\note This is called as a separate thread. Do not call directly.

	\param pData Pointer to the parent FileManager object.

	\return 0 or error code if shut down suddenly.

***************************************/

#if !defined(BURGER_WINDOWS) || defined(DOXYGEN)
uintptr_t BURGER_API Burger::FileManager::queue_handler(
	void* pData) BURGER_NOEXCEPT
{
	BURGER_UNUSED(pData);
	return 0;
}
#endif

/*! ************************************

	\class Burger::FileManagerSimple
	\brief Initialization class for the global FileManager structure.

	By default, the FileManager structure does not automatically
	initialize. The application determines the best time to start up and shut
	down the global singleton. To make this process easier, create an instance
	of this class in your main() function or use one of the application class
	templates which will do this task for you.

	There should be only one instance of this class in existence in the
	application.

	\code
	int main(int argc,char **argv)
	{
		// Initialize the memory manager
		Burger::MemoryManagerGlobalANSI MyMemory;
		// Initialize the file system
		Burger::FileManagerSimple MyFiles;

		DoStuff();
		return 0;
	}
	\endcode

	\note This class can and will allocate memory at runtime via the
		initialization of \ref FileManager. It's crucial that the
		\ref GlobalMemoryManager is initialized BEFORE this class instance is
		created.

	\sa GlobalMemoryManager or FileManager

***************************************/

/*! ************************************

	\fn Burger::FileManagerSimple::FileManagerSimple()
	\brief Initializes the global \ref FileManager structure.

	\sa FileManager or FileManager::initialize(void)

***************************************/

/*! ************************************

	\fn Burger::FileManagerSimple::~FileManagerSimple()
	\brief Shuts down the global FileManager structure.

	\sa FileManager or FileManager::shut_down(void)

***************************************/
