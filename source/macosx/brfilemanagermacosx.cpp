/***************************************

	File Manager Class: MacOS X version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
#include "brfile.h"
#include "brmemoryfunctions.h"
#include "brstring.h"

#include <crt_externs.h>
#include <dirent.h>
#include <fcntl.h>
#include <mach-o/dyld.h>
#include <stdlib.h>
#include <string.h>
#include <sys/attr.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/vnode.h>
#include <unistd.h>

#include <AvailabilityMacros.h>

#if defined(BURGER_METROWERKS)
#include <CarbonCore/Files.h>
#include <CarbonCore/Folders.h>
#else
#include <Carbon/Carbon.h>
#endif

#include <Foundation/NSAutoreleasePool.h>
#include <Foundation/NSFileManager.h>
#include <Foundation/NSString.h>
#undef Free

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
	char TempBuffer[PATH_MAX + 3];
	eError uResult = kErrorVolumeNotFound;

	// Open the volume directory
	DIR* fp = opendir("/Volumes");
	if (fp) {

		// Volume not found, nor the boot volume
		uint_t bFoundIt = FALSE;
		uint_t bFoundRoot = FALSE;

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
			if (!bFoundRoot && (uType == DT_LNK)) {

				// Read in the link to see if it's pointing to the home folder
				// of '/'
				String Linkname("/Volumes/", pName);
				ssize_t uLinkDataSize =
					readlink(Linkname.c_str(), TempBuffer, PATH_MAX);

				// Only care if it resolves to '/', all others, ignore,
				// including errors
				if (uLinkDataSize == 1 && TempBuffer[0] == '/') {

					// This is the boot volume
					bFoundRoot = TRUE;
					// Is the user looking for the boot volume?
					if (!uVolumeNum) {
						bFoundIt = TRUE;
					}
				} else {
					// Pretend it's a normal mounted volume
					uType = DT_DIR;
				}

				// Ignore . and ..
			} else if (pDirEntry->d_name[0] == '.') {
				if ((pDirEntry->d_namlen == 1) ||
					((pDirEntry->d_namlen == 2) &&
						(pDirEntry->d_name[1] == '.'))) {

					// Force the test ahead to fail
					uType = DT_BLK;
				}
			}

			// Normal volume (Enumate them)
			if (uType == DT_DIR) {
				if (uVolumeNum == uEntry) {
					bFoundIt = TRUE;
				}
				++uEntry;
			}

			// Matched a volume!

			if (bFoundIt) {
				if (pOutput) {

					// Insert a starting and ending colon
					TempBuffer[0] = ':';
					uintptr_t uIndex = StringLength(pName);

					// Failsafe
					uIndex = Min(uIndex, static_cast<uintptr_t>(PATH_MAX));

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
		}

		// Close the directory
		closedir(fp);
	}

	// Clear on error
	if ((uResult != kErrorNone) && pOutput) {
		// Kill the string since I have an error
		pOutput->Clear();
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
	// Set the standard work prefix
	eError uResult = MyFilename.SetSystemWorkingDirectory();
	SetPrefix(kPrefixCurrent, &MyFilename);

	uResult = GetVolumeName(&MyFilename, 0); // Get the boot volume name
	if (uResult == kErrorNone) {
		// Set the initial prefix
		const char* pBootName = MyFilename.c_str();
		SetPrefix(kPrefixBoot, pBootName);
		Free(g_pFileManager->m_pBootName);
		uintptr_t uMax = StringLength(pBootName);
		g_pFileManager->m_uBootNameSize = static_cast<uint_t>(uMax);
		g_pFileManager->m_pBootName = StringDuplicate(pBootName);
	}

	char* pTemp = getcwd(nullptr, 0); // This covers all versions
	if (pTemp) {
		MyFilename.SetFromNative(pTemp);
		SetPrefix(kPrefixCurrent,
			MyFilename.c_str()); // Set the standard work prefix
		free(pTemp);
	}

	// Get the location of the application binary
	MyFilename.SetApplicationDirectory();
	SetPrefix(kPrefixApplication,
		MyFilename.c_str()); // Set the standard work prefix

	char NameBuffer[2048];
	FSRef MyRef;
	if (!FSFindFolder(
			kOnSystemDisk, kSystemFolderType, kDontCreateFolder, &MyRef)) {
		if (!FSRefMakePath(&MyRef, reinterpret_cast<uint8_t*>(NameBuffer),
				static_cast<UInt32>(sizeof(NameBuffer)))) {
			MyFilename.SetFromNative(NameBuffer);
			// Set the standard work prefix
			SetPrefix(kPrefixSystem, MyFilename.c_str());
		}
	}

	if (!FSFindFolder(
			kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder, &MyRef)) {
		if (!FSRefMakePath(&MyRef, reinterpret_cast<uint8_t*>(NameBuffer),
				static_cast<UInt32>(sizeof(NameBuffer)))) {
			MyFilename.SetFromNative(NameBuffer);
			// Set the standard work prefix
			SetPrefix(kPrefixPrefs, MyFilename.c_str());
		}
	}
	return kErrorNone;
}

/***************************************

	This routine will get the time and date from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Burger::eError BURGER_API Burger::FileManager::GetModificationTime(
	Burger::Filename* pFileName, Burger::TimeDate_t* pOutput)
{
	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		uint32_t m_uLength;          // Length of this data structure
		timespec m_ModificationDate; // Creation date
	};

	// Buffer to hold the attributes and the filename
	uint8_t Entry[sizeof(FInfoAttrBuf)];

	// Attributes requested
	attrlist AttributesList;
	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_MODTIME;

	// Get the directory entry
	int iError = getattrlist(
		pFileName->GetNative(), &AttributesList, Entry, sizeof(Entry), 0);

	// No errors?

	eError uResult;
	if (iError < 0) {
		pOutput->Clear();
		uResult = kErrorFileNotFound;
	} else {
		const timespec* pTemp =
			reinterpret_cast<const timespec*>(&Entry[sizeof(uint32_t)]);
		// Get the file dates
		pOutput->Load(pTemp);
		// It's parsed!
		uResult = kErrorNone;
	}
	return uResult;
}

/***************************************

	This routine will get the time and date from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Burger::eError BURGER_API Burger::FileManager::GetCreationTime(
	Burger::Filename* pFileName, Burger::TimeDate_t* pOutput)
{
	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		uint32_t m_uLength;      // Length of this data structure
		timespec m_CreationDate; // Creation date
	};
	// Buffer to hold the attributes and the filename
	uint8_t Entry[sizeof(FInfoAttrBuf)];

	// Attributes requested
	attrlist AttributesList;

	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_CRTIME;

	// Get the directory entry
	int iError = getattrlist(
		pFileName->GetNative(), &AttributesList, Entry, sizeof(Entry), 0);

	// No errors?

	eError uResult;
	if (iError < 0) {
		pOutput->Clear();
		uResult = kErrorFileNotFound;
	} else {
		// Get the file dates
		const timespec* pTemp =
			reinterpret_cast<const timespec*>(&Entry[sizeof(uint32_t)]);
		pOutput->Load(pTemp);
		// It's parsed!
		uResult = kErrorNone;
	}
	return uResult;
}

/***************************************

	Determine if a file exists.
	I will return TRUE if the specified path is a path to a file that exists, if
	it doesn't exist or it's a directory, I return FALSE.

	Note : I do not check if the file havs any data in it. Just the existence of
	the file.

***************************************/

