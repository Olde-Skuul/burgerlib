/***************************************

	Filename Class

	MacOS version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brfile.h"
#include "brfilemanager.h"
#include "brglobalmemorymanager.h"
#include "brmacromanus.h"
#include "brmemoryfunctions.h"
#include "brstring.h"
#include "brstring16.h"
#include "brtick.h"
#include "brutf8.h"
#include <Files.h>
#include <Folders.h>
#include <Processes.h>
#include <string.h>

Burger::Filename::ExpandCache_t
	Burger::Filename::m_DirectoryCache[Burger::Filename::kDirectoryCacheSize];

/*! ************************************

	\struct Burger::Filename::ExpandCache_t
	\brief Structure to contain a directory cache entry

	For performance, a cache of the last Burger::Filename::kDirectoryCacheSize
	MacOS directories are stored with their Directory IDs and volume reference
	numbers. Since these number can ba invalidate when a directory is deleted or
	created, any call to a Burgerlib function that performs that action will
	also purge this cache.

	When a Burgerlib path is parsed, a Directory ID and Volume Reference needs
	to be generated. If the directory exists in the cache, the values found here
	will be used. If not, it will be determined and added to the cache.

	\sa Burger::FileManager and Burger::Filename::init_directory_cache(void)
	\note Only available on MacOS

***************************************/

/*! ************************************

	\brief Initialize the directory cache.

	This function is called by FileManager::initialize(void).
	It's not meant to be called by applications.

	\note Only available on MacOS

	\sa purge_directory_cache(void) or \ref ExpandCache_t

***************************************/

void BURGER_API Burger::Filename::init_directory_cache(void)
{
	uintptr_t i = BURGER_ARRAYSIZE(m_DirectoryCache);
	ExpandCache_t* pWork = m_DirectoryCache;
	do {
		pWork->m_pName = nullptr;
		++pWork;
	} while (--i);

	// Sanity check for the FSRef record
	BURGER_STATIC_ASSERT(sizeof(pWork->m_FSRef) >= sizeof(FSSpec));
}

/*! ************************************

	\brief Dispose of my directory cache

	This function is called by FileManager::shut_down(void) or any
	internal function that can modify the MacOS directory structure. It's not
	meant to be called by applications.

	\note Only available on MacOS

	\sa init_directory_cache(void) or \ref ExpandCache_t

***************************************/

void BURGER_API Burger::Filename::purge_directory_cache(void)
{
	uintptr_t i = BURGER_ARRAYSIZE(m_DirectoryCache);
	ExpandCache_t* pWork = m_DirectoryCache;
	do {
		Free(pWork->m_pName);
		pWork->m_pName = nullptr;
		++pWork;
	} while (--i);
}

/***************************************

	Convert a Burgerlib path to a MacOS path

	MacOS filenames have a 256 character limit. To get around this, Burgerlib
	will traverse the path to find the Volume Reference number and Directory ID
	to the deepest entry in the path. This way, in most cases, only the filename
	itself is stored in the filename buffer.

	This function initializes the Volume Reference number and Directory ID.
	Assume that they are valid at the conclusion of this call.

	Only available on MacOS. HFS only supports filenames that are 31 characters
	or less for each component, so avoid using extremely long filename
	components.

***************************************/

