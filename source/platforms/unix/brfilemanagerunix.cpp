/***************************************

	File Manager Class: Unix/Posix version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_UNIX) || defined(DOXYGEN)
#include "brglobals.h"
#include "brmemoryfunctions.h"
#include "brosstringfunctions.h"
#include "brunixtypes.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#if !defined(BURGER_DARWIN)
#include <linux/limits.h>
#include <sys/sendfile.h>
#else
#include <copyfile.h>
#endif

#if !defined(DOXYGEN)
// iOS doesn't have stat64, but stat performs the same action
#if (defined(BURGER_DARWIN) && defined(BURGER_ARM)) || defined(BURGER_IOS)
#define stat64 stat
#define fstat64 fstat
#endif

#if defined(BURGER_DARWIN)
#undef st_mtime
#undef st_atime
#define off64_t off_t
#define lseek64 lseek
#define st_mtime st_mtimespec
#define st_atime st_atimespec
#endif
#endif

#if !defined(BURGER_DARWIN) && !defined(DOXYGEN)

// List of file systems that are always present
static const char* g_pPermanentFileSystems[] = {
#if defined(BURGER_LINUX)
	"zfs", "wslfs", "drvfs", "smbfs"
#else
	"ext3", "ext4", "vfat", "msdos", "sdfat", "fuseblk", "f2fs"
#endif
};

// List of file system types to block
static const char* g_pBlockedFileSystems[] = {"nodev", "squashfs", "nullfs"};

// List of invalid mounted volumes (OS controlled)
static const char* g_pBlockedMountedVolumes[] = {"none", "swap"};

#endif

#if !defined(BURGER_DARWIN) || defined(DOXYGEN)

/*! ************************************

	\brief Find the location of /proc

	Checks for the location of /proc and if the application has permission to
	access it. It will check for /proc first, then /proc/self, otherwise it will
	return \ref nullptr.

	\unixonly

	\return Pointer to the name of the /proc folder.

***************************************/

const char* BURGER_API Burger::FileManager::UnixFindProcFolder(
	void) BURGER_NOEXCEPT
{
	// Start by finding the /proc folder that the application has permission to
	// access
	struct stat tempstat;

	const char* pResult;
	// Try the global one first, will generate an error if permission is not
	// granted
	if (!stat("/proc", &tempstat) && S_ISDIR(tempstat.st_mode)) {
		pResult = "/proc";

		// Try the application's one instead
	} else if (!stat("/proc/self", &tempstat) && S_ISDIR(tempstat.st_mode)) {
		pResult = "/proc/self";
	} else {

		// This app is boned.
		pResult = nullptr;
	}
	return pResult;
}

/*! ************************************

	\brief Find the location of /etc/mtab

	Checks for the location of /etc/mtab and if the application has permission
	to access it. Otherwise, it tries /proc/mounts, and then /proc/self/mounts

	\unixonly

	\param rOutput String that will contain the filename for /etc/mtab
	\param pProcPath "C" string containing the name of the /proc directory

***************************************/

void BURGER_API Burger::FileManager::UnixFind_etc_mtab(
	String& rOutput, const char* pProcPath) BURGER_NOEXCEPT
{
	struct stat tempstat;

	// Default if path not found
	const char* pResult = "/proc/self/mounts";

	// Can't go out of scope
	String Temp;

	// Do the easy one first
	if (!stat("/etc/mtab", &tempstat)) {
		pResult = "/etc/mtab";

	} else {
		// Try mounts instead but only if there's a /proc folder
		if (!pProcPath || !pProcPath[0]) {

			// Note: the proc folder could be /proc or /proc/self, so
			// Factor that in to the code below

			// Try /proc/mounts
			Temp = pProcPath;
			Temp += "/mounts";
			if (!stat(Temp.c_str(), &tempstat)) {
				pResult = Temp.c_str();
			} else {
				// Try /proc/self/mounts
				Temp = pProcPath;
				Temp += "/self/mounts";
				if (!stat(Temp.c_str(), &tempstat)) {
					pResult = Temp.c_str();
				}
			}
		}
	}
	// Save the result
	rOutput = pResult;
}

/*! ************************************

	\brief Create the list of file system types

	Parse /proc/filesystems and generate a list of strings of all the valid
	mounted file systems.

	On Android, this function will likely fail due to strong permissions but
	will succeed on a rooted device. Android will always return the list at the
	minimum, ``ext3``, ``ext4``, ``vfat``, ``msdos``, ``sdfat``, ``fuseblk``,
	``f2fs``

	Linux devices will always return at the minimum, ``zfs``, ``wslfs``,
	``drvfs``, ``smbfs``

	``none``, ``squashfs``, and ``swap`` are always excluded.

	\unixonly

	\param rFSTypes Array to store all the parsed file systems
	\param pProcPath "C" string containing the name of the /proc directory

***************************************/

