/***************************************

	File Manager Class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"
#include "brfile.h"
#include "brfileansihelpers.h"
#include "brdebug.h"
#include "brglobals.h"
#include <stdio.h>

#if defined(BURGER_XBOX360)
#define NOD3D
#define NONET
#include <xtl.h>
// C'mon, they defined these?!?!?
#undef DeleteFile
#undef CopyFile
#endif

/*! ************************************

	\struct Burger::FileManager
	\brief Global file system manager
	
	Since file systems are all tied to a single device
	such as a hard drive or DVD Rom, all file calls 
	are routed through a singleton class to enable
	Burgerlib to prioritize file streaming to
	reduce seeks and read/write operations
	for peak performance.

	To ensure cross platform compatibility, all file/pathnames
	use a generic universal format that BurgerLib will accept
	and the library will perform any translations needed to
	the exact same behavior is consistant across operating systems.
	
	The strings are "C" format, 8 bit ascii with a zero termination byte. They
	are encoded using strict UTF8 format. Colons are used at filename
	delimiters.
	
	The concept of a BurgerLib path is simple. The first part is either
	a drive specifier, a volume name or a directory prefix. Drive specifiers
	act much like "C:" does on Windows machines. Volume names allow you to 
	ask for a CD or DVD by name and query if it is mounted irregardless of
	which drive it physically resides. Prefixes act like current working
	directories except that there are over 30 of them active at the same
	time and with a numeric or special character code you can quickly select
	the desired one at runtime.
	
	Some prefixes are preset after a call to Burger::FileManager::DefaultPrefixes().
	
	Default prefixes:<br>
	"8:" = Current working directory at application launch<br>
	"9:" = Directory where the application resides<br>
	"*:" = Boot drive volume<br>
	"@:" = Directory for the user's data. OS specific as to the location (Usually the Users' folder)<br>
	"$:" = System folder (System on MacOS or Windows on Windows)<br>
	"14:" = User prefix #14, the number can be 0 through 31.
	
	Example pathnames:<br>
	"9:GameData.dat" = Data file sitting next to the application<br>
	"@:Contraband Entertainment:CoolGame:MyPrefs.bin" = Preferences file<br>
	"*:Root.txt" = File at the root of the boot drive<br>
	"20:LevelData.bin" = Application defined prefix and file within the expected directory structure
		
	\sa Burger::UTF8
	
***************************************/

Burger::FileManager Burger::FileManager::g_FileManager;

/*! ************************************

	\brief Initialize the global file system manager
	
	Call this function once on startup to start up
	the burgerlib file manager. This function's primary
	purpose is to initialize the default prefixes.
	
	\sa Burger::FileManager::DefaultPrefixes(void) or Burger::FileManager::Shutdown(void)

***************************************/

void BURGER_API Burger::FileManager::Init(void)
{
	// Init the directory cache (MacOS)
#if defined(BURGER_MAC)
	Filename::InitDirectoryCache();
#endif
#if defined(BURGER_XBOX360)
#if defined(NDEBUG)
	// Init the file cache to something small
	XSetFileCacheSize(128 * 1024);
#else
	// Since runtime files can be added, and dev kits have more memory,
	// bump up the file cache size a bit.
	XSetFileCacheSize(1024 * 1024);
#endif
#endif
	// Load the default prefixes
	DefaultPrefixes();
}

/*! ************************************

	\brief Shut down the global file system manager
	
	Call this function once on shutdown to 
	the burgerlib file manager. This function's primary
	purpose is to release all allocated memory and send
	out a report in case any files are still open on debug
	builds. The application is responsible for shutting down
	file usage before application shutdown.

	\sa Burger::FileManager::Init(void)

***************************************/

void BURGER_API Burger::FileManager::Shutdown(void)
{
	// Release all of my prefixes
	g_FileManager.ReleasePrefixes();
	// Release any directories cached (MacOS)
#if defined(BURGER_MAC)
	Filename::PurgeDirectoryCache();
#endif
}

/*! ************************************

	\fn Burger::FileManager::AreLongFilenamesAllowed(void)
	\brief Returns \ref TRUE if long filenames are allowed.
	
	On some MSDOS file systems, only 8.3 filenames are permitted.
	Test if support for longer than 8.3 filenames is present
	and if so, return \ref TRUE, otherwise return \ref FALSE.
	
	\note This function returns \ref TRUE on all target platforms
	except \ref BURGER_MSDOS, the MS/DOS target performs a check
	to see if it's running under Windows 95 or higher and if
	so, Burgerlib will automatically support long filenames.
	
	\sa Burger::FileManager
	
***************************************/

/*! ************************************

	\fn Word BURGER_API Burger::FileManager::GetVolumeName(Filename *pOutput,Word uVolumeNum)
	\brief Return the name of a drive
	
	Given a drive number (0-?), return the name of
	the volume in the format of ":Volume name:". 
	The function will guarantee the existance of the colons.

	\note This function should be used with caution. Only mounted
	drives would return immdiately and if the drive has
	ejectable media may take a while for it to respond to
	a volume name query.

	\param pOutput A Burger::Filename structure to contain the filename (Can be \ref NULL)
	\param uVolumeNum A valid drive number from 0-?? with ?? being the maximum number of drives in the system
	\return Zero if no error, non-zero if an error occured
	\sa Burger::FileManager::GetVolumeNumber(const char *)
	
***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MSDOS) && !defined(BURGER_MACOS) && !defined(BURGER_IOS) && !defined(BURGER_XBOX360)) || defined(DOXYGEN)

Word BURGER_API Burger::FileManager::GetVolumeName(Filename *pOutput,Word /* uVolumeNum */)
{
	if (pOutput) {
		pOutput->Clear();	// Clear the output on error
	}
	return File::OUTOFRANGE;			// Error!
}

#endif

/*! ************************************

	\brief Given a volume name, return the volume number it's mounted on.
	
	Scanning all the disk volumes, search for
	a drive that is labeled by the input name. The input is in the 
	form of ":home:". The search is case insensitive.

	\param pVolumeName Valid pointer to "C" string in the form of ":name:"
	\return Volume number from 0-max drives and -1 in case of the volume was not found.
	
	\note This function will NOT search floppy drives A: and B: on MSDOS
	and Windows platforms. This is to prevent an annoying error for bad media.
	\sa Burger::FileManager::GetVolumeName(Burger::Filename *,Word)

***************************************/

