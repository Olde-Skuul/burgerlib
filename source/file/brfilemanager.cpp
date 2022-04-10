/***************************************

	File Manager Class

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

// Xbox 360 headers
#if defined(BURGER_XBOX360)
#define NOD3D
#define NONET
#include <xtl.h>

#include <xbdm.h>
// C'mon, they defined these?!?!?
#undef DeleteFile
#undef CopyFile
#endif

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

	Some prefixes are preset after a call to \ref DefaultPrefixes().

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

	\sa Burger::UTF8

***************************************/

Burger::FileManager* Burger::FileManager::g_pFileManager;

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
	m_Thread.Start(QueueHandler,this);
#endif
}

/*! ************************************

	\brief Destroy the file manager

***************************************/

Burger::FileManager::~FileManager()
{
	// Send a message to kill the thread
	AddQueue(nullptr, kIOCommandEndThread, nullptr, 0);

#if 0
	// Wait until the thread dies
	m_Thread.Wait();
#endif

	PlatformShutdown();
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

	\sa PlatformShutdown(), Burger::FileManager

***************************************/

#if !(defined(BURGER_MSDOS) || defined(BURGER_LINUX) || \
	defined(BURGER_ANDROID) || defined(BURGER_DARWIN)) || \
	defined(DOXYGEN)
void BURGER_API Burger::FileManager::PlatformSetup(void) BURGER_NOEXCEPT {}
#endif

/*! ************************************

	\brief Handle platform specific shutdown code

	Calls system functions to release cached values obtained from the platform's
	operating system.

	\sa PlatformSetup(), Burger::FileManager

***************************************/

#if !(defined(BURGER_LINUX) || defined(BURGER_ANDROID) || \
	defined(BURGER_DARWIN)) || \
	defined(DOXYGEN)
void BURGER_API Burger::FileManager::PlatformShutdown(void) BURGER_NOEXCEPT {}
#endif

/*! ************************************

	\brief Initialize the global file system manager

	Call this function once on startup to start up the Burgerlib file manager.
	This function's primary purpose is to initialize the default prefixes.

	\sa Burger::FileManager::DefaultPrefixes(void) or
		Burger::FileManager::Shutdown(void)

***************************************/

Burger::eError BURGER_API Burger::FileManager::Init(void) BURGER_NOEXCEPT
{
	eError uError = kErrorNone;
	FileManager* pThis = g_pFileManager;
	if (!pThis) {
		pThis = new (Alloc(sizeof(FileManager))) FileManager;
		g_pFileManager = pThis;

		// Set the platform specific variables
		pThis->PlatformSetup();

#if defined(BURGER_MAC)
		// Init the directory cache (MacOS)
		Filename::InitDirectoryCache();
#endif

#if defined(BURGER_XBOX360)
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
#endif

		// Load the default prefixes
		uError = DefaultPrefixes();
	}
	return uError;
}

/*! ************************************

	\brief Shut down the global file system manager

	Call this function once on shutdown to the Burgerlib file manager. This
	function's primary purpose is to release all allocated memory and send out a
	report in case any files are still open on debug builds. The application is
	responsible for shutting down file usage before application shutdown.

	\sa Burger::FileManager::Init(void)

***************************************/

void BURGER_API Burger::FileManager::Shutdown(void) BURGER_NOEXCEPT
{
#if defined(BURGER_MAC)
	// Release any directories cached (MacOS)
	Filename::PurgeDirectoryCache();
#endif

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

	\param pOutput A Burger::Filename structure to contain the filename (Can be
		nullptr)

	\param uVolumeNum A valid drive number from 0-?? with ?? being the
		maximum number of drives in the system

	\return Zero if no error, non-zero if an error occurred

	\sa Burger::FileManager::GetVolumeNumber(const char *)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_UNIX) || defined(BURGER_MACOS) || defined(BURGER_IOS) || \
	defined(BURGER_XBOX360) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)