void BURGER_API Burger::FileManager::UnixParseFSTypes(
	ClassArray<String>& rFSTypes, const char* pProcPath) BURGER_NOEXCEPT
{
	// Start with a default list of file system types
	rFSTypes.clear();

	uintptr_t i = BURGER_ARRAYSIZE(g_pPermanentFileSystems);
	const char** ppWork = g_pPermanentFileSystems;
	do {
		// Add in the entries
		rFSTypes.push_back(*ppWork);
		++ppWork;
	} while (--i);

	// If there is no /proc folder, end
	if (pProcPath && pProcPath[0]) {

		// Scan the file "/proc/filesystems" for valid file systems
		String FileSystemsName(pProcPath, "/filesystems");
		FILE* filesystemfp = fopen(FileSystemsName.c_str(), "r");

		// Did it open? Android likely fails this test
		if (filesystemfp) {
			// Parse every string to determine which mounted files systems
			// are valid
			char templine[256];
			while (fgets(templine, sizeof(templine), filesystemfp)) {
				char* pTempLine;
				char* pToken =
					StringToken(templine, g_TokenDelimiters, &pTempLine);
				if (pToken) {
					uintptr_t uTemp = BURGER_ARRAYSIZE(g_pBlockedFileSystems);
					const char** ppWork2 = g_pBlockedFileSystems;
					do {
						// Is it an invalid entry?
						if (!StringCaseCompare(pToken, *ppWork2)) {
							break;
						}
						++ppWork2;
					} while (--uTemp);
					if (!uTemp) {
						// Valid mounted file system
						rFSTypes.push_back(pToken);
					}
				}
			}
			fclose(filesystemfp);
		}
	}
}

/*! ************************************

	\brief Create the list of mounted volumes

	Parse /etc/fstab and generate a list of strings of all the valid
	mounted volumes

	\unixonly

	\param rFSTabs Array to store all the parsed volumes

***************************************/

void BURGER_API Burger::FileManager::UnixParseFStab(
	ClassArray<String>& rFSTabs) BURGER_NOEXCEPT
{
	// Start with a clean output
	rFSTabs.clear();

	// Parse out the system table
	FILE* fstabfp = fopen("/etc/fstab", "r");
	// Did it open?
	if (fstabfp) {
		// Parse every string to determine which mounted files systems
		// are valid
		char templine[256];
		while (fgets(templine, sizeof(templine), fstabfp)) {
			char* pTempLine;
			char* pToken = StringToken(templine, g_TokenDelimiters, &pTempLine);

			// Was there a token? And not a comment?
			if (pToken && (pToken[0] != '#')) {
				// Use the second entry.
				pToken = StringToken(nullptr, g_TokenDelimiters, &pTempLine);
				if (pToken) {
					uintptr_t i = BURGER_ARRAYSIZE(g_pBlockedMountedVolumes);
					const char** ppWork = g_pBlockedMountedVolumes;
					do {
						// Is it an invalid entry?
						if (!StringCaseCompare(pToken, *ppWork)) {
							break;
						}
						++ppWork;
					} while (--i);
					if (!i) {
						// Valid mounted file system
						rFSTabs.push_back(pToken);
					}
				}
			}
		}
		fclose(fstabfp);
	}
}

/*! ************************************

	\brief Create the list of mounted volumes

	Parse /etc/mtab generate a list of strings of all the valid
	mounted volumes paths. The path /proc/mounts is an acceptable alternate.

	\param rVolumePaths Array to store all the parsed volumes
	\param pEtcMtab Pointer to "C" string path to /etc/mtab
	\param rFSTypes Array of valid file systems

***************************************/