Word BURGER_API Burger::FileManager::GetVolumeNumber(const char *pVolumeName)
{
#if defined(BURGER_MSDOS) || defined(BURGER_WINDOWS)
	const Word LASTDRIVE=26;
	Word uDriveNum = 2;		// Start at drive C:
#else
	Word uDriveNum = 0;		// Start at drive A:
	const Word LASTDRIVE=32;
#endif

	Word uResult = static_cast<Word>(-1);	// Assume failure
	Filename MyFilename;
	do {
		// Convert to name
		Word uError = FileManager::GetVolumeName(&MyFilename,uDriveNum);
		if (uError==File::OKAY) {
			// Compare
			int iResult = StringCaseCompare(MyFilename.GetPtr(),pVolumeName);
			if (!iResult) {				// Match?
				uResult = uDriveNum;	// Return the drive number
				break;
			}
		} else if (uError==File::OUTOFRANGE) {
			break;
		}
	} while (++uDriveNum<LASTDRIVE);	// All drives checked?
	return uResult;						// Return error or drive number
}

/*! ************************************

	\brief Set the initial default prefixs for a power up state

	Sets these prefixes based on the current setup of
	the machine the application is running on.

	"*:" = Boot volume<br>
	"$:" = System folder<br>
	"@:" = Prefs folder<br>
	"8:" = Default directory<br>
	"9:" = Application directory

	\sa Burger::FileManager::Init(void), Burger::FileManager::GetPrefix(Burger::Filename *,Word)
	or Burger::FileManager::SetPrefix(Word,const char *)
	
***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MSDOS) && !defined(BURGER_MACOS) && !defined(BURGER_IOS) && !defined(BURGER_XBOX360)) || defined(DOXYGEN)

void BURGER_API Burger::FileManager::DefaultPrefixes(void)
{
}

#endif

/*! ************************************

	\brief Return the contents of a prefix

	Given a prefix number, fill in a \ref Burger::Filename structure
	with a copy of the prefix pathname. The string could
	be an empty string in the case of an unused or \ref NULL 
	prefix.

	\param pOutput Pointer to a Burger::Filename class to store the string
	\param uPrefixNum Index to the requested prefix to obtain
	\return Zero if no error, non-zero if error
	\sa Burger::FileManager::SetPrefix(Word,const char *)

***************************************/

Word BURGER_API Burger::FileManager::GetPrefix(Filename *pOutput,Word uPrefixNum)
{
	Word uResult;
	if (uPrefixNum<FileManager::PREFIXMAX) {		// Is the prefix number valid?
		pOutput->Set(FileManager::g_FileManager.m_pPrefix[uPrefixNum]);	// Get the prefix string
		uResult = File::OKAY;
	} else {
		pOutput->Clear();
		uResult = File::OUTOFRANGE;
	}
	return uResult;	// Return the error code
}

/*! ************************************

	\brief Set the contents of a prefix

	Given a prefix number and a "C" string to a new pathname,
	set that prefix to the new value. The prefix is expanded
	BEFORE it's applied, so if you set prefix 10 with "10:foo"
	and prefix 10 was already ":Work:Temp:", then prefix
	10 will result in ":Work:Temp:foo:"
	
	Passing an empty string or a \ref NULL will cause the prefix
	to be released and considered empty.
	
	\param uPrefixNum Index to the requested prefix to obtain
	\param pPrefixName Pointer to a "C" string of a new BurgerLib pathname
	\return Zero if no error, non-zero if error (Usually out of memory or out of bounds)
	\sa Burger::FileManager::GetPrefix(Burger::Filename *,Word)
	
***************************************/

Word BURGER_API Burger::FileManager::SetPrefix(Word uPrefixNum,const char *pPrefixName)
{
	Filename PrefixName(pPrefixName);
	return SetPrefix(uPrefixNum,&PrefixName);
}

/*! ************************************

	\brief Set the contents of a prefix with a filename

	Given a prefix number and a Burger::Filename that has a new pathname,
	set that prefix to the new value. The prefix is expanded
	BEFORE it's applied, so if you set prefix 10 with "10:foo"
	and prefix 10 was already ":Work:Temp:", then prefix
	10 will result in ":Work:Temp:foo:"
	
	Passing an empty string or a \ref NULL will cause the prefix
	to be released and considered empty.
	
	\param uPrefixNum Index to the requested prefix to obtain
	\param pPrefixName Pointer to a Burger::Filename of a new BurgerLib pathname
	\return Zero if no error, non-zero if error (Usually out of memory or out of bounds)
	\sa Burger::FileManager::GetPrefix(Burger::Filename *,Word)
	
***************************************/

Word BURGER_API Burger::FileManager::SetPrefix(Word uPrefixNum,Filename *pPrefixName)
{
	Word uResult;
	if (uPrefixNum>=FileManager::PREFIXMAX) {		// Is the prefix valid?
		uResult = File::OUTOFRANGE;
	} else {
		uResult = File::OKAY;
		const char *pTemp = NULL;					// Zap the prefix
		// Valid prefix?
		if (pPrefixName) {
			const char *pPath = pPrefixName->GetPtr();
			if (pPath[0]) {
				pPrefixName->Expand();				// Convert to full pathname
				pPath = pPrefixName->GetPtr();
				if (pPath[0]) {						// Blank string?
					pTemp = StringDuplicate(pPath);	// Allocate memory for new prefix
				}
			}	
		}
		// Was there a path already?
		Free(Burger::FileManager::g_FileManager.m_pPrefix[uPrefixNum]);
		// Save the new pathname
		FileManager::g_FileManager.m_pPrefix[uPrefixNum] = pTemp;
	}
	return uResult;		// Return the result
}

/*! ************************************

	\brief Remove the last entry of a prefix

	Given a prefix number, pop off the last entry so that it
	effectively goes up one entry in a directory hiearchy.
	Imagine performing the operation "cd .." on the prefix.

	This can force the prefix to become an empty string if the
	prefix is pointing to a root folder.
	
	Examples:<br>
	":foo:bar:temp:" = ":foo:bar:"<br>
	":foo:bar:" = ":foo:"<br>
	":foo:" = ""
	
	\param uPrefixNum Index to the prefix to modify.
	\sa Burger::FileManager::GetPrefix(Burger::Filename *,Word) or Burger::FileManager::SetPrefix(Word,const char *)
	
***************************************/

void BURGER_API Burger::FileManager::PopPrefix(Word uPrefixNum)
{
	Filename TempName;
	GetPrefix(&TempName,uPrefixNum);	// Get the current prefix
	TempName.DirName();					// Remove a directory
	SetPrefix(uPrefixNum,&TempName);	// Store the prefix
}

/*! ************************************

	\brief Reset all file prefixes.
	
	Clear all file prefixes and release
	any memory allocated for them.
	
	\note This is an internal function called
	on Burger::FileManager shutdown. It's not meant
	for application use.

	\sa Burger::FileManager::DefaultPrefixes(void)
	
***************************************/