uint_t BURGER_API Burger::FileManager::DoesFileExist(
	Burger::Filename* pFileName) BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	struct stat MyStat;
	int eError = stat(pFileName->GetNative(), &MyStat);
	if (eError >= 0) {
		// If it succeeded, the file must exist
		uResult = TRUE;
	}
	return uResult;
}

/***************************************

	Get a file's Filetype
	Only valid for GSOS and MacOS

***************************************/

uint32_t BURGER_API Burger::FileManager::GetFileType(
	Burger::Filename* pFileName)
{
	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		uint32_t m_uLength;  // Length of this data structure
		char finderInfo[32]; // Aux/File type are the first 8 bytes
	};
	// Buffer to hold the attributes and the filename
	uint8_t Entry[sizeof(FInfoAttrBuf)];

	// Attributes requested
	attrlist AttributesList;

	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int eError = getattrlist(
		pFileName->GetNative(), &AttributesList, Entry, sizeof(Entry), 0);

	// No errors?

	uint_t uResult;
	if (eError < 0) {
		uResult = 0;
	} else {
		// It's parsed!
		uResult = reinterpret_cast<const uint32_t*>(Entry)[1];
	}
	return uResult;
}

/***************************************

	Get a file's Auxtype
	Only valid for GSOS and MacOS

***************************************/

