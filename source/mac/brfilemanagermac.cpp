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
	return Globals::GetMacOSVersion() >= 0x810;
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

	// Volume information buffer
	FSVolumeInfoParam pb;
	MemoryClear(&pb, sizeof(pb));

	// Unicode name of the volume returned
	HFSUniStr255 Name;
	FSVolumeInfo Info;
	MemoryClear(&Info, sizeof(Info));
	// pb.ioVRefNum = kFSInvalidVolumeRefNum;	// Want the name
	pb.volumeIndex = uVolumeNum + 1;    // Drive starts with volume #1
	pb.whichInfo = kFSVolInfoDriveInfo; // volumeIndex = Drive #
	pb.volumeInfo = &Info;              // I don't want extra data
	pb.volumeName = &Name;              // Name please
	// pb.ref = nullptr;				// I don't want the volume's
	// file reference

	// Got the data?
	OSErr iError = PBGetVolumeInfoSync(&pb);
	if (!iError) {
		uint_t uStrLen = Name.length;
		if (uStrLen) {
			if (pOutput) {
				// Convert Unicode to a CFString
				// Unicode could be as much as 4 bytes per char
				char NameUTF8[(256 * 4) + 4];
				uintptr_t uLength = UTF8::FromUTF16(NameUTF8 + 1,
					sizeof(NameUTF8) - 4, Name.unicode, uStrLen * 2);
				NameUTF8[0] = ':';
				NameUTF8[uLength + 1] = ':';
				NameUTF8[uLength + 2] = 0;
				pOutput->Set(NameUTF8);
				pOutput->SetVRefNum(static_cast<int16_t>(Info.driveNumber));
				pOutput->SetDirID(0);
			}
			// No error!
			uResult = kErrorNone;
		}

		// If the call failed, it's possible the version of MacOS is 7.5 instead
		// of 9.0 or higher.
	} else if (iError == paramErr) {

		// Note: The volume returned will be in MacRoman encoding
		Str31 DriveName;
		HVolumeParam hpb;
		hpb.ioNamePtr = DriveName;
		hpb.ioVRefNum = 0;
		hpb.filler2 = 0;
		hpb.ioVolIndex = static_cast<short>(uVolumeNum + 1);
		OSStatus uStatus = PBHGetVInfoSync((HParamBlockRec*)&hpb);
		if (!uStatus) {
			if (pOutput) {
				char NameMacRomanUS[64];
				PStringToCString(NameMacRomanUS + 1, DriveName);
				uintptr_t uLength7 = DriveName[0];
				NameMacRomanUS[0] = ':';
				NameMacRomanUS[uLength7 + 1] = ':';
				NameMacRomanUS[uLength7 + 2] = 0;
				pOutput->Set(NameMacRomanUS);
				pOutput->SetVRefNum(static_cast<int16_t>(hpb.ioVRefNum));
				pOutput->SetDirID(0);
			}
			// No error!
			uResult = kErrorNone;
		}
	}

	// Clear on error
	if ((uResult != kErrorNone) && pOutput) {
		// Kill the string since I have an error
		pOutput->Clear();
	}
	return uResult;
}

/***************************************

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Burger::eError Burger::FileManager::GetModificationTime(
	Filename* pFileName, TimeDate_t* pOutput)
{
	pOutput->Clear(); // Zap it

	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr iError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	eError uResult = kErrorNotSupportedOnThisPlatform;
	if (!iError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoContentMod;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		iError = PBGetCatalogInfoSync(&Block);
		if (!iError) {
			// If it succeeded, the file must exist
			pOutput->Load(&MyInfo.contentModDate);
			uResult = kErrorNone;
		}
	}
	return uResult; // Return
}

/***************************************

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Burger::eError Burger::FileManager::GetCreationTime(
	Filename* pFileName, TimeDate_t* pOutput)
{
	pOutput->Clear(); // Zap it

	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr iError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	eError uResult = kErrorReadFailure;
	if (!iError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoCreateDate;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		iError = PBGetCatalogInfoSync(&Block);
		if (!iError) {
			// If it succeeded, the file must exist
			pOutput->Load(&MyInfo.createDate);
			uResult = kErrorNone;
		}
	}
	return uResult; // Return
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
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr iError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	uint_t uResult = FALSE;
	if (!iError) {
		// If it succeeded, the file must exist
		uResult = TRUE;
	}
	return uResult; // Return
}

/***************************************

	Get a file's Auxtype
	Only valid for GSOS and MacOS

***************************************/

uint32_t Burger::FileManager::GetAuxType(Filename* pFileName)
{
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr iError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	uint_t uResult = 0;
	if (!iError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoFinderInfo;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		iError = PBGetCatalogInfoSync(&Block);
		if (!iError) {
			// If it succeeded, the file must exist
			uResult =
				reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileCreator;
		}
	}
	return uResult; // Return
}

