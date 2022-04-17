/***************************************

	Filename Class

	MacOS version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

	\sa Burger::FileManager and Burger::Filename::InitDirectoryCache(void)
	\note Only available on MacOS

***************************************/

/*! ************************************

	\brief Initialize the directory cache.

	This function is called by Burger::FileManager::Init(void).
	It's not meant to be called by applications.

	\note Only available on MacOS

	\sa Burger::Filename::PurgeDirectoryCache(void) or
		Burger::Filename::ExpandCache_t

***************************************/

void BURGER_API Burger::Filename::InitDirectoryCache(void)
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

	This function is called by Burger::FileManager::Shutdown(void) or any
	internal function that can modify the MacOS directory structure. It's not
	meant to be called by applications.

	\note Only available on MacOS

	\sa Burger::Filename::InitDirectoryCache(void) or
		Burger::Filename::ExpandCache_t

***************************************/

void BURGER_API Burger::Filename::PurgeDirectoryCache(void)
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

const char* Burger::Filename::GetNative(void) BURGER_NOEXCEPT
{
	// Resolve prefixes
	Expand();

	// If already parsed, skip the conversion.
	if (!m_bNativeValid) {

		// Mac filenames are always short, so no need to reserve space
		m_NativeFilename.clear();

		const char* pFilename = m_Filename.c_str();

		// Init the Macintosh volume numbers to the current working directory
		short sVRefNum = 0;
		long lDirID = 0;

		// Assume no device found
		uint_t uDeviceNum = BURGER_MAXUINT;

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
				uDeviceNum = FileManager::GetVolumeNumber(pFilename);
				pEndVolume[1] = cTemp;

				// Was a volume found?
				if (uDeviceNum != BURGER_MAXUINT) {

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

		if (uDeviceNum != BURGER_MAXUINT) {
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
		uError = GetNativeClassic(pFilename + uDirLength, lDirID, sVRefNum);
#else
		// All others support both new and old styles
		uError = GetNativeCarbon(pFilename + uDirLength, lDirID, sVRefNum);
		if (uError == kErrorNotSupportedOnThisPlatform) {
			uError = GetNativeClassic(pFilename + uDirLength, lDirID, sVRefNum);
		}
#endif

#if 0
		// Since the MacOS is so slow in creating FSSpecs on 603 machines
		// I have to use a directory cache to see if have already determined
		// the volume ID and dir ref

		// Note, If I have a perfect match, then exit now, else
		// Set up myself for the partial match

		// Is there an ending colon? (The answer should be yes)
		char* pWork = StringCharacterReverse(pFullPathName, ':');

		// No match found
		ExpandCache_t* pBiggestMatch = nullptr;
		uint_t uBiggestLen = 0;

		// Assume the directory has no length
		uintptr_t uDirLength = 0;
		char* pFileName = nullptr;
		// If no colon, don't do a scan
		if (pWork) {

			// Temporarily end the string at the colon and find a SECOND colon
			// in the pathname
			pWork[0] = 0;
			pFileName = StringCharacterReverse(pFullPathName, ':');
			pWork[0] = ':';
			if (pFileName == pFullPathName) {
				pFileName = pWork;
			}

			// What's on the right of the colon is the filename
			// and what's on the left is the directory containing the file

			if (pFileName) {

				// Length of the directory including the ending colon
				uDirLength =
					static_cast<uintptr_t>(pFileName - pFullPathName) + 1;

				// Not a single char? ":a"?
				if (uDirLength >= 2) {

					// Scan against the cache
					uint_t i = kDirectoryCacheSize;
					ExpandCache_t* pCache = m_DirectoryCache;
					do {
						// If there is an entry, check if this directory is a
						// match
						if (pCache->m_pName &&
							pCache->m_uNameLength <= uDirLength) {

							// Partial match?
							if (!MemoryCaseCompare(pCache->m_pName,
									pFullPathName, pCache->m_uNameLength)) {

								// If this is a full match, use it and exit
								// immediately
								if (pCache->m_uNameLength == uDirLength) {
									// Grab the FSRef and other values from the
									// cache
									MemoryCopy(m_FSRef, pCache->m_FSRef,
										sizeof(m_FSRef));
									m_lDirID = pCache->m_lDirID;
									m_sVRefNum = pCache->m_sVRefNum;

									// Mark this as the most recently used
									pCache->m_uHitTick = Tick::Read();

									// Length of the result (The leading colon
									// is needed since it tells Mac OS 9 this is
									// a partial pathname)
									if (pFileName[0] == ':') {
										++pFileName;
									}
									uintptr_t uFinalLength =
										StringLength(pFileName);

									// Store the file name
									m_NativeFilename.assign(pFileName);

									// Kill the trailing colon
									if (uFinalLength) {
										m_NativeFilename.pop_back();
									}
									m_bNativeValid = TRUE;
									return m_NativeFilename.c_str();
								}

								// On partial hits, store the closest hit
								if (pCache->m_uNameLength >= uBiggestLen) {
									pBiggestMatch = pCache;
									uBiggestLen = pCache->m_uNameLength;
								}
							}
						}
						++pCache;
					} while (--i);
				}
			}
		}

		// At this point, the directory is one that is not in the cache, and as
		// such needs to be created manually.

		// First parse either the volume name or a .DXX device number
		// I either return a fully qualified volume name or I get the proper
		// MacVRefNum of the volume number to work with

		// Is this a volume name?

		uDirLength = 0;
		// Init the Macintosh volume numbers to the current working directory
		short sVRefNum = 0;
		long lDirID = 0;

		// Broken for now.


			// Assume no device found
			uint_t uDeviceNum = BURGER_MAXUINT;
			// No directory parsed yet

			// Find the volume by name?
			if (pFullPathName[0] == ':') {

				// Get the volume number
				char* pEndVolume = StringCharacter(pFullPathName + 1, ':');

				// No ending colon? Error condition, should never happen
				if (pEndVolume) {
					// Truncate the string and find the volume number
					char cTemp = pEndVolume[1];
					pEndVolume[1] = 0;
					uDeviceNum = FileManager::GetVolumeNumber(pFullPathName);
					pEndVolume[1] = cTemp;
					if (uDeviceNum != BURGER_MAXUINT) {
						sVRefNum = static_cast<short>(uDeviceNum);
						lDirID = fsRtDirID;
						uDirLength =
							static_cast<uintptr_t>(pEndVolume - pFullPathName) +
							1;
					}
				}

				// Find the volume by number?
			} else if (pFullPathName[0] == '.') {
				// ".dxx:"?
				uint_t uTemp = pFullPathName[1];
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
						uTemp = pFullPathName[uLength];
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
			FSSpec CurrentSpec;
			CurrentSpec.parID = lDirID;
			CurrentSpec.vRefNum = sVRefNum;
			CurrentSpec.name[0] = 0;
			FSpMakeFSRef(&CurrentSpec, reinterpret_cast<FSRef*>(m_FSRef));
		

		//
		// At this point, the volume is set up
		// Follow the path as far as possible and
		// exclude the last entry
		//

		// Start here
		const char* pWorkPath = pFullPathName + uDirLength;
		if (pFileName) {
			while (pWorkPath[0]) {
				// Find the colon at the end of the string
				char* pEnd = StringCharacter(pWorkPath, ':');
				// No colon end? Assume filename
				if (!pEnd) {
					break;
				}
				// Nothing beyond the end colon? Assume filename
				if (!pEnd[1]) {
					break;
				}
				// Terminate the fragment
				pEnd[0] = 0;
				OSErr eError;
				FSRef TempRef;
				{
					// Convert to UTF16
					String16 Name16(pWorkPath);
					// Restore the original fragment
					pEnd[0] = ':';
					// Follow the FSRef chain
					eError = FSMakeFSRefUnicode(
						reinterpret_cast<FSRef*>(m_FSRef), Name16.length(),
						Name16.c_str(), kUnicode16BitFormat, &TempRef);
				}
				// If there was an error, abort
				if (eError) {
					break;
				}
				// Since this worked, skip to the next
				MemoryCopy(m_FSRef, &TempRef, sizeof(TempRef));
				// Accept the entry
				pWorkPath = pEnd + 1;
			}
		}

		//
		// Now that I have a new entry, add it into the cache
		// I may need to purge the oldest entry.
		//

		uDirLength = static_cast<uintptr_t>(pWorkPath - pFullPathName);
		if (uDirLength >= 3) {
			uint32_t uMark = Tick::Read();
			uint_t i = kDirectoryCacheSize;
			ExpandCache_t* pCache = m_DirectoryCache;
			ExpandCache_t* pEntry = pCache;
			uint32_t uTime = 0;
			do {
				// Empty?
				if (!pCache->m_pName) {
					pEntry = pCache;
					break;
				}
				if ((uMark - pCache->m_uHitTick) > uTime) {
					pEntry = pCache;
					uTime = uMark - pCache->m_uHitTick;
				}
				++pCache;
			} while (--i);

			// pEntry is the new entry
			MemoryCopy(pEntry->m_FSRef, m_FSRef, sizeof(m_FSRef));
			pEntry->m_lDirID = lDirID;
			pEntry->m_sVRefNum = sVRefNum;
			pEntry->m_uHitTick = uMark;

			// Directories must always end in a colon
			pEntry->m_uNameLength = uDirLength; // Save the strlen size
			Free(pEntry->m_pName);              // Copy the string for the cache

			// Store the new entry
			char* pNewName =
				static_cast<char*>(AllocCopy(pFullPathName, uDirLength + 1));
			pNewName[uDirLength] = 0;
			pEntry->m_pName = pNewName;
		}

		uintptr_t uNameLength = StringLength(pWorkPath);
		m_NativeFilename.assign(pWorkPath);

		// Get rid of the trailing colon
		if (uNameLength) {
			m_NativeFilename.pop_back();
		}
#endif
	}
	return m_NativeFilename.c_str();
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
	clear();
	long lDirID;
	short sVRefNum;

	// Call OS
	HGetVol(0, &sVRefNum, &lDirID);
	// Set lDirID to 0 to hack to simulate GetVol()

	// Get the directory
	SetFromDirectoryID(0, sVRefNum);
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
		uResult = SetFromDirectoryID(
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

Burger::eError BURGER_API Burger::Filename::SetMachinePrefsDirectory(
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
		uResult = SetFromDirectoryID(MyDirID, MyVRef);
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

Burger::eError BURGER_API Burger::Filename::SetUserPrefsDirectory(
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
		uResult = SetFromDirectoryID(MyDirID, MyVRef);
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

Burger::eError BURGER_API Burger::Filename::SetFromNative(
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
		eError uResult = MyFilename.SetFromDirectoryID(
			lDirID, sVRefNum);                      /* Get the directory */
		if (!uResult) {                             /* Did I get a path? */
			StringCopy(Output, MyFilename.c_str()); /* Copy to output */
			Output = Output + StringLength(Output); /* Fix pointer */
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

	\sa GetFSRef()

***************************************/

Burger::eError BURGER_API Burger::Filename::GetFSSpec(
	FSSpec* pFSSpec) BURGER_NOEXCEPT
{
	// Clear it out
	MemoryClear(pFSSpec, sizeof(FSSpec));

	// Make sure the path has been processed
	GetNative();

	// Was the filename unparsable?
	eError uResult = kErrorNotInitialized;
	if (m_bNativeValid) {

		// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// So, it's parsed as a FSRef, convert back to a FSSpec
		if (m_bNativeValid == 2) {
			FSRef MyRef;
			uResult = kErrorFileNotFound;
			if (!GetFinalFSRef(&MyRef)) {

				// Do the conversion
				FSRefParam Block;
				InitFSRefParam(&Block, &MyRef, kFSCatInfoNone);
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
			MacRomanUS::TranslateFromUTF8(
				NameBuffer, sizeof(NameBuffer), GetNative());

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

	\sa GetFSSpec(FSSpec*) or GetFinalFSRef(FSRef *)

***************************************/

FSRef* BURGER_API BURGER_API Burger::Filename::GetFSRef(void) BURGER_NOEXCEPT
{
	GetNative();
	if (m_bNativeValid == 2) {
		return reinterpret_cast<FSRef*>(m_FSRef);
	}
	return nullptr;
}

/*! ************************************

	\brief Create an FSRef including the final object name.

	GetFSRef() returns the reference to the parent directory. This function will
	append the filename to the FSRef so it directly points to the object
	requested.

	\note Available on MacOS 7.1 and higher, except 68K CFM

	\param pFSRef Pointer to an uninitialized FSRef to receive the final FSRef.

	\return Zero if no error.

	\sa GetFSRef(void)

***************************************/

Burger::eError BURGER_API Burger::Filename::GetFinalFSRef(
	FSRef* pFSRef) BURGER_NOEXCEPT
{
	// 68K CFM doesn't support FSRef
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Found it
	FSRef* pInternalFSRef = GetFSRef();
	eError uResult = kErrorFileNotFound;
	if (pInternalFSRef) {

		// Convert the filename to unicode
		String16 TempName(GetNative());

		// Create a UFT16 FSRef
		OSErr iError = FSMakeFSRefUnicode(pInternalFSRef, TempName.length(),
			TempName.c_str(), kUnicode16BitFormat, pFSRef);
		if (!iError) {
			// No error
			uResult = kErrorNone;
		}
	}
	return uResult;
#else
	BURGER_UNUSED(pFSRef);
	// Only generate FSRef on platforms that support it
	return kErrorNotSupportedOnThisPlatform;
#endif
}

/*! ************************************

	\fn Burger::Filename::GetFSRefOld(void)
	\brief Return the FSRef stored in the class.
	\return A pointer to the FSRef for this filename
	\sa Burger::Filename::SetDirID(long) or Burger::Filename::GetVRefNum(void)
		const

	\note Only available on MacOS and only valid after a call to \ref
		Burger::Filename::GetNative()

***************************************/

/*! ************************************

	\fn Burger::Filename::GetDirID(void) const
	\brief Return the Directory ID stored in the class.
	\return The Directory ID for this filename
	\sa Burger::Filename::SetDirID(long) or Burger::Filename::GetVRefNum(void)
		const

	\note Only available on MacOS and only valid after a call to \ref
		Burger::Filename::GetNative()

***************************************/

/*! ************************************

	\fn Burger::Filename::GetVRefNum(void) const
	\brief Return the Volume Reference number stored in the class.
	\return The Volume Reference number for this filename.
	\sa Burger::Filename::SetVRefNum(short) or Burger::Filename::GetDirID(void)
		const

	\note Only available on MacOS and only valid after a call to \ref
		Burger::Filename::GetNative()

***************************************/

/*! ************************************

	\fn Burger::Filename::SetDirID(long)
	\brief Set the Directory ID in the class.
	\sa Burger::Filename::GetDirID(void) const or
		Burger::Filename::SetVRefNum(short)

	\note Only available on MacOS

***************************************/

/*! ************************************

	\fn Burger::Filename::SetVRefNum(short)
	\brief Set Volume Reference number in the class.
	\sa Burger::Filename::GetVRefNum(void) const or
		Burger::Filename::SetDirID(long)

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

Burger::eError BURGER_API Burger::Filename::SetFromDirectoryID(
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
	uResult = SetFromDirectoryIDClassic();
#else
	// Carbon and Classic
	uResult = SetFromDirectoryIDCarbon();

	// Not 9.0 or higher.
	if (uResult == kErrorNotSupportedOnThisPlatform) {
		// Do it the old way
		uResult = SetFromDirectoryIDClassic();
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

Burger::eError BURGER_API Burger::Filename::SetFromDirectoryIDClassic(
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
Burger::eError BURGER_API Burger::Filename::SetFromDirectoryIDCarbon(
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

	\maconly

	\param pInput Pointer to the "C" of the pathname WITHOUT the root volume or
		drive id. Only the directories to traverse.
	\param lDirID 32-bit directory ID supplied by MacOS
	\param sVolRefNum 16-bit volume reference number

	\return Zero if no error, non zero if there was an error

***************************************/

Burger::eError BURGER_API Burger::Filename::GetNativeClassic(
	const char* pInput, long lDirID, short sVolRefNum) BURGER_NOEXCEPT
{
	// Clear the output
	m_NativeFilename.clear();
	eError uResult = kErrorNone;

	// Anything to traverse?
	if (pInput) {
		String TempString;
		while (pInput[0]) {
			// Find the colon at the end of the string
			const char* pEnd = StringCharacter(pInput, ':');

			// No colon end? Assume it's a filename
			if (!pEnd) {
				m_NativeFilename.assign(pInput);
				break;
			}

			// Nothing beyond the end colon? Assume filename
			if (!pEnd[1]) {
				// Get the filename without the ending colon
				m_NativeFilename.assign(
					pInput, static_cast<uintptr_t>(pEnd - pInput));
				break;
			}

			// Create a string from the directory name
			TempString.assign(pInput, static_cast<uintptr_t>(pEnd - pInput));

			// Traverse the directory
			long lNewDirID;
			uint_t bDirectory;
			int iResult = GetDirectoryID(sVolRefNum, lDirID, TempString.c_str(),
				&lNewDirID, &bDirectory);

			// Issue in the traversal?
			if (iResult) {
				// File not found is acceptable.
				if (iResult == fnfErr) {
					m_NativeFilename.assign(pInput);
					break;
				}
				uResult = kErrorIO;
				break;
			}

			// It wasn't a directory
			if (!bDirectory) {
				uResult = kErrorNotADirectory;
				break;
			}

			// Since this worked, skip to the next folder
			lDirID = lNewDirID;

			// Accept the entry
			pInput = pEnd + 1;
		}
	}

	// If no issues parsing, assume it's okay
	if (!uResult) {
		m_lDirID = lDirID;
		m_sVRefNum = sVolRefNum;

		// Mark as an FSSpec
		m_bNativeValid = 1;
	}
	return uResult;
}

/*! ************************************

	\brief Create an FSRef from a Burgerlib path.

	Given a volume and root directory, traverse a Burgerlib style pathname and
	create the values needed to create an FSSpec that best represents the
	pathname.

	\maconly

	\param pInput Pointer to the "C" of the pathname WITHOUT the root volume or
		drive id. Only the directories to traverse.
	\param lDirID 32-bit directory ID supplied by MacOS
	\param sVolRefNum 16-bit volume reference number

	\return Zero if no error, non zero if there was an error

***************************************/

#if !(defined(BURGER_CFM) && defined(BURGER_68K)) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::Filename::GetNativeCarbon(
	const char* pInput, long lDirID, short sVolRefNum) BURGER_NOEXCEPT
{
	// Clear the output
	m_NativeFilename.clear();
	eError uResult = kErrorNone;

	// Create an initial FSRef
	FSSpec CurrentSpec;
	CurrentSpec.vRefNum = sVolRefNum;
	CurrentSpec.parID = lDirID;
	CurrentSpec.name[0] = 0;

	OSErr iError =
		FSpMakeFSRef(&CurrentSpec, reinterpret_cast<FSRef*>(m_FSRef));

	if (iError == paramErr) {
		// MacOS 7.1-8.6 will generate this error
		uResult = kErrorNotSupportedOnThisPlatform;

	} else if (iError) {
		uResult = kErrorNotADirectory;
	} else if (pInput) {
		String16 Name16;
		String TempString;

		while (pInput[0]) {
			// Find the colon at the end of the string
			const char* pEnd = StringCharacter(pInput, ':');

			// No colon end? Assume filename
			if (!pEnd) {
				m_NativeFilename.assign(pInput);
				break;
			}
			// Nothing beyond the end colon? Assume filename
			if (!pEnd[1]) {
				// Get the filename without the ending colon
				m_NativeFilename.assign(
					pInput, static_cast<uintptr_t>(pEnd - pInput));
				break;
			}

			// Create a string from the directory name
			TempString.assign(pInput, static_cast<uintptr_t>(pEnd - pInput));

			// Convert to UTF16
			Name16.Set(TempString.c_str());

			// Follow the FSRef chain using Unicode
			FSRef TempRef;
			iError = FSMakeFSRefUnicode(reinterpret_cast<FSRef*>(m_FSRef),
				Name16.length(), Name16.c_str(), kUnicode16BitFormat, &TempRef);

			// If there was an error, abort
			if (iError) {
				// File not found is acceptable.
				if (iError == fnfErr) {
					m_NativeFilename.assign(pInput);
					break;
				}
				uResult = kErrorIO;
				break;
			}
			// Since this worked, skip to the next
			MemoryCopy(m_FSRef, &TempRef, sizeof(TempRef));
			// Accept the entry
			pInput = pEnd + 1;
		}
	}

	// If no issues parsing, assume it's okay
	if (!uResult) {
		// Mark as an FSRef
		m_bNativeValid = 2;
	}
	return uResult;
}
#endif

#endif