void BURGER_API Burger::FileManager::UnixParseMTab(
	ClassArray<String>& rVolumePaths, const char* pEtcMtab,
	const ClassArray<String>& rFSTypes) BURGER_NOEXCEPT
{
	rVolumePaths.clear();
	// Parse out the system table
	FILE* etcmtabfp = fopen(pEtcMtab, "r");
	// Did it open?
	if (etcmtabfp) {
		// Parse every string to determine which mounted files systems
		// are valid
		char templine[256];
		while (fgets(templine, sizeof(templine), etcmtabfp)) {
			char* pTempLine = templine;
			// Skip the first entry
			Burger::StringToken(nullptr, Burger::g_TokenDelimiters, &pTempLine);

			// Mounted volume name
			char* pVolumePath = Burger::StringToken(
				nullptr, Burger::g_TokenDelimiters, &pTempLine);

			// File system used for the volume
			char* pFileSystem = Burger::StringToken(
				nullptr, Burger::g_TokenDelimiters, &pTempLine);

			// Was there a token? And not a comment?
			if (pFileSystem) {

				uintptr_t i = rFSTypes.size();
				if (i) {
					const Burger::String* ppWork = rFSTypes.data();
					do {
						// Is it an invalid entry?
						if (!Burger::StringCaseCompare(
								pFileSystem, ppWork->c_str())) {
							rVolumePaths.push_back(pVolumePath);
							break;
						}
						++ppWork;
					} while (--i);
				}
			}
		}
		fclose(etcmtabfp);
	}
}

/***************************************

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

void BURGER_API Burger::FileManager::platform_setup(void) BURGER_NOEXCEPT
{
	// Start by finding the /proc folder that the application has permission to
	// access /proc
	m_ProcPath = UnixFindProcFolder();

	// Now try to find /etc/mtab
	UnixFind_etc_mtab(m_EtcMtab, m_ProcPath.c_str());

	unix_find_volumes_folder(&m_VolumePrefix);

	// Parse /proc/filesystems
	UnixParseFSTypes(m_FSTypes, m_ProcPath.c_str());

	// Parse /etc/fstab
	UnixParseFStab(m_FSTabs);

	// Parse /etc/mtab to get mounted volumes
	UnixParseMTab(m_VolumePaths, m_EtcMtab, m_FSTypes);

	// Pull in uname()

#if 0
	Debug::Message("m_ProcPath %s\n", m_ProcPath.c_str());
	Debug::Message("m_EtcMtab %s\n", m_EtcMtab.c_str());
	Debug::Message("m_VolumePrefix %s\n", m_VolumePrefix.c_str());
	Uname LocalUName;
	LocalUName.Init();
	Debug::Message("Uname.m_sysnam %s\n", LocalUName.m_sysnam.c_str());
	Debug::Message("Uname.m_nodename %s\n", LocalUName.m_nodename.c_str());
	Debug::Message("Uname.m_release %s\n", LocalUName.m_release.c_str());
	Debug::Message("Uname.m_version %s\n", LocalUName.m_version.c_str());
	Debug::Message("Uname.m_machine %s\n", LocalUName.m_machine.c_str());
	Debug::Message("Uname.m_domainname %s\n", LocalUName.m_domainname.c_str());

	uintptr_t i;
	for (i = 0; i < m_FSTypes.size(); ++i) {
		Debug::Message("m_FSTypes %s\n", m_FSTypes[i].c_str());
	}

	for (i = 0; i < m_FSTabs.size(); ++i) {
		Debug::Message("m_FSTabs %s\n", m_FSTabs[i].c_str());
	}

	for (i = 0; i < m_VolumePaths.size(); ++i) {
		Debug::Message("m_VolumePaths %s\n", m_VolumePaths[i].c_str());
	}
#endif
}

/***************************************

	\brief Handle platform specific shutdown code

	Calls system functions to release cached values obtained from the platform's
	operating system.

	\sa platform_setup() or \ref FileManager

***************************************/