void Burger::FileManager::ReleasePrefixes(void)
{
	Word i = PREFIXMAX;
	const char **pTable = m_pPrefix;
	do {
		Free(pTable[0]);	// Release the memory
		pTable[0] = NULL;	// Gone
		++pTable;
	} while (--i);
#if defined(BURGER_MACOSX) || defined(BURGER_IOS)
	Free(m_pBootName);
	m_pBootName = NULL;
	m_uBootNameSize = 0;
#endif
}

/*! ************************************

	\brief Get a file's last modification time.
	
	Given a BurgerLib path, access the file and return the
	time/date that it was last modified.
	
	\param pFileName Pointer to a "C" string to a BurgerLib pathname
	\param pOutput Pointer to an uninitialized Burger::TimeDate_t structure that will receive the time/date.
	\return Zero on success, non-zero in the event of an error (Usually file not found)
	
	\sa Burger::FileManager::GetModificationTime(Burger::Filename *,Burger::TimeDate_t *)
	
***************************************/

Word BURGER_API Burger::FileManager::GetModificationTime(const char *pFileName,TimeDate_t *pOutput)
{
	Filename PathName(pFileName);
	return GetModificationTime(&PathName,pOutput);
}

/*! ************************************

	\brief Get a file's last modification time.
	
	Given a native operating system path, access the file and return the
	time/date that it was last modified.
	
	\param pFileName Pointer to a Burger::Filename class that is properly initialized to the native file system.
	\param pOutput Pointer to an uninitialized Burger::TimeDate_t structure that will receive the time/date.
	\return Zero on success, non-zero in the event of an error (Usually file not found)
	
	\sa Burger::Filename::SetFromNative(const char *) or Burger::FileManager::GetModificationTime(const char *,Burger::TimeDate_t *)
	
***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MSDOS) && !defined(BURGER_MACOS) && !defined(BURGER_IOS) && !defined(BURGER_XBOX360)) || defined(DOXYGEN)
Word BURGER_API Burger::FileManager::GetModificationTime(Filename * /* pFileName */,TimeDate_t * /* pOutput */)
{
	return TRUE;		// Error!
}
#endif

/*! ************************************

	\brief Get a file's creation time.
	
	Given a BurgerLib path, access the file and return the
	time/date that it was created.
	
	\param pFileName Pointer to a "C" string to a BurgerLib pathname
	\param pOutput Pointer to an uninitialized Burger::TimeDate_t structure that will receive the time/date.
	\return Zero on success, non-zero in the event of an error (Usually file not found)
	
	\sa Burger::FileManager::GetCreationTime(Burger::Filename *,Burger::TimeDate_t *)
	
***************************************/

Word BURGER_API Burger::FileManager::GetCreationTime(const char *pFileName,TimeDate_t *pOutput)
{
	Filename PathName(pFileName);							
	return GetCreationTime(&PathName,pOutput);
}

/*! ************************************

	\brief Get a file's creation time.
	
	Given a native operating system path, access the file and return the
	time/date that it was created.
	
	\param pFileName Pointer to a Burger::Filename class that is properly initialized to the native file system.
	\param pOutput Pointer to an uninitialized Burger::TimeDate_t structure that will receive the time/date.
	\return Zero on success, non-zero in the event of an error (Usually file not found)
	
	\sa Burger::Filename::SetFromNative(const char *) or Burger::FileManager::GetCreationTime(const char *,Burger::TimeDate_t *)
	
***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MSDOS) && !defined(BURGER_MACOS) && !defined(BURGER_IOS) && !defined(BURGER_XBOX360)) || defined(DOXYGEN)
Word BURGER_API Burger::FileManager::GetCreationTime(Filename * /* pFileName */,TimeDate_t * /* pOutput */)
{
	return TRUE;		// Error!
}
#endif

/*! ************************************

	\brief Detect for a file's existance.
	
	Given a BurgerLib pathname, return \ref TRUE if the file
	exists. \ref FALSE if there is an disk error or the file does not exist.
	
	\param pFileName Pointer to a "C" string of a BurgerLib pathname,
	\return \ref FALSE if the file does not exist, \ref TRUE if the file exists.
	\note This does not guarantee that the file can be opened due to security permissions
	or the file being on a locked folder. It only guarantees its existence.		
	\sa Burger::FileManager::DoesFileExist(Burger::Filename *)
	
***************************************/

Word BURGER_API Burger::FileManager::DoesFileExist(const char *pFileName)
{
	Filename PathName(pFileName);
	return DoesFileExist(&PathName);
}

/*! ************************************

	\brief Detect for a file's existance using a native pathname.
	
	Given a OS native pathname, return \ref TRUE if the file
	exists. \ref FALSE if there is an disk error or the file does not exist.
	
	\param pFileName Pointer to a Burger::Filename which contains a native version of the path to the file.
	\return \ref FALSE if the file does not exist, \ref TRUE if the file exists.
	\note This does not guarantee that the file can be opened due to security permissions
	or the file being on a locked folder. It only guarantees its existence.		

	\sa Burger::Filename::SetFromNative(const char *) or Burger::FileManager::DoesFileExist(const char *)
	
***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MSDOS) && !defined(BURGER_MACOS) && !defined(BURGER_IOS) && !defined(BURGER_XBOX360)) || defined(DOXYGEN)
Word BURGER_API Burger::FileManager::DoesFileExist(Filename *pFileName)
{
#if defined(BURGER_DS)
	pFileName = NULL;
	return FALSE;
#else
	FILE *fp = fopen(pFileName->GetNative(),"rb");	// Get file info
	if (!fp) {
		return FALSE;		// Bad file!
	}
	fclose(fp);
	return TRUE;			// File exists
#endif
}
#endif

/*! ************************************

	\brief Return a file's creator code.
	
	On MacOS, a file has extended data which includes the application code
	that owns the file. This function will retrieve the 4 byte code from the
	file.
	
	On non MacOS platforms, this function only return 0 (An error condition) and
	perform nothing..
	
	\param pFileName Pointer to a "C" string of a BurgerLib pathname,
	\return 0 if the file doesn't exist, the function isn't implemented of if the
		file doesn't have extended information. A 32 bit value if the data can be retrieved.
	\sa Burger::FileManager::GetAuxType(Burger::Filename *),
		Burger::FileManager::GetFileType(const char *) or
		Burger::FileManager::GetFileAndAuxType(const char *,Word32 *,Word32 *)
	
***************************************/