uint32_t BURGER_API Burger::FileManager::GetAuxType(Burger::Filename* pFileName)
{
	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		uint32_t m_uLength;  // Length of this data structure
		char finderInfo[32]; // Aux/File type are the first 8 bytes
	};
	// Buffer to hold the attributes and the filename
	uint8_t Entry[sizeof(FInfoAttrBuf)];

	// Attributes requested
	attrlist AttributesList;

	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int eError = getattrlist(
		pFileName->GetNative(), &AttributesList, Entry, sizeof(Entry), 0);

	// No errors?

	uint_t uResult;
	if (eError < 0) {
		uResult = 0;
	} else {
		// It's parsed!
		uResult = reinterpret_cast<const uint32_t*>(Entry)[2];
	}
	return uResult;
}

/***************************************

	Get a file's Auxtype and FileType
	Only valid for GSOS and MacOS

***************************************/

Burger::eError BURGER_API Burger::FileManager::GetFileAndAuxType(
	Burger::Filename* pFileName, uint32_t* pFileType, uint32_t* pAuxType)
{
	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		uint32_t m_uLength;  // Length of this data structure
		char finderInfo[32]; // Aux/File type are the first 8 bytes
	};
	// Buffer to hold the attributes and the filename
	uint8_t Entry[sizeof(FInfoAttrBuf)];

	// Attributes requested
	attrlist AttributesList;

	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int iError = getattrlist(
		pFileName->GetNative(), &AttributesList, Entry, sizeof(Entry), 0);

	// No errors?

	eError uResult;
	if (iError < 0) {
		uResult = kErrorFileNotFound;
	} else {
		// It's parsed!
		pFileType[0] = reinterpret_cast<const uint32_t*>(Entry)[1];
		pAuxType[0] = reinterpret_cast<const uint32_t*>(Entry)[2];
		uResult = kErrorNone;
	}
	return uResult;
}

/***************************************

	Set a file's Filetype
	Only valid for GSOS and MacOS

***************************************/

