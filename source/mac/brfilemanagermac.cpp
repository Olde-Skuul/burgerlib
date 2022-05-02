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

	\sa MSDOS_HasLongFilenames()

***************************************/

uint_t BURGER_API Burger::FileManager::IsUTF8FileSystem(void)
{
	// MacOS 9 or higher supports the HFSStr calls.
	return Globals::GetMacOSVersion() >= 0x1000;
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
				uintptr_t uLength = UTF8::FromUTF16(
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

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Burger::eError Burger::FileManager::GetModificationTime(
	Filename* pFileName, TimeDate_t* pOutput) BURGER_NOEXCEPT
{
	// Zap it
	pOutput->Clear();

	eError uResult;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->GetFSRef();
	if (pFSRef) {
		uResult = kErrorFileNotFound;
		const char* pNative = pFileName->GetNative();
		if (!pNative[0]) {
			FSCatalogInfo MyInfo;
			iMacError = DoGetCatInfo(&MyInfo, pFSRef, kFSCatInfoContentMod);
			if (!iMacError) {
				// If it succeeded, the file must exist
				pOutput->Load(&MyInfo.contentModDate);
			}
			uResult = MacConvertError(iMacError);
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		uResult = pFileName->GetFSSpec(&MySpec);
		if (!uResult) {
			iMacError = DoGetCatInfo(&InfoPBRec, &MySpec);
			if (!iMacError) {
				pOutput->LoadFileSeconds(InfoPBRec.hFileInfo.ioFlMdDat);
			}
			uResult = MacConvertError(iMacError);
		}
	}

	// Return
	return uResult;
}

/***************************************

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Burger::eError Burger::FileManager::GetCreationTime(
	Filename* pFileName, TimeDate_t* pOutput) BURGER_NOEXCEPT
{
	// Zap it
	pOutput->Clear();

	eError uResult;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->GetFSRef();
	if (pFSRef) {
		uResult = kErrorFileNotFound;
		const char* pNative = pFileName->GetNative();
		if (!pNative[0]) {
			FSCatalogInfo MyInfo;
			iMacError = DoGetCatInfo(&MyInfo, pFSRef, kFSCatInfoCreateDate);
			if (!iMacError) {
				// If it succeeded, the file must exist
				pOutput->Load(&MyInfo.createDate);
			}
			uResult = MacConvertError(iMacError);
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		uResult = pFileName->GetFSSpec(&MySpec);
		if (!uResult) {
			iMacError = DoGetCatInfo(&InfoPBRec, &MySpec);
			if (!iMacError) {
				pOutput->LoadFileSeconds(InfoPBRec.hFileInfo.ioFlCrDat);
			}
			uResult = MacConvertError(iMacError);
		}
	}

	// Return
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

uint_t Burger::FileManager::DoesFileExist(Filename* pFileName) BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	FSRef* pFSRef = pFileName->GetFSRef();
	if (pFSRef) {
		const char* pNative = pFileName->GetNative();
		if (!pNative[0]) {
			// If it succeeded, the file must exist
			uResult = TRUE;
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		uResult = pFileName->GetFSSpec(&MySpec);
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

	Get a file's Auxtype
	Only valid for GSOS and MacOS

***************************************/

uint32_t Burger::FileManager::GetAuxType(Filename* pFileName)
{
	uint32_t uResult = 0;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->GetFSRef();
	if (pFSRef) {
		const char* pNative = pFileName->GetNative();
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
		if (!pFileName->GetFSSpec(&MySpec)) {
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

	Get a file's Filetype
	Only valid for GSOS and MacOS

***************************************/

uint32_t Burger::FileManager::GetFileType(Filename* pFileName)
{
	uint32_t uResult = 0;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->GetFSRef();
	if (pFSRef) {
		const char* pNative = pFileName->GetNative();
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
		if (!pFileName->GetFSSpec(&MySpec)) {
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

	Get a file's Auxtype
	Only valid for GSOS and MacOS

***************************************/

Burger::eError Burger::FileManager::GetFileAndAuxType(
	Filename* pFileName, uint32_t* pFileType, uint32_t* pAuxType)
{
	*pFileType = 0;
	*pAuxType = 0;
	eError uResult;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->GetFSRef();
	if (pFSRef) {
		uResult = kErrorFileNotFound;
		const char* pNative = pFileName->GetNative();
		if (!pNative[0]) {
			FSCatalogInfo MyInfo;
			iMacError = DoGetCatInfo(&MyInfo, pFSRef, kFSCatInfoFinderInfo);
			if (!iMacError) {
				// If it succeeded, the file must exist
				*pFileType =
					reinterpret_cast<const FileInfo*>(&MyInfo.finderInfo)
						->fileType;
				*pAuxType =
					reinterpret_cast<const FileInfo*>(&MyInfo.finderInfo)
						->fileCreator;
			}
			uResult = MacConvertError(iMacError);
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		uResult = pFileName->GetFSSpec(&MySpec);
		if (!uResult) {
			iMacError = DoGetCatInfo(&InfoPBRec, &MySpec);
			if (!iMacError) {
				// Only files have creator types
				if (!(InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask)) {
					*pFileType = InfoPBRec.hFileInfo.ioFlFndrInfo.fdType;
					*pAuxType = InfoPBRec.hFileInfo.ioFlFndrInfo.fdCreator;
				}
			}
			uResult = MacConvertError(iMacError);
		}
	}

	// Return error code
	return uResult;
}

/***************************************

	Set a file's Auxtype
	Only valid for GSOS and MacOS

***************************************/

Burger::eError Burger::FileManager::SetAuxType(
	Filename* pFileName, uint32_t uAuxType)
{
	eError uResult;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->GetFSRef();
	if (pFSRef) {
		uResult = kErrorFileNotFound;
		const char* pNative = pFileName->GetNative();
		if (!pNative[0]) {
			FSRefParam Block;
			FSCatalogInfo MyInfo;
			iMacError =
				DoGetCatInfo(&MyInfo, &Block, pFSRef, kFSCatInfoFinderInfo);
			if (!iMacError) {
				// If it succeeded, modify data
				reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileCreator =
					uAuxType;
				iMacError = PBSetCatalogInfoSync(&Block);
			}
			uResult = MacConvertError(iMacError);
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		uResult = pFileName->GetFSSpec(&MySpec);
		if (!uResult) {
			// Use a local name buffer for reading and writing to the file
			uint8_t TempBuffer[256];
			iMacError = DoGetCatInfo(&InfoPBRec, TempBuffer, &MySpec);

			// No error? File was found
			if (!iMacError) {

				// Only files have creator types
				if (!(InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask)) {

					// Set the new type
					InfoPBRec.hFileInfo.ioFlFndrInfo.fdCreator = uAuxType;

					// Restore the Directory ID (Since it was modified)
					InfoPBRec.hFileInfo.ioDirID = pFileName->GetDirID();

					// Save out the new state
					iMacError = PBSetCatInfoSync(&InfoPBRec);

					if (!iMacError) {
						uResult = kErrorNone;
					}
				}
			}
			uResult = MacConvertError(iMacError);
		}
	}

	// Return error code
	return uResult;
}

/***************************************

	Set a file's Filetype
	Only valid for GSOS and MacOS

***************************************/

Burger::eError Burger::FileManager::SetFileType(
	Filename* pFileName, uint32_t uFileType)
{
	eError uResult = kErrorFileNotFound;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->GetFSRef();
	if (pFSRef) {
		uResult = kErrorFileNotFound;
		const char* pNative = pFileName->GetNative();
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
			uResult = MacConvertError(iMacError);
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		uResult = pFileName->GetFSSpec(&MySpec);
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
					InfoPBRec.hFileInfo.ioDirID = pFileName->GetDirID();

					// Save out the new state
					iMacError = PBSetCatInfoSync(&InfoPBRec);
				}
			}
			uResult = MacConvertError(iMacError);
		}
	}

	// Return error code
	return uResult;
}

/***************************************

	Set a file's FileType and Auxtype
	Only valid for GSOS and MacOS

***************************************/

Burger::eError Burger::FileManager::SetFileAndAuxType(
	Filename* pFileName, uint32_t uFileType, uint32_t uAuxType)
{
	eError uResult;
	int iMacError;

	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Use the Carbon version?
	FSRef* pFSRef = pFileName->GetFSRef();
	if (pFSRef) {
		uResult = kErrorFileNotFound;
		const char* pNative = pFileName->GetNative();
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
					uAuxType;
				iMacError = PBSetCatalogInfoSync(&Block);
			}
			uResult = MacConvertError(iMacError);
		}
	} else
#endif
	{
		CInfoPBRec InfoPBRec;
		FSSpec MySpec;
		uResult = pFileName->GetFSSpec(&MySpec);
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
					InfoPBRec.hFileInfo.ioFlFndrInfo.fdCreator = uAuxType;

					// Restore the Directory ID (Since it was modified)
					InfoPBRec.hFileInfo.ioDirID = pFileName->GetDirID();

					// Save out the new state
					iMacError = PBSetCatInfoSync(&InfoPBRec);
				}
			}
			uResult = MacConvertError(iMacError);
		}
	}

	// Return error code
	return uResult;
}

/***************************************

	Create a directory path using an operating system native name
	Return FALSE if successful, or TRUE if an error

***************************************/

Burger::eError Burger::FileManager::CreateDirectoryPath(
	Filename* pFileName) BURGER_NOEXCEPT
{
	// Save the full pathname
	String SavedName(pFileName->c_str());

	eError uResult = kErrorAccessDenied;
	const char* pNative = pFileName->GetNative();
	if (pNative) {

		char* pColon;
		uResult = kErrorNone;

		// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		FSRef* pFSRef = pFileName->GetFSRef();
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
				uResult = MyName.Set(pNative);
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
			long lDirID = pFileName->GetDirID();
			do {
				// Look for a colon in the native name (Means there are multiple
				// segments
				pColon = StringCharacter(pNative, ':');
				if (pColon) {
					// End at a segment
					pColon[0] = 0;
				}

				// Convert the filename to Mac Roman US
				uintptr_t uRomanLength = MacRomanUS::TranslateFromUTF8(
					reinterpret_cast<char*>(MacRoman + 1), 256, pNative);
				MacRoman[0] = static_cast<uint8_t>(uRomanLength);
				// Restore the filename
				if (pColon) {
					pColon[0] = ':';
				}

				// Create a folder
				long lNewDirID;
				OSErr iError = DirCreate(
					pFileName->GetVRefNum(), lDirID, MacRoman, &lNewDirID);

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

	Delete a file using native file system

***************************************/

Burger::eError Burger::FileManager::DeleteFile(
	Filename* pFileName) BURGER_NOEXCEPT
{
	eError uResult = kErrorFileNotFound;
	OSErr iMacError = 0;
	const char* pNative = pFileName->GetNative();
	if (pNative) {

		// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		FSRef* pFSRef = pFileName->GetFSRef();
		if (pFSRef) {
			if (!pNative[0]) {
				uResult = kErrorNone;
				iMacError = FSDeleteObject(pFSRef);
			}
		} else
#endif
		{
			FSSpec MySpec;
			uResult = pFileName->GetFSSpec(&MySpec);
			if (!uResult) {
				// Delete the file if possible
				iMacError = FSpDelete(&MySpec);
			}
		}
	}

	if (!uResult && iMacError) {
		uResult = MacConvertError(iMacError);
	}
	return uResult;
}

/***************************************

	Rename a file using native pathnames

***************************************/

Burger::eError Burger::FileManager::RenameFile(
	Filename* pNewName, Filename* pOldName)
{
	eError uResult = kErrorFileNotFound;
	int iMacError;
	
	// Carbon version first
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	FSRef* pFSRef = pOldName->GetFSRef();
	if (pFSRef) {
		const char *pNative = pOldName->GetNative();
		if (!pNative[0]) {
			uResult = kErrorFileExists;
			pNative = pNewName->GetNative();
			if (pNative[0]) {
				// Convert the filename to unicode
				String16 DestName(pNative);
				iMacError = FSRenameUnicode(pFSRef, DestName.length(),
					DestName.c_str(), kUnicode16BitFormat, nullptr);
				uResult = MacConvertError(iMacError);
			}
		}
	} else
#endif
	{
		// Get the source file FSSpec
		FSSpec MySpec;
		uResult = pOldName->GetFSSpec(&MySpec);
		if (!uResult) {
			// Create the new name
			FSSpec NewSpec;
			pNewName->GetFSSpec(&NewSpec);

			// Rename the file if possible
			iMacError = FSpRename(&MySpec, NewSpec.name);
			uResult = MacConvertError(iMacError);
		}
	}
	return uResult;
}

/***************************************

	Change a directory using long filenames
	This only accepts Native OS filenames

***************************************/

Burger::eError Burger::FileManager::ChangeOSDirectory(Filename* pDirName)
{

	eError uResult = kErrorFileNotFound;
	const char* pNative = pDirName->GetNative();
	if (pNative) {
		// Get the source file FSSpec
		FSSpec MySpec;
		uResult = pDirName->GetFSSpec(&MySpec);
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

	Open a file using a native path

***************************************/

FILE* BURGER_API Burger::FileManager::OpenFile(
	Filename* pFileName, const char* pType) BURGER_NOEXCEPT
{
	int iMacError;
	// If MSL, there's a call for that ;)
	FILE* fp = nullptr;
	if (pType) {

		// Carbon version first
#if !defined(BURGER_68K) && defined(__MSL__)
		FSRef* pFSRef = pFileName->GetFSRef();
		if (pFSRef) {
			iMacError = noErr;
			FSRef TempRef;
			
			// Check if the FSRef is complete
			const char* pNative = pFileName->GetNative();
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
							InitFileInfo(reinterpret_cast<FileInfo*>(&MyInfo.finderInfo));
							MyInfo.textEncodingHint = kUnicode16BitFormat;

							iMacError = FSCreateFileUnicode(pFSRef,
								MyName.length(), MyName.c_str(),
								kFSCatInfoTextEncoding +
									kFSCatInfoFinderInfo,
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
				pFileName->GetFSSpec(&MySpec);
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

	Copy a file using native pathnames

***************************************/

Burger::eError BURGER_API Burger::FileManager::CopyFile(
	Filename* pDestName, Filename* pSourceName) BURGER_NOEXCEPT
{
	FSSpec SourceSpec;

	// Get the source FSSpec
	eError uResult = pSourceName->GetFSSpec(&SourceSpec);
	if (!uResult) {
		// Destination is tricky. Break it into the directory and then the file
		String Basename;
		pDestName->get_basename(&Basename);
		String Dirname;
		pDestName->get_dirname(&Dirname);

		// Is there a destination directory?
		Filename TempDest(Dirname.c_str());
		FSSpec DestSpec;
		uResult = TempDest.GetFSSpec(&DestSpec);
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
	MyFilename.GetFSSpec(&MySpec);

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
	MyFilename.GetFSSpec(&MySpec);

	// Open the file
	HCreateResFile(MySpec.vRefNum, MySpec.parID, MySpec.name);
	return ResError();
}

#endif