Word32 BURGER_API Burger::FileManager::GetAuxType(const char *pFileName)
{
	Filename TempName(pFileName);		// Get the true path
	return GetAuxType(&TempName);	// Call the function
}

/*! ************************************

	\brief Return a file's type code using a native filename.
	
	On MacOS, a file has extended data which includes the file's type, which
	is independent of the filename. This function will retrieve the 4 byte code from the
	file.
	
	On non MacOS platforms, this function only return 0 (An error condition) and
	perform nothing..
	
	\param pFileName Pointer to a Burger::Filename of a native OS pathname,
	\return 0 if the file doesn't exist, the function isn't implemented of if the 
		file doesn't have extended information. A 32 bit value if the data can be retrieved.
	\sa Burger::FileManager::GetFileType(const char *), 
		Burger::FileManager::GetAuxType(const char *) or
		Burger::FileManager::GetFileAndAuxType(const char *,Word32 *,Word32 *)

***************************************/

#if (!defined(BURGER_MACOS) && !defined(BURGER_IOS)) || defined(DOXYGEN)
Word32 BURGER_API Burger::FileManager::GetAuxType(Filename * /*pFileName */)
{
	return 0;		// Don't do anything!
}
#endif

/*! ************************************

	\brief Return a file's type code.
	
	On MacOS, a file has extended data which includes the file's type, which
	is independent of the filename. This function will retrieve the 4 byte code from the
	file.
	
	On non MacOS platforms, this function only return 0 (An error condition) and
	perform nothing..
	
	\param pFileName Pointer to a "C" string of a BurgerLib pathname,
	\return 0 if the file doesn't exist, the function isn't implemented of if the 
		file doesn't have extended information. A 32 bit value if the data can be retrieved.
	\sa Burger::FileManager::GetFileType(Burger::Filename *), 
		Burger::FileManager::GetAuxType(const char *) or
		Burger::FileManager::GetFileAndAuxType(const char *,Word32 *,Word32 *)

***************************************/

Word32 BURGER_API Burger::FileManager::GetFileType(const char *pFileName)
{
	Filename TempName(pFileName);		// Get the true path
	return GetFileType(&TempName);	// Call the function
}

/*! ************************************

	\brief Return a file's creator code using a native filename.
	
	On MacOS, a file has extended data which includes the application code
	that owns the file. This function will retrieve the 4 byte code from the
	file.
	
	On non MacOS platforms, this function only return 0 (An error condition) and
	perform nothing..
	
	\param pFileName Pointer to a Burger::Filename of a native OS pathname,
	\return 0 if the file doesn't exist, the function isn't implemented of if the
		file doesn't have extended information. A 32 bit value if the data can be retrieved.
	\sa Burger::FileManager::GetAuxType(const char *),
		Burger::FileManager::GetFileType(const char *) or
		Burger::FileManager::GetFileAndAuxType(const char *,Word32 *,Word32 *)
	
***************************************/

#if (!defined(BURGER_MACOS) && !defined(BURGER_IOS)) || defined(DOXYGEN)
Word32 BURGER_API Burger::FileManager::GetFileType(Filename * /*pFileName*/)
{
	return 0;		// Don't do anything!
}
#endif

/*! ************************************

	\brief Return a file's type and creator codes.
	
	On MacOS, a file has extended data which includes the file's type, which
	is independent of the filename, and the creator code of the application that
	owns this file. This function will retrieve the 4 byte codes from the
	file.
	
	On non MacOS platforms, this function only return non-zero (An error condition) and
	perform store zeros in the result values..
	
	\param pFileName Pointer to a "C" string of a BurgerLib pathname,
	\param pFileType Pointer to a \ref Word32 that will receive the file type code.
	\param pAuxType Pointer to a \ref Word32 that will receive the file creator code.
	\return Non-zero if the file doesn't exist, the function isn't implemented of if the 
		file doesn't have extended information. Zero is returned on successfull completion.
	\sa Burger::FileManager::GetFileAndAuxType(Burger::Filename *,Word32 *,Word32 *),
		Burger::FileManager::GetFileType(const char *) or
		Burger::FileManager::GetAuxType(const char *)
	
***************************************/

Word BURGER_API Burger::FileManager::GetFileAndAuxType(const char *pFileName,Word32 *pFileType,Word32 *pAuxType)
{
	Filename TempName(pFileName);		// Get the true path
	return GetFileAndAuxType(&TempName,pFileType,pAuxType);	// Call the function
}

/*! ************************************

	\brief Return a file's type and creator codes using a native filename.
	
	On MacOS, a file has extended data which includes the file's type, which
	is independent of the filename, and the creator code of the application that
	owns this file. This function will retrieve the 4 byte codes from the
	file.
	
	On non MacOS platforms, this function only return non-zero (An error condition) and
	perform store zeros in the result values..
	
	\param pFileName Pointer to a Burger::Filename of a native OS pathname,
	\param pFileType Pointer to a \ref Word32 that will receive the file type code.
	\param pAuxType Pointer to a \ref Word32 that will receive the file creator code.
	\return Non-zero if the file doesn't exist, the function isn't implemented of if the 
		file doesn't have extended information. Zero is returned on successfull completion.
	\sa Burger::FileManager::GetFileAndAuxType(Burger::Filename *,Word32 *,Word32 *),
		Burger::FileManager::GetFileType(const char *) or
		Burger::FileManager::GetAuxType(const char *)
	
***************************************/

#if (!defined(BURGER_MACOS) && !defined(BURGER_IOS)) || defined(DOXYGEN)
Word BURGER_API Burger::FileManager::GetFileAndAuxType(Filename * /* pFileName */,Word32 *pFileType,Word32 *pAuxType)
{
	pFileType[0] = 0;
	pAuxType[0] = 0;
	return File::NOT_IMPLEMENTED;
}
#endif

/*! ************************************

	\brief Set a file's creator code.
	
	On MacOS, a file has extended data which includes the application code
	that owns the file. This function will set the 4 byte code for the
	file.
	
	On non MacOS platforms, this function will perform nothing..
	
	\param pFileName Pointer to a "C" string of a BurgerLib pathname,
	\param uAuxType Four byte character code to set.
	\return Non-zero if the file doesn't exist, the function isn't implemented of if the
		file doesn't have extended information. Zero if successful.
	\sa Burger::FileManager::SetAuxType(Burger::Filename *,Word32),
		Burger::FileManager::SetFileType(const char *,Word32) or
		Burger::FileManager::SetFileAndAuxType(const char *,Word32,Word32)
	
***************************************/