const char* Burger::Filename::get_native(void) BURGER_NOEXCEPT
{
	// If already parsed, skip the conversion.
	if (!m_bNativeValid) {

		// Resolve prefixes and convert to absolute path
		abs_path();

		// Mac filenames are always short, so no need to reserve space
		m_NativeFilename.clear();

		const char* pFilename = m_Filename.c_str();

		// Init the Macintosh volume numbers to the current working directory
		short sVRefNum = 0;
		long lDirID = 0;

		// Assume no device found
		uint_t uDeviceNum = UINT32_MAX;

		// Assume the directory has no length
		uintptr_t uDirLength = 0;

		// No directory parsed yet

		// Find the volume by name?
		if (pFilename[0] == ':') {

			// Get the volume number
			char* pEndVolume = StringCharacter(pFilename + 1, ':');

			// No ending colon? Error condition, should never happen
			if (pEndVolume) {

				// Truncate the string and find the volume number
				char cTemp = pEndVolume[1];
				pEndVolume[1] = 0;
				uDeviceNum = FileManager::get_volume_number(pFilename);
				pEndVolume[1] = cTemp;

				// Was a volume found?
				if (uDeviceNum != UINT32_MAX) {

					// Set up the root volume number and directory ID
					sVRefNum = static_cast<short>(uDeviceNum);
					lDirID = fsRtDirID;
					uDirLength =
						static_cast<uintptr_t>(pEndVolume - pFilename) + 1;
				}
			}

			// Find the volume by number?
		} else if (pFilename[0] == '.') {
			// ".dxx:"?
			uint_t uTemp = pFilename[1];
			if (uTemp >= 'a' && uTemp < ('z' + 1)) {
				uTemp &= 0xDF;
			}
			// Is it a 'D'?
			if (uTemp == 'D') {

				// Index past the ".D"
				uintptr_t uLength = 2;

				// Init drive number
				uDeviceNum = 0;
				for (;;) {
					// Get an ASCII char
					uTemp = pFilename[uLength];
					++uLength;
					if (uTemp == ':') {
						sVRefNum = static_cast<short>(uDeviceNum);
						lDirID = fsRtDirID;
						uDirLength = uLength;
						break;
					}

					// String ended now?
					if (!uTemp) {
						break;
					}

					// Numeric value?
					if ((uTemp < '0') || (uTemp >= '9' + 1)) {
						break;
					}
					// Add in the new digit
					uTemp = uTemp - '0';
					uDeviceNum = uDeviceNum * 10;
					uDeviceNum = uDeviceNum + uTemp;
				}
			}
		}

		// Convert the device number (Index) to an actual device number

		if (uDeviceNum != UINT32_MAX) {
			Str63 DriveName;
			HParamBlockRec hpb;
			hpb.volumeParam.ioNamePtr = DriveName;
			hpb.volumeParam.ioVRefNum = 0;
			hpb.volumeParam.filler2 = 0;
			hpb.volumeParam.ioVolIndex = static_cast<short>(uDeviceNum + 1);
			// Convert the index to a volume reference
			OSStatus uStatus = PBHGetVInfoSync(&hpb);
			if (!uStatus) {
				// No error! Update the reference
				sVRefNum = hpb.volumeParam.ioVRefNum;
			}
		}

		// At thie point the path is ready

		eError uError;
#if defined(BURGER_CFM) && defined(BURGER_68K)
		// CFM 68K only supports the old style
		uError = get_native_internal(pFilename + uDirLength, lDirID, sVRefNum);
#else
		// All others support both new and old styles
		uError = get_native_Carbon(pFilename + uDirLength, lDirID, sVRefNum);
		if (uError == kErrorNotSupportedOnThisPlatform) {
			uError =
				get_native_internal(pFilename + uDirLength, lDirID, sVRefNum);
		}
#endif
	}

	// Return the translated path
	return m_NativeFilename.c_str();
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
	clear();
	long lDirID;
	short sVRefNum;

	// Call OS
	HGetVol(0, &sVRefNum, &lDirID);
	// Set lDirID to 0 to hack to simulate GetVol()

	// Get the directory
	set_native(0, sVRefNum);
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

Burger::eError BURGER_API Burger::Filename::set_application_directory(
	void) BURGER_NOEXCEPT
{
	clear();

	// Init to my application's serial number
	ProcessSerialNumber MyNumber;
	MyNumber.lowLongOfPSN = kCurrentProcess;
	MyNumber.highLongOfPSN = 0;

	// FSSpec of the current app
	FSSpec MySpec;
	MemoryClear(&MySpec, sizeof(MySpec));

	// My input process
	ProcessInfoRec MyProcess;
	MemoryClear(&MyProcess, sizeof(MyProcess));
	MyProcess.processInfoLength = sizeof(MyProcess);

	// I don't want the name
	// MyProcess.processName = 0;
	// Get the FSSpec
	MyProcess.processAppSpec = &MySpec;

	// Locate the application
	eError uResult = kErrorThreadNotFound;
	if (!GetProcessInformation(&MyNumber, &MyProcess)) {
		uResult = set_native(
			MyProcess.processAppSpec->parID, MyProcess.processAppSpec->vRefNum);
	}
	return uResult;
}

/***************************************

	\brief Set the filename to the local machine preferences directory

	Determine the directory where the user's preferences that are local to the
	machine is located. The path is converted into UTF8 character encoding and
	stored in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like an
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::set_system_prefs_directory(
	void) BURGER_NOEXCEPT
{
	clear();

	// Internal volume references
	short MyVRef;
	// Internal drive ID
	long MyDirID;

	eError uResult = kErrorNone;

	// Get the system preferences folder (NacOSX First)
	if (FindFolder(kOnSystemDisk, kSystemPreferencesFolderType,
			kDontCreateFolder, &MyVRef, &MyDirID)) {
		// Try the macos 7-9 folder instead
		if (FindFolder(kOnSystemDisk, kSystemFolderType, kDontCreateFolder,
				&MyVRef, &MyDirID)) {
			uResult = kErrorNotADirectory;
		}
	}

	if (!uResult) {
		// Convert to burgerlib path
		uResult = set_native(MyDirID, MyVRef);
	}
	return uResult;
}

/***************************************

	\brief Set the filename to the user's preferences directory

	Determine the directory where the user's preferences that could be shared
	among all machines the user has an account with is located. The path is
	converted into UTF8 character encoding and stored in Burgerlib filename
	format.

	On platforms where a current working directory doesn't make sense, like an
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::set_user_prefs_directory(
	void) BURGER_NOEXCEPT
{
	clear();

	// Internal volume references
	short MyVRef;

	// Internal drive ID
	long MyDirID;

	eError uResult = kErrorNotADirectory;

	// Where are the user preferences stored?
	if (!FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder,
			&MyVRef, &MyDirID)) {
		uResult = set_native(MyDirID, MyVRef);
	}
	return uResult;
}

/*! ************************************

	\brief Convert a MacOS path to a Burgerlib path

	Given a "C" string to the pathname, a Directory ID and a Volume Reference
	number, create a full pathname in Burgerlib format.

	This function is commonly used when creating a file selection dialog and the
	input needs to converted into a format that is compatible with most
	Burgerlib file functions.

	\param pInput Pointer to a "C" string of a MacOS formatted filename
	\param lDirID Directory ID for the filename
	\param sVRefNum Volume ID for the filename
	\note Only available on MacOS

***************************************/

Burger::eError BURGER_API Burger::Filename::set_native(
	const char* pInput, long lDirID, short sVRefNum) BURGER_NOEXCEPT
{
	clear(); // Clear out the previous string

	uint_t Temp;         // Ascii Temp
	char TempPath[8192]; // Handle to temp buffer
	char* Output;        // Running pointer to temp buffer
	uint_t Length;       // Length of finished string

	Output = TempPath; // Get running pointer

	if (!pInput[0] ||
		pInput[0] == ':') {         // Must I prefix with the current directory?
		if (!sVRefNum && !lDirID) { // If both are zero then look up default
			HGetVol(0, &sVRefNum, &lDirID); // Call OS
			lDirID = 0;                     // Hack to simulate GetVol()
		}
		Filename MyFilename;
		eError uResult =
			MyFilename.set_native(lDirID, sVRefNum); /* Get the directory */
		if (!uResult) {                              /* Did I get a path? */
			StringCopy(Output, MyFilename.c_str());  /* Copy to output */
			Output = Output + StringLength(Output);  /* Fix pointer */
		}
		if (pInput[0]) { /* Was there a leading colon? */
			++pInput;    /* Accept the leading colon */
		}
	} else {
		Output[0] = ':'; /* Place a leading colon in the output */
		++Output;
	}

	/* Now, just copy the rest of the path */

	Temp = pInput[0];
	if (Temp) { /* Any more? */
		do {
			Output[0] = static_cast<char>(Temp); /* Save char */
			++Output;
			++pInput;         /* Accept char */
			Temp = pInput[0]; /* Next char */
		} while (Temp);       /* Still more? */
	}

	/* The wrap up... */
	/* Make sure it's appended with a colon */

	Length = static_cast<uint32_t>(
		Output - TempPath);                /* How many bytes is the new path? */
	if (Length) {                          /* Valid length? */
		if (TempPath[Length - 1] != ':') { /* Last char a colon? */
			TempPath[Length] = ':';        /* End with a colon! */
			++Length;                      /* Increase length! */
		}
	}
	TempPath[Length] = 0;
	assign(TempPath);
	return kErrorNone;
}

/*! ************************************

	\brief Create an FSSpec from the filename.

	Given a FSSpec record, fill it in for all the data needed from this filename
	so that MacOS can use the FSSpec to manipulate files.

	\maconly

	\param pFSSpec Pointer to an empty FSSpec

	\return Zero if no error.

	\sa get_FSRef()

***************************************/

Burger::eError BURGER_API Burger::Filename::get_FSSpec(
	FSSpec* pFSSpec) BURGER_NOEXCEPT
{
	// Clear it out
	MemoryClear(pFSSpec, sizeof(FSSpec));

	// Make sure the path has been processed
	const char* pNative = get_native();

	// Was the filename unparsable?
	eError uResult = kErrorNotInitialized;
	if (m_bNativeValid) {

		// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// So, it's parsed as a FSRef, convert back to a FSSpec
		if (m_bNativeValid == 2) {

			// Check if the FSRef is to an existing file
			uResult = kErrorFileNotFound;

			// Is this an incomplete FSRef?
			if (pNative[0]) {
				// Manually complete it
				pFSSpec->parID = m_lDirID;
				pFSSpec->vRefNum = m_sVRefNum;
				char TempString[64];
				MacRomanUS::translate_from_UTF8(
					TempString, sizeof(TempString), pNative);
				CStringToPString(pFSSpec->name, TempString);
			} else {

				// Do the conversion from a complete FSRef to a FSSpec
				FSRefParam Block;
				InitFSRefParam(
					&Block, reinterpret_cast<FSRef*>(m_FSRef), kFSCatInfoNone);
				Block.spec = pFSSpec;
				OSErr iError = PBGetCatalogInfoSync(&Block);
				if (!iError) {
					// All good!
					uResult = kErrorNone;
				}
			}
		} else
#endif
		{
			// Convert from UTF8 to Mac Roman
			char NameBuffer[64];
			MacRomanUS::translate_from_UTF8(
				NameBuffer, sizeof(NameBuffer), pNative);

			// Set the values of the FSSpec
			pFSSpec->vRefNum = m_sVRefNum;
			pFSSpec->parID = m_lDirID;
			CStringToPString(pFSSpec->name, NameBuffer);
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/*! ************************************

	\brief Create an FSRef from the filename.

	Convert the Filename into an FSRef record and return a pointer to it. If the
	computer cannot create an FSRef, then it's likely that the application is
	running on MacOS previous to 9.0.

	\maconly

	\return Pointer to an FSRef or \ref nullptr if not supported.

	\sa get_FSSpec(FSSpec*)

***************************************/

FSRef* BURGER_API BURGER_API Burger::Filename::get_FSRef(void) BURGER_NOEXCEPT
{
	get_native();
	if (m_bNativeValid == 2) {
		return reinterpret_cast<FSRef*>(m_FSRef);
	}
	return nullptr;
}

/*! ************************************

	\fn Burger::Filename::get_DirID(void) const
	\brief Return the Directory ID stored in the class.
	\return The Directory ID for this filename
	\sa Burger::Filename::set_DirID(long) or Burger::Filename::get_VRefNum(void)
		const

	\note Only available on MacOS and only valid after a call to \ref
		Burger::Filename::get_native()

***************************************/

/*! ************************************

	\fn Burger::Filename::get_VRefNum(void) const
	\brief Return the Volume Reference number stored in the class.
	\return The Volume Reference number for this filename.
	\sa set_VRefNum(short) or get_DirID(void) const

	\note Only available on MacOS and only valid after a call to
		\ref get_native()

***************************************/

/*! ************************************

	\fn Burger::Filename::set_DirID(long)
	\brief Set the Directory ID in the class.
	\sa Burger::Filename::get_DirID(void) const or
		Burger::Filename::set_VRefNum(short)

	\note Only available on MacOS

***************************************/

/*! ************************************

	\fn Burger::Filename::set_VRefNum(short)
	\brief Set Volume Reference number in the class.
	\sa Burger::Filename::get_VRefNum(void) const or
		Burger::Filename::set_DirID(long)

	\note Only available on MacOS

***************************************/

/*! ************************************

	\brief Convert a Mac DirID and a Volume reference into a Burgerlib path.

	Given a 32 bit directory ID and a 16 bit volume number, determine the full
	Burgerlib pathname that is its equivalent.

	What is returned is either \ref nullptr for an error condition, pOutput if
	the user supplied buffer was large enough to contain the requested data or a
	newly allocated pointer to a larger buffer to hold the requested data.

	\note If the returned pointer is not the pointer passed in via pOutput,
		dispose of it with a called to \ref Burger::Free(const void *) when
		done.

	\maconly

	\param lDirID 32-bit directory ID supplied by MacOS
	\param sVolRefNum 16-bit volume reference number supplied by MacOS

	\return Zero if no error, non zero if there was an error

***************************************/

Burger::eError BURGER_API Burger::Filename::set_native(
	long lDirID, short sVolRefNum)
{

	// Using a Macintosh directory ID and a volume reference number, return the
	// full path that the ID generates.

	// It uses the current values to get the current directory name, then by
	// traversing the directories PARENT entry, follow the tree BACKWARDS back
	// to the root. So it must constantly be prefixing my current data with the
	// newly located PARENT entry until it's gotten to the root entry.

	// The Mac is brain dead.

	// Initialize the proposed final string
	m_Filename.assign(":");

	// First step, create an FS Ref from the parent ID and volume reference
	// number for handling the name assuming a UTF8 file system

	// Save the true filename as known by MacOS Classic
	m_lDirID = lDirID;
	m_sVRefNum = sVolRefNum;
	m_NativeFilename.clear();
	m_bNativeValid = 0;

	// Assume failure
	eError uResult;

	// CFM 68K only supports the 7.1-8.1 APIs
#if defined(BURGER_CFM) && defined(BURGER_68K)
	// Use the MacOS Classic version with FSSpec
	uResult = set_native_internal();
#else
	// Carbon and Classic
	uResult = set_native_Carbon();

	// Not 9.0 or higher.
	if (uResult == kErrorNotSupportedOnThisPlatform) {
		// Do it the old way
		uResult = set_native_internal();
	}
#endif

	// All good?
	if (uResult != kErrorNone) {
		clear();
	}

	return uResult;
}

/*! ************************************

	\brief Convert Directory to Burgerlib with FSSpec

	Internal routine that converts a directory ID and a volume number into a
	Burgerlib path.

	\maconly

	\return Zero if no error, non zero if there was an error

***************************************/

Burger::eError BURGER_API Burger::Filename::set_native_internal(
	void) BURGER_NOEXCEPT
{
	short sVolRefNum = m_sVRefNum;
	long lDirID = m_lDirID;

	// Init the initial FSSpec
	FSSpec CurrentSpec;
	CurrentSpec.vRefNum = sVolRefNum;
	CurrentSpec.parID = lDirID;
	CurrentSpec.name[0] = 0;

	// Init the output string
	m_Filename.clear();

	// Since this is a HFS volume, filenames are limited to 64 characters
	char TempString[80];

	// Insert a permanent ':' for each insertion
	TempString[0] = ':';

	eError uResult = kErrorNone;
	do {
		// Get the parent folder.
		if (FSMakeFSSpec(sVolRefNum, lDirID, "\p", &CurrentSpec)) {
			uResult = kErrorIO;
			break;
		}

		// Convert from pascal to "C"
		PStringToCString(TempString + 1, CurrentSpec.name);

		// Convert from macRoman to UTF8
		// TM can take 3 characters, so 64*3 = 192. 200 will do.

		char UTF8Buffer[200];
		uintptr_t uNewLength = UTF8::FromMacRomanUS(UTF8Buffer,
			sizeof(UTF8Buffer), TempString, CurrentSpec.name[0] + 1U);

		// Insert to the final result into the filename with a preceeding colon
		uResult = m_Filename.insert(0, UTF8Buffer, uNewLength);
		if (uResult) {
			break;
		}

		// Move up one directory
		lDirID = CurrentSpec.parID;
	} while (lDirID != fsRtParID);

	// If no error, make sure there is an ending colon
	if (!uResult) {
		m_bNativeValid = 1;
		uResult = end_with_colon();
	}
	return uResult;
}

/*! ************************************

	\brief Convert Directory to Burgerlib with FSRef

	Internal routine that converts a directory ID and a volume number into a
	Burgerlib path.

	\maconly

	\return Zero if no error, non zero if there was an error

***************************************/

#if !(defined(BURGER_CFM) && defined(BURGER_68K)) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::Filename::set_native_Carbon(
	void) BURGER_NOEXCEPT
{
	short sVolRefNum = m_sVRefNum;
	long lDirID = m_lDirID;

	// Attempt to convert to an FSRef, if it works, the OS is 9.0 or higher
	// and supports UTF8 filenames natively

	FSRef CurrentRef;

	// Make the FSSpec by hand to access the name of the folder
	FSSpec CurrentSpec;
	CurrentSpec.vRefNum = sVolRefNum;
	CurrentSpec.parID = lDirID;
	CurrentSpec.name[0] = 0;

	// If the call fails, it's because this is not MacOS 9.0 or higher.
	OSErr iError = FSpMakeFSRef(&CurrentSpec, &CurrentRef);
	eError uResult;
	if (iError == paramErr) {
		// MacOS 7.1-8.6 will generate this error
		uResult = kErrorNotSupportedOnThisPlatform;
	} else if (iError) {
		uResult = kErrorFileNotFound;
	} else {

		uResult = kErrorNone;
		do {
			// Add padding to prevent buffer overruns
			struct Padded {
				HFSUniStr255 Uni;
				uint8_t padding[8];
			} UnicodeName;

			// Get the name and the parent's File reference
			// Note: Do not use the same reference with .ref
			// and .parentRef. Some drivers will break if they
			// are the same.

			FSRefParam Block;
			InitFSRefParam(&Block, &CurrentRef, kFSCatInfoNone);
			FSRef ParentRef;
			Block.spec = &CurrentSpec;
			Block.parentRef = &ParentRef;
			Block.outName = &UnicodeName.Uni;

			// Get the UTF16 name and parent directory
			if (PBGetCatalogInfoSync(&Block)) {
				// Should not happen, but catch anyway
				uResult = kErrorIO;
				break;
			}

			// Copy the parent reference for directory traversal
			CurrentRef = ParentRef;

			// Convert the name from UTF16 to UTF8

			// Use a trick to insert a leading colon by replacing
			// the length with a colon (Which conveniently happens
			// to be a short preceding the Unicode filename)
			// This will remove an extra call to Insert() on
			// the FinalPath

			// Convert to a wide "C" string
			UnicodeName.Uni.unicode[UnicodeName.Uni.length] = 0;

			// Insert a colon
			UnicodeName.Uni.length = ':';

			// Convert to UTF8
			String NameUTF8(UnicodeName.Uni.unicode - 1);

			// Insert to the final result
			uResult = m_Filename.insert(0, NameUTF8.c_str(), NameUTF8.length());
			if (uResult) {
				break;
			}
			// If the root volume is hit, stop traversal
		} while (CurrentSpec.parID != fsRtParID);
	}

	// If no error, make sure there is an ending colon
	if (!uResult) {
		m_bNativeValid = 2;
		uResult = end_with_colon();
	}
	return uResult;
}

#endif

/*! ************************************

	\brief Create an FSSpec from a Burgerlib path.

	Given a volume and root directory, traverse a Burgerlib style pathname and
	create the values needed to create an FSSpec that best represents the
	pathname.

	If the FSSpec can be parsed all the way down to the final entry,
	m_NativeFilename will be set to an empty string. If not, it means the file
	or directory at the end of the chain does not exist and will need to be
	created or otherwise handled due to it not being present on the file system.

	m_NativeFilename always contains the name of the file, since FSSpec records
	only need to parse parent ID and volume ID information.

	The Directory ID and volume reference number will be properly set if the
	pathname is legitimate. If the pathname is to an existing directory, the
	directory ID will be of the directory itself. If the path is to a file, the
	directory id is for the directory that contains the file.

	\maconly

	\param pInput Pointer to the "C" of the pathname WITHOUT the root volume or
		drive id. Only the directories to traverse.
	\param lDirID 32-bit directory ID supplied by MacOS
	\param sVolRefNum 16-bit volume reference number

	\return Zero if no error, non zero if there was an error

***************************************/

Burger::eError BURGER_API Burger::Filename::get_native_internal(
	const char* pInput, long lDirID, short sVolRefNum) BURGER_NOEXCEPT
{
	// Clear the output
	m_NativeFilename.clear();

	// Assume no error
	eError uResult = kErrorNone;

	int iMacError;

	// Anything to traverse?
	if (pInput) {
		String TempString;

		while (pInput[0]) {
			// Delimit the string using colons
			const char* pEnd = StringCharacter(pInput, ':');

			// No colon end? Point to the terminating zero
			if (!pEnd) {
				pEnd = pInput + StringLength(pInput);
			}

			// Create a string from the directory name
			TempString.assign(pInput, static_cast<uintptr_t>(pEnd - pInput));

			// Traverse the directory
			long lNewDirID;
			uint_t bDirectory;
			uint8_t Temp[256 + 1];
			uintptr_t uLength = MacRomanUS::translate_from_UTF8(
				reinterpret_cast<char*>(Temp + 1), 256, TempString.c_str());
			Temp[0] = static_cast<uint8_t>(uLength);

			iMacError = GetDirectoryID(
				sVolRefNum, lDirID, Temp, &lNewDirID, &bDirectory);

			// Issue in the traversal?
			if (!bDirectory || (iMacError == fnfErr)) {
				// File not found is acceptable.
				m_NativeFilename.assign(pInput);
				// If there is a terminating colon, remove it.
				if (m_NativeFilename.ends_with(':')) {
					m_NativeFilename.pop_back();
				}
				break;
			}

			// Some other error occured
			if (iMacError) {
				uResult = platform_convert_to_error(iMacError);
				break;
			}

			// Since this worked, skip to the next folder
			lDirID = lNewDirID;

			// Already end of string?
			if (!pEnd[0]) {
				break;
			}
			// Accept the entry
			pInput = pEnd + 1;
		}
	}

	// If no issues parsing, assume it's okay
	if (!uResult) {
		// Mark as an FSSpec
		m_bNativeValid = 1;

		m_lDirID = lDirID;
		m_sVRefNum = sVolRefNum;
	}
	return uResult;
}

/*! ************************************

	\brief Create an FSRef from a Burgerlib path.

	Given a volume and root directory, traverse a Burgerlib style pathname and
	create the values needed to create an FSSpec that best represents the
	pathname.

	If the FSRef can be parsed all the way down to the final entry,
	m_NativeFilename will be set to an empty string. If not, it means the file
	or directory at the end of the chain does not exist and will need to be
	created or otherwise handled due to it not being present on the file system.

	The Directory ID and volume reference number will be properly set if the
	pathname is legitimate. If the pathname is to an existing directory, the
	directory ID will be of the directory itself. If the path is to a file, the
	directory id is for the directory that contains the file.

	\maconly

	\param pInput Pointer to the "C" of the pathname WITHOUT the root volume or
		drive id. Only the directories to traverse.
	\param lDirID 32-bit directory ID supplied by MacOS
	\param sVolRefNum 16-bit volume reference number

	\return Zero if no error, non zero if there was an error

***************************************/

#if !(defined(BURGER_CFM) && defined(BURGER_68K)) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::Filename::get_native_Carbon(
	const char* pInput, long lDirID, short sVolRefNum) BURGER_NOEXCEPT
{
	// Clear the output
	m_NativeFilename.clear();

	// Create an initial FSRef
	FSSpec CurrentSpec;
	CurrentSpec.vRefNum = sVolRefNum;
	CurrentSpec.parID = lDirID;
	CurrentSpec.name[0] = 0;

	int iMacError =
		FSpMakeFSRef(&CurrentSpec, reinterpret_cast<FSRef*>(m_FSRef));

	eError uResult;
	if (iMacError == paramErr) {
		// MacOS 7.1-8.6 will generate this error
		uResult = kErrorNotSupportedOnThisPlatform;

	} else if (iMacError) {
		// Other error
		uResult = platform_convert_to_error(iMacError);

	} else if (pInput) {
		String16 Name16;
		String TempString;

		uResult = kErrorNone;
		while (pInput[0]) {
			// Delimit the string using colons
			const char* pEnd = StringCharacter(pInput, ':');

			// No colon end? Point to the terminating zero
			if (!pEnd) {
				pEnd = pInput + StringLength(pInput);
			}

			// Create a string from the directory name
			TempString.assign(pInput, static_cast<uintptr_t>(pEnd - pInput));

			// Convert to UTF16
			Name16.assign(TempString.c_str());

			// Follow the FSRef chain using Unicode
			FSRef TempRef;
			iMacError = FSMakeFSRefUnicode(reinterpret_cast<FSRef*>(m_FSRef),
				Name16.length(), Name16.c_str(), kUnicode16BitFormat, &TempRef);

			// If there was an error, abort
			if (iMacError) {
				// File not found is acceptable.
				if (iMacError == fnfErr) {
					m_NativeFilename.assign(pInput);
					// If there is a terminating colon, remove it.
					if (m_NativeFilename.ends_with(':')) {
						m_NativeFilename.pop_back();
					}
					break;
				}
				uResult = platform_convert_to_error(iMacError);
				break;
			}
			// Since this worked, skip to the next
			MemoryCopy(m_FSRef, &TempRef, sizeof(TempRef));

			// Already end of string?
			if (!pEnd[0]) {
				break;
			}
			// Accept the entry and skip the colon
			pInput = pEnd + 1;
		}
	}

	// If no issues parsing, assume it's okay
	if (!uResult) {
		// Mark as an FSRef
		m_bNativeValid = 2;

		// Finishing touch, obtain the volume reference number and the directory
		// ID for the FSRef and store them in the constants
		FSRefParam Block;
		InitFSRefParam(&Block, reinterpret_cast<FSRef*>(m_FSRef),
			kFSCatInfoNodeFlags | kFSCatInfoVolume | kFSCatInfoParentDirID |
				kFSCatInfoNodeID);

		FSCatalogInfo MyInfo;
		Block.catInfo = &MyInfo;
		iMacError = PBGetCatalogInfoSync(&Block);

		// No error?
		if (!iMacError) {
			// If this is a directory, use the actual ID
			if (MyInfo.nodeFlags & kFSNodeIsDirectoryMask) {
				m_lDirID = static_cast<long>(MyInfo.nodeID);
			} else {
				// For files, set the parent directory ID
				m_lDirID = static_cast<long>(MyInfo.parentDirID);
			}
			m_sVRefNum = MyInfo.volume;
		}
	}
	return uResult;
}
#endif

#endif
