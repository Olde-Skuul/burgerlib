/***************************************

	Filename Class

	Mac OS Version

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brdebug.h"
#include "brfile.h"
#include "brglobalmemorymanager.h"
#include "brstring16.h"
#include "brmemoryfunctions.h"
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

/***************************************

	Given a drive number, return in generic format
	the drive's name.

***************************************/

Word BURGER_API Burger::FileManager::GetVolumeName(
	Burger::Filename* pOutput, Word uVolumeNum)
{
	pOutput->Clear();

	FSVolumeInfoParam pb; // Volume information buffer
	MemoryClear(&pb, sizeof(pb));

	HFSUniStr255 Name;	// Unicode name of the volume returned

	// pb.ioVRefNum = kFSInvalidVolumeRefNum;	// I only want the name
	pb.volumeIndex = uVolumeNum + 1; // Drive starts with volume #1
	// pb.whichInfo = kFSVolInfoNone;			// volumeIndex = Drive #
	// pb.volumeInfo = NULL;					// I don't want extra data
	pb.volumeName = &Name; // Name please
	// pb.ref = NULL;							// I don't want the volume's file
	// reference
	if (!PBGetVolumeInfoSync(&pb)) { // Got the data?
		Word uStrLen = Name.length;
		if (uStrLen) {
			// Convert Unicode to a CFString
			char NameUTF8[(256 * 4)
				+ 4]; // Unicode could be as much as 4 bytes per char
			WordPtr uLength = UTF8::FromUTF16(
				NameUTF8 + 1, sizeof(NameUTF8) - 4, Name.unicode, uStrLen * 2);
			NameUTF8[0] = ':';
			NameUTF8[uLength + 1] = ':';
			NameUTF8[uLength + 2] = 0;
			pOutput->Set(NameUTF8);
			return File::OKAY; // No error!
		}
	}
	return File::OUTOFRANGE;
}

/***************************************

	Set the initial default prefixs for a power up state
	*: = Boot volume
	$: = System folder
	@: = Prefs folder
	8: = Default directory
	9: = Application directory

***************************************/

void BURGER_API Burger::FileManager::DefaultPrefixes(void)
{
	// Set the standard work prefix
	Filename MyFilename;
	MyFilename.SetFromNative("");
	SetPrefix(PREFIXCURRENT, MyFilename.GetPtr());

	// Get the boot volume name
	Word uResult = GetVolumeName(&MyFilename, 0);
	if (uResult == File::OKAY) {
		// Set the initial prefix
		SetPrefix(PREFIXBOOT, MyFilename.GetPtr());
	}

	short MyVRef; // Internal volume references
	long MyDirID; // Internal drive ID
	// Get the system folder
	if (!FindFolder(kOnSystemDisk, kSystemFolderType, kDontCreateFolder,
			&MyVRef, &MyDirID)) {
		uResult = MyFilename.SetFromDirectoryID(MyDirID, MyVRef);
		if (!uResult) {
			SetPrefix(PREFIXSYSTEM, MyFilename.GetPtr());
		}
	}

	// Where are the user preferences stored?
	if (!FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder,
			&MyVRef, &MyDirID)) {
		uResult = MyFilename.SetFromDirectoryID(MyDirID, MyVRef);
		if (!uResult) {
			// Set the prefs folder
			SetPrefix(PREFIXPREFS, MyFilename.GetPtr());
		}
	}

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
		uResult = MyFilename.SetFromDirectoryID(
			MyProcess.processAppSpec->parID, MyProcess.processAppSpec->vRefNum);
		if (!uResult) {
			// Application's directory pathname
			SetPrefix(9, MyFilename.GetPtr());
		}
	}
}

/***************************************

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Word Burger::FileManager::GetModificationTime(
	Filename* pFileName, TimeDate_t* pOutput)
{
	pOutput->Clear(); // Zap it

	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr eError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	Word uResult = TRUE;
	if (!eError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoContentMod;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		OSErr eError = PBGetCatalogInfoSync(&Block);
		if (!eError) {
			// If it succeeded, the file must exist
			pOutput->Load(&MyInfo.contentModDate);
			uResult = FALSE;
		}
	}
	return uResult; // Return
}

/***************************************

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

Word Burger::FileManager::GetCreationTime(
	Filename* pFileName, TimeDate_t* pOutput)
{
	pOutput->Clear(); // Zap it

	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr eError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	Word uResult = TRUE;
	if (!eError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoCreateDate;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		OSErr eError = PBGetCatalogInfoSync(&Block);
		if (!eError) {
			// If it succeeded, the file must exist
			pOutput->Load(&MyInfo.createDate);
			uResult = FALSE;
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

Word Burger::FileManager::DoesFileExist(Filename* pFileName)
{
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr eError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	Word uResult = FALSE;
	if (!eError) {
		// If it succeeded, the file must exist
		uResult = TRUE;
	}
	return uResult; // Return
}

/***************************************

	Get a file's Auxtype
	Only valid for GSOS and MacOS

***************************************/