Word BURGER_API Burger::FileManager::SetAuxType(const char *pFileName,Word32 uAuxType)
{
	Filename TempName(pFileName);	// Get the true path
	return SetAuxType(&TempName,uAuxType);
}

/*! ************************************

	\brief Set a file's type code using a native filename.
	
	On MacOS, a file has extended data which includes the file's type, which
	is independent of the filename. This function will set the 4 byte code for the
	file.
	
	On non MacOS platforms, this function will perform nothing..
	
	\param pFileName Pointer to a Burger::Filename of a native OS pathname,
	\param uAuxType Four byte character code to set.
	\return Non-zero if the file doesn't exist, the function isn't implemented of if the 
		file doesn't have extended information. Zero if successful.
	\sa Burger::FileManager::SetFileType(const char *,Word32), 
		Burger::FileManager::SetAuxType(const char *,Word32) or
		Burger::FileManager::SetFileAndAuxType(const char *,Word32,Word32)

***************************************/

#if (!defined(BURGER_MACOS) && !defined(BURGER_IOS)) || defined(DOXYGEN)
Word BURGER_API Burger::FileManager::SetAuxType(Filename * /*pFileName*/,Word32 /* uAuxType */)
{
	return File::NOT_IMPLEMENTED;
}
#endif

/*! ************************************

	\brief Set a file's type code.
	
	On MacOS, a file has extended data which includes the file's type, which
	is independent of the filename. This function will set the 4 byte code for the
	file.
	
	On non MacOS platforms, this function will perform nothing..
	
	\param pFileName Pointer to a "C" string of a BurgerLib pathname,
	\param uFileType Four byte character code to set.
	\return Non-zero if the file doesn't exist, the function isn't implemented of if the 
		file doesn't have extended information. Zero if successful.
	\sa Burger::FileManager::SetFileType(Burger::Filename *,Word32), 
		Burger::FileManager::SetAuxType(const char *,Word32) or
		Burger::FileManager::SetFileAndAuxType(const char *,Word32,Word32)

***************************************/

Word BURGER_API Burger::FileManager::SetFileType(const char *pFileName,Word32 uFileType)
{
	Filename TempName(pFileName);	// Get the true path
	return SetFileType(&TempName,uFileType);
}

/*! ************************************

	\brief Set a file's type code using a native filename.
	
	On MacOS, a file has extended data which includes the file's type, which
	is independent of the filename. This function will set the 4 byte code for the
	file.
	
	On non MacOS platforms, this function will perform nothing..
	
	\param pFileName Pointer to a Burger::Filename of a native OS pathname,
	\param uFileType Four byte character code to set.
	\return Non-zero if the file doesn't exist, the function isn't implemented of if the 
		file doesn't have extended information. Zero if successful.
	\sa Burger::FileManager::SetFileType(const char *,Word32), 
		Burger::FileManager::SetAuxType(const char *,Word32) or
		Burger::FileManager::SetFileAndAuxType(const char *,Word32,Word32)

***************************************/

#if (!defined(BURGER_MACOS) && !defined(BURGER_IOS)) || defined(DOXYGEN)
Word BURGER_API Burger::FileManager::SetFileType(Filename * /*pFileName*/,Word32 /*uFileType */)
{
	return File::NOT_IMPLEMENTED;
}
#endif

/*! ************************************

	\brief Set a file's type and creator codes.
	
	On MacOS, a file has extended data which includes the file's type, which
	is independent of the filename, and the creator code of the application that
	owns this file. This function will set the 4 byte codes for the
	file.
	
	On non MacOS platforms, this function will perform nothing..
	
	\param pFileName Pointer to a "C" string of a BurgerLib pathname,
	\param uFileType A \ref Word32 of the new file type code.
	\param uAuxType A \ref Word32 of the new file creator code.
	\return Non-zero if the file doesn't exist, the function isn't implemented of if the 
		file doesn't have extended information. Zero is returned on successful completion.
	\sa Burger::FileManager::GetFileAndAuxType(Burger::Filename *,Word32 *,Word32 *),
		Burger::FileManager::GetFileType(const char *) or
		Burger::FileManager::GetAuxType(const char *)
	
***************************************/

Word BURGER_API Burger::FileManager::SetFileAndAuxType(const char *pFileName,Word32 uFileType,Word32 uAuxType)
{
	Filename TempName(pFileName);	// Get the true path
	return SetFileAndAuxType(&TempName,uFileType,uAuxType);
}

/*! ************************************

	\brief Set a file's type and creator codes using a native filename.
	
	On MacOS, a file has extended data which includes the file's type, which
	is independent of the filename, and the creator code of the application that
	owns this file. This function will set the 4 byte codes for the
	file.
	
	On non MacOS platforms, this function will perform nothing..
	
	\param pFileName Pointer to a Burger::Filename of a native OS pathname,
	\param uFileType A \ref Word32 of the new file type code.
	\param uAuxType A \ref Word32 of the new file creator code.
	\return Non-zero if the file doesn't exist, the function isn't implemented of if the 
		file doesn't have extended information. Zero is returned on successful completion.
	\sa Burger::FileManager::GetFileAndAuxType(const char *,Word32 *,Word32 *),
		Burger::FileManager::GetFileType(const char *) or
		Burger::FileManager::GetAuxType(const char *)
	
***************************************/

#if (!defined(BURGER_MACOS) && !defined(BURGER_IOS)) || defined(DOXYGEN)
Word BURGER_API Burger::FileManager::SetFileAndAuxType(Filename * /*pFileName*/,Word32 /* uFileType */,Word32 /* uAuxType */)
{
	return File::NOT_IMPLEMENTED;
}
#endif

/*! ************************************

	\brief Create a directory path
	
	Given a BurgerLib directory, create each and every part of the directory.
	If the directory already exists, or is successfully created, the
	call is considered successful.
	
	Example:<br>
	If the path :Foo:Bar: exists, a call with ":Foo:Bar:Fooey:Temp:" will
	create both "Fooey" and "Temp" in this single call.
	
	\param pFileName Pointer to a "C" string of a BurgerLib path.
	\return Zero if successful, non-zero on error.
	\sa Burger::FileManager::CreateDirectoryPathDirName(const char *)
	
***************************************/

Word BURGER_API Burger::FileManager::CreateDirectoryPath(const char *pFileName)
{
	Filename PathName(pFileName);					// Convert to native path
	return CreateDirectoryPath(&PathName);	// Create the path
}