void BURGER_API Burger::FileManager::platform_shutdown(void) BURGER_NOEXCEPT
{
	// Clear out the caches
	m_VolumePaths.clear();
	m_FSTabs.clear();
	m_FSTypes.clear();
	m_VolumePrefix.clear();
	m_EtcMtab.clear();
	m_ProcPath.clear();
}

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
	// Linux has no name for the boot volume, so create one.
	if (!uVolumeNum) {
		if (pOutput) {
			pOutput->assign(":boot_volume:");
		}
		return kErrorNone;
	}

	// Assume volume not permitted
	eError uResult = kErrorInvalidParameter;

	// Was a parent directory found?
	if (!g_pFileManager->m_VolumePrefix.empty()) {
		// Change the error to a simple volume not found error
		char TempBuffer[PATH_MAX + 3];
		uResult = kErrorVolumeNotFound;

		// Open the volume directory
		DIR* fp = opendir(g_pFileManager->m_VolumePrefix.c_str());
		if (fp) {
			// Start with #1 (Boot volume is special cased)
			uint_t uEntry = 1;
			for (;;) {
				// Get the directory entry
				struct dirent* pDirEntry = readdir(fp);

				// No errors and an entry was returned?
				if (!pDirEntry) {
					break;
				}

				// Get the pointer to the volume data
				const char* pName = pDirEntry->d_name;

				// Special case for the root volume, it's a special link
				uint_t uType = pDirEntry->d_type;

				// Ignore . and ..
				if (pDirEntry->d_name[0] == '.') {
					const uintptr_t uD_Namlen = StringLength(pDirEntry->d_name);
					if ((uD_Namlen == 1) ||
						((uD_Namlen == 2) && (pDirEntry->d_name[1] == '.'))) {

						// Force the test ahead to fail
						uType = DT_BLK;
					}
				}

				// Normal volume (Enumate them)
				if (uType == DT_DIR) {

					// Found the entry?
					if (uVolumeNum == uEntry) {
						if (pOutput) {

							// Insert a starting and ending colon
							TempBuffer[0] = ':';
							uintptr_t uIndex = StringLength(pName);

							// Failsafe
							uIndex =
								minimum(uIndex, static_cast<uintptr_t>(PATH_MAX));

							MemoryCopy(TempBuffer + 1, pName, uIndex);
							TempBuffer[uIndex + 1] = ':';
							TempBuffer[uIndex + 2] = 0;

							// Set the filename
							pOutput->assign(TempBuffer);
						}
						// Exit okay!
						uResult = kErrorNone;
						break;
					}
					++uEntry;
				}
			}

			// Close the directory
			closedir(fp);
		}
	}

	// Clear on error
	if ((uResult != kErrorNone) && pOutput) {
		// Kill the string since I have an error
		pOutput->clear();
	}
	return uResult;
}

#endif

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
	eError uResult = kErrorFileNotFound;

	struct stat MyStat;
	int iError = stat(pFileName->get_native(), &MyStat);
	if (iError != -1) {
		// Return the modification time
		pOutput->Load(reinterpret_cast<const timespec*>(&MyStat.st_mtime));
		uResult = kErrorNone;
	}

	if (uResult != kErrorNone) {
		pOutput->Clear();
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
// Darwin has creation time in stat64
#if defined(BURGER_DARWIN)

	eError uResult = kErrorFileNotFound;

	// Get the data
	struct stat64 MyStat;
	int iError = stat64(pFileName->get_native(), &MyStat);

	if (iError != -1) {
		// If it succeeded, the file must exist
		pOutput->Load(&MyStat.st_birthtimespec);
		uResult = kErrorNone;
	}

	if (uResult != kErrorNone) {
		pOutput->Clear();
	}
	return uResult;

// Linux and Android 11 or higher support statx
#elif defined(BURGER_LINUX) || \
	(defined(BURGER_ANDROID) && (__ANDROID_API__ >= 30))

	// If the filesystem supports it, get the creation time
	// Note: Usually this is for files on the network. Most
	// Linux filesystems do not support file creation time

	eError uResult = kErrorFileNotFound;

	// Use statx
	struct statx MyStat;
	int iError = statx(AT_FDCWD, pFileName->get_native(),
		AT_SYMLINK_NOFOLLOW | AT_STATX_FORCE_SYNC,
		STATX_BTIME | STATX_BASIC_STATS, &MyStat);

	// Successful call?
	if (iError != -1) {

		// stx_btime is a statx_timestamp, translate to timespec
		struct timespec Spec;

		// If the time returned is zero, it means the file system
		// didn't support creation time
		if ((Spec.tv_sec = MyStat.stx_btime.tv_sec) == 0) {
			uResult = kErrorNotSupportedOnThisPlatform;
		} else {
			Spec.tv_nsec = MyStat.stx_btime.tv_nsec;
			pOutput->Load(&Spec);
			uResult = kErrorNone;
		}
	}

	// Clear on error
	if (uResult != kErrorNone) {
		pOutput->Clear();
	}
	return uResult;

#else
	// Not on this platform
	pOutput->Clear();
	return kErrorNotSupportedOnThisPlatform;
#endif
}

/***************************************

	Determine if a file exists.
	I will return TRUE if the specified path is a path to a file that exists, if
	it doesn't exist or it's a directory, I return FALSE.

	Note : I do not check if the file havs any data in it. Just the existence of
	the file.

***************************************/