Word32 Burger::FileManager::GetAuxType(Filename* pFileName)
{
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr eError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	Word uResult = 0;
	if (!eError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoFinderInfo;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		OSErr eError = PBGetCatalogInfoSync(&Block);
		if (!eError) {
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

Word32 Burger::FileManager::GetFileType(Filename* pFileName)
{
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr eError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	Word uResult = 0;
	if (!eError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoFinderInfo;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		OSErr eError = PBGetCatalogInfoSync(&Block);
		if (!eError) {
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

Word Burger::FileManager::GetFileAndAuxType(
	Filename* pFileName, Word32* pFileType, Word32* pAuxType)
{
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr eError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	Word uResult = TRUE;
	if (!eError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoFinderInfo;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		OSErr eError = PBGetCatalogInfoSync(&Block);
		if (!eError) {
			// If it succeeded, the file must exist
			pFileType[0] =
				reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileType;
			pAuxType[0] =
				reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileCreator;
			uResult = FALSE;
		}
	}
	return uResult; // Return
}

/***************************************

	Set a file's Auxtype
	Only valid for GSOS and MacOS

***************************************/

Word Burger::FileManager::SetAuxType(Filename* pFileName, Word32 uAuxType)
{
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr eError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	Word uResult = TRUE;
	if (!eError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoFinderInfo;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		OSErr eError = PBGetCatalogInfoSync(&Block);
		if (!eError) {
			// If it succeeded, modify data
			reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileCreator =
				uAuxType;
			eError = PBSetCatalogInfoSync(&Block);
			if (!eError) {
				uResult = FALSE;
			}
		}
	}
	return uResult; // Return
}

/***************************************

	Set a file's Filetype
	Only valid for GSOS and MacOS

***************************************/

Word Burger::FileManager::SetFileType(Filename* pFileName, Word32 uFileType)
{
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr eError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	Word uResult = File::FILENOTFOUND;
	if (!eError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoFinderInfo;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		OSErr eError = PBGetCatalogInfoSync(&Block);
		if (!eError) {
			// If it succeeded, modify data
			reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileType =
				uFileType;
			eError = PBSetCatalogInfoSync(&Block);
			if (!eError) {
				uResult = File::OKAY;
			}
		}
	}
	return uResult; // Return
}

/***************************************

	Set a file's FileType and Auxtype
	Only valid for GSOS and MacOS

***************************************/

Word Burger::FileManager::SetFileAndAuxType(
	Filename* pFileName, Word32 uFileType, Word32 uAuxType)
{
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr eError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	Word uResult = TRUE;
	if (!eError) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoFinderInfo;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		OSErr eError = PBGetCatalogInfoSync(&Block);
		if (!eError) {
			// If it succeeded, modify data
			reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileType =
				uFileType;
			reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileCreator =
				uAuxType;
			eError = PBSetCatalogInfoSync(&Block);
			if (!eError) {
				uResult = FALSE;
			}
		}
	}
	return uResult; // Return
}

/***************************************

	Create a directory path using an operating system native name
	Return FALSE if successful, or TRUE if an error

***************************************/

Word Burger::FileManager::CreateDirectoryPath(Filename* pFileName)
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
		OSErr eError =
			FSCreateDirectoryUnicode(pFileName->GetFSRef(), MyName.GetLength(),
				MyName.GetPtr(), kFSCatInfoNone, NULL, NULL, NULL, NULL);
		// Error?
		if (eError && (eError != dupFNErr)) {
			return File::FILENOTFOUND;
		}
		// Completed?
	} while (pColon);
	return File::OKAY;
}

/***************************************

	Delete a file using native file system

***************************************/

Word Burger::FileManager::DeleteFile(Filename* pFileName)
{
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr eError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	Word uResult = File::FILENOTFOUND;
	if (!eError) {
		eError = FSDeleteObject(&MyRef);
		if (!eError) {
			uResult = File::OKAY;
		}
	}
	return uResult;
}

/***************************************

	Rename a file using native pathnames

***************************************/

Word Burger::FileManager::RenameFile(Filename* pNewName, Filename* pOldName)
{
	// Convert the filename to unicode
	String16 SourceName(pOldName->GetNative());
	FSRef SourceRef;
	// Create a UFT16 FSRef
	OSErr eError =
		FSMakeFSRefUnicode(pNewName->GetFSRef(), SourceName.GetLength(),
			SourceName.GetPtr(), kUnicode16BitFormat, &SourceRef);
	Word uResult = File::FILENOTFOUND;
	if (!eError) {
		// Convert the filename to unicode
		String16 DestName(pNewName->GetNative());
		OSErr eError = FSRenameUnicode(&SourceRef, DestName.GetLength(),
			DestName.GetPtr(), kUnicode16BitFormat, NULL);
		if (!eError) {
			uResult = File::OKAY;
		}
	}
	return uResult;
}

/***************************************

	Change a directory using long filenames
	This only accepts Native OS filenames

***************************************/

Word Burger::FileManager::ChangeOSDirectory(Filename* pDirName)
{
	// Convert the filename to unicode
	String16 MyName(pDirName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr eError = FSMakeFSRefUnicode(pDirName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	Word uResult = File::FILENOTFOUND;
	if (!eError) {
		FSSpec MySpec;
		FSRefParam Block;
		Block.ref = &MyRef;
		Block.whichInfo = kFSCatInfoNone;
		Block.catInfo = NULL;
		Block.spec = &MySpec;
		Block.parentRef = NULL;
		Block.outName = NULL;
		eError = PBGetCatalogInfoSync(&Block);
		if (!eError) {
			eError = HSetVol(MySpec.name, MySpec.vRefNum, MySpec.parID);
			if (!eError) {
				uResult = File::OKAY;
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
	OSErr eError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat, &MyRef);
	if (eError == fnfErr && pType) {
		Word uTemp = pType[0];
		if (uTemp) {
			Word i = 1;
			do {
				Word uTest = uTemp & 0xDF;
				if ((uTest == 'A') || (uTest == 'W')) {
					FSCatalogInfo MyInfo;
					((FileInfo*)(&MyInfo.finderInfo))->fileType = 'BINA';
					((FileInfo*)(&MyInfo.finderInfo))->fileCreator = '????';
					((FileInfo*)(&MyInfo.finderInfo))->finderFlags = 0;
					((FileInfo*)(&MyInfo.finderInfo))->location.h = 0;
					((FileInfo*)(&MyInfo.finderInfo))->location.v = 0;
					((FileInfo*)(&MyInfo.finderInfo))->reservedField = 0;
					MyInfo.textEncodingHint = kUnicode16BitFormat;

					eError = FSCreateFileUnicode(pFileName->GetFSRef(),
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
	if (!eError) {
	#if !defined(BURGER_68K)
		fp = FSRef_fopen(&MyRef, pType); // Open using standard fopen
	#endif
	}
#else
	// MPW StdC, I have to change the working directory
	short sSavedVol;
	long lDirID;
	StrFileName Name;
	OSErr eErr = HGetVol(Name, &sSavedVol, &lDirID);
	if (eErr == noErr) {
		// Get the FSSpec of the file
		pFileName->GetNative(); // Expand a filename
		// Calling HSetVol sucks. Try to avoid it.
		if ((pFileName->GetVRefNum() == sSavedVol)
			&& (pFileName->GetDirID() == lDirID)) {
			fp = fopen(pFileName->GetNative(), pType);
		} else {
			eErr = HSetVol(0, pFileName->GetVRefNum(), pFileName->GetDirID());
			if (eErr == noErr) {
				fp = fopen(pFileName->GetPtr(), pType);
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

static Word CopyFork(short f1, short f2, Word8* pBuffer)
{
	SInt64 lFileSize;
	OSErr eError = FSGetForkSize(f1, &lFileSize);
	if (!eError) {
		if (lFileSize) { /* Shall I copy anything? */
			SInt64 lOffset = 0;
			do {
				Word32 Chunk;
				if (lFileSize > 65536) {
					Chunk = 65536; /* Only copy the chunk */
				} else {
					Chunk = static_cast<Word32>(lFileSize);
				}
				eError =
					FSReadFork(f1, fsFromStart, lOffset, Chunk, pBuffer, NULL);
				if (eError) { /* Read data */
					break;
				}
				eError = FSWriteFork(f2, fsFromStart, lOffset, Chunk, pBuffer,
					NULL); /* Write data */
				if (eError) {
					break;
				}
				lFileSize -= Chunk;
				lOffset += Chunk;
			} while (lFileSize); /* Any data left? */
		}
	}
	return eError;
}

/***************************************

	Copy a file using native pathnames

***************************************/

Word BURGER_API Burger::FileManager::CopyFile(
	Filename* pDestName, Filename* pSourceName)
{
	Word uResult = File::IOERROR; /* Assume error */
	Word8* pBuffer = static_cast<Word8*>(Burger::Alloc(65536));
	if (pBuffer) {
		// Convert the filename to unicode
		String16 SourceName(pSourceName->GetNative());
		FSRef SrcRef;
		// Create a UFT16 FSRef
		OSErr eError =
			FSMakeFSRefUnicode(pSourceName->GetFSRef(), SourceName.GetLength(),
				SourceName.GetPtr(), kUnicode16BitFormat, &SrcRef);
		if (!eError) {
			String16 DestName(pDestName->GetNative());
			FSRef DestRef;
			// Create a UFT16 FSRef
			eError =
				FSMakeFSRefUnicode(pDestName->GetFSRef(), DestName.GetLength(),
					DestName.GetPtr(), kUnicode16BitFormat, &DestRef);
			if (eError == fnfErr) {
				FSCatalogInfo MyInfo;
				((FileInfo*)(&MyInfo.finderInfo))->fileType = 'BINA';
				((FileInfo*)(&MyInfo.finderInfo))->fileCreator = '????';
				((FileInfo*)(&MyInfo.finderInfo))->finderFlags = 0;
				((FileInfo*)(&MyInfo.finderInfo))->location.h = 0;
				((FileInfo*)(&MyInfo.finderInfo))->location.v = 0;
				((FileInfo*)(&MyInfo.finderInfo))->reservedField = 0;
				MyInfo.textEncodingHint = kUnicode16BitFormat;

				eError = FSCreateFileUnicode(pDestName->GetFSRef(),
					DestName.GetLength(), DestName.GetPtr(),
					kFSCatInfoTextEncoding + kFSCatInfoFinderInfo, &MyInfo,
					&DestRef, NULL);
			}
			if (!eError) {
				HFSUniStr255 ForkName;
				short Srcfp;
				short Destfp;
				FSGetDataForkName(&ForkName);

				if (!FSOpenFork(&SrcRef, ForkName.length, ForkName.unicode,
						fsRdPerm, &Srcfp)) {
					if (!FSOpenFork(&DestRef, ForkName.length, ForkName.unicode,
							fsWrPerm, &Destfp)) {
						if (!CopyFork(Srcfp, Destfp, pBuffer)) {
							uResult = File::OKAY;
						}
						if (FSClose(Destfp)) {
							uResult = File::IOERROR;
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
								uResult = File::OKAY;
							}
							if (FSClose(Destfp)) {
								uResult = File::IOERROR;
							}
						}
						FSClose(Srcfp);
					}

					/* If still ok, then copy the finder's data */
				}
				if (!uResult) {
					// uResult = TRUE;
					FSCatalogInfo MyInfo;
					OSErr eError = FSGetCatalogInfo(&SrcRef,
						kFSCatInfoFinderInfo, &MyInfo, NULL, NULL, NULL);
					if (!eError) {
						eError = FSSetCatalogInfo(
							&DestRef, kFSCatInfoFinderInfo, &MyInfo);
						if (!eError) {
							// Data was updated!
							uResult = File::OKAY;
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
	Word8 NameBuffer[256];
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

Word BURGER_API Burger::FileManager::CreateResourceFIle(const char* pFileName)
{
	Word8 NameBuffer[256];

	// Create the directories
	Filename MyFilename(pFileName);
	CStringToPString(NameBuffer, MyFilename.GetNative());
	// Open the file
	HCreateResFile(MyFilename.GetVRefNum(), MyFilename.GetDirID(), NameBuffer);
	return ResError();
}

#endif
