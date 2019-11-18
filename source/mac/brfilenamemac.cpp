/***************************************

	Filename Class

	MacOS version

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brfile.h"
#include "brfilemanager.h"
#include "brglobalmemorymanager.h"
#include "brstring.h"
#include "brstring16.h"
#include "brtick.h"
#include "brmemoryfunctions.h"
#include <Files.h>
#include <Folders.h>
#include <Processes.h>
#include <string.h>


Burger::Filename::ExpandCache_t
	Burger::Filename::m_DirectoryCache[Burger::Filename::DIRCACHESIZE];

/*! ************************************

	\struct Burger::Filename::ExpandCache_t
	\brief Structure to contain a directory cache entry

	For performance, a cache of the last Burger::FileManager::DIRCACHESIZE MacOS
	directories are stored with their Directory IDs and volume reference
numbers. Since these number can ba invalidate when a directory is deleted or
created, any call to a Burgerlib function that performs that action will also
purge this cache.

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

	\sa Burger::Filename::PurgeDirectoryCache(void) or
Burger::Filename::ExpandCache_t \note Only available on MacOS

***************************************/

void BURGER_API Burger::Filename::InitDirectoryCache(void)
{
	Word i = BURGER_ARRAYSIZE(m_DirectoryCache);
	ExpandCache_t* pWork = m_DirectoryCache;
	do {
		pWork->m_pName = NULL;
		++pWork;
	} while (--i);
}

/*! ************************************

	\brief Dispose of my directory cache

	This function is called by Burger::FileManager::Shutdown(void) or
	any internal function that can modify the MacOS directory structure.
	It's not meant to be called by applications.

	\sa Burger::Filename::InitDirectoryCache(void) or
Burger::Filename::ExpandCache_t \note Only available on MacOS

***************************************/

void BURGER_API Burger::Filename::PurgeDirectoryCache(void)
{
	Word i = BURGER_ARRAYSIZE(m_DirectoryCache);
	ExpandCache_t* pWork = m_DirectoryCache;
	do {
		Free(pWork->m_pName);
		pWork->m_pName = NULL;
		++pWork;
	} while (--i);
}

/***************************************

	Convert a Burgerlib path to a MacOS path

	MacOS filenames have a 256 character limit. To get around
	this, Burgerlib will traverse the path to find the Volume
	Reference number and Directory ID to the deepest entry in
	the path. This way, in most cases, only the filename itself
	is stored in the filename buffer.

	This function initializes the Volume Reference number
	and Directory ID. Assume that they are valid at the
	conclusion of this call.

	Only available on MacOS. HFS only supports filenames
	that are 31 characters or less for each component, so avoid
	using extremely long filename components.

***************************************/