/*! ************************************

	\brief Create a directory path using an OS native pathname.
	
	Given an OS native directory, create each and every part of the directory.
	If the directory already exists, or is successfully created, the
	call is considered successful.
	
	Example:<br>
	If the path c:\\Foo\\Bar exists, a call with "c:\Foo\Bar\Fooey\Temp\" will
	create both "Fooey" and "Temp" in this single call.
	
	\param pFileName Pointer to a Burger::Filename that contains a native OS pathname.
	\return Zero if successful, non-zero on error.
	\sa Burger::FileManager::CreateDirectoryPath(const char *) or
		Burger::FileManager::CreateDirectoryPathDirName(const char *)
	
***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MSDOS) && !defined(BURGER_MACOS) && !defined(BURGER_IOS) && !defined(BURGER_XBOX360)) || defined(DOXYGEN)
Word BURGER_API Burger::FileManager::CreateDirectoryPath(Filename * /* pFileName */ )
{
	return File::NOT_IMPLEMENTED;		// Always error out
}
#endif

/*! ************************************

	\brief Create a directory path sans the last filename token.
	
	Given a BurgerLib directory, create each and every part of the directory
	from a path that includes a filename.
	If the directory already exists, or is successfully created, the
	call is considered successful.
	
	Example:<br>
	If the path :Foo:Bar: exists, a call with ":Foo:Bar:Fooey:Temp:File.txt" will
	create both "Fooey" and "Temp" in this single call. The last token of "File.txt"
	is ignored.
	
	\param pFileName Pointer to a "C" string of a BurgerLib path.
	\return Zero if successful, non-zero on error.
	\sa Burger::FileManager::CreateDirectoryPath(const char *)
	
***************************************/

Word BURGER_API Burger::FileManager::CreateDirectoryPathDirName(const char *pFileName)
{
	Filename FileNameCopy(pFileName);
	FileNameCopy.DirName();
	return CreateDirectoryPath(&FileNameCopy);
}

/*! ************************************

	\brief Create a directory path sans the last filename token.
	
	Given a BurgerLib directory, create each and every part of the directory
	from a path that includes a filename.
	If the directory already exists, or is successfully created, the
	call is considered successful.
	
	Example:<br>
	If the path :Foo:Bar: exists, a call with ":Foo:Bar:Fooey:Temp:File.txt" will
	create both "Fooey" and "Temp" in this single call. The last token of "File.txt"
	is ignored.
	
	\param pFileName Pointer to a Filename class
	\return Zero if successful, non-zero on error.
	\sa Burger::FileManager::CreateDirectoryPath(Filename *)
	
***************************************/

Word BURGER_API Burger::FileManager::CreateDirectoryPathDirName(Filename *pFileName)
{
	Filename FileNameCopy(*pFileName);
	FileNameCopy.DirName();
	return CreateDirectoryPath(&FileNameCopy);
}

/*! ************************************

	\brief Delete a file.
	
	Given a BurgerLib pathname, delete the file at the end of the pathname.
	
	Example:<br>
	If a call with ":Foo:Bar:Fooey:Temp.txt" is issued, only the file "Temp.txt"
	is deleted. The rest of the path is left intact. This call will not delete
	directories, only files. If the file is already deleted, an error is returned.
	
	\param pFileName Pointer to a "C" string of a BurgerLib path.
	\return Zero if successful, non-zero on error.
	\sa Burger::FileManager::DeleteFile(Burger::Filename *)
	
***************************************/

Word BURGER_API Burger::FileManager::DeleteFile(const char *pFileName)
{
	Filename Dest(pFileName);		// Expand the path to a full filename
	return DeleteFile(&Dest);		// Copy the file
}

/*! ************************************

	\brief Delete a file using an OS pathname.
	
	Given an OS native pathname, delete the file at the end of the pathname.
	
	Example:<br>
	If a call with "c:\Foo\Bar\Fooey\Temp.txt" is issued, only the file "Temp.txt"
	is deleted. The rest of the path is left intact. This call will not delete
	directories, only files. If the file is already deleted, an error is returned.
	
	\param pFileName Pointer to a Burger::Filename of a native OS path.
	\return Zero if successful, non-zero on error.
	\sa Burger::FileManager::DeleteFile(const char *)
	
***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MSDOS) && !defined(BURGER_MACOS) && !defined(BURGER_IOS) && !defined(BURGER_XBOX360)) || defined(DOXYGEN)
Word BURGER_API Burger::FileManager::DeleteFile(Filename *pFileName)
{
#if defined(BURGER_DS)
	return File::NOT_IMPLEMENTED;	// Always error out
#else
	Word uResult = TRUE;	// Assume error
	if (remove(pFileName->GetNative())) {
		uResult = FALSE;	// No error
	}
	return uResult;
#endif
}
#endif

/*! ************************************

	\brief Rename a file.
	
	Given a BurgerLib formatted old pathname and new pathname, rename
	the file at the end of the pathname.
	
	\note The source and destination directories must be the same.
	
	\param pNewName Pointer to a "C" string of a BurgerLib path for the new name.
	\param pOldName Pointer to a "C" string of a BurgerLib path for the current file name.
	\return Zero if successful, non-zero on error.
	\sa Burger::FileManager::RenameFile(Burger::Filename *,Burger::Filename *)
	
***************************************/

Word BURGER_API Burger::FileManager::RenameFile(const char *pNewName,const char *pOldName)
{
	Filename Dest(pNewName);		// Expand the path to a full filename
	Filename Src(pOldName);			// Expand the source path
	return RenameFile(&Dest,&Src);	// Rename or move the file
}

/*! ************************************

	\brief Rename a file using OS native pathnames.
	
	Given an OS native formatted old pathname and new pathname, rename
	the file at the end of the pathname.
	
	\note The source and destination directories must be the same.
	
	\param pNewName Pointer to a Burger::Filename of an OS native path for the new name.
	\param pOldName Pointer to a Burger::Filename of an OS native path for the current file name.
	\return Zero if successful, non-zero on error.
	\sa Burger::FileManager::RenameFile(const char *,const char *)
	
***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MACOS) && !defined(BURGER_IOS) && !defined(BURGER_XBOX360)) || defined(DOXYGEN)
Word BURGER_API Burger::FileManager::RenameFile(Filename *pNewName,Filename *pOldName)
{
#if defined(BURGER_DS)
	// Always error out
	return File::NOT_IMPLEMENTED;
#else
	Word uResult = TRUE;		// Assume error
	if (rename(pOldName->GetNative(),pNewName->GetNative())) {
		uResult = FALSE;
	}
	return uResult;
#endif
}
#endif

/*! ************************************

	\brief Change the OS working directory.
	
	Given a BurgerLib pathname, set the OS's current working directory
	to this path.
	
	This function is useful in cases where an OS native file dialog to 
	select a file or perform an OS native function where the current
	working directory needs to be preset. This function under most circumstances
	is not necessary to use, however, it exists since there are some
	OS native functions in MacOS and Windows that require a little "help".
	
	\param pDirName Pointer to a "C" string of a BurgerLib path.
	\return Zero if successful, non-zero on error.
	\sa Burger::FileManager::ChangeOSDirectory(Burger::Filename *)
	
***************************************/

