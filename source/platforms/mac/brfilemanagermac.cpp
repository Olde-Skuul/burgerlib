/***************************************

	File Manager Class: MacOS version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brdebug.h"
#include "brfile.h"
#include "brglobalmemorymanager.h"
#include "brglobals.h"
#include "brmacromanus.h"
#include "brmemoryfunctions.h"
#include "brstring16.h"
#include "brtick.h"
#include "brutf8.h"
#include "mac_version.h"
#include <Files.h>
#include <Folders.h>
#include <Processes.h>
#include <Resources.h>
#include <Script.h>
#include <stdlib.h>

#if defined(__MSL__)
#undef _MSL_USE_NEW_FILE_APIS
#define _MSL_USE_NEW_FILE_APIS 1
#include <FSp_fopen.h>
#endif

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
	// Init the directory cache
	Filename::init_directory_cache();
}

/***************************************

	\brief Handle platform specific shutdown code

	Calls system functions to release cached values obtained from the platform's
	operating system.

	\sa platform_setup() or \ref FileManager

***************************************/

void BURGER_API Burger::FileManager::platform_shutdown(void) BURGER_NOEXCEPT
{
	// Release any directories cached (MacOS)
	Filename::purge_directory_cache();
}

/*! ************************************

	\brief Test if the file system supports UTF8 pathnames.

	Returns \ref TRUE if the filesystem has native Unicode filenames. On legacy
	or some game consoles, only the ASCII character set is supported.

	Mac OS classic is special because if the application is running under
	MacOS 8.1 or higher, Unicode is supported, if it's earlier, only character
	codes that exist in Mac Roman US are used.

	\note While MacOS 8.1 supports Unicode, it's only available with HFS+
		volumes

	\return \ref TRUE if Unicode is supported.

	\sa MSDOS_has_long_filenames()

***************************************/

uint_t BURGER_API Burger::FileManager::is_UTF8_file_system(void)
{
	// MacOS 9 or higher supports the HFSStr calls.
	return MacOS::get_os_version() >= 0x1000;
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
	eError uResult = kErrorVolumeNotFound;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Volume information buffer
	FSVolumeInfoParam pb;
	MemoryClear(&pb, sizeof(pb));

	// Unicode name of the volume returned
	HFSUniStr255 Name;
	FSVolumeInfo Info;
	MemoryClear(&Info, sizeof(Info));

	// Want the name
	// pb.ioVRefNum = kFSInvalidVolumeRefNum;

	// Drive starts with volume #1
	pb.volumeIndex = uVolumeNum + 1;
	// volumeIndex = Drive #
	pb.whichInfo = kFSVolInfoDriveInfo;
	// I don't want extra data
	pb.volumeInfo = &Info;
	// Name please
	pb.volumeName = &Name;

	// I don't want the volume's file reference
	// pb.ref = nullptr;

	// Got the data?
	OSErr iError = PBGetVolumeInfoSync(&pb);
	if (!iError) {
		uint_t uStrLen = Name.length;
		if (uStrLen) {
			if (pOutput) {
				// Convert Unicode to a CFString
				// Unicode could be as much as 4 bytes per char

				char NameUTF8[(256 * 4) + 4];
				uintptr_t uLength = UTF8::from_UTF16(
					NameUTF8 + 1, sizeof(NameUTF8) - 4, Name.unicode, uStrLen);

				NameUTF8[0] = ':';
				NameUTF8[uLength + 1] = ':';
				NameUTF8[uLength + 2] = 0;
				pOutput->assign(NameUTF8);
			}
			// No error!
			uResult = kErrorNone;
		}

		// If the call failed, it's possible the version of MacOS is 7.5 instead
		// of 9.0 or higher.
	} else if (iError == paramErr)
#endif
	{
		// Note: The volume returned will be in MacRoman encoding
		Str63 DriveName;
		HParamBlockRec hpb;
		hpb.volumeParam.ioNamePtr = DriveName;
		hpb.volumeParam.ioVRefNum = 0;
		hpb.volumeParam.filler2 = 0;
		hpb.volumeParam.ioVolIndex = static_cast<short>(uVolumeNum + 1);
		OSStatus uStatus = PBHGetVInfoSync(&hpb);
		if (!uStatus) {
			if (pOutput) {
				String FromMac;

				// Pre and post pend a colon and convert to a "C" string
				uintptr_t uLengthDrive = DriveName[0];
				DriveName[0] = ':';
				DriveName[uLengthDrive + 1] = ':';
				DriveName[uLengthDrive + 2] = 0;
				FromMac.assign_mac_roman_us(reinterpret_cast<char*>(DriveName));

				pOutput->assign(FromMac.c_str());
			}
			// No error!
			uResult = kErrorNone;
		}
	}

	// Clear on error
	if (uResult && pOutput) {
		// Kill the string since I have an error
		pOutput->clear();
	}
	return uResult;
}

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