const char* Burger::Filename::GetNative(void)
{
	FSSpec CurrentSpec;

	Expand(); // Resolve prefixes
	char* pPath = m_pFilename;

	// Since the MacOS is so slow in creating FSSpecs on 603 machines
	// I have to use a directory cache to see if have already determined
	// the volume ID and dir ref

	// Note, If I have a perfect match, then exit now, else
	// Set up myself for the partial match

	// Is there an ending colon? (The answer should be yes)
	char* pWork = StringCharacterReverse(pPath, ':');
	// No match found
	ExpandCache_t* pBiggestMatch = NULL;
	Word uBiggestLen = 0;

	// Assume the directory has no length
	WordPtr uDirLength = 0;
	char* pFileName = NULL;
	// If no colon, don't do a scan
	if (pWork) {

		// Temporarily end the string at the colon and find a SECOND colon in
		// the pathname
		pWork[0] = 0;
		pFileName = StringCharacterReverse(pPath, ':');
		pWork[0] = ':';
		if (pFileName == pPath) {
			pFileName = pWork;
		}

		// What's on the right of the colon is the filename
		// and what's on the left is the directory containing the file

		if (pFileName) {
			// Length of the directory including the ending colon
			uDirLength = static_cast<WordPtr>(pFileName - pPath) + 1;
			// Not a single char? ":a"?
			if (uDirLength >= 2) {
				// Scan against the cache
				Word i = DIRCACHESIZE;
				ExpandCache_t* pCache = m_DirectoryCache;
				do {
					// If there is an entry, check if this directory is a match
					if (pCache->m_pName
						&& pCache->m_uNameLength <= uDirLength) {

						// Partial match?
						if (!MemoryCaseCompare(pCache->m_pName, pPath,
								pCache->m_uNameLength)) {

							// If this is a full match, use it and exit
							// immediately
							if (pCache->m_uNameLength == uDirLength) {
								// Grab the volume reference and directory ID
								MemoryCopy(
									m_FSRef, pCache->m_FSRef, sizeof(m_FSRef));

								// Mark this as the most recently used
								pCache->m_uHitTick = Tick::Read();

								// Length of the result (The leading colon is
								// needed since it tells Mac OS 9 this is a
								// partial pathname)
								if (pFileName[0] == ':') {
									++pFileName;
								}
								WordPtr uFinalLength = StringLength(pFileName);

								// Store the file name
								char* pNew = m_NativeFilename;
								if (uFinalLength < sizeof(m_NativeFilename)) {
									MemoryCopy(
										pNew, pFileName, uFinalLength + 1);
								} else {
									pNew = StringDuplicate(pFileName);
								}
								m_pNativeFilename = pNew;

								// Kill the trailing colon
								if (uFinalLength) {
									pNew[uFinalLength - 1] = 0;
								}
								return pNew;
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

	// At this point, the directory is one that is not in the cache, and as such
	// needs to be created manually.

	// First parse either the volume name or a .DXX device number
	// I either return a fully qualified volume name or I get the proper
	// MacVRefNum of the volume number to work with

	// Is this a volume name?

	uDirLength = 0;
	if (pBiggestMatch && 0) {
		MemoryCopy(
			m_FSRef, pBiggestMatch->m_FSRef, sizeof(m_FSRef)); // Get the cache
		uDirLength = pBiggestMatch->m_uNameLength;
		pBiggestMatch->m_uHitTick = Tick::Read();
	} else {
		short sVRefNum = 0; // Init the Macintosh volume numbers to the current
							// working directory
		long lDirID = 0;
		// Assume no device found
		Word uDeviceNum = static_cast<Word>(-1);
		// No directory parsed yet

		// Find the volume by name?
		if (pPath[0] == ':') {

			// Get the volume number
			char* pEndVolume = StringCharacter(pPath + 1, ':');

			// No ending colon? Error condition, should never happen
			if (pEndVolume) {
				// Truncate the string and find the volume number
				char cTemp = pEndVolume[1];
				pEndVolume[1] = 0;
				uDeviceNum = FileManager::GetVolumeNumber(pPath);
				pEndVolume[1] = cTemp;
				if (uDeviceNum != static_cast<Word>(-1)) {
					sVRefNum = static_cast<short>(uDeviceNum);
					lDirID = fsRtDirID;
					uDirLength = static_cast<WordPtr>(pEndVolume - pPath) + 1;
				}
			}

			// Find the volume by number?
		} else if (pPath[0] == '.') {
			// ".dxx:"?
			Word uTemp = pPath[1];
			if (uTemp >= 'a' && uTemp < ('z' + 1)) {
				uTemp &= 0xDF;
			}
			// Is it a 'D'?
			if (uTemp == 'D') {
				// Index past the ".D"
				WordPtr uLength = 2;
				// Init drive number
				uDeviceNum = 0;
				for (;;) {
					// Get an ASCII char
					uTemp = pPath[uLength];
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
		CurrentSpec.parID = lDirID;
		CurrentSpec.vRefNum = sVRefNum;
		CurrentSpec.name[0] = 0;
		FSpMakeFSRef(&CurrentSpec, reinterpret_cast<FSRef*>(m_FSRef));
	}

	//
	// At this point, the volume is set up
	// Follow the path as far as possible and
	// exclude the last entry
	//

	// Start here
	char* pWorkPath = pPath + uDirLength;
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
			// Terminate the fragement
			pEnd[0] = 0;
			OSErr eError;
			FSRef TempRef;
			{
				// Convert to UTF16
				String16 Name16(pWorkPath);
				// Restore the original fragment
				pEnd[0] = ':';
				// Follow the FSRef chain
				eError = FSMakeFSRefUnicode(reinterpret_cast<FSRef*>(m_FSRef),
					Name16.GetLength(), Name16.GetPtr(), kUnicode16BitFormat,
					&TempRef);
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

	uDirLength = static_cast<WordPtr>(pWorkPath - pPath);
	if (uDirLength >= 3) {
		Word32 uMark = Tick::Read();
		Word i = DIRCACHESIZE;
		ExpandCache_t* pCache = m_DirectoryCache;
		ExpandCache_t* pEntry = pCache;
		Word32 uTime = 0;
		do {
			if (!pCache->m_pName) { // Empty?
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
		pEntry->m_uHitTick = uMark;
		// Directories must always end in a colon
		pEntry->m_uNameLength = uDirLength; // Save the strlen size
		Free(pEntry->m_pName);				// Copy the string for the cache
		char uTemp = pPath[uDirLength];
		pPath[uDirLength] = 0; // Make sure it's zero terminated
		pEntry->m_pName = StringDuplicate(pPath); // Store the new entry
		pPath[uDirLength] = uTemp;
	}

	WordPtr uNameLength = StringLength(pWorkPath);
	if (uNameLength < sizeof(m_NativeFilename)) {
		MemoryCopy(m_NativeFilename, pWorkPath, uNameLength + 1);
	} else {
		m_pNativeFilename = StringDuplicate(pWorkPath);
	}
	// Get rid of the trailing colon
	if (uNameLength) {
		m_pNativeFilename[uNameLength - 1] = 0;
	}
	return m_pNativeFilename;
}

/***************************************

	\brief Set the filename to the current working directory

	Query the operating system for the current working directory and
	set the filename to that directory. The path is converted
	into UTF8 character encoding and stored in Burgerlib
	filename format

	On platforms where a current working directory doesn't make sense,
	like an ROM based system, the filename is cleared out.

***************************************/

void BURGER_API Burger::Filename::SetSystemWorkingDirectory(void)
{
	Clear();
	long lDirID;
	short sVRefNum;
	HGetVol(0, &sVRefNum, &lDirID); // Call OS
	// Set lDirID to 0 to hack to simulate GetVol()
	SetFromDirectoryID(0, sVRefNum); // Get the directory
}

/***************************************

	\brief Set the filename to the application's directory

	Determine the directory where the application resides and set
	the filename to that directory. The path is converted
	into UTF8 character encoding and stored in Burgerlib
	filename format.

	On platforms where a current working directory doesn't make sense,
	like an ROM based system, the filename is cleared out.

***************************************/

void BURGER_API Burger::Filename::SetApplicationDirectory(void)
{
	Clear();
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
	// MyProcess.processName = 0;			// I don't want the name
	MyProcess.processAppSpec = &MySpec; // Get the FSSpec

	// Locate the application
	if (!GetProcessInformation(&MyNumber, &MyProcess)) {
		SetFromDirectoryID(
			MyProcess.processAppSpec->parID, MyProcess.processAppSpec->vRefNum);
	}
}

/***************************************

	\brief Set the filename to the local machine preferences directory

	Determine the directory where the user's preferences that are
	local to the machine is located. The path is converted
	into UTF8 character encoding and stored in Burgerlib
	filename format.

	On platforms where a current working directory doesn't make sense,
	like an ROM based system, the filename is cleared out.

***************************************/

void BURGER_API Burger::Filename::SetMachinePrefsDirectory(void)
{
	Clear();
	short MyVRef; // Internal volume references
	long MyDirID; // Internal drive ID
	// Get the system folder
	if (!FindFolder(kOnSystemDisk, kSystemPreferencesFolderType,
			kDontCreateFolder, &MyVRef, &MyDirID)) {
		SetFromDirectoryID(MyDirID, MyVRef);
	}
}

/***************************************

	\brief Set the filename to the user's preferences directory

	Determine the directory where the user's preferences that
	could be shared among all machines the user has an account
	with is located. The path is converted
	into UTF8 character encoding and stored in Burgerlib
	filename format.

	On platforms where a current working directory doesn't make sense,
	like an ROM based system, the filename is cleared out.

***************************************/

void BURGER_API Burger::Filename::SetUserPrefsDirectory(void)
{
	Clear();
	short MyVRef; // Internal volume references
	long MyDirID; // Internal drive ID
	// Where are the user preferences stored?
	if (!FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder,
			&MyVRef, &MyDirID)) {
		SetFromDirectoryID(MyDirID, MyVRef);
	}
}

/*! ************************************

	\brief Convert a MacOS path to a Burgerlib path

	Given a "C" string to the pathname, a Directory ID
	and a Volume Reference number, create a full pathname
	in Burgerlib format.

	This function is commonly used when creating a file selection dialog
	and the input needs to converted into a format that is compatible
	with most Burgerlib file functions.

	\param pInput Pointer to a "C" string of a MacOS formatted filename
	\param lDirID Directory ID for the filename
	\param sVRefNum Volume ID for the filename
	\note Only available on MacOS

***************************************/

void Burger::Filename::SetFromNative(
	const char* pInput, long lDirID, short sVRefNum)
{
	Clear(); // Clear out the previous string

	Word Temp;			 // Ascii Temp
	char TempPath[8192]; // Handle to temp buffer
	char* Output;		 // Running pointer to temp buffer
	Word Length;		 // Length of finished string

	Output = TempPath; // Get running pointer

	if (!pInput[0]
		|| pInput[0] == ':') {		// Must I prefix with the current directory?
		if (!sVRefNum && !lDirID) { // If both are zero then look up default
			HGetVol(0, &sVRefNum, &lDirID); // Call OS
			lDirID = 0;						// Hack to simulate GetVol()
		}
		Filename MyFilename;
		Word uResult = MyFilename.SetFromDirectoryID(
			lDirID, sVRefNum);						 /* Get the directory */
		if (!uResult) {								 /* Did I get a path? */
			StringCopy(Output, MyFilename.GetPtr()); /* Copy to output */
			Output = Output + StringLength(Output);  /* Fix pointer */
		}
		if (pInput[0]) { /* Was there a leading colon? */
			++pInput;	/* Accept the leading colon */
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
			++pInput;		  /* Accept char */
			Temp = pInput[0]; /* Next char */
		} while (Temp);		  /* Still more? */
	}

	/* The wrap up... */
	/* Make sure it's appended with a colon */

	Length = static_cast<Word32>(
		Output - TempPath);				   /* How many bytes is the new path? */
	if (Length) {						   /* Valid length? */
		if (TempPath[Length - 1] != ':') { /* Last char a colon? */
			TempPath[Length] = ':';		   /* End with a colon! */
			++Length;					   /* Increase length! */
		}
	}
	TempPath[Length] = 0;
	Set(TempPath);
}

/*! ************************************

	\fn Burger::Filename::GetFSRef(void)
	\brief Return the FSRef stored in the class.
	\return A pointer to the FSRef for this filename
	\sa Burger::Filename::SetDirID(long) or Burger::Filename::GetVRefNum(void)
		const \note Only available on MacOS and only valid after a call to \ref
		Burger::Filename::GetNative()

***************************************/

/*! ************************************

	\fn Burger::Filename::GetDirID(void) const
	\brief Return the Directory ID stored in the class.
	\return The Directory ID for this filename
	\sa Burger::Filename::SetDirID(long) or Burger::Filename::GetVRefNum(void)
		const \note Only available on MacOS and only valid after a call to \ref
		Burger::Filename::GetNative()

***************************************/

/*! ************************************

	\fn Burger::Filename::GetVRefNum(void) const
	\brief Return the Volume Reference number stored in the class.
	\return The Volume Reference number for this filename.
	\sa Burger::Filename::SetVRefNum(short) or Burger::Filename::GetDirID(void)
		const \note Only available on MacOS and only valid after a call to \ref
		Burger::Filename::GetNative()

***************************************/

/*! ************************************

	\fn Burger::Filename::SetDirID(long)
	\brief Set the Directory ID in the class.
	\sa Burger::Filename::GetDirID(void) const or
		Burger::Filename::SetVRefNum(short) \note Only available on MacOS

***************************************/

/*! ************************************

	\fn Burger::Filename::SetVRefNum(short)
	\brief Set Volume Reference number in the class.
	\sa Burger::Filename::GetVRefNum(void) const or
		Burger::Filename::SetDirID(long) \note Only available on MacOS

***************************************/

/*! ************************************

	\brief Convert a Mac DirID and a Volume reference into a Burgerlib path.

	Given a 32 bit directory ID and a 16 bit volume number, determine the full
	Burgerlib pathname that is its equivalent.

	What is returned is either \ref NULL for an error condition, pOutput if the
	user supplied buffer was large enough to contain the requested data or a newly
	allocated pointer to a larger buffer to hold the requested data.

	\note If the returned pointer is not the pointer passed in via pOutput,
		dispose of it with a called to \ref Burger::Free(const void *) when done.

	\param lDirID 32-bit directory ID supplied by MacOS
	\param sVolRefNum 16-bit volume reference number supplied by MacOS
	\return Zero if no error, non zero if there was an error
	\note Only available on MacOS

***************************************/

Word BURGER_API Burger::Filename::SetFromDirectoryID(
	long lDirID, short sVolRefNum)
{
	/*
		Using a Macintosh directory ID and a volume referance number,
		return the full path that the ID generates.
		It uses the current values to get the current
		directory name, then by traversing the directories PARENT entry,
		follow the tree BACKWARDS back to the root. So it must constantly be
		prefixing my current data with the newly located PARENT entry until
		it's gotten to the root entry.

		The Mac is brain dead.
	*/

	Clear();
	// FIrst step, create an FS Ref from the parent ID and volume reference
	// number
	FSRef CurrentRef;
	FSSpec CurrentSpec;
	CurrentSpec.parID = lDirID;
	CurrentSpec.vRefNum = sVolRefNum;
	CurrentSpec.name[0] = 0;

	// Assume failure
	Word uResult = File::FILENOTFOUND;
	if (!FSpMakeFSRef(&CurrentSpec, &CurrentRef)) {
		// Initialize the proposed final string
		uResult = File::OKAY;
		String FinalPath(":");
		do {
			// Add padding to prevent buffer overruns
			struct Padded {
				HFSUniStr255 Uni;
				Word8 padding[4];
			} UnicodeName;

			// Get the name and the parent's File reference
			// Note: Do not use the same reference with .ref
			// and .parentRef. Some drivers will break if they
			// are the same.
			FSRefParam Param;
			FSRef ParentRef;
			Param.ref = &CurrentRef;
			Param.whichInfo = kFSCatInfoNone;
			Param.catInfo = NULL;
			Param.spec = &CurrentSpec;
			Param.parentRef = &ParentRef;
			Param.outName = &UnicodeName.Uni;
			if (PBGetCatalogInfoSync(&Param)) {
				uResult = File::IOERROR;
				break;
			}
			
			// Copy the parent
			CurrentRef = ParentRef;

			// Convert the name from UTF16 to UTF8
			// Use a trick to insert a leading colon by replacing
			// the length with a colon (Which conviently happens
			// to be a short preceeding the Unicode filename)
			// This will remove an extra call to Insert() on
			// the FinalPath

			// Convert to a wide "C" string
			UnicodeName.Uni.unicode[UnicodeName.Uni.length] = 0;
			// Insert a colon
			UnicodeName.Uni.length = ':';
			// Convert to UTF8
			String NameUTF8(UnicodeName.Uni.unicode - 1);
			// Insert to the final result
			FinalPath.Insert(0, NameUTF8.GetPtr(), NameUTF8.GetLength());
		} while (CurrentSpec.parID != fsRtParID);
		// All good?
		if (uResult == File::OKAY) {
			Set(FinalPath.GetPtr());
		}
	}
	return uResult;
}

#endif