/***************************************

	Get a file's Filetype
	Only valid for GSOS and MacOS

***************************************/

uint32_t Burger::FileManager::GetFileType(Filename* pFileName)
{
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr iError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	uint_t uResult = 0;
	if (!iError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoFinderInfo;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		iError = PBGetCatalogInfoSync(&Block);
		if (!iError) {
			// If it succeeded, the file must exist
			uResult = reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileType;
		}
	}
	return uResult; // Return
}

/***************************************

	Get a file's Auxtype
	Only valid for GSOS and MacOS

***************************************/

Burger::eError Burger::FileManager::GetFileAndAuxType(
	Filename* pFileName, uint32_t* pFileType, uint32_t* pAuxType)
{
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr iError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	eError uResult = kErrorFileNotFound;
	if (!iError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoFinderInfo;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		iError = PBGetCatalogInfoSync(&Block);
		if (!iError) {
			// If it succeeded, the file must exist
			pFileType[0] =
				reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileType;
			pAuxType[0] =
				reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileCreator;
			uResult = kErrorNone;
		}
	}
	return uResult; // Return
}

/***************************************

	Set a file's Auxtype
	Only valid for GSOS and MacOS

***************************************/

Burger::eError Burger::FileManager::SetAuxType(
	Filename* pFileName, uint32_t uAuxType)
{
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr iError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	eError uResult = kErrorFileNotFound;
	if (!iError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoFinderInfo;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		iError = PBGetCatalogInfoSync(&Block);
		if (!iError) {
			// If it succeeded, modify data
			reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileCreator =
				uAuxType;
			iError = PBSetCatalogInfoSync(&Block);
			if (!iError) {
				uResult = kErrorNone;
			}
		}
	}
	return uResult; // Return
}

/***************************************

	Set a file's Filetype
	Only valid for GSOS and MacOS

***************************************/

Burger::eError Burger::FileManager::SetFileType(
	Filename* pFileName, uint32_t uFileType)
{
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr iError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	eError uResult = kErrorFileNotFound;
	if (!iError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoFinderInfo;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		iError = PBGetCatalogInfoSync(&Block);
		if (!iError) {
			// If it succeeded, modify data
			reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileType =
				uFileType;
			iError = PBSetCatalogInfoSync(&Block);
			if (!iError) {
				uResult = kErrorNone;
			}
		}
	}
	return uResult; // Return
}

/***************************************

	Set a file's FileType and Auxtype
	Only valid for GSOS and MacOS

***************************************/

Burger::eError Burger::FileManager::SetFileAndAuxType(
	Filename* pFileName, uint32_t uFileType, uint32_t uAuxType)
{
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr iError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	eError uResult = kErrorFileNotFound;
	if (!iError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoFinderInfo;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		iError = PBGetCatalogInfoSync(&Block);
		if (!iError) {
			// If it succeeded, modify data
			reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileType =
				uFileType;
			reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileCreator =
				uAuxType;
			iError = PBSetCatalogInfoSync(&Block);
			if (!iError) {
				uResult = kErrorNone;
			}
		}
	}
	return uResult; // Return
}

/***************************************

	Create a directory path using an operating system native name
	Return FALSE if successful, or TRUE if an error

***************************************/

Burger::eError Burger::FileManager::CreateDirectoryPath(Filename* pFileName)
{
	char* pColon;
	do {
		const char* pNative = pFileName->GetNative();
		// Look for a colon in the native name (Means there are multiple
		// segments
		pColon = StringCharacter(const_cast<char*>(pNative), ':');
		if (pColon) {
			// End at a segment
			pColon[0] = 0;
		}
		// Convert the filename to unicode
		String16 MyName(pNative);
		// Restore the filename
		if (pColon) {
			pColon[0] = ':';
		}
		// Create a folder
		OSErr iError =
			FSCreateDirectoryUnicode(pFileName->GetFSRef(), MyName.GetLength(),
				MyName.GetPtr(), kFSCatInfoNone, NULL, NULL, NULL, NULL);
		// Error?
		if (iError && (iError != dupFNErr)) {
			return kErrorFileNotFound;
		}
		// Completed?
	} while (pColon);
	return kErrorNone;
}

/***************************************

	Delete a file using native file system

***************************************/