Word BURGER_API Burger::FileManager::ChangeOSDirectory(const char *pDirName)
{
	Filename DirName(pDirName);				// Expand the path to a full filename
	return ChangeOSDirectory(&DirName);		// Set the directory here
}

/*! ************************************

	\brief Change the OS working directory using an OS native pathname..
	
	Given an OS native pathname, set the OS's current working directory
	to this path.
	
	This function is useful in cases where an OS native file dialog to 
	select a file or perform an OS native function where the current
	working directory needs to be preset. This function under most circumstances
	is not necessary to use, however, it exists since there are some
	OS native functions in MacOS and Windows that require a little "help".
	
	\param pDirName Pointer to a Burger::Filename of a native OS path.
	\return Zero if successful, non-zero on error.
	\sa Burger::FileManager::ChangeOSDirectory(const char *)
	
***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MSDOS) && !defined(BURGER_MACOS) && !defined(BURGER_IOS)) || defined(DOXYGEN)
Word BURGER_API Burger::FileManager::ChangeOSDirectory(Filename * /* pDirName */)
{
	return File::NOT_IMPLEMENTED;	// Error!
}
#endif

/*! ************************************

	\brief Open a file using stdio.
	
	Given a BurgerLib pathname, open a file as if fopen() supported
	BurgerLib pathnames,
	
	This is a cross platform method for opening a standard "C" library
	file stream. As such, this pointer must eventually be closed with
	a call to fclose().
	
	The second parameter is passed as is to a subsequent call to fopen(). See
	the docs on fopen() on valid input such as "rb" for read binary and "w" for
	write text file.
	
	\param pFileName Pointer to a "C" string of a BurgerLib path.
	\param pType Pointer to a "C" string that will be passed to a call to fopen().
	\return \ref NULL on error, a valid FILE * compatible with stdio.h file calls.
	\sa Burger::File
	
***************************************/

FILE * BURGER_API Burger::FileManager::OpenFile(const char *pFileName,const char *pType)
{
	Filename TempPath(pFileName);				// Expand the path to a full filename
	return OpenFile(&TempPath,pType);
}

/*! ************************************

	\brief Open a file using stdio.
	
	Given a BurgerLib pathname, open a file as if fopen() supported
	BurgerLib pathnames,
	
	This is a cross platform method for opening a standard "C" library
	file stream. As such, this pointer must eventually be closed with
	a call to fclose().
	
	The second parameter is passed as is to a subsequent call to fopen(). See
	the docs on fopen() on valid input such as "rb" for read binary and "w" for
	write text file.
	
	\param pFileName Pointer to a "C" string of a BurgerLib path.
	\param pType Pointer to a "C" string that will be passed to a call to fopen().
	\return \ref NULL on error, a valid FILE * compatible with stdio.h file calls.
	\sa Burger::File
	
***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MACOS) && !defined(BURGER_IOS)) || defined(DOXYGEN)
FILE * BURGER_API Burger::FileManager::OpenFile(Filename *pFileName,const char *pType)
{
#if defined(BURGER_DS)
	return NULL;
#else
	return fopen(pFileName->GetNative(),pType);	// Open using standard fopen
#endif
}
#endif

/*! ************************************

	\brief Copy a file.
	
	Given a BurgerLib formatted old pathname and new pathname, make a duplicate of
	the file at the end of the pathname.
	
	\param pDestName Pointer to a "C" string of a BurgerLib path for the new name.
	\param pSrcName Pointer to a "C" string of a BurgerLib path for the current file name.
	\return Zero if successful, non-zero on error.
	\sa Burger::FileManager::CopyFile(Burger::Filename *,Burger::Filename *)
	
***************************************/

Word BURGER_API Burger::FileManager::CopyFile(const char *pDestName,const char *pSrcName)
{
	Filename Dest(pDestName);		// Expand the path to a full filename
	Filename Src(pSrcName);			// Expand the source path
	return CopyFile(&Dest,&Src);	// Copy the file
}

/*! ************************************

	\brief Copy a file using OS native pathnames.
	
	Given an OS native formatted old pathname and new pathname, make a duplicate of
	the file at the end of the pathname.
	
	\param pDestName Pointer to a Burger::Filename of an OS native path for the new name.
	\param pSourceName Pointer to a Burger::Filename of an OS native path for the current file name.
	\return Zero if successful, non-zero on error.
	\sa Burger::FileManager::CopyFile(const char *,const char *)
	
***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MACOS) && !defined(BURGER_IOS) && !defined(BURGER_XBOX360)) || defined(DOXYGEN)
Word BURGER_API Burger::FileManager::CopyFile(Filename *pDestName,Filename *pSourceName)
{
#if defined(BURGER_DS)
	return TRUE;
#else

	Word uResult = File::IOERROR;						// Assume error
	File fpsrc(pSourceName,File::READONLY);		// Open the source file
	WordPtr uLength = fpsrc.GetSize();		// Get the size of the source file
	if (uLength) {						// Shall I copy anything?
		WordPtr uMaxChunk = (uLength<0x100000) ? uLength : 0x100000;
		Word8 *pBuffer = static_cast<Word8 *>(Alloc(uMaxChunk));
		if (pBuffer) {
			File fpdst(pDestName,File::WRITEONLY);		// Open the dest file
			do {
				WordPtr uChunk = uLength;	// Base chunk
				if (uChunk>uMaxChunk) {
					uChunk = uMaxChunk;		// Only copy the chunk
				}
				WordPtr uReadSize = fpsrc.Read(pBuffer,uChunk);	// Read data
				if (uReadSize!=uChunk) {
					break;
				}
				uReadSize = fpdst.Write(pBuffer,uChunk);	// Write data
				if (uReadSize!=uChunk) {
					break;
				}
				uLength -= uChunk;
			} while (uLength);			// Any data left?
			if (!uLength) {				// All data copied?
				uResult = File::OKAY;	// No error (So far)
			}
			if (fpdst.Close()) {		// Did the file have an error in closing?
				uResult = File::IOERROR;	// Crap.
			}
		}
		Free(pBuffer);		// Release the copy buffer
	}
	fpsrc.Close();			// Close the source file
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
	\sa Burger::FileManager::SaveFile(Burger::Filename *,const void *,WordPtr)
	
***************************************/

