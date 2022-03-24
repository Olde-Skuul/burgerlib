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
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#if !defined(BURGER_DARWIN)
#include <linux/limits.h>
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

	\sa Burger::FileManager or BURGER_MSDOS

***************************************/

void BURGER_API Burger::FileManager::PlatformSetup(void) BURGER_NOEXCEPT
{
	// Start by finding the /proc folder that the application has permission to
	// access /proc
	m_ProcPath = UnixFindProcFolder();

	// Now try to find /etc/mtab
	UnixFind_etc_mtab(m_EtcMtab, m_ProcPath.c_str());

	UnixFindVolumesFolder(m_VolumePrefix);

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

	\sa PlatformSetup(), Burger::FileManager

***************************************/

void BURGER_API Burger::FileManager::PlatformShutdown(void) BURGER_NOEXCEPT
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
	// Linux has no name for the boot volume, so create one.
	if (!uVolumeNum) {
		if (pOutput) {
			pOutput->Set(":boot_volume:");
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
								Min(uIndex, static_cast<uintptr_t>(PATH_MAX));

							MemoryCopy(TempBuffer + 1, pName, uIndex);
							TempBuffer[uIndex + 1] = ':';
							TempBuffer[uIndex + 2] = 0;

							// Set the filename
							pOutput->Set(TempBuffer);
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
		pOutput->Clear();
	}
	return uResult;
}

#endif

/*! ************************************

	\brief Find the folder the has the mounted volumes.

	Unix like operating systems can't agree on the name of the mount point for
	all mounted block devices. This function will invoke dark magic on the
	native linux platform to determine where the folder is located. It's usually
	/mnt, but for Ubuntu it's /media/&lt;username&gt; etc.

	\unixonly

	\param rOutput String object to receive the folder name.

	\sa g_VolumePrefixes or UnixGetFlavor()

***************************************/

void BURGER_API Burger::FileManager::UnixFindVolumesFolder(String& rOutput)
{
	eUnixFlavor uFlavor = UnixGetFlavor();

	// These platforms are hard coded, so just keep the runtime simple
#if defined(BURGER_DARWIN) || defined(BURGER_ANDROID) || defined(BURGER_STADIA)
	rOutput = g_VolumePrefixes[uFlavor];
#else

	// Native linux is not forgiving, let's find it.
	if (uFlavor != kUnixNative) {
		rOutput = g_VolumePrefixes[uFlavor];
	} else {

		// Ubuntu 20 uses /media/ followed by the logged in user's name
		String Temp;
		if (!GetUserLoginName(&Temp)) {
			rOutput = "/media/";
			rOutput += Temp;
		} else {
			// Punt
			rOutput = g_VolumePrefixes[uFlavor];
		}
	}

#endif
}
#endif