Burger::eError Burger::FileManager::DeleteFile(
	Filename* pFileName) BURGER_NOEXCEPT
{
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr iError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	eError uResult = kErrorFileNotFound;
	if (!iError) {
		iError = FSDeleteObject(&MyRef);
		if (!iError) {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	Rename a file using native pathnames

***************************************/

Burger::eError Burger::FileManager::RenameFile(
	Filename* pNewName, Filename* pOldName)
{
	// Convert the filename to unicode
	String16 SourceName(pOldName->GetNative());
	FSRef SourceRef;
	// Create a UFT16 FSRef
	OSErr iError =
		FSMakeFSRefUnicode(pNewName->GetFSRef(), SourceName.GetLength(),
			SourceName.GetPtr(), kUnicode16BitFormat, &SourceRef);
	eError uResult = kErrorFileNotFound;
	if (!iError) {
		// Convert the filename to unicode
		String16 DestName(pNewName->GetNative());
		iError = FSRenameUnicode(&SourceRef, DestName.GetLength(),
			DestName.GetPtr(), kUnicode16BitFormat, NULL);
		if (!iError) {
			uResult = kErrorNone;
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
	// Convert the filename to unicode
	String16 MyName(pDirName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr iError = FSMakeFSRefUnicode(pDirName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	eError uResult = kErrorFileNotFound;
	if (!iError) {
		FSSpec MySpec;
		FSRefParam Block;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoNone;
		Block.catInfo = NULL;
		Block.spec = &MySpec;
		Block.parentRef = NULL;
		Block.outName = NULL;
		iError = PBGetCatalogInfoSync(&Block);
		if (!iError) {
			iError = HSetVol(MySpec.name, MySpec.vRefNum, MySpec.parID);
			if (!iError) {
				uResult = kErrorNone;
			}
		}
	}
	return uResult; /* Error! */
}

/***************************************

	Open a file using a native path

***************************************/

FILE* BURGER_API Burger::FileManager::OpenFile(
	Filename* pFileName, const char* pType)
{
	// If MSL, there's a call for that ;)
	FILE* fp = NULL;
#if defined(__MSL__)
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr iError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	if (iError == fnfErr && pType) {
		uint_t uTemp = pType[0];
		if (uTemp) {
			uint_t i = 1;
			do {
				uint_t uTest = uTemp & 0xDF;
				if ((uTest == 'A') || (uTest == 'W')) {
					FSCatalogInfo MyInfo;
					((FileInfo*)(&MyInfo.finderInfo))->fileType = 'BINA';
					((FileInfo*)(&MyInfo.finderInfo))->fileCreator = '????';
					((FileInfo*)(&MyInfo.finderInfo))->finderFlags = 0;
					((FileInfo*)(&MyInfo.finderInfo))->location.h = 0;
					((FileInfo*)(&MyInfo.finderInfo))->location.v = 0;
					((FileInfo*)(&MyInfo.finderInfo))->reservedField = 0;
					MyInfo.textEncodingHint = kUnicode16BitFormat;

					iError = FSCreateFileUnicode(pFileName->GetFSRef(),
						MyName.GetLength(), MyName.GetPtr(),
						kFSCatInfoTextEncoding + kFSCatInfoFinderInfo, &MyInfo,
						&MyRef, NULL);
					break;
				}
				uTemp = pType[i];
				++i;
			} while (uTemp);
		}
	}
	if (!iError) {
#if !defined(BURGER_68K)
		fp = FSRef_fopen(&MyRef, pType); // Open using standard fopen
#endif
	}
#else
	// MPW StdC, I have to change the working directory
	short sSavedVol;
	long lDirID;
	StrFileName Name;
	OSErr iError = HGetVol(Name, &sSavedVol, &lDirID);
	if (iError == noErr) {
		// Get the FSSpec of the file
		pFileName->GetNative(); // Expand a filename
		// Calling HSetVol sucks. Try to avoid it.
		if ((pFileName->GetVRefNum() == sSavedVol) &&
			(pFileName->GetDirID() == lDirID)) {
			fp = fopen(pFileName->GetNative(), pType);
		} else {
			iError = HSetVol(0, pFileName->GetVRefNum(), pFileName->GetDirID());
			if (iError == noErr) {
				fp = fopen(pFileName->c_str(), pType);
			}
			HSetVol(Name, sSavedVol, lDirID);
		}
	}
#endif
	return fp;
}

/***************************************

	Copy the contents of a fork
	Buffer is assumed to be 64K

***************************************/

static uint_t CopyFork(short f1, short f2, uint8_t* pBuffer)
{
	SInt64 lFileSize;
	OSErr iError = FSGetForkSize(f1, &lFileSize);
	if (!iError) {
		if (lFileSize) { /* Shall I copy anything? */
			SInt64 lOffset = 0;
			do {
				uint32_t Chunk;
				if (lFileSize > 65536) {
					Chunk = 65536; /* Only copy the chunk */
				} else {
					Chunk = static_cast<uint32_t>(lFileSize);
				}
				iError =
					FSReadFork(f1, fsFromStart, lOffset, Chunk, pBuffer, NULL);
				if (iError) { /* Read data */
					break;
				}
				iError = FSWriteFork(f2, fsFromStart, lOffset, Chunk, pBuffer,
					NULL); /* Write data */
				if (iError) {
					break;
				}
				lFileSize -= Chunk;
				lOffset += Chunk;
			} while (lFileSize); /* Any data left? */
		}
	}
	return iError;
}

/***************************************

	Copy a file using native pathnames

***************************************/

Burger::eError BURGER_API Burger::FileManager::CopyFile(
	Filename* pDestName, Filename* pSourceName)
{
	eError uResult = kErrorIO; /* Assume error */
	uint8_t* pBuffer = static_cast<uint8_t*>(Burger::Alloc(65536));
	if (pBuffer) {
		// Convert the filename to unicode
		String16 SourceName(pSourceName->GetNative());
		FSRef SrcRef;
		// Create a UFT16 FSRef
		OSErr iError =
			FSMakeFSRefUnicode(pSourceName->GetFSRef(), SourceName.GetLength(),
				SourceName.GetPtr(), kUnicode16BitFormat, &SrcRef);
		if (!iError) {
			String16 DestName(pDestName->GetNative());
			FSRef DestRef;
			// Create a UFT16 FSRef
			iError =
				FSMakeFSRefUnicode(pDestName->GetFSRef(), DestName.GetLength(),
					DestName.GetPtr(), kUnicode16BitFormat, &DestRef);
			if (iError == fnfErr) {
				FSCatalogInfo MyInfo;
				((FileInfo*)(&MyInfo.finderInfo))->fileType = 'BINA';
				((FileInfo*)(&MyInfo.finderInfo))->fileCreator = '????';
				((FileInfo*)(&MyInfo.finderInfo))->finderFlags = 0;
				((FileInfo*)(&MyInfo.finderInfo))->location.h = 0;
				((FileInfo*)(&MyInfo.finderInfo))->location.v = 0;
				((FileInfo*)(&MyInfo.finderInfo))->reservedField = 0;
				MyInfo.textEncodingHint = kUnicode16BitFormat;

				iError = FSCreateFileUnicode(pDestName->GetFSRef(),
					DestName.GetLength(), DestName.GetPtr(),
					kFSCatInfoTextEncoding + kFSCatInfoFinderInfo, &MyInfo,
					&DestRef, NULL);
			}
			if (!iError) {
				HFSUniStr255 ForkName;
				short Srcfp;
				short Destfp;
				FSGetDataForkName(&ForkName);

				if (!FSOpenFork(&SrcRef, ForkName.length, ForkName.unicode,
						fsRdPerm, &Srcfp)) {
					if (!FSOpenFork(&DestRef, ForkName.length, ForkName.unicode,
							fsWrPerm, &Destfp)) {
						if (!CopyFork(Srcfp, Destfp, pBuffer)) {
							uResult = kErrorNone;
						}
						if (FSClose(Destfp)) {
							uResult = kErrorIO;
						}
					}
					FSClose(Srcfp);
				}

				/* If ok, then copy the resource fork */

				if (!uResult) {
					FSGetResourceForkName(&ForkName);
					if (!FSOpenFork(&SrcRef, ForkName.length, ForkName.unicode,
							fsRdPerm, &Srcfp)) {
						if (!FSOpenFork(&DestRef, ForkName.length,
								ForkName.unicode, fsWrPerm, &Destfp)) {
							if (!CopyFork(Srcfp, Destfp, pBuffer)) {
								uResult = kErrorNone;
							}
							if (FSClose(Destfp)) {
								uResult = kErrorIO;
							}
						}
						FSClose(Srcfp);
					}

					/* If still ok, then copy the finder's data */
				}
				if (!uResult) {
					// uResult = TRUE;
					FSCatalogInfo MyInfo;
					iError = FSGetCatalogInfo(&SrcRef, kFSCatInfoFinderInfo,
						&MyInfo, NULL, NULL, NULL);
					if (!iError) {
						iError = FSSetCatalogInfo(
							&DestRef, kFSCatInfoFinderInfo, &MyInfo);
						if (!iError) {
							// Data was updated!
							uResult = kErrorNone;
							;
						}
					}
				}
			}
		}
		Free(pBuffer); /* Release the copy buffer */
	}
	return uResult; /* Return the end result */
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
	uint8_t NameBuffer[256];
	// Create the directories
	Filename MyFilename(pFileName);
	CStringToPString(NameBuffer, MyFilename.GetNative());
	// Open the file
	return HOpenResFile(MyFilename.GetVRefNum(), MyFilename.GetDirID(),
		NameBuffer, uPermission);
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
	uint8_t NameBuffer[256];

	// Create the directories
	Filename MyFilename(pFileName);
	CStringToPString(NameBuffer, MyFilename.GetNative());
	// Open the file
	HCreateResFile(MyFilename.GetVRefNum(), MyFilename.GetDirID(), NameBuffer);
	return ResError();
}

#endif