Word BURGER_API Burger::FileManager::SaveFile(const char *pFileName,const void *pInput,WordPtr uLength)
{
	Filename MyName(pFileName);
	return SaveFile(&MyName,pInput,uLength);
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
	\sa Burger::FileManager::SaveFile(const char *,const void *,WordPtr)
	
***************************************/

Word BURGER_API Burger::FileManager::SaveFile(Filename *pFileName,const void *pInput,WordPtr uLength)
{
	File FileRef;
	Word uResult = FileRef.Open(pFileName,File::WRITEONLY);
	if (uResult!=File::OKAY) {
		// Try creating the directory
		CreateDirectoryPathDirName(pFileName);
		// Now open the file
		uResult = FileRef.Open(pFileName,File::WRITEONLY);
	}
	// File opened.
	if (uResult==File::OKAY) {
		WordPtr uWritten = FileRef.Write(pInput,uLength);
		uResult = FileRef.Close();
		if ((uResult==File::OKAY) && (uWritten==uLength)) {
			return TRUE;
		}
	}
	return FALSE;
}

/*! ************************************

	\brief Save a text file.
	
	Given a BurgerLib pathname and a buffer, save the contents of the buffer
	into a file and close the file.
	
	This function is a quick and easy way to write a buffer from memory directly
	to disk (Or any other storage medium).
	
	\note This differs from Burger::FileManager::SaveFile(const char *,const void *,WordPtr)
	in that all "\n" values will be translated to the proper line feeds for the
	target operating system. Unix and Linux, no change, Windows and MSDos, it's
	converted to a "\n\r", MacOS and MacOSX it's converted to "\r".
	
	\param pFileName Pointer to a "C" string of a BurgerLib path.
	\param pInput Pointer to an array of bytes to save as a text file.
	\param uLength Number of bytes in the buffer
	\return \ref TRUE if successful, \ref FALSE on error.
	\sa Burger::FileManager::SaveTextFile(Burger::Filename *,const void *,WordPtr)
	
***************************************/

Word BURGER_API Burger::FileManager::SaveTextFile(const char *pFileName,const void *pInput,WordPtr uLength)
{
	Burger::Filename MyName(pFileName);
	return SaveTextFile(&MyName,pInput,uLength);
}

/*! ************************************

	\brief Save a text file using an OS native filename.
	
	Given an OS native pathname and a buffer, save the contents of the buffer
	into a file and close the file.
	
	This function is a quick and easy way to write a buffer from memory directly
	to disk (Or any other storage medium).
	
	\note This differs from Burger::FileManager::SaveFile(const Burger::Filename *,const void *,WordPtr)
	in that all "\n" values will be translated to the proper line feeds for the
	target operating system. Unix and Linux, no change, Windows and MSDos, it's
	converted to a "\n\r", MacOS and MacOSX it's converted to "\r".
	
	\param pFileName Pointer to a Burger::Filename of an OS native path.
	\param pInput Pointer to an array of bytes to save as a text file.
	\param uLength Number of bytes in the buffer
	\return \ref TRUE if successful, \ref FALSE on error.
	\sa Burger::FileManager::SaveTextFile(const char *,const void *,WordPtr)
	
***************************************/

Word BURGER_API Burger::FileManager::SaveTextFile(Filename *pFileName,const void *pInput,WordPtr uLength)
{
	FILE *fp = OpenFile(pFileName,"w");		// Open the file
	if (fp) {
		return Burger::SaveFile(fp,pInput,uLength);
	}
	return FALSE;
}

/*! ************************************

	\brief Load a file into memory.
	
	Given a BurgerLib pathname, open the file, allocate a buffer for it, load
	it into memory and return the pointer to the allocated buffer and the optional
	buffer size.
	
	This function is a quick and easy way to read the contents of a file into a buffer.
	The buffer must be deleted with a call to Burger::Free(const void *)
	
	\note This function does no data translation. The buffer contains an exact byte for
	byte copy of the contents of the file on disk.
	
	\param pFileName Pointer to a "C" string of a BurgerLib path.
	\param pLength Pointer to a \ref WordPtr to receive the size of the buffer. 
	\return \ref NULL if failure, a valid pointer to the buffer on success.
	\sa Burger::FileManager::LoadFile(Burger::Filename *,WordPtr *)
	
***************************************/

void * BURGER_API Burger::FileManager::LoadFile(const char *pFileName,WordPtr *pLength)
{
	Filename MyName(pFileName);
	return LoadFile(&MyName,pLength);
}

/*! ************************************

	\brief Load a file into memory using a native OS pathname.
	
	Given an OS native pathname, open the file, allocate a buffer for it, load
	it into memory and return the pointer to the allocated buffer and the optional
	buffer size.
	
	This function is a quick and easy way to read the contents of a file into a buffer.
	The buffer must be deleted with a call to Burger::Free(const void *)
	
	\note This function does no data translation. The buffer contains an exact byte for
	byte copy of the contents of the file on disk.

	\param pFileName Pointer to a Burger::Filename of an OS native path.
	\param pLength Pointer to a \ref WordPtr to receive the size of the buffer. 
	\return \ref NULL if failure, a valid pointer to the buffer on success.
	\sa Burger::FileManager::LoadFile(const char *,WordPtr *)
	
***************************************/

void * BURGER_API Burger::FileManager::LoadFile(Filename *pFileName,WordPtr *pLength)
{
#if defined(BURGER_DS)
	return NULL;
#else
	File FileRef(pFileName,File::READONLY);
	WordPtr uNewSize = FileRef.GetSize();
	void *pResult = NULL;
	if (uNewSize) {
#if defined(_DEBUG)
		if (Globals::GetTraceFlag()&Globals::TRACE_FILELOAD) {
			Debug::String("Loading native file ");
			Debug::String(pFileName->GetNative());
			Debug::String(".\n");
		}
#endif	
		pResult = Alloc(uNewSize);
		if (pResult) {
			if (FileRef.Read(pResult,uNewSize)!=uNewSize) {
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


/*! ************************************

	\class Burger::FileManagerSimple
	\brief Initialization class for the global Burger::FileManager structure.

	By default, the Burger::FileManager structure does not automatically initialize.
	The application determines the best time to start up and shut down the global singleton.
	To make this process easier, create an instance of this class in your main() function
	or use one of the application class templates which will do this task for you.

	There should be only one instance of this class in existance in the application. 

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
	
	\note This class can and will allocate memory at runtime via the initialization
	of \ref Burger::FileManager. It's crucial
	that the \ref Burger::GlobalMemoryManager is initialized BEFORE this
	class instance is created.
	
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