Burger::eError BURGER_API Burger::FileManager::SetFileType(
	Burger::Filename* pFileName, uint32_t uFileType)
{
	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		uint32_t m_uLength;  // Length of this data structure
		char finderInfo[32]; // Aux/File type are the first 8 bytes
	};
	// Buffer to hold the attributes and the filename
	uint8_t Entry[sizeof(FInfoAttrBuf)];

	// Attributes requested
	attrlist AttributesList;

	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int iError = getattrlist(
		pFileName->GetNative(), &AttributesList, Entry, sizeof(Entry), 0);

	// No errors?

	eError uResult;
	if (iError < 0) {
		uResult = kErrorFileNotFound;
	} else {
		// It's parsed!
		reinterpret_cast<uint32_t*>(Entry)[1] = uFileType;
		iError = setattrlist(pFileName->GetNative(), &AttributesList,
			Entry + sizeof(uint32_t), 32, 0);
		if (iError < 0) {
			uResult = kErrorIO;
		} else {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	Set a file's Auxtype
	Only valid for GSOS and MacOS

***************************************/

Burger::eError BURGER_API Burger::FileManager::SetAuxType(
	Burger::Filename* pFileName, uint32_t uAuxType)
{
	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		uint32_t m_uLength;  // Length of this data structure
		char finderInfo[32]; // Aux/File type are the first 8 bytes
	};
	// Buffer to hold the attributes and the filename
	uint8_t Entry[sizeof(FInfoAttrBuf)];

	// Attributes requested
	attrlist AttributesList;

	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int iError = getattrlist(
		pFileName->GetNative(), &AttributesList, Entry, sizeof(Entry), 0);

	// No errors?

	eError uResult;
	if (iError < 0) {
		uResult = kErrorFileNotFound;
	} else {
		// It's parsed!
		reinterpret_cast<uint32_t*>(Entry)[2] = uAuxType;
		iError = setattrlist(pFileName->GetNative(), &AttributesList,
			Entry + sizeof(uint32_t), 32, 0);
		if (iError < 0) {
			uResult = kErrorIO;
		} else {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	Set a file's Filetype and Auxtype
	Only valid for GSOS and MacOS

***************************************/

Burger::eError BURGER_API Burger::FileManager::SetFileAndAuxType(
	Burger::Filename* pFileName, uint32_t uFileType, uint32_t uAuxType)
{
	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		uint32_t m_uLength;  // Length of this data structure
		char finderInfo[32]; // Aux/File type are the first 8 bytes
	};
	// Buffer to hold the attributes and the filename
	uint8_t Entry[sizeof(FInfoAttrBuf)];

	// Attributes requested
	attrlist AttributesList;
	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int iError = getattrlist(
		pFileName->GetNative(), &AttributesList, Entry, sizeof(Entry), 0);

	// No errors?

	eError uResult;
	if (iError < 0) {
		uResult = kErrorFileNotFound;
	} else {
		// It's parsed!
		reinterpret_cast<uint32_t*>(Entry)[1] = uFileType;
		reinterpret_cast<uint32_t*>(Entry)[2] = uAuxType;
		iError = setattrlist(pFileName->GetNative(), &AttributesList,
			Entry + sizeof(uint32_t), 32, 0);
		if (iError < 0) {
			uResult = kErrorIO;
		} else {
			uResult = kErrorNone;
		}
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
	// Assume an eror condition
	eError uResult = kErrorIO;
	// Get the full path
	const char* pPath = pFileName->GetNative();

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

	Change a directory using long filenames
	This only accepts Native OS filenames

***************************************/

Burger::eError BURGER_API Burger::FileManager::ChangeOSDirectory(
	Burger::Filename* pDirName)
{
	if (!chdir(pDirName->GetNative())) {
		return kErrorNone;
	}
	return kErrorIO; // Error!
}

/***************************************

	Open a file using a native path

***************************************/

FILE* BURGER_API Burger::FileManager::OpenFile(
	Burger::Filename* pFileName, const char* pType)
{
	return fopen(pFileName->GetNative(), pType); /* Do it the MacOSX way */
}

/***************************************

	Copy a file using native pathnames

***************************************/

Burger::eError BURGER_API Burger::FileManager::CopyFile(
	Burger::Filename* pDestName, Burger::Filename* pSourceName)
{
	eError uResult = kErrorIO;
	NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];

	NSFileManager* pFileManager = [[NSFileManager alloc] init];
	if (pFileManager) {
		NSString* pSrcString =
			[NSString stringWithUTF8String:pSourceName->GetNative()];
		if (pSrcString) {
			NSString* pDestString =
				[NSString stringWithUTF8String:pDestName->GetNative()];
			if (pDestString) {
#if defined(MAC_OS_X_VERSION_10_5) && 0
				if ([pFileManager copyItemAtPath:pSrcString
										  toPath:pDestString
										   error:NULL] == YES) {
					uResult = kErrorNone;
				}
#else
				if ([pFileManager copyPath:pSrcString
									toPath:pDestString
								   handler:NULL] == YES) {
					uResult = kErrorNone;
				}
#endif
			}
		}
		[pFileManager release];
	}
	if (pPool) {
		[pPool release];
	}
	// Free all allocated temp memory
	return uResult;
}

/***************************************

	Delete a file using native file system

***************************************/

Burger::eError BURGER_API Burger::FileManager::DeleteFile(
	Burger::Filename* pFileName) BURGER_NOEXCEPT
{
	if (!remove(pFileName->GetNative())) {
		return kErrorNone;
	}
	return kErrorIO; /* Oh oh... */
}

/***************************************

	Rename a file using native pathnames

***************************************/

Burger::eError BURGER_API Burger::FileManager::RenameFile(
	Burger::Filename* pNewName, Burger::Filename* pOldName)
{
	if (!rename(pOldName->GetNative(), pNewName->GetNative())) {
		return kErrorNone;
	}
	return kErrorIO; /* Oh oh... */
}

#endif