uint_t BURGER_API Burger::FileManager::does_file_exist(
	Filename* pFileName) BURGER_NOEXCEPT
{
	// stat the file
	struct stat MyStat;
	int eError = stat(pFileName->get_native(), &MyStat);

	uint_t uResult = FALSE;
	if (eError >= 0) {
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
	// Assume an eror condition
	eError uResult = kErrorIO;

	// Get the full path
	const char* pPath = pFileName->get_native();

	// Already here?
	struct stat MyStat;
	int iError = stat(pPath, &MyStat);
	if (iError == 0) {
		// Ensure it's a directory for sanity's sake
		if (S_ISDIR(MyStat.st_mode)) {
			// There already is a directory here by this name.
			// Exit okay!
			uResult = kErrorNone;
		}

	} else {
		// No folder here...
		// Let's try the easy way

		iError = mkdir(pPath, 0777);
		if (iError == 0) {
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
						iError = mkdir(pPath, 0777);
						// Restore the pathname
						pEnd[0] = '/';
						// Error and it's not because it's already present
						if (iError != 0 && errno != EEXIST) {
							// Uh, oh... Perhaps not enough permissions?
							uResult = kErrorIO;
							break;
						}
						// Skip past this fragment
						pWork = pEnd + 1;
						// Get to the next fragement
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
	eError uResult = kErrorNone;
	if (unlink(pFileName->get_native()) != 0) {

		// If it doesn't exist, report it.
		if (errno == ENOENT) {
			uResult = kErrorFileNotFound;
		} else {
			uResult = kErrorIO;

			// Was it a directory?
			if ((errno == EISDIR) || (errno == EPERM)) {
				if (!rmdir(pFileName->get_native())) {
					uResult = kErrorNone;
				}
			}
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
	if (!rename(pOldName->get_native(), pNewName->get_native())) {
		return kErrorNone;
	}

	// Oh oh...
	return kErrorIO;
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
	if (!chdir(pDirName->get_native())) {
		return kErrorNone;
	}
	return kErrorIO; // Error!
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
	// Do it the stdlib way!
	return fopen(pFileName->get_native(), pType);
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
	// Try opening the input file
	int input_fp = open(pSourceName->get_native(), O_RDONLY);
	if (input_fp == -1) {
		return kErrorOpenFailure;
	}

	// Try creating the output file
	int output_fp = creat(pDestName->get_native(), 0660);
	if (output_fp == -1) {
		close(input_fp);
		return kErrorAccessDenied;
	}

	eError uResult = kErrorNone;

#if defined(BURGER_DARWIN)
	// Apple has an app for that!
	int iResult = fcopyfile(input_fp, output_fp, 0, COPYFILE_ALL);
	if (iResult < 0) {
		uResult = kErrorIO;
	}
#else

	// sendfile() will copy the file for Android or Linux
	off_t iTransferred = 0;
	struct stat MyStat;
	MemoryClear(&MyStat, sizeof(MyStat));
	fstat(input_fp, &MyStat);
	ssize_t iResult =
		sendfile(output_fp, input_fp, &iTransferred, MyStat.st_size);

	// Did it fail?
	if (iResult == -1) {
		uResult = kErrorIO;
	}
#endif

	// Close the input and output files
	close(input_fp);
	close(output_fp);

	return uResult;
}

/*! ************************************

	\fn Burger::FileManager::get_volume_prefix(void)
	\brief Access the volume prefix string.

	Unix like operating systems can't agree on the name of the mount point for
	all mounted block devices. This function will return the prefix for the
	pathname for a mount point.

	\unixonly

	\return String pointer to the volume prefix string.

	\sa unix_find_volumes_folder(String*)

***************************************/

/*! ************************************

	\brief Find the folder the has the mounted volumes.

	Unix like operating systems can't agree on the name of the mount point for
	all mounted block devices. This function will invoke dark magic on the
	native linux platform to determine where the folder is located. It's usually
	/mnt, but for Ubuntu it's /media/&lt;username&gt; etc.

	\unixonly

	\param pOutput String pointer to receive the folder name.

	\sa g_VolumePrefixes or UnixGetFlavor()

***************************************/

void BURGER_API Burger::FileManager::unix_find_volumes_folder(
	String* pOutput) BURGER_NOEXCEPT
{
	eUnixFlavor uFlavor = UnixGetFlavor();

	// These platforms are hard coded, so just keep the runtime simple
#if defined(BURGER_DARWIN) || defined(BURGER_ANDROID) || defined(BURGER_STADIA)
	*pOutput = g_VolumePrefixes[uFlavor];
#else

	// Native linux is not forgiving, let's find it.
	if (uFlavor != kUnixNative) {
		*pOutput = g_VolumePrefixes[uFlavor];
	} else {

		// Ubuntu 20 uses /media/ followed by the logged in user's name
		String Temp;
		if (!GetUserLoginName(&Temp)) {
			*pOutput = "/media/";
			*pOutput += Temp;
		} else {
			// Punt
			*pOutput = g_VolumePrefixes[uFlavor];
		}
	}

#endif
}
#endif