Burger::eError BURGER_API Burger::FileManager::GetVolumeName(
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

	\sa Burger::FileManager::GetVolumeName(Burger::Filename *,uint_t)

***************************************/

uint_t BURGER_API Burger::FileManager::GetVolumeNumber(
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
	uint_t uResult = BURGER_MAXUINT;
	Filename TempFilename;
	do {
		// Convert to name
		const eError uError =
			FileManager::GetVolumeName(&TempFilename, uDriveNum);
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

	"*:" = Boot volume<br>
	"$:" = System folder<br>
	"@:" = Preferences folder<br>
	"8:" = Default directory<br>
	"9:" = Application directory

	\sa Burger::FileManager::Init(void),
		Burger::FileManager::GetPrefix(Burger::Filename *,uint_t) or
		Burger::FileManager::SetPrefix(uint_t,const char *)

***************************************/

#if !( \
	defined(BURGER_IOS) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)

Burger::eError BURGER_API Burger::FileManager::DefaultPrefixes(
	void) BURGER_NOEXCEPT
{
	Filename MyFilename;

	// Set the standard work prefix
	eError uResult = MyFilename.SetSystemWorkingDirectory();
	SetPrefix(kPrefixCurrent, &MyFilename);

	// Set the application directory
	eError uTempResult = MyFilename.SetApplicationDirectory();
	SetPrefix(kPrefixApplication, &MyFilename);
	if (uTempResult) {
		uResult = uTempResult;
	}

	// Set the boot volume
	uTempResult = MyFilename.SetBootVolumeDirectory();
	SetPrefix(kPrefixBoot, &MyFilename);
	if (uTempResult) {
		uResult = uTempResult;
	}

	// Set the application
	uTempResult = MyFilename.SetMachinePrefsDirectory();
	SetPrefix(kPrefixSystem, &MyFilename);
	if (uTempResult) {
		uResult = uTempResult;
	}

	// Set the application's preferences location
	uTempResult = MyFilename.SetUserPrefsDirectory();
	SetPrefix(kPrefixPrefs, &MyFilename);
	if (uTempResult) {
		uResult = uTempResult;
	}

	return uResult;
}

#endif

/*! ************************************

	\brief Return the contents of a prefix

	Given a prefix number, fill in a \ref Burger::Filename structure with a copy
	of the prefix pathname. The string could be an empty string in the case of
	an unused or nullptr prefix.

	\param pOutput Pointer to a Burger::Filename class to store the string
	\param uPrefixNum Index to the requested prefix to obtain
	\return Zero if no error, non-zero if error
	\sa Burger::FileManager::SetPrefix(uint_t,const char *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::GetPrefix(
	Filename* pOutput, uint_t uPrefixNum) BURGER_NOEXCEPT
{
	eError uResult;
	// Is the prefix number valid?
	if (uPrefixNum >= FileManager::kPrefixCount) {

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
	\sa GetPrefix(Filename *,uint_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::GetPrefix(
	String* pOutput, uint_t uPrefixNum) BURGER_NOEXCEPT
{
	eError uResult;
	// Is the prefix number valid?
	if (uPrefixNum >= FileManager::kPrefixCount) {

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

	\sa Burger::FileManager::GetPrefix(Burger::Filename *,uint_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::SetPrefix(
	uint_t uPrefixNum, const char* pPrefixName) BURGER_NOEXCEPT
{
	eError uResult;
	// Is the prefix valid?
	if (uPrefixNum >= FileManager::kPrefixCount) {
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

			uResult = Temp.Expand(pPrefixName);
			if (!uResult) {
				pPrefixName = Temp.c_str();
				if (pPrefixName[0]) { // Blank string?
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

	Given a prefix number and a Burger::Filename that has a new pathname, set
	that prefix to the new value. The prefix is expanded BEFORE it's applied, so
	if you set prefix 10 with "10:foo" and prefix 10 was already ":Work:Temp:",
	then prefix 10 will result in ":Work:Temp:foo:"

	Passing an empty string or a ``nullptr`` will cause the prefix to be
	released and considered empty.

	\param uPrefixNum Index to the requested prefix to obtain
	\param pPrefixName Pointer to a Burger::Filename of a new BurgerLib pathname

	\return Zero if no error, non-zero if error (Usually out of memory or out of
		bounds)

	\sa Burger::FileManager::GetPrefix(Burger::Filename *,uint_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::SetPrefix(
	uint_t uPrefixNum, const Filename* pPrefixName) BURGER_NOEXCEPT
{
	eError uResult;
	// Is the prefix valid?
	if (uPrefixNum >= FileManager::kPrefixCount) {
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
				Temp.Expand(pPrefixName->c_str());
				pPath = Temp.c_str();
				if (pPath[0]) { // Blank string?
					// Allocate memory for new prefix
					pTemp = pPath;
				}
			}
		}
		// Was there a path already?
		g_pFileManager->m_Prefixes[uPrefixNum] = pTemp;
	}
	return uResult; // Return the result
}

/*! ************************************

	\brief Remove the last entry of a prefix

	Given a prefix number, pop off the last entry so that it effectively goes up
	one entry in a directory hierarchy. Imagine performing the operation "cd .."
	on the prefix.

	This can force the prefix to become an empty string if the prefix is
	pointing to a root folder.

	Examples:<br>
	":foo:bar:temp:" = ":foo:bar:"<br>
	":foo:bar:" = ":foo:"<br>
	":foo:" = ""

	\param uPrefixNum Index to the prefix to modify.

	\sa Burger::FileManager::GetPrefix(Burger::Filename *,uint_t) or
		Burger::FileManager::SetPrefix(uint_t,const char *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::PopPrefix(
	uint_t uPrefixNum) BURGER_NOEXCEPT
{
	Filename TempName;
	GetPrefix(&TempName, uPrefixNum);        // Get the current prefix
	TempName.dirname();                      // Remove a directory
	return SetPrefix(uPrefixNum, &TempName); // Store the prefix
}

/*! ************************************

	\brief Get a file's last modification time.

	Given a BurgerLib path, access the file and return the time/date that it was
	last modified.

	\param pFileName Pointer to a "C" string to a BurgerLib pathname
	\param pOutput Pointer to an uninitialized Burger::TimeDate_t structure that
		will receive the time/date.

	\return Zero on success, non-zero in the event of an error (Usually file not
		found)

	\sa Burger::FileManager::GetModificationTime(
		Burger::Filename *,Burger::TimeDate_t *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::GetModificationTime(
	const char* pFileName, TimeDate_t* pOutput) BURGER_NOEXCEPT
{
	Filename PathName(pFileName);
	return GetModificationTime(&PathName, pOutput);
}

/*! ************************************

	\brief Get a file's last modification time.

	Given a native operating system path, access the file and return the
	time/date that it was last modified.

	\param pFileName Pointer to a Burger::Filename class that is properly
		initialized to the native file system.
	\param pOutput Pointer to an uninitialized Burger::TimeDate_t structure that
		will receive the time/date.

	\return Zero on success, non-zero in the event of an error (Usually file not
		found)

	\sa Burger::Filename::SetFromNative(const char *) or
		Burger::FileManager::GetModificationTime(
			const char *,Burger::TimeDate_t*)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_IOS) || defined(BURGER_XBOX360) || \
	defined(BURGER_VITA)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::GetModificationTime(
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
	\param pOutput Pointer to an uninitialized Burger::TimeDate_t structure that
		will receive the time/date.

	\return Zero on success, non-zero in the event of an error (Usually file not
		found)

	\sa Burger::FileManager::GetCreationTime(
		Burger::Filename *,Burger::TimeDate_t *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::GetCreationTime(
	const char* pFileName, TimeDate_t* pOutput) BURGER_NOEXCEPT
{
	Filename PathName(pFileName);
	return GetCreationTime(&PathName, pOutput);
}

/*! ************************************

	\brief Get a file's creation time.

	Given a native operating system path, access the file and return the
	time/date that it was created.

	\param pFileName Pointer to a Burger::Filename class that is properly
		initialized to the native file system.
	\param pOutput Pointer to an uninitialized Burger::TimeDate_t structure that
		will receive the time/date.

	\return Zero on success, non-zero in the event of an error (Usually file not
		found)

	\sa Burger::Filename::SetFromNative(const char *) or
		Burger::FileManager::GetCreationTime(
			const char *,Burger::TimeDate_t *)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_IOS) || defined(BURGER_XBOX360) || \
	defined(BURGER_VITA)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::GetCreationTime(
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

	\sa Burger::FileManager::DoesFileExist(Burger::Filename *)

***************************************/

uint_t BURGER_API Burger::FileManager::DoesFileExist(
	const char* pFileName) BURGER_NOEXCEPT
{
	Filename PathName(pFileName);
	return DoesFileExist(&PathName);
}

/*! ************************************

	\brief Detect for a file's existence using a native pathname.

	Given a OS native pathname, return \ref TRUE if the file exists. \ref FALSE
	if there is an disk error or the file does not exist.

	\note This does not guarantee that the file can be opened due to security
		permissions or the file being on a locked folder. It only guarantees its
		existence.

	\param pFileName Pointer to a Burger::Filename which contains a native
		version of the path to the file.

	\return \ref FALSE if the file does not exist, \ref TRUE if the file exists.

	\sa Burger::Filename::SetFromNative(const char *) or
		Burger::FileManager::DoesFileExist(const char *)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_IOS) || defined(BURGER_XBOX360) || \
	defined(BURGER_VITA)) || \
	defined(DOXYGEN)
uint_t BURGER_API Burger::FileManager::DoesFileExist(
	Filename* pFileName) BURGER_NOEXCEPT
{
#if defined(BURGER_DS)
	pFileName = NULL;
	return FALSE;
#else
	FILE* fp = fopen(pFileName->GetNative(), "rb"); // Get file info
	if (!fp) {
		return FALSE; // Bad file!
	}
	fclose(fp);
	return TRUE;               // File exists
#endif
}
#endif

/*! ************************************

	\brief Return a file's creator code.

	On MacOS, a file has extended data which includes the application code that
	owns the file. This function will retrieve the 4 byte code from the file.

	On non MacOS platforms, this function only return 0 (An error condition) and
	perform nothing..

	\param pFileName Pointer to a "C" string of a BurgerLib pathname

	\return 0 if the file doesn't exist, the function isn't implemented of if
		the file doesn't have extended information. A 32 bit value if the data
		can be retrieved.

	\sa Burger::FileManager::GetAuxType(Burger::Filename *),
		Burger::FileManager::GetFileType(const char *) or
		Burger::FileManager::GetFileAndAuxType(
			const char *,uint32_t *,uint32_t *)

***************************************/

uint32_t BURGER_API Burger::FileManager::GetAuxType(const char* pFileName)
{
	Filename TempName(pFileName); // Get the true path
	return GetAuxType(&TempName); // Call the function
}

/*! ************************************

	\brief Return a file's type code using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename. This function will retrieve the 4 byte code
	from the file.

	On non MacOS platforms, this function only return 0 (An error condition) and
	perform nothing..

	\param pFileName Pointer to a Burger::Filename of a native OS pathname

	\return 0 if the file doesn't exist, the function isn't implemented of if
		the file doesn't have extended information. A 32 bit value if the data
		can be retrieved.

	\sa Burger::FileManager::GetFileType(const char *),
		Burger::FileManager::GetAuxType(const char *) or
		Burger::FileManager::GetFileAndAuxType(
			const char *,uint32_t *,uint32_t *)

***************************************/

#if !(defined(BURGER_MACOS) || defined(BURGER_IOS)) || defined(DOXYGEN)
uint32_t BURGER_API Burger::FileManager::GetAuxType(Filename* /*pFileName */)
{
	return 0; // Don't do anything!
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

	\sa Burger::FileManager::GetFileType(Burger::Filename *),
		Burger::FileManager::GetAuxType(const char *) or
		Burger::FileManager::GetFileAndAuxType(
			const char *,uint32_t *,uint32_t *)

***************************************/

uint32_t BURGER_API Burger::FileManager::GetFileType(const char* pFileName)
{
	Filename TempName(pFileName);  // Get the true path
	return GetFileType(&TempName); // Call the function
}

/*! ************************************

	\brief Return a file's creator code using a native filename.

	On MacOS, a file has extended data which includes the application code that
	owns the file. This function will retrieve the 4 byte code from the file.

	On non MacOS platforms, this function only return 0 (An error condition) and
	perform nothing..

	\param pFileName Pointer to a Burger::Filename of a native OS pathname

	\return 0 if the file doesn't exist, the function isn't implemented of if
		the file doesn't have extended information. A 32 bit value if the data
		can be retrieved.

	\sa Burger::FileManager::GetAuxType(const char *),
		Burger::FileManager::GetFileType(const char *) or
		Burger::FileManager::GetFileAndAuxType(
			const char *,uint32_t *,uint32_t *)

***************************************/

#if !(defined(BURGER_MACOS) || defined(BURGER_IOS)) || defined(DOXYGEN)
uint32_t BURGER_API Burger::FileManager::GetFileType(Filename* /*pFileName*/)
{
	return 0; // Don't do anything!
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
	\param pFileType Pointer to a uint32_t that will receive the file type code.
	\param pAuxType Pointer to a uint32_t that will receive the file creator
		code.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero is returned on
		successful completion.

	\sa Burger::FileManager::GetFileAndAuxType(
		Burger::Filename *,uint32_t *,uint32_t *),
		Burger::FileManager::GetFileType(const char *) or
		Burger::FileManager::GetAuxType(const char *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::GetFileAndAuxType(
	const char* pFileName, uint32_t* pFileType, uint32_t* pAuxType)
{
	Filename TempName(pFileName); // Get the true path
	return GetFileAndAuxType(
		&TempName, pFileType, pAuxType); // Call the function
}

/*! ************************************

	\brief Return a file's type and creator codes using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename, and the creator code of the application that
	owns this file. This function will retrieve the 4 byte codes from the file.

	On non MacOS platforms, this function only return non-zero (An error
	condition) and perform store zeros in the result values..

	\param pFileName Pointer to a Burger::Filename of a native OS pathname
	\param pFileType Pointer to a uint32_t that will receive the file type code.
	\param pAuxType Pointer to a uint32_t that will receive the file creator
		code.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero is returned on
		successful completion.

	\sa Burger::FileManager::GetFileAndAuxType(
		Burger::Filename *,uint32_t *,uint32_t *),
		Burger::FileManager::GetFileType(const char *) or
		Burger::FileManager::GetAuxType(const char *)

***************************************/

#if !(defined(BURGER_MACOS) || defined(BURGER_IOS)) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::GetFileAndAuxType(
	Filename* /* pFileName */, uint32_t* pFileType, uint32_t* pAuxType)
{
	pFileType[0] = 0;
	pAuxType[0] = 0;
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Set a file's creator code.

	On MacOS, a file has extended data which includes the application code that
	owns the file. This function will set the 4 byte code for the file.

	On non MacOS platforms, this function will perform nothing.

	\param pFileName Pointer to a "C" string of a BurgerLib pathname
	\param uAuxType Four byte character code to set.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero if successful.

	\sa Burger::FileManager::SetAuxType(Burger::Filename *,uint32_t),
		Burger::FileManager::SetFileType(const char *,uint32_t) or
		Burger::FileManager::SetFileAndAuxType(const char *,uint32_t,uint32_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::SetAuxType(
	const char* pFileName, uint32_t uAuxType)
{
	Filename TempName(pFileName); // Get the true path
	return SetAuxType(&TempName, uAuxType);
}

/*! ************************************

	\brief Set a file's type code using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename. This function will set the 4 byte code for the
	file.

	On non MacOS platforms, this function will perform nothing.

	\param pFileName Pointer to a Burger::Filename of a native OS pathname
	\param uAuxType Four byte character code to set.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero if successful.

	\sa Burger::FileManager::SetFileType(const char *,uint32_t),
		Burger::FileManager::SetAuxType(const char *,uint32_t) or
		Burger::FileManager::SetFileAndAuxType(const char *,uint32_t,uint32_t)

***************************************/

#if !(defined(BURGER_MACOS) || defined(BURGER_IOS)) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::SetAuxType(
	Filename* /*pFileName*/, uint32_t /* uAuxType */)
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

	\sa Burger::FileManager::SetFileType(Burger::Filename *,uint32_t),
		Burger::FileManager::SetAuxType(const char *,uint32_t) or
		Burger::FileManager::SetFileAndAuxType(const char *,uint32_t,uint32_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::SetFileType(
	const char* pFileName, uint32_t uFileType)
{
	Filename TempName(pFileName); // Get the true path
	return SetFileType(&TempName, uFileType);
}

/*! ************************************

	\brief Set a file's type code using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename. This function will set the 4 byte code for the
	file.

	On non MacOS platforms, this function will perform nothing.

	\param pFileName Pointer to a Burger::Filename of a native OS pathname,
	\param uFileType Four byte character code to set.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero if successful.

	\sa Burger::FileManager::SetFileType(const char *,uint32_t),
		Burger::FileManager::SetAuxType(const char *,uint32_t) or
		Burger::FileManager::SetFileAndAuxType(const char *,uint32_t,uint32_t)

***************************************/

#if !(defined(BURGER_MACOS) || defined(BURGER_IOS)) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::SetFileType(
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
	\param uFileType A uint32_t of the new file type code.
	\param uAuxType A uint32_t of the new file creator code.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero is returned on
		successful completion.

	\sa Burger::FileManager::GetFileAndAuxType(
		Burger::Filename *,uint32_t *,uint32_t *),
		Burger::FileManager::GetFileType(const char *) or
		Burger::FileManager::GetAuxType(const char *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::SetFileAndAuxType(
	const char* pFileName, uint32_t uFileType, uint32_t uAuxType)
{
	Filename TempName(pFileName); // Get the true path
	return SetFileAndAuxType(&TempName, uFileType, uAuxType);
}

/*! ************************************

	\brief Set a file's type and creator codes using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename, and the creator code of the application that
	owns this file. This function will set the 4 byte codes for the file.

	On non MacOS platforms, this function will perform nothing.

	\param pFileName Pointer to a Burger::Filename of a native OS pathname
	\param uFileType A uint32_t of the new file type code.
	\param uAuxType A uint32_t of the new file creator code.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero is returned on
		successful completion.

	\sa Burger::FileManager::GetFileAndAuxType(
		const char *,uint32_t *,uint32_t *),
		Burger::FileManager::GetFileType(const char *) or
		Burger::FileManager::GetAuxType(const char *)

***************************************/

#if !(defined(BURGER_MACOS) || defined(BURGER_IOS)) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::SetFileAndAuxType(
	Filename* /*pFileName*/, uint32_t /* uFileType */, uint32_t /* uAuxType */)
{
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Create a directory path

	Given a BurgerLib directory, create each and every part of the directory. If
	the directory already exists, or is successfully created, the call is
	considered successful.

	Example:<br>
	If the path :Foo:Bar: exists, a call with ":Foo:Bar:Fooey:Temp:" will
	create both "Fooey" and "Temp" in this single call.

	\param pFileName Pointer to a "C" string of a BurgerLib path.
	\return Zero if successful, non-zero on error.

	\sa Burger::FileManager::CreateDirectoryPathDirName(const char *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::CreateDirectoryPath(
	const char* pFileName) BURGER_NOEXCEPT
{
	Filename PathName(pFileName);          // Convert to native path
	return CreateDirectoryPath(&PathName); // Create the path
}

/*! ************************************

	\brief Create a directory path using an OS native pathname.

	Given an OS native directory, create each and every part of the directory.
	If the directory already exists, or is successfully created, the call is
	considered successful.

	Example:<br>
	If the path c:\\Foo\\Bar exists, a call with "c:\Foo\Bar\Fooey\Temp\" will
	create both "Fooey" and "Temp" in this single call.

	\param pFileName Pointer to a Burger::Filename that contains a native OS
		pathname.

	\return Zero if successful, non-zero on error.

	\sa Burger::FileManager::CreateDirectoryPath(const char *) or
		Burger::FileManager::CreateDirectoryPathDirName(const char *)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_IOS) || defined(BURGER_XBOX360) || \
	defined(BURGER_VITA)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::CreateDirectoryPath(
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

	Example:<br>
	If the path :Foo:Bar: exists, a call with ":Foo:Bar:Fooey:Temp:File.txt"
	will create both "Fooey" and "Temp" in this single call. The last token of
	"File.txt" is ignored.

	\param pFileName Pointer to a "C" string of a BurgerLib path.

	\return Zero if successful, non-zero on error.

	\sa Burger::FileManager::CreateDirectoryPath(const char *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::CreateDirectoryPathDirName(
	const char* pFileName)
{
	Filename FileNameCopy(pFileName);
	FileNameCopy.dirname();
	return CreateDirectoryPath(&FileNameCopy);
}

/*! ************************************

	\brief Create a directory path sans the last filename token.

	Given a BurgerLib directory, create each and every part of the directory
	from a path that includes a filename. If the directory already exists, or is
	successfully created, the call is considered successful.

	Example:<br>
	If the path :Foo:Bar: exists, a call with ":Foo:Bar:Fooey:Temp:File.txt"
	will create both "Fooey" and "Temp" in this single call. The last token of
	"File.txt" is ignored.

	\param pFileName Pointer to a Filename class

	\return Zero if successful, non-zero on error.

	\sa Burger::FileManager::CreateDirectoryPath(Filename *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::CreateDirectoryPathDirName(
	Filename* pFileName)
{
	Filename FileNameCopy(*pFileName);
	FileNameCopy.dirname();
	return CreateDirectoryPath(&FileNameCopy);
}

/*! ************************************

	\brief Delete a file.

	Given a BurgerLib pathname, delete the file at the end of the pathname.

	Example:<br>
	If a call with ":Foo:Bar:Fooey:Temp.txt" is issued, only the file "Temp.txt"
	is deleted. The rest of the path is left intact. This call will not delete
	directories, only files. If the file is already deleted, an error is
	returned.

	\param pFileName Pointer to a "C" string of a BurgerLib path.

	\return Zero if successful, non-zero on error.

	\sa Burger::FileManager::DeleteFile(Burger::Filename *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::DeleteFile(
	const char* pFileName) BURGER_NOEXCEPT
{
	Filename Dest(pFileName); // Expand the path to a full filename
	return DeleteFile(&Dest); // Copy the file
}

/*! ************************************

	\brief Delete a file using an OS pathname.

	Given an OS native pathname, delete the file at the end of the pathname.

	Example:<br>
	If a call with "c:\Foo\Bar\Fooey\Temp.txt" is issued, only the file
	"Temp.txt" is deleted. The rest of the path is left intact. This call will
	not delete directories, only files. If the file is already deleted, an error
	is returned.

	\param pFileName Pointer to a Burger::Filename of a native OS path.

	\return Zero if successful, non-zero on error.

	\sa Burger::FileManager::DeleteFile(const char *)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_IOS) || defined(BURGER_XBOX360) || \
	defined(BURGER_VITA)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::DeleteFile(
	Filename* pFileName) BURGER_NOEXCEPT
{
#if defined(BURGER_DS)
	return kErrorNotSupportedOnThisPlatform; // Always error out
#else
	eError uResult = kErrorIO; // Assume error
	if (remove(pFileName->GetNative())) {
		uResult = kErrorNone; // No error
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

	\sa Burger::FileManager::RenameFile(Burger::Filename *,Burger::Filename *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::RenameFile(
	const char* pNewName, const char* pOldName) BURGER_NOEXCEPT
{
	Filename Dest(pNewName);        // Expand the path to a full filename
	Filename Src(pOldName);         // Expand the source path
	return RenameFile(&Dest, &Src); // Rename or move the file
}

/*! ************************************

	\brief Rename a file using OS native pathnames.

	Given an OS native formatted old pathname and new pathname, rename the file
	at the end of the pathname.

	\note The source and destination directories must be the same.

	\param pNewName Pointer to a Burger::Filename of an OS native path for the
		new name.
	\param pOldName Pointer to a Burger::Filename of an OS native path for the
		current file name.

	\return Zero if successful, non-zero on error.

	\sa Burger::FileManager::RenameFile(const char *,const char *)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOS) || \
	defined(BURGER_IOS) || defined(BURGER_XBOX360) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::RenameFile(
	Filename* pNewName, Filename* pOldName) BURGER_NOEXCEPT
{
#if defined(BURGER_DS)
	// Always error out
	return kErrorNotSupportedOnThisPlatform;
#else
	eError uResult = kErrorIO; // Assume error
	if (!rename(pOldName->GetNative(), pNewName->GetNative())) {
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

	\param pDirName Pointer to a "C" string of a BurgerLib path.

	\return Zero if successful, non-zero on error.

	\sa Burger::FileManager::ChangeOSDirectory(Burger::Filename *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::ChangeOSDirectory(
	const char* pDirName)
{
	Filename DirName(pDirName);         // Expand the path to a full filename
	return ChangeOSDirectory(&DirName); // Set the directory here
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

	\param pDirName Pointer to a Burger::Filename of a native OS path.

	\return Zero if successful, non-zero on error.

	\sa Burger::FileManager::ChangeOSDirectory(const char *)

***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MSDOS) && \
	!defined(BURGER_MACOS) && !defined(BURGER_IOS)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::ChangeOSDirectory(
	Filename* /* pDirName */)
{
	return kErrorNotSupportedOnThisPlatform; // Error!
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

	\return nullptr on error, a valid FILE * compatible with stdio.h file
		calls.

	\sa Burger::File

***************************************/

FILE* BURGER_API Burger::FileManager::OpenFile(
	const char* pFileName, const char* pType) BURGER_NOEXCEPT
{
	Filename TempPath(pFileName); // Expand the path to a full filename
	return OpenFile(&TempPath, pType);
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

	\return nullptr on error, a valid FILE * compatible with stdio.h file
		calls.

	\sa Burger::File

***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MACOS) && \
	!defined(BURGER_IOS)) || \
	defined(DOXYGEN)
FILE* BURGER_API Burger::FileManager::OpenFile(
	Filename* pFileName, const char* pType) BURGER_NOEXCEPT
{
#if defined(BURGER_DS)
	return NULL;
#else
	return fopen(pFileName->GetNative(), pType); // Open using standard fopen
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

	\sa Burger::FileManager::CopyFile(Burger::Filename *,Burger::Filename *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::CopyFile(
	const char* pDestName, const char* pSrcName) BURGER_NOEXCEPT
{
	Filename Dest(pDestName);     // Expand the path to a full filename
	Filename Src(pSrcName);       // Expand the source path
	return CopyFile(&Dest, &Src); // Copy the file
}

/*! ************************************

	\brief Copy a file using OS native pathnames.

	Given an OS native formatted old pathname and new pathname, make a duplicate
	of the file at the end of the pathname.

	\param pDestName Pointer to a Burger::Filename of an OS native path for the
		new name.
	\param pSourceName Pointer to a Burger::Filename of an OS native path
		for the current file name.

	\return Zero if successful, non-zero on error.

	\sa Burger::FileManager::CopyFile(const char *,const char *)

***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MACOS) && \
	!defined(BURGER_IOS) && !defined(BURGER_XBOX360)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::FileManager::CopyFile(
	Filename* pDestName, Filename* pSourceName) BURGER_NOEXCEPT
{
#if defined(BURGER_DS)
	return kErrorNotSupportedOnThisPlatform;
#else

	eError uResult = kErrorIO;                // Assume error
	File fpsrc(pSourceName, File::kReadOnly); // Open the source file
	uintptr_t uLength = fpsrc.GetSize();      // Get the size of the source file
	if (uLength) {                            // Shall I copy anything?
		uintptr_t uMaxChunk = (uLength < 0x100000) ? uLength : 0x100000;
		uint8_t* pBuffer = static_cast<uint8_t*>(Alloc(uMaxChunk));
		if (pBuffer) {
			File fpdst(pDestName, File::kWriteOnly); // Open the dest file
			do {
				uintptr_t uChunk = uLength; // Base chunk
				if (uChunk > uMaxChunk) {
					uChunk = uMaxChunk; // Only copy the chunk
				}
				uintptr_t uReadSize = fpsrc.Read(pBuffer, uChunk); // Read data
				if (uReadSize != uChunk) {
					break;
				}
				uReadSize = fpdst.Write(pBuffer, uChunk); // Write data
				if (uReadSize != uChunk) {
					break;
				}
				uLength -= uChunk;
			} while (uLength);        // Any data left?
			if (!uLength) {           // All data copied?
				uResult = kErrorNone; // No error (So far)
			}
			if (fpdst.Close()) {    // Did the file have an error in closing?
				uResult = kErrorIO; // Crap.
			}
		}
		Free(pBuffer); // Release the copy buffer
	}
	fpsrc.Close(); // Close the source file
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

	\sa Burger::FileManager::SaveFile(Burger::Filename *,const void *,uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::SaveFile(const char* pFileName,
	const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	Filename MyName(pFileName);
	return SaveFile(&MyName, pInput, uLength);
}

/*! ************************************

	\brief Save a file using an OS native pathname.

	Given an OS native pathname and a buffer, save the contents of the buffer
	into a file and close the file.

	This function is a quick and easy way to write a buffer from memory directly
	to disk (Or any other storage medium).

	\param pFileName Pointer to a Burger::Filename of an OS native path.
	\param pInput Pointer to an array of bytes to save
	\param uLength Number of bytes in the buffer

	\return \ref TRUE if successful, \ref FALSE on error.

	\sa Burger::FileManager::SaveFile(const char *,const void *,uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::SaveFile(
	Filename* pFileName, const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	File FileRef;
	eError uResult = FileRef.Open(pFileName, File::kWriteOnly);
	if (uResult != kErrorNone) {
		// Try creating the directory
		CreateDirectoryPathDirName(pFileName);
		// Now open the file
		uResult = FileRef.Open(pFileName, File::kWriteOnly);
	}
	// File opened.
	if (uResult == kErrorNone) {
		const uintptr_t uWritten = FileRef.Write(pInput, uLength);
		uResult = FileRef.Close();
		if ((uResult == kErrorNone) && (uWritten == uLength)) {
			return kErrorNone;
		}
	}
	return kErrorIO;
}

/*! ************************************

	\brief Save a text file.

	Given a BurgerLib pathname and a buffer, save the contents of the buffer
	into a file and close the file.

	This function is a quick and easy way to write a buffer from memory directly
	to disk (Or any other storage medium).

	\note This differs from Burger::FileManager::SaveFile(
		const char *,const void *,uintptr_t) in that all "\n" values will be
		translated to the proper line feeds for the target operating system.
		Unix and Linux, no change, Windows and MSDos, it's converted to a
		"\n\r", MacOS and MacOSX it's converted to "\r".

	\param pFileName Pointer to a "C" string of a BurgerLib path.
	\param pInput Pointer to an array of bytes to save as a text file.
	\param uLength Number of bytes in the buffer

	\return \ref TRUE if successful, \ref FALSE on error.

	\sa Burger::FileManager::SaveTextFile(
		Burger::Filename *,const void *,uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::SaveTextFile(
	const char* pFileName, const void* pInput,
	uintptr_t uLength) BURGER_NOEXCEPT
{
	Burger::Filename MyName(pFileName);
	return SaveTextFile(&MyName, pInput, uLength);
}

/*! ************************************

	\brief Save a text file using an OS native filename.

	Given an OS native pathname and a buffer, save the contents of the buffer
	into a file and close the file.

	This function is a quick and easy way to write a buffer from memory directly
	to disk (Or any other storage medium).

	\note This differs from Burger::FileManager::SaveFile(
		const Burger::Filename *,const void *,uintptr_t) in that all "\n" values
		will be translated to the proper line feeds for the target operating
		system. Unix and Linux, no change, Windows and MSDos, it's converted to
		a "\n\r", MacOS and MacOSX it's converted to "\r".

	\param pFileName Pointer to a Burger::Filename of an OS native path.
	\param pInput Pointer to an array of bytes to save as a text file.
	\param uLength Number of bytes in the buffer

	\return \ref TRUE if successful, \ref FALSE on error.

	\sa Burger::FileManager::SaveTextFile(const char *,const void *,uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::SaveTextFile(
	Filename* pFileName, const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	FILE* fp = OpenFile(pFileName, "w"); // Open the file
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
	buffer. The buffer must be deleted with a call to Burger::Free(const void *)

	\note This function does no data translation. The buffer contains an exact
		byte for byte copy of the contents of the file on disk.

	\param pFileName Pointer to a "C" string of a BurgerLib path.
	\param pLength Pointer to a uintptr_t to receive the size of the buffer.

	\return nullptr if failure, a valid pointer to the buffer on success.

	\sa Burger::FileManager::LoadFile(Burger::Filename *,uintptr_t *)

***************************************/

void* BURGER_API Burger::FileManager::LoadFile(
	const char* pFileName, uintptr_t* pLength) BURGER_NOEXCEPT
{
	Filename MyName(pFileName);
	return LoadFile(&MyName, pLength);
}

/*! ************************************

	\brief Load a file into memory using a native OS pathname.

	Given an OS native pathname, open the file, allocate a buffer for it, load
	it into memory and return the pointer to the allocated buffer and the
	optional buffer size.

	This function is a quick and easy way to read the contents of a file into a
	buffer. The buffer must be deleted with a call to Burger::Free(const void *)

	\note This function does no data translation. The buffer contains an exact
		byte for byte copy of the contents of the file on disk.

	\param pFileName Pointer to a Burger::Filename of an OS native path.
	\param pLength Pointer to a uintptr_t to receive the size of the buffer.

	\return nullptr if failure, a valid pointer to the buffer on success.

	\sa Burger::FileManager::LoadFile(const char *,uintptr_t *)

***************************************/

void* BURGER_API Burger::FileManager::LoadFile(
	Filename* pFileName, uintptr_t* pLength) BURGER_NOEXCEPT
{
#if defined(BURGER_DS)
	return NULL;
#else
	File FileRef(pFileName, File::kReadOnly);
	uintptr_t uNewSize = FileRef.GetSize();
	void* pResult = NULL;
	if (uNewSize) {
#if defined(_DEBUG)
		if (Globals::GetTraceFlag() & Globals::TRACE_FILELOAD) {
			Debug::PrintString("Loading native file ");
			Debug::PrintString(pFileName->GetNative());
			Debug::PrintString(".\n");
		}
#endif
		pResult = Alloc(uNewSize);
		if (pResult) {
			if (FileRef.Read(pResult, uNewSize) != uNewSize) {
				Free(pResult);
				pResult = NULL;
				uNewSize = 0;
			}
		}
	}
	FileRef.Close();
	// Return the file length and allocated memory pointer
	if (pLength) {
		pLength[0] = uNewSize;
	}
	return pResult;
#endif
}

void BURGER_API Burger::FileManager::AddQueue(File* pFile,
	eIOCommand uIOCommand, void* pBuffer, uintptr_t uLength) BURGER_NOEXCEPT
{
	WaitUntilQueueHasSpace();

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
	m_PingIOThread.Release();
#endif
}

/*! ************************************

	\class Burger::FileManagerSimple
	\brief Initialization class for the global Burger::FileManager structure.

	By default, the Burger::FileManager structure does not automatically
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
		initialization of \ref Burger::FileManager. It's crucial that the \ref
		Burger::GlobalMemoryManager is initialized BEFORE this class instance is
		created.

	\sa Burger::GlobalMemoryManager or Burger::FileManager

***************************************/

/*! ************************************

	\fn Burger::FileManagerSimple::FileManagerSimple()
	\brief Initializes the global Burger::FileManager structure.

	\sa Burger::FileManager or Burger::FileManager::Init(void)

***************************************/

/*! ************************************

	\fn Burger::FileManagerSimple::~FileManagerSimple()
	\brief Shuts down the global Burger::FileManager structure.

	\sa Burger::FileManager or Burger::FileManager::Shutdown(void)

***************************************/