Burger::eError Burger::FileManager::get_modification_time(
	Filename* pFileName, TimeDate_t* pOutput) BURGER_NOEXCEPT
{
	// Zap it
	pOutput->Clear();

	eError uResult;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->get_FSRef();
	if (pFSRef) {
		uResult = kErrorFileNotFound;
		const char* pNative = pFileName->get_native();
		if (!pNative[0]) {
			FSCatalogInfo MyInfo;
			iMacError = DoGetCatInfo(&MyInfo, pFSRef, kFSCatInfoContentMod);
			if (!iMacError) {
				// If it succeeded, the file must exist
				pOutput->Load(&MyInfo.contentModDate);
			}
			uResult = platform_convert_to_error(iMacError);
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		uResult = pFileName->get_FSSpec(&MySpec);
		if (!uResult) {
			iMacError = DoGetCatInfo(&InfoPBRec, &MySpec);
			if (!iMacError) {
				pOutput->LoadFileSeconds(InfoPBRec.hFileInfo.ioFlMdDat);
			}
			uResult = platform_convert_to_error(iMacError);
		}
	}

	// Return
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
	// Zap it
	pOutput->Clear();

	eError uResult;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->get_FSRef();
	if (pFSRef) {
		uResult = kErrorFileNotFound;
		const char* pNative = pFileName->get_native();
		if (!pNative[0]) {
			FSCatalogInfo MyInfo;
			iMacError = DoGetCatInfo(&MyInfo, pFSRef, kFSCatInfoCreateDate);
			if (!iMacError) {
				// If it succeeded, the file must exist
				pOutput->Load(&MyInfo.createDate);
			}
			uResult = platform_convert_to_error(iMacError);
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		uResult = pFileName->get_FSSpec(&MySpec);
		if (!uResult) {
			iMacError = DoGetCatInfo(&InfoPBRec, &MySpec);
			if (!iMacError) {
				pOutput->LoadFileSeconds(InfoPBRec.hFileInfo.ioFlCrDat);
			}
			uResult = platform_convert_to_error(iMacError);
		}
	}

	// Return
	return uResult;
}

/***************************************

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

uint_t BURGER_API Burger::FileManager::does_file_exist(
	Filename* pFileName) BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	FSRef* pFSRef = pFileName->get_FSRef();
	if (pFSRef) {
		const char* pNative = pFileName->get_native();
		if (!pNative[0]) {
			// If it succeeded, the file must exist
			uResult = TRUE;
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		uResult = pFileName->get_FSSpec(&MySpec);
		if (!uResult) {
			iMacError = DoGetCatInfo(&InfoPBRec, &MySpec);
			if (!iMacError) {
				uResult = TRUE;
			}
		}
	}

	// Return TRUE or FALSE
	return uResult;
}

/***************************************

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

uint32_t BURGER_API Burger::FileManager::get_creator_type(Filename* pFileName)
{
	uint32_t uResult = 0;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->get_FSRef();
	if (pFSRef) {
		const char* pNative = pFileName->get_native();
		if (!pNative[0]) {
			FSCatalogInfo MyInfo;
			iMacError = DoGetCatInfo(&MyInfo, pFSRef, kFSCatInfoFinderInfo);
			if (!iMacError) {
				// If it succeeded, the file must exist
				uResult = reinterpret_cast<const FileInfo*>(&MyInfo.finderInfo)
							  ->fileCreator;
			}
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		if (!pFileName->get_FSSpec(&MySpec)) {
			iMacError = DoGetCatInfo(&InfoPBRec, &MySpec);
			if (!iMacError) {
				// Only files have creator types
				if (!(InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask)) {
					uResult = InfoPBRec.hFileInfo.ioFlFndrInfo.fdCreator;
				}
			}
		}
	}

	// Return 32 bit value
	return uResult;
}

/***************************************

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

uint32_t Burger::FileManager::get_file_type(Filename* pFileName)
{
	uint32_t uResult = 0;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->get_FSRef();
	if (pFSRef) {
		const char* pNative = pFileName->get_native();
		if (!pNative[0]) {
			FSCatalogInfo MyInfo;
			iMacError = DoGetCatInfo(&MyInfo, pFSRef, kFSCatInfoFinderInfo);
			if (!iMacError) {
				// If it succeeded, the file must exist
				uResult = reinterpret_cast<const FileInfo*>(&MyInfo.finderInfo)
							  ->fileType;
			}
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		if (!pFileName->get_FSSpec(&MySpec)) {
			iMacError = DoGetCatInfo(&InfoPBRec, &MySpec);
			if (!iMacError) {
				// Only files have creator types
				if (!(InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask)) {
					uResult = InfoPBRec.hFileInfo.ioFlFndrInfo.fdType;
				}
			}
		}
	}
	// Return 32 bit value
	return uResult;
}

/***************************************

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

Burger::eError Burger::FileManager::get_creator_and_file_type(
	Filename* pFileName, uint32_t* pCreatorType, uint32_t* pFileType)
{
	*pFileType = 0;
	*pCreatorType = 0;
	eError uResult;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->get_FSRef();
	if (pFSRef) {
		uResult = kErrorFileNotFound;
		const char* pNative = pFileName->get_native();
		if (!pNative[0]) {
			FSCatalogInfo MyInfo;
			iMacError = DoGetCatInfo(&MyInfo, pFSRef, kFSCatInfoFinderInfo);
			if (!iMacError) {
				// If it succeeded, the file must exist
				*pFileType =
					reinterpret_cast<const FileInfo*>(&MyInfo.finderInfo)
						->fileType;
				*pCreatorType =
					reinterpret_cast<const FileInfo*>(&MyInfo.finderInfo)
						->fileCreator;
			}
			uResult = platform_convert_to_error(iMacError);
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		uResult = pFileName->get_FSSpec(&MySpec);
		if (!uResult) {
			iMacError = DoGetCatInfo(&InfoPBRec, &MySpec);
			if (!iMacError) {
				// Only files have creator types
				if (!(InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask)) {
					*pFileType = InfoPBRec.hFileInfo.ioFlFndrInfo.fdType;
					*pCreatorType = InfoPBRec.hFileInfo.ioFlFndrInfo.fdCreator;
				}
			}
			uResult = platform_convert_to_error(iMacError);
		}
	}

	// Return error code
	return uResult;
}

/***************************************

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

Burger::eError BURGER_API Burger::FileManager::set_creator_type(
	Filename* pFileName, uint32_t uCreatorType)
{
	eError uResult;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->get_FSRef();
	if (pFSRef) {
		uResult = kErrorFileNotFound;
		const char* pNative = pFileName->get_native();
		if (!pNative[0]) {
			FSRefParam Block;
			FSCatalogInfo MyInfo;
			iMacError =
				DoGetCatInfo(&MyInfo, &Block, pFSRef, kFSCatInfoFinderInfo);
			if (!iMacError) {
				// If it succeeded, modify data
				reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileCreator =
					uCreatorType;
				iMacError = PBSetCatalogInfoSync(&Block);
			}
			uResult = platform_convert_to_error(iMacError);
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		uResult = pFileName->get_FSSpec(&MySpec);
		if (!uResult) {
			// Use a local name buffer for reading and writing to the file
			uint8_t TempBuffer[256];
			iMacError = DoGetCatInfo(&InfoPBRec, TempBuffer, &MySpec);

			// No error? File was found
			if (!iMacError) {

				// Only files have creator types
				if (!(InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask)) {

					// Set the new type
					InfoPBRec.hFileInfo.ioFlFndrInfo.fdCreator = uCreatorType;

					// Restore the Directory ID (Since it was modified)
					InfoPBRec.hFileInfo.ioDirID = pFileName->get_DirID();

					// Save out the new state
					iMacError = PBSetCatInfoSync(&InfoPBRec);

					if (!iMacError) {
						uResult = kErrorNone;
					}
				}
			}
			uResult = platform_convert_to_error(iMacError);
		}
	}

	// Return error code
	return uResult;
}

/***************************************

	\brief Set a file's type code using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename. This function will set the 4 byte code for the
	file.

	On non MacOS platforms, this function will perform nothing.

	\param pFileName Pointer to a \ref Filename of a native OS pathname,
	\param uFileType Four byte character code to set.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero if successful.

	\sa set_file_type(const char*, uint32_t), set_creator_type(
		const char *,uint32_t) or set_creator_and_file_type(
		const char*, uint32_t, uint32_t)

***************************************/

Burger::eError Burger::FileManager::set_file_type(
	Filename* pFileName, uint32_t uFileType)
{
	eError uResult = kErrorFileNotFound;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->get_FSRef();
	if (pFSRef) {
		uResult = kErrorFileNotFound;
		const char* pNative = pFileName->get_native();
		if (!pNative[0]) {
			FSRefParam Block;
			FSCatalogInfo MyInfo;
			iMacError =
				DoGetCatInfo(&MyInfo, &Block, pFSRef, kFSCatInfoFinderInfo);
			if (!iMacError) {
				// If it succeeded, modify data
				reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileType =
					uFileType;
				iMacError = PBSetCatalogInfoSync(&Block);
			}
			uResult = platform_convert_to_error(iMacError);
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		uResult = pFileName->get_FSSpec(&MySpec);
		if (!uResult) {
			// Use a local name buffer for reading and writing to the file
			uint8_t TempBuffer[256];
			iMacError = DoGetCatInfo(&InfoPBRec, TempBuffer, &MySpec);

			// No error? File was found
			if (!iMacError) {

				// Only files have creator types
				if (!(InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask)) {

					// Set the new type
					InfoPBRec.hFileInfo.ioFlFndrInfo.fdType = uFileType;

					// Restore the Directory ID (Since it was modified)
					InfoPBRec.hFileInfo.ioDirID = pFileName->get_DirID();

					// Save out the new state
					iMacError = PBSetCatInfoSync(&InfoPBRec);
				}
			}
			uResult = platform_convert_to_error(iMacError);
		}
	}

	// Return error code
	return uResult;
}

/***************************************

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

Burger::eError Burger::FileManager::set_creator_and_file_type(
	Filename* pFileName, uint32_t uCreatorType, uint32_t uFileType)
{
	eError uResult;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->get_FSRef();
	if (pFSRef) {
		uResult = kErrorFileNotFound;
		const char* pNative = pFileName->get_native();
		if (!pNative[0]) {
			FSRefParam Block;
			FSCatalogInfo MyInfo;
			iMacError =
				DoGetCatInfo(&MyInfo, &Block, pFSRef, kFSCatInfoFinderInfo);

			if (!iMacError) {
				// If it succeeded, modify data
				reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileType =
					uFileType;
				reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileCreator =
					uCreatorType;
				iMacError = PBSetCatalogInfoSync(&Block);
			}
			uResult = platform_convert_to_error(iMacError);
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		uResult = pFileName->get_FSSpec(&MySpec);
		if (!uResult) {
			// Use a local name buffer for reading and writing to the file
			uint8_t TempBuffer[256];
			iMacError = DoGetCatInfo(&InfoPBRec, TempBuffer, &MySpec);

			// No error? File was found
			if (!iMacError) {

				// Only files have creator types
				if (!(InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask)) {

					// Set the new type
					InfoPBRec.hFileInfo.ioFlFndrInfo.fdType = uFileType;
					InfoPBRec.hFileInfo.ioFlFndrInfo.fdCreator = uCreatorType;

					// Restore the Directory ID (Since it was modified)
					InfoPBRec.hFileInfo.ioDirID = pFileName->get_DirID();

					// Save out the new state
					iMacError = PBSetCatInfoSync(&InfoPBRec);
				}
			}
			uResult = platform_convert_to_error(iMacError);
		}
	}

	// Return error code
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

Burger::eError Burger::FileManager::create_directory_path(
	Filename* pFileName) BURGER_NOEXCEPT
{
	// Save the full pathname
	String SavedName(pFileName->c_str());

	eError uResult = kErrorAccessDenied;
	const char* pNative = pFileName->get_native();
	if (pNative) {

		char* pColon;
		uResult = kErrorNone;

		// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		FSRef* pFSRef = pFileName->get_FSRef();
		if (pFSRef) {

			String16 MyName;
			FSRef NewRef;
			FSRef RefCopy;
			FSRef* pNewRef = &NewRef;
			do {
				// Look for a colon in the native name (Means there are multiple
				// segments
				pColon = StringCharacter(pNative, ':');
				if (pColon) {
					// End at a segment
					pColon[0] = 0;
				}

				// Convert the filename to unicode
				uResult = MyName.assign(pNative);
				// Restore the filename
				if (pColon) {
					pColon[0] = ':';
				}
				if (uResult) {
					break;
				}

				// Create a folder
				OSErr iError = FSCreateDirectoryUnicode(pFSRef, MyName.length(),
					MyName.c_str(), kFSCatInfoNone, nullptr, pNewRef, nullptr,
					nullptr);

				// Error?
				if (iError && (iError != dupFNErr)) {
					return kErrorFileNotFound;
				}
				if (!pColon) {
					break;
				}
				pNative = pColon + 1;
				if (pNewRef == &NewRef) {
					pFSRef = pNewRef;
					pNewRef = &RefCopy;
				} else {
					pFSRef = pNewRef;
					pNewRef = &NewRef;
				}
				// Completed?
			} while (pNative[0]);
			// Invalidate the native path since it's created
			pFileName->assign(SavedName.c_str());
		} else
#endif
		{
			// Buffer for text encoding conversion
			uint8_t MacRoman[257];
			// Current directory ID
			long lDirID = pFileName->get_DirID();
			do {
				// Look for a colon in the native name (Means there are multiple
				// segments
				pColon = StringCharacter(pNative, ':');
				if (pColon) {
					// End at a segment
					pColon[0] = 0;
				}

				// Convert the filename to Mac Roman US
				uintptr_t uRomanLength = MacRomanUS::translate_from_UTF8(
					reinterpret_cast<char*>(MacRoman + 1), 256, pNative);
				MacRoman[0] = static_cast<uint8_t>(uRomanLength);
				// Restore the filename
				if (pColon) {
					pColon[0] = ':';
				}

				// Create a folder
				long lNewDirID;
				OSErr iError = DirCreate(
					pFileName->get_VRefNum(), lDirID, MacRoman, &lNewDirID);

				// Error?
				if (iError && (iError != dupFNErr)) {
					return kErrorFileNotFound;
				}
				// Is the end of the segments?
				if (!pColon) {
					break;
				}
				// Skip to the next segment
				pNative = pColon + 1;
				// Update the root folder
				lDirID = lNewDirID;
				// Completed?
			} while (pNative[0]);
			// Invalidate the native path since it's created
			pFileName->assign(SavedName.c_str());
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

Burger::eError Burger::FileManager::delete_file(
	Filename* pFileName) BURGER_NOEXCEPT
{
	eError uResult = kErrorFileNotFound;
	OSErr iMacError = 0;
	const char* pNative = pFileName->get_native();
	if (pNative) {

		// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		FSRef* pFSRef = pFileName->get_FSRef();
		if (pFSRef) {
			if (!pNative[0]) {
				uResult = kErrorNone;
				iMacError = FSDeleteObject(pFSRef);
			}
		} else
#endif
		{
			FSSpec MySpec;
			uResult = pFileName->get_FSSpec(&MySpec);
			if (!uResult) {
				// Delete the file if possible
				iMacError = FSpDelete(&MySpec);
			}
		}
	}

	if (!uResult && iMacError) {
		uResult = platform_convert_to_error(iMacError);
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

Burger::eError Burger::FileManager::rename_file(
	Filename* pNewName, Filename* pOldName)
{
	eError uResult = kErrorFileNotFound;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	FSRef* pFSRef = pOldName->get_FSRef();
	if (pFSRef) {
		const char* pNative = pOldName->get_native();
		if (!pNative[0]) {
			uResult = kErrorFileExists;
			pNative = pNewName->get_native();
			if (pNative[0]) {
				// Convert the filename to unicode
				String16 DestName(pNative);
				iMacError = FSRenameUnicode(pFSRef, DestName.length(),
					DestName.c_str(), kUnicode16BitFormat, nullptr);
				uResult = platform_convert_to_error(iMacError);
			}
		}
	} else
#endif
	{
		// Get the source file FSSpec
		FSSpec MySpec;
		uResult = pOldName->get_FSSpec(&MySpec);
		if (!uResult) {
			// Create the new name
			FSSpec NewSpec;
			pNewName->get_FSSpec(&NewSpec);

			// Rename the file if possible
			iMacError = FSpRename(&MySpec, NewSpec.name);
			uResult = platform_convert_to_error(iMacError);
		}
	}
	return uResult;
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

Burger::eError Burger::FileManager::change_OS_directory(Filename* pDirName)
{

	eError uResult = kErrorFileNotFound;
	const char* pNative = pDirName->get_native();
	if (pNative) {
		// Get the source file FSSpec
		FSSpec MySpec;
		uResult = pDirName->get_FSSpec(&MySpec);
		if (!uResult) {

			// Rename the file if possible
			OSErr iMacError =
				HSetVol(MySpec.name, MySpec.vRefNum, MySpec.parID);
			if (!iMacError) {
				uResult = kErrorNone;
			}
		}
	}

	// Return error!
	return uResult;
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
	int iMacError;
	// If MSL, there's a call for that ;)
	FILE* fp = nullptr;
	if (pType) {

		// Carbon version first
#if !defined(BURGER_68K) && defined(__MSL__)
		FSRef* pFSRef = pFileName->get_FSRef();
		if (pFSRef) {
			iMacError = noErr;
			FSRef TempRef;

			// Check if the FSRef is complete
			const char* pNative = pFileName->get_native();
			if (pNative[0]) {
				iMacError = fnfErr;
				uint_t uTemp = pType[0];
				if (uTemp) {
					String16 MyName(pNative);
					uint_t i = 1;
					do {
						uint_t uTest = uTemp & 0xDF;
						if ((uTest == 'A') || (uTest == 'W')) {
							FSCatalogInfo MyInfo;
							InitFileInfo(reinterpret_cast<FileInfo*>(
								&MyInfo.finderInfo));
							MyInfo.textEncodingHint = kUnicode16BitFormat;

							iMacError = FSCreateFileUnicode(pFSRef,
								MyName.length(), MyName.c_str(),
								kFSCatInfoTextEncoding + kFSCatInfoFinderInfo,
								&MyInfo, &TempRef, nullptr);
							pFSRef = &TempRef;
							break;
						}
						uTemp = pType[i];
						++i;
					} while (uTemp);
				}
			}
			if (!iMacError) {
				// Open using standard fopen using FSRef
				fp = FSRef_fopen(pFSRef, pType);
			}
		} else
#endif
		// All other versions use the old style
		{
			short sSavedVol;
			long lDirID;
			iMacError = HGetVol(0, &sSavedVol, &lDirID);
			if (!iMacError) {
				FSSpec MySpec;
				pFileName->get_FSSpec(&MySpec);
				iMacError = HSetVol(0, MySpec.vRefNum, MySpec.parID);
				if (!iMacError) {
					char CName[256];
					PStringToCString(CName, MySpec.name);
					fp = fopen(CName, pType);
				}
				HSetVol(0, sSavedVol, lDirID);
			}
		}
	}
	return fp;
}

/***************************************

	Copy a file using the Carbon API

***************************************/

#if !(defined(BURGER_CFM) && defined(BURGER_68K))
static int BURGER_API CopyForkCarbon(HFSUniStr255* pForkName, FSRef* pSource,
	FSRef* pDest, uint8_t* pBuffer, uintptr_t uBufferSize)
{
	int iMacError;
	short Srcfp;
	short Destfp;
	iMacError = FSOpenFork(
		pSource, pForkName->length, pForkName->unicode, fsRdPerm, &Srcfp);
	if (!iMacError) {
		iMacError = FSOpenFork(
			pDest, pForkName->length, pForkName->unicode, fsWrPerm, &Destfp);
		if (!iMacError) {
			iMacError =
				Burger::CopyForkClassic(Destfp, Srcfp, pBuffer, uBufferSize);
			FSClose(Destfp);
		}
		FSClose(Srcfp);
	}
	return iMacError;
}
#endif

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
	FSSpec SourceSpec;

	// Get the source FSSpec
	eError uResult = pSourceName->get_FSSpec(&SourceSpec);
	if (!uResult) {
		// Destination is tricky. Break it into the directory and then the file
		String Basename;
		pDestName->get_basename(&Basename);
		String Dirname;
		pDestName->get_dirname(&Dirname);

		// Is there a destination directory?
		Filename TempDest(Dirname.c_str());
		FSSpec DestSpec;
		uResult = TempDest.get_FSSpec(&DestSpec);
		if (!uResult) {
			uint8_t DestName[256];
			CStringToPString(DestName, Basename.c_str());

			// Use the Mac OS low level library to perform the actual copy
			int iMacError =
				FileCopy(&SourceSpec, &DestSpec, DestName, nullptr, 0, FALSE);
			if (iMacError) {
				uResult = kErrorFileNotFound;
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Open a MacOS resource fork

	Using a Burgerlib pathname, open a resource fork of the
	requested MacOS file using a call to HOpenResFile().

	\note This function is only available for Mac OS Carbon/Classic

	\param pFileName Pointer to a Burgerlib file name
	\param uPermission Permission value passed to HOpenResFile()
	\return Error returned by HOpenResFile()

***************************************/

short BURGER_API Burger::FileManager::OpenResourceFile(
	const char* pFileName, char uPermission)
{
	// Open the resource fork
	Filename MyFilename(pFileName);

	FSSpec MySpec;
	MyFilename.get_FSSpec(&MySpec);

	// Open the file
	return HOpenResFile(MySpec.vRefNum, MySpec.parID, MySpec.name, uPermission);
}

/*! ************************************

	\brief Create a Mac OS resource fork

	This will use a generic pathname and creates a Macintosh
	resource file with a call to HCreateResFile().

	\note This function is only available for Mac OS Carbon/Classic

	\param pFileName Pointer to a Burgerlib file name
	\return The value returned by ResError() immediately after the call to
		HCreateResFile()

***************************************/

uint_t BURGER_API Burger::FileManager::CreateResourceFIle(const char* pFileName)
{
	// Create the resource fork
	Filename MyFilename(pFileName);

	FSSpec MySpec;
	MyFilename.get_FSSpec(&MySpec);

	// Open the file
	HCreateResFile(MySpec.vRefNum, MySpec.parID, MySpec.name);
	return ResError();
}

#endif
