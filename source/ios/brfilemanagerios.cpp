/***************************************

	File Manager Class: iOS version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_IOS) || defined(DOXYGEN)
#include "brerror.h"
#include "brfile.h"
#include "brmemoryfunctions.h"
#include "brstring.h"
#include "brstringfunctions.h"
#include <CoreFoundation/CFString.h>
#include <Foundation/NSAutoreleasePool.h>
#include <Foundation/NSFileManager.h>
#include <Foundation/NSString.h>
#include <fcntl.h>
#include <mach-o/dyld.h>
#include <stdlib.h>
#include <sys/attr.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <unistd.h>
#undef Free

#if !defined(DOXYGEN)
//
// vnode is missing, so include the values directly
//
// #include <sys/vnode.h>
enum vtype {
	VNON,
	VREG,
	VDIR,
	VBLK,
	VCHR,
	VLNK,
	VSOCK,
	VFIFO,
	VBAD,
	VSTR,
	VCPLX
};

//
// crt_externs.h is missing, so include the values directly
//

extern "C" char*** _NSGetArgv();
extern "C" int* _NSGetArgc();
#endif

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
	Burger::Filename* pOutput, uint_t uVolumeNum) BURGER_NOEXCEPT
{
	eError uResult = kErrorInvalidParameter;

	// If running in the emulator, it's really running on top of the MacOSX
	// file system, so perform the MacOSX handler to obtain volume names

	int fp = open("/Volumes", O_RDONLY, 0);
	if (fp != -1) {
		int eError;
		uint_t bScore = FALSE;
		uint_t bFoundRoot = FALSE;
		uint_t uEntry = 1;
		do {
			// Structure declaration of data coming from getdirentriesattr()
			struct FInfoAttrBuf {
				u_int32_t length;     // Length of this data structure
				attrreference_t name; // Offset for the filename
				fsobj_type_t objType; // VREG for file, VREG for directory
				char m_Name[256 * 4];
			};

			// Attributes requested
			attrlist AttributesList;
			// Buffer to hold the attributes and the filename
			FInfoAttrBuf Entry;

			// Initialize the attributes list
			MemoryClear(&AttributesList, sizeof(AttributesList));
			// "sizeof" for the structure
			AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;
			// Let's get the name, type of file, creation time, modification
			// time, finder information and hidden/locked flags

			// Note: If these flags are changed, the FInfoAttrBuf MUST be
			// adjusted to reflect the request or weird stuff will happen
			AttributesList.commonattr = ATTR_CMN_NAME | ATTR_CMN_OBJTYPE;

			// For some dumb reason, SDK 10.5 insists this is declared unsigned
			// int on 64 bit CPUs
#if defined(BURGER_64BITCPU)
			unsigned int uCount = 1;
			unsigned int uJunkBase;
			unsigned int uNewState;
#else
			unsigned long uCount = 1; // Load only a single directory entry
			unsigned long uJunkBase;
			unsigned long uNewState;
#endif

			// Get the directory entry
			eError = getdirentriesattr(fp, &AttributesList, &Entry,
				sizeof(Entry), &uCount, &uJunkBase, &uNewState, 0);

			// No errors and an entry was returned?
			// Note: eError is 0 if more data is pending, 1 if this is the last
			// entry. uCount is zero when no entry is loaded

			if (eError >= 0 && uCount) {

				// Get the pointer to the volume name
				char* pName = (reinterpret_cast<char*>(&Entry.name) +
					Entry.name.attr_dataoffset);

				// Special case for the root volume, it's a special link
				if (!bFoundRoot && Entry.objType == VLNK) {

					// Read in the link to see if it's pointing to the home
					// folder

					char LinkBuffer[128];
					String Linkname("/Volumes/", pName);
					ssize_t uLinkDataSize = readlink(
						Linkname.GetPtr(), LinkBuffer, sizeof(LinkBuffer));
					if (uLinkDataSize == 1 && LinkBuffer[0] == '/') {

						// This is the boot volume
						bFoundRoot = TRUE;
						// Is the user looking for the boot volume?
						if (!uVolumeNum) {
							bScore = TRUE;
						}
					} else {
						// Pretend it's a normal mounted volume
						Entry.objType = VDIR;
					}
				}
				// Normal volume (Enumate them)
				if (Entry.objType == VDIR) {
					if (uVolumeNum == uEntry) {
						bScore = TRUE;
					}
					++uEntry;
				}
				// Matched a volume!

				if (bScore) {
					--pName;
					// Insert a starting and ending colon
					pName[0] = ':';
					uintptr_t uIndex = StringLength(pName);
					pName[uIndex] = ':';
					pName[uIndex + 1] = 0;
					// Set the filename
					pOutput->Set(pName);
					// Exit okay!
					uResult = kErrorNone;
					break;
				}
			}
		} while (eError == 0);
		// Close the directory
		close(fp);

		// Assume this is running on a mobile device.
		// Fake a single volume named :iOSDevice:

	} else if (!uVolumeNum) {
		pOutput->Set(":iOSDevice:");
		uResult = kErrorNone;
	}
	// Clear on error
	if (uResult != kErrorNone) {
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
		const char* pBootName = MyFilename.GetPtr();
		SetPrefix(kPrefixBoot, pBootName);
		Free(g_pFileManager->m_pBootName);
		uintptr_t uMax = StringLength(pBootName);
		g_pFileManager->m_uBootNameSize = static_cast<uint_t>(uMax);
		g_pFileManager->m_pBootName = StringDuplicate(pBootName);
	}

	//
	// iOS has no "Current directory", so pull the directory the
	// *.app file resides in
	//

	char NameBuffer[2048];
	if (CFStringGetCString(reinterpret_cast<CFStringRef>(NSHomeDirectory()),
			NameBuffer, sizeof(NameBuffer), kCFStringEncodingUTF8)) {
		if (NameBuffer[0]) {
			MyFilename.SetFromNative(NameBuffer);
			SetPrefix(kPrefixCurrent,
				MyFilename.GetPtr()); // Set the standard work prefix
		}
	}

	//
	// The parameters have the app filename. Pop the name
	// and the directory where all the data resides is determined
	//

	// Get the location of the application binary
	uint32_t uSize = sizeof(NameBuffer);
	int iTest = _NSGetExecutablePath(NameBuffer, &uSize);
	if (!iTest) {
		MyFilename.SetFromNative(NameBuffer);
		MyFilename.DirName();
		SetPrefix(kPrefixApplication,
			MyFilename.GetPtr()); // Set the standard work prefix
	}

	//
	// Get the directory where the preferences should be stored
	//

	CFArrayRef pArray =
		reinterpret_cast<CFArrayRef>(NSSearchPathForDirectoriesInDomains(
			NSApplicationSupportDirectory, NSUserDomainMask, YES));
	if (CFArrayGetCount(pArray) >= 1) {
		CFStringRef pString =
			static_cast<CFStringRef>(CFArrayGetValueAtIndex(pArray, 0));
		if (CFStringGetCString(pString, NameBuffer, sizeof(NameBuffer),
				kCFStringEncodingUTF8)) {
			if (NameBuffer[0]) {
				MyFilename.SetFromNative(NameBuffer);
				SetPrefix(kPrefixPrefs,
					MyFilename.GetPtr()); // Set the preferences prefix
			}
		}
	}

	//
	// Get the /System folder
	//

	pArray = reinterpret_cast<CFArrayRef>(NSSearchPathForDirectoriesInDomains(
		NSCoreServiceDirectory, NSSystemDomainMask, YES));
	if (CFArrayGetCount(pArray) >= 1) {
		CFStringRef pString =
			static_cast<CFStringRef>(CFArrayGetValueAtIndex(pArray, 0));
		if (CFStringGetCString(pString, NameBuffer, sizeof(NameBuffer),
				kCFStringEncodingUTF8)) {
			if (NameBuffer[0]) {
				MyFilename.SetFromNative(NameBuffer);
				MyFilename.DirName();
				MyFilename.DirName();
				SetPrefix(kPrefixSystem,
					MyFilename.GetPtr()); // Set the /System folder
			}
		}
	}
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
	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		unsigned long length;               // Length of this data structure
		struct timespec m_ModificationDate; // Creation date
	};

	// Attributes requested
	attrlist AttributesList;
	// Buffer to hold the attributes and the filename
	FInfoAttrBuf Entry;

	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_MODTIME;

	// Get the directory entry
	int eError = getattrlist(
		pFileName->GetNative(), &AttributesList, &Entry, sizeof(Entry), 0);

	// No errors?

	Burger::eError uResult;
	if (eError < 0) {
		pOutput->Clear();
		uResult = kErrorFileNotFound;
	} else {
		// Get the file dates
		pOutput->Load(&Entry.m_ModificationDate);
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
	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		unsigned long length;           // Length of this data structure
		struct timespec m_CreationDate; // Creation date
	};

	// Attributes requested
	attrlist AttributesList;
	// Buffer to hold the attributes and the filename
	FInfoAttrBuf Entry;

	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_CRTIME;

	// Get the directory entry
	int eError = getattrlist(
		pFileName->GetNative(), &AttributesList, &Entry, sizeof(Entry), 0);

	// No errors?

	Burger::eError uResult;
	if (eError < 0) {
		pOutput->Clear();
		uResult = kErrorFileNotFound;
	} else {
		// Get the file dates
		pOutput->Load(&Entry.m_CreationDate);
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
	Burger::Filename* pFileName)
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
		unsigned long length; // Length of this data structure
		char finderInfo[32];  // Aux/File type are the first 8 bytes
	};

	// Attributes requested
	attrlist AttributesList;
	// Buffer to hold the attributes and the filename
	FInfoAttrBuf Entry;

	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int eError = getattrlist(
		pFileName->GetNative(), &AttributesList, &Entry, sizeof(Entry), 0);

	// No errors?

	uint_t uResult;
	if (eError < 0) {
		uResult = 0;
	} else {
		// It's parsed!
		uResult = reinterpret_cast<uint32_t*>(Entry.finderInfo)[0];
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
		unsigned long length; // Length of this data structure
		char finderInfo[32];  // Aux/File type are the first 8 bytes
	};

	// Attributes requested
	attrlist AttributesList;
	// Buffer to hold the attributes and the filename
	FInfoAttrBuf Entry;

	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int eError = getattrlist(
		pFileName->GetNative(), &AttributesList, &Entry, sizeof(Entry), 0);

	// No errors?

	uint_t uResult;
	if (eError < 0) {
		uResult = 0;
	} else {
		// It's parsed!
		uResult = reinterpret_cast<uint32_t*>(Entry.finderInfo)[1];
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
		unsigned long length; // Length of this data structure
		char finderInfo[32];  // Aux/File type are the first 8 bytes
	};

	// Attributes requested
	attrlist AttributesList;
	// Buffer to hold the attributes and the filename
	FInfoAttrBuf Entry;

	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int eError = getattrlist(
		pFileName->GetNative(), &AttributesList, &Entry, sizeof(Entry), 0);

	// No errors?

	Burger::eError uResult;
	if (eError < 0) {
		uResult = kErrorFileNotFound;
	} else {
		// It's parsed!
		pFileType[0] = reinterpret_cast<uint32_t*>(Entry.finderInfo)[0];
		pAuxType[0] = reinterpret_cast<uint32_t*>(Entry.finderInfo)[1];
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
		unsigned long length; // Length of this data structure
		char finderInfo[32];  // Aux/File type are the first 8 bytes
	};

	// Attributes requested
	attrlist AttributesList;
	// Buffer to hold the attributes and the filename
	FInfoAttrBuf Entry;

	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int eError = getattrlist(
		pFileName->GetNative(), &AttributesList, &Entry, sizeof(Entry), 0);

	// No errors?

	Burger::eError uResult;
	if (eError < 0) {
		uResult = kErrorFileNotFound;
	} else {
		// It's parsed!
		reinterpret_cast<uint32_t*>(Entry.finderInfo)[0] = uFileType;
		eError = setattrlist(pFileName->GetNative(), &AttributesList,
			&Entry.finderInfo, sizeof(Entry.finderInfo), 0);
		if (eError < 0) {
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
		unsigned long length; // Length of this data structure
		char finderInfo[32];  // Aux/File type are the first 8 bytes
	};

	// Attributes requested
	attrlist AttributesList;
	// Buffer to hold the attributes and the filename
	FInfoAttrBuf Entry;

	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int eError = getattrlist(
		pFileName->GetNative(), &AttributesList, &Entry, sizeof(Entry), 0);

	// No errors?

	Burger::eError uResult;
	if (eError < 0) {
		uResult = kErrorFileNotFound;
	} else {
		// It's parsed!
		reinterpret_cast<uint32_t*>(Entry.finderInfo)[1] = uAuxType;
		eError = setattrlist(pFileName->GetNative(), &AttributesList,
			&Entry.finderInfo, sizeof(Entry.finderInfo), 0);
		if (eError < 0) {
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
		unsigned long length; // Length of this data structure
		char finderInfo[32];  // Aux/File type are the first 8 bytes
	};

	// Attributes requested
	attrlist AttributesList;
	// Buffer to hold the attributes and the filename
	FInfoAttrBuf Entry;

	// Initialize the attributes list
	MemoryClear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int eError = getattrlist(
		pFileName->GetNative(), &AttributesList, &Entry, sizeof(Entry), 0);

	// No errors?

	Burger::eError uResult;
	if (eError < 0) {
		uResult = kErrorFileNotFound;
	} else {
		// It's parsed!
		reinterpret_cast<uint32_t*>(Entry.finderInfo)[0] = uFileType;
		reinterpret_cast<uint32_t*>(Entry.finderInfo)[1] = uAuxType;
		eError = setattrlist(pFileName->GetNative(), &AttributesList,
			&Entry.finderInfo, sizeof(Entry.finderInfo), 0);
		if (eError < 0) {
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
	// Assume an error condition
	eError uResult = kErrorIO;
	// Get the full path
	const char* pPath = pFileName->GetNative();

	// Already here?

	struct stat MyStat;
	int eError = stat(pPath, &MyStat);
	if (eError == 0) {
		// Ensure it's a directory for sanity's sake
		if (S_ISDIR(MyStat.st_mode)) {
			// There already is a directory here by this name.
			// Exit okay!
			uResult = kErrorNone;
		}

	} else {
		// No folder here...
		// Let's try the easy way
		eError = mkdir(pPath, 0777);
		if (eError == 0) {
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
						eError = mkdir(pPath, 0777);
						// Restore the pathname
						pEnd[0] = '/';
						// Error and it's not because it's already present
						if (eError != 0 && errno != EEXIST) {
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
				if ([pFileManager copyItemAtPath:pSrcString
										  toPath:pDestString
										   error:NULL] == YES) {
					uResult = kErrorNone;
				}
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
