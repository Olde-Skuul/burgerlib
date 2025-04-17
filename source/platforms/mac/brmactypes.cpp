/***************************************

	MacOS specific convenience functions

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brmactypes.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brglobals.h"
#include "brmacromanus.h"
#include "brmemoryfunctions.h"
#include "brstring.h"
#include "brstring16.h"
#include "brstringfunctions.h"
#include "mac_volparms.h"
#include "mac_version.h"
#include "mac_folders.h"

#include <Devices.h>
#include <DrawSprocket.h>
#include <FSM.h>
#include <Folders.h>
#include <Gestalt.h>
#include <HFSVolumes.h>
#include <InputSprocket.h>
#include <LowMem.h>
#include <Navigation.h>
#include <Script.h>
#include <Sound.h>

#if !defined(BURGER_CFM) || !defined(BURGER_MACCARBON)
#include <Traps.h>
#endif

#if defined(BURGER_POWERPC)
#include <Movies.h>
#endif

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === Structure initialization functions ===
#endif

/*! ************************************

	\brief Initialize a Mac Finder FileInfo

	When creating a file on MacOS, a file type and creator code needs to be
	assigned. The default for generic files is file type "BINA" for binary and
	the creator code is "????" for unknown. It's up to the app developer to call
	set_creator_and_file_type() to assign the proper values after the file is
	created or modified.

	\maconly

	\param pFileInfo Pointer to an uninitialized FileInfo to be filled in.

	\sa FileManager::set_creator_and_file_type(const char *, uint32_t, uint32_t)

***************************************/

void BURGER_API Burger::InitFileInfo(FileInfo* pFileInfo) BURGER_NOEXCEPT
{
	pFileInfo->fileType = 'BINA';
	pFileInfo->fileCreator = 0x3F3F3F3F; // ???? kills doxygen
	pFileInfo->finderFlags = 0;
	pFileInfo->location.h = 0;
	pFileInfo->location.v = 0;
	pFileInfo->reservedField = 0;
}

/*! ************************************

	\brief Initialize a Mac FSRefParam

	When calling the Mac function PBGetCatalogInfoSync(), a FSRefParam needs to
	be prepared so only the information queried is returned. This structure is
	large, so this subroutine clears it out quickly. All entries except for
	"ref" and "whichInfo" are cleared to zero. These two values are set to
	the passed values.

	\maconly

	\param pFSRefParam Pointer to an uninitialized FSRefParam to be filled in.
	\param pFSRef Pointer to a valid FSRef of the file of interest.
	\param uCatInfoBitmap Value to put in ``whichInfo``.

***************************************/

void BURGER_API Burger::InitFSRefParam(FSRefParam* pFSRefParam, FSRef* pFSRef,
	uint32_t uCatInfoBitmap) BURGER_NOEXCEPT
{
	// Initialize the record
	MemoryClear(pFSRefParam, sizeof(FSRefParam));
	pFSRefParam->ref = pFSRef;
	pFSRefParam->whichInfo = uCatInfoBitmap;
}

/*! ************************************

	\brief Extract a filename at the end of a MacOS path.

	Scan a Pascal filename string and extract the last component which should be
	the filename. If the string passed in is a volume path, such as
	":Macintosh HD:", it will intentionally fail.

	Do not pass the same pointer for both input and output.

	Error codes returned, ``notAFileErr``, or ``noErr``.

	\maconly

	\param pOutput Pointer to a 256 byte Pascal string buffer
	\param pInput Pointer to the Pascal filename path

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::GetFilenameFromPathname(
	uint8_t* pOutput, const uint8_t* pInput) BURGER_NOEXCEPT
{
	// Initialize the output
	pOutput[0] = 0;

	// Assume failure
	int iMacError = notAFileErr;

	// It's a directory, abort
	if (pInput) {

		// Empty string is a directory again
		uint_t uIndex = pInput[0];
		if (uIndex) {
			// Ignore trailing colons
			if (pInput[uIndex] == ':') {
				--uIndex;
			}

			// Check for a double colon, which is bad.
			if (pInput[uIndex] != ':') {

				// Save the length of the string (Minus the ending colon)
				uint_t uIndexEnd = uIndex;

				// Reverse search for a colon
				while (uIndex && (pInput[uIndex] != ':')) {
					--uIndex;
				}

				// Check for ":Macintosh HD:" which is a volume name, not a
				// filename
				if (uIndex || (pInput[pInput[0]] != ':')) {
					// Get the length of the filename
					uIndexEnd -= uIndex;

					// Sanity check
					if (uIndexEnd > 255U) {
						uIndexEnd = 255U;
					}

					// Create the Pascal filename output
					pOutput[0] = static_cast<uint8_t>(uIndexEnd);
					MemoryCopy(&pOutput[1], &pInput[uIndex + 1], uIndexEnd);

					// We're good!
					iMacError = noErr;
				}
			}
		}
	}
	return iMacError;
}

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === Simple file functions ===
#endif

/*! ************************************

	\brief Find a file location from a file reference number.

	When a file is opened, a file reference number is generated. This function
	will return the volume, directory ID and filename of the opened file by
	calling PBGetFCBInfoSync().

	\maconly

	\param pVRefNum Pointer to variable to receive the volume reference number
	\param pDirID Pointer to variable to receive the file's parent ID.
	\param pOutputFilename Pointer to a 256 byte buffer to receive the Pascal
		filename
	\param iFileRefNum Open file reference ID

	\return Zero if no error, or mac error code

	\sa GetFileLocation(FSRef *, short) or GetFileLocation(FSSpec *, short)

***************************************/

int BURGER_API Burger::GetFileLocation(short* pVRefNum, long* pDirID,
	uint8_t* pOutputFilename, short iFileRefNum) BURGER_NOEXCEPT
{
	// File control block record
	FCBPBRec fbcpbrec;

	// Save the open file reference number
	fbcpbrec.ioRefNum = iFileRefNum;

	// Set the buffer to capture the filename
	fbcpbrec.ioNamePtr = pOutputFilename;
	fbcpbrec.ioVRefNum = 0;
	fbcpbrec.ioFCBIndx = 0;

	// Grab it
	int iMacError = PBGetFCBInfoSync(&fbcpbrec);
	if (!iMacError) {
		// Return the volume reference and the directory ID
		// The filename is already stored in pOutputFilename
		*pVRefNum = fbcpbrec.ioFCBVRefNum;
		*pDirID = fbcpbrec.ioFCBParID;
	}
	return iMacError;
}

/*! ************************************

	\brief Find the FSSpec from a file reference number.

	When a file is opened, a file reference number is generated. This function
	will return the FSSpec of the opened file by calling PBGetFCBInfoSync().

	\maconly

	\param pOutput Pointer to an uninitialized FSSpec
	\param iFileRefNum Open file reference ID

	\return Zero if no error, or mac error code

	\sa GetFileLocation(FSRef* , short) or
		GetFileLocation(short* , long* , uint8_t* , short)

***************************************/

int BURGER_API Burger::GetFileLocation(
	FSSpec* pOutput, short iFileRefNum) BURGER_NOEXCEPT
{
	return GetFileLocation(
		&pOutput->vRefNum, &pOutput->parID, pOutput->name, iFileRefNum);
}

/*! ************************************

	\brief Use an open file reference to return the FSRef

	If a file was opened using an FSRef, use this function to retrieve the
	FSRef from the open file reference by calling FSGetForkCBInfo().

	This function will fail if the application is not running on MacOS 9 or
	higher with a ``paramErr``.

	\maconly

	\param pOutput Pointer to a buffer to receive the FSRef
	\param iFileRefNum Open file reference to query for the FSRef

	\return Zero if no error, or mac error code

	\sa GetFileLocation(FSSpec* ,short) or
		GetFileLocation(short* ,long* ,uint8_t* ,short)

***************************************/

int BURGER_API Burger::GetFileLocation(
	FSRef* pOutput, short iFileRefNum) BURGER_NOEXCEPT
{
	// Not available on 68K CFM
#if defined(BURGER_CFM) && defined(BURGER_68K)
	BURGER_UNUSED(pOutput);
	BURGER_UNUSED(iFileRefNum);
	return paramErr;
#else
	return FSGetForkCBInfo(
		iFileRefNum, 0, nullptr, nullptr, nullptr, pOutput, nullptr);
#endif
}

/*! ************************************

	\brief Call PBHCreateSync() to create a file.

	Call PBHCreateSync() to create an empty file with no data or type.

	\maconly

	\param svRefNum Volume reference number of the new file
	\param lDirID Parent ID of the new file
	\param pFilename Pointer to a Pascal string for the new filename

	\return Zero if no error, or mac error code

	\sa CreateEmptyFile(const FSSpec*)

***************************************/

int BURGER_API Burger::CreateEmptyFile(
	short svRefNum, long lDirID, const uint8_t* pFilename) BURGER_NOEXCEPT
{
	HParamBlockRec pb;

	pb.fileParam.ioNamePtr =
		reinterpret_cast<StringPtr>(const_cast<uint8_t*>(pFilename));
	pb.fileParam.ioVRefNum = svRefNum;
	pb.ioParam.ioVersNum = 0;
	pb.fileParam.ioDirID = lDirID;
	return PBHCreateSync(&pb);
}

/*! ************************************

	\brief Call FSpCreate() to create a file.

	Call FSpCreate() to create an empty file with no data or type.

	\maconly

	\param pFSSpec Pointer to an FSSpec of the new file

	\return Zero if no error, or mac error code

	\sa CreateEmptyFile(short, long, const uint8_t*)

***************************************/

int BURGER_API Burger::CreateEmptyFile(const FSSpec* pFSSpec) BURGER_NOEXCEPT
{
	return FSpCreate(pFSSpec, 0, 0, smSystemScript);
}

/*! ************************************

	\brief Call PBCreateFileUnicodeSync() to create a file.

	Call PBCreateFileUnicodeSync() to create an empty file with no data or type.

	This function always returns ``paramErr`` if running on MacOS 7 or 8.

	\maconly

	\param pOutput Pointer to store the new FSRef, can be nullptr
	\param pFSRef Pointer to an FSRef of the new file's parent directory
	\param pName Pointer to UTF8 encoded "C" string of the new file name

	\return Zero if no error, or mac error code

	\sa CreateEmptyFile(short, long, const uint8_t*)

***************************************/

int BURGER_API Burger::CreateEmptyFile(
	FSRef* pOutput, const FSRef* pFSRef, const char* pName) BURGER_NOEXCEPT
{
	// Not available on 68K CFM
#if defined(BURGER_CFM) && defined(BURGER_68K)
	BURGER_UNUSED(pOutput);
	BURGER_UNUSED(pFSRef);
	BURGER_UNUSED(pName);
	return paramErr;
#else
	// Create the filename
	String16 Name16(pName);

	// File reference block
	FSRefParam Block;
	InitFSRefParam(&Block, const_cast<FSRef*>(pFSRef), kFSCatInfoNone);
	Block.newRef = pOutput;
	Block.name = Name16.c_str();
	Block.nameLength = Name16.length();

	// Perform the file creation
	return PBCreateFileUnicodeSync(&Block);
#endif
}

/*! ************************************

	\brief Test if a file has either a resource or data fork.

	Before copying a file, check if the source has a resource or data fork, so
	only the detected forks will be copied. Return two booleans that are either
	\ref TRUE or \ref FALSE if either fork is detected. PBHGetFInfoSync() is
	used to check the fork sizes.

	\maconly

	\param svRefNum Volume the file resides on
	\param lDirID Directory ID the file resides on
	\param pName Pointer to a PASCAL string of the filename
	\param pbDataFork Pointer to a boolean to receive the presence of a data
		fork.
	\param pbResourceFork Pointer to a boolean to receive the presence of a
		resource fork.

***************************************/

int BURGER_API Burger::CheckForForks(short svRefNum, long lDirID,
	const uint8_t* pName, uint_t* pbDataFork,
	uint_t* pbResourceFork) BURGER_NOEXCEPT
{
	// Use PBHGetFInfoSync() for old school API
	HParamBlockRec hpb;

	hpb.fileParam.ioNamePtr =
		reinterpret_cast<StringPtr>(const_cast<unsigned char*>(pName));
	hpb.fileParam.ioVRefNum = svRefNum;
	hpb.fileParam.ioFVersNum = 0;
	hpb.fileParam.ioDirID = lDirID;
	hpb.fileParam.ioFDirIndex = 0;

	// Read in the file sizes
	int iMacError = PBHGetFInfoSync(&hpb);

	// Gotcha!
	*pbDataFork = (hpb.fileParam.ioFlLgLen != 0);
	*pbResourceFork = (hpb.fileParam.ioFlRLgLen != 0);
	return iMacError;
}

/*! ************************************

	\brief Test if a file has either a resource or data fork.

	Before copying a file, check if the source has a resource or data fork, so
	only the detected forks will be copied. Return two booleans that are either
	\ref TRUE or \ref FALSE if either fork is detected. PBHGetFInfoSync() is
	used to check the fork sizes.

	\maconly

	\param pFSSpec Pointer to the FSSpec of the file of interest.
	\param pbDataFork Pointer to a boolean to receive the presence of a data
		fork.
	\param pbResourceFork Pointer to a boolean to receive the presence of a
		resource fork.

	\sa CheckForForks(short, long, const uint8_t *, uint_t *, uint_t *)

***************************************/

int BURGER_API Burger::CheckForForks(const FSSpec* pFSSpec, uint_t* pbDataFork,
	uint_t* pbResourceFork) BURGER_NOEXCEPT
{
	return CheckForForks(pFSSpec->vRefNum, pFSSpec->parID, pFSSpec->name,
		pbDataFork, pbResourceFork);
}

/*! ************************************

	\brief Test if a file has either a resource or data fork.

	Before copying a file, check if the source has a resource or data fork, so
	only the detected forks will be copied. Return two booleans that are either
	\ref TRUE or \ref FALSE if either fork is detected. FSGetCatalogInfo() is
	used to check the fork sizes.

	This function always returns ``paramErr`` if running on MacOS 7 or 8.

	\maconly

	\param pFSRef Pointer to the FSRef of the file of interest.
	\param pbDataFork Pointer to a boolean to receive the presence of a data
		fork.
	\param pbResourceFork Pointer to a boolean to receive the presence of a
		resource fork.

***************************************/

int BURGER_API Burger::CheckForForks(const FSRef* pFSRef, uint_t* pbDataFork,
	uint_t* pbResourceFork) BURGER_NOEXCEPT
{
	// 68K CFM doesn't support FSRef
#if !defined(BURGER_CFM) || !defined(BURGER_68K)
	FSCatalogInfo MyInfo;

	int iMacError =
		FSGetCatalogInfo(pFSRef, kFSCatInfoDataSizes | kFSCatInfoRsrcSizes,
			&MyInfo, nullptr, nullptr, nullptr);
	*pbDataFork = (MyInfo.dataLogicalSize != 0);
	*pbResourceFork = (MyInfo.rsrcLogicalSize != 0);

	return iMacError;
#else
	// Not supported
	BURGER_UNUSED(pFSRef);
	BURGER_UNUSED(pbDataFork);
	BURGER_UNUSED(pbResourceFork);
	return paramErr;
#endif
}

/*! ************************************

	\brief Copy a file using PBHCopyFileSync()

	Use the Mac OS call PBHCopyFileSync() to copy a file from one place to
	another on the same volume. This call will fail if either the source and
	destination volumes are different or if the volume doesn't support file
	copying. Use has_copy_file(const GetVolParmsInfoBuffer*) to test if a
	volume supports this call before invoking it.

	Network drives almost always implement this API

	\maconly

	\param svRefNumDest Volume reference number of the destination file
	\param lDirIDDest Parent ID of the destination file
	\param pFilenameDest Pointer to a Pascal string for the destination file
	\param svRefNumSource Volume reference number of the source file
	\param lDirIDSource Parent ID of the source file
	\param pFilenameSource Pointer to a Pascal string for the source file

	\return Zero if no error, or mac error code

	\sa DoCopyFile(const FSSpec*, const FSSpec*) and
		has_copy_file(const GetVolParmsInfoBuffer*)

***************************************/

int BURGER_API Burger::DoCopyFile(short svRefNumDest, long lDirIDDest,
	const uint8_t* pFilenameDest, short svRefNumSource, long lDirIDSource,
	const uint8_t* pFilenameSource)
{
	HParamBlockRec pb;

	pb.copyParam.ioVRefNum = svRefNumSource;
	pb.copyParam.ioDirID = lDirIDSource;
	pb.copyParam.ioNamePtr =
		reinterpret_cast<StringPtr>(const_cast<uint8_t*>(pFilenameSource));
	pb.copyParam.ioDstVRefNum = svRefNumDest;
	pb.copyParam.ioNewDirID = lDirIDDest;
	pb.copyParam.ioNewName = nullptr;
	pb.copyParam.ioCopyName =
		reinterpret_cast<StringPtr>(const_cast<uint8_t*>(pFilenameDest));

	// Copy the file
	return PBHCopyFileSync(&pb);
}

/*! ************************************

	\brief Copy a file using PBHCopyFileSync()

	Use the Mac OS call PBHCopyFileSync() to copy a file from one place to
	another on the same volume. This call will fail if either the source and
	destination volumes are different or if the volume doesn't support file
	copying. Use has_copy_file(const GetVolParmsInfoBuffer*) to test if a
	volume supports this call before invoking it.

	Network drives almost always implement this API

	\maconly

	\param pFSDest FSSpec for where the file will be copied to
	\param pFSSource FSSpec of the file being copied

	\return Zero if no error, or mac error code

	\sa DoCopyFile(short, long,	const uint8_t* , short, long, const uint8_t*)
		and has_copy_file(const GetVolParmsInfoBuffer*)

***************************************/

int BURGER_API Burger::DoCopyFile(
	const FSSpec* pFSDest, const FSSpec* pFSSource) BURGER_NOEXCEPT
{
	return DoCopyFile(pFSDest->vRefNum, pFSDest->parID, pFSDest->name,
		pFSSource->vRefNum, pFSSource->parID, pFSSource->name);
}

/*! ************************************

	\brief Copy a fork of a file using FSSpec APIs

	Given an opened source and destination file fork, and a buffer, perform a
	complete copy of the data from the source file fork to the destination file
	fork, replacing the destination file fork with a copy of the source fork.
	Buffer is assumed to a size that's a minimum of 512 bytes or in a chunk size
	of 512 bytes. Forks must be opened using the PBH class of functions using
	either an FSSpec or volume / directory / name, not an FSRef. If the fork was
	opened with an FSRef, use CopyForkCarbon(short, short, uint8_t*, uintptr_t)
	instead.

	\note Due to a limitation of the classic mac APIs, the maximum file size
	that can be copied is 4GB.

	\maconly

	\param iDestRefNum Opened file reference for the destination file
	\param iSourceRefNum Opened file reference for the source file
	\param pBuffer Pointer to the buffer to use for data transfer
	\param uBufferSize Size in bytes of the data transfer buffer

	\return Zero or Mac OSErr error code (Not Burgerlib)

	\sa CopyForkCarbon(short, short, uint8_t*, uintptr_t)

***************************************/

int BURGER_API Burger::CopyForkClassic(short iDestRefNum, short iSourceRefNum,
	uint8_t* pBuffer, uintptr_t uBufferSize) BURGER_NOEXCEPT
{
	// Param blocks for reading and writing
	ParamBlockRec SourcePB;
	ParamBlockRec DestPB;

	// Init the Param block records for the source and dest files
	SourcePB.ioParam.ioRefNum = iSourceRefNum;
	DestPB.ioParam.ioRefNum = iDestRefNum;

	// Ensure the destination file is the correct size (To ensure there is space
	// for the operation)
	int iMacError;
	for (;;) {

		// Get the size of the fork to copy
		iMacError = PBGetEOFSync(&SourcePB);
		if (iMacError) {
			break;
		}

		// Preallocate space for the fork on the destination file
		// Will fail if there is not enough space for the new fork
		DestPB.ioParam.ioMisc = SourcePB.ioParam.ioMisc;
		iMacError = PBSetEOFSync(&DestPB);
		if (iMacError) {
			break;
		}

		// Set the marks for both source and destination
		SourcePB.ioParam.ioPosMode = fsFromStart;
		SourcePB.ioParam.ioPosOffset = 0;
		DestPB.ioParam.ioPosMode = fsFromStart;
		DestPB.ioParam.ioPosOffset = 0;

		iMacError = PBSetFPosSync(&SourcePB);
		if (iMacError) {
			break;
		}
		iMacError = PBSetFPosSync(&DestPB);
		if (iMacError) {
			break;
		}

		// Prepare the Param blocks for the transfer loop

		// Buffer to work with
		SourcePB.ioParam.ioBuffer = reinterpret_cast<Ptr>(pBuffer);
		DestPB.ioParam.ioBuffer = reinterpret_cast<Ptr>(pBuffer);

		// Don't bother caching
		SourcePB.ioParam.ioPosMode = fsAtMark + noCacheMask;
		DestPB.ioParam.ioPosMode = fsAtMark + noCacheMask;

		// Read in this chunk size
		SourcePB.ioParam.ioReqCount = static_cast<long>(uBufferSize);

		// Copy the data
		do {
			// Read it
			int iSourceError = PBReadSync(&SourcePB);
			// Only EOF and no error are acceptable
			if (iSourceError && (iSourceError != eofErr)) {
				iMacError = iSourceError;
				break;
			}
			// Write it
			DestPB.ioParam.ioReqCount = SourcePB.ioParam.ioActCount;
			iMacError = PBWriteSync(&DestPB);

			// All done?
			if (iSourceError == eofErr) {
				break;
			}
		} while (!iMacError);

		break;
	}
	// Return the error code
	return iMacError;
}

/*! ************************************

	\brief Copy a fork of a file using FSRef APIs

	Given an opened source and destination file fork, and a buffer, perform a
	complete copy of the data from the source file fork to the destination file
	fork, replacing the destination file fork with a copy of the source fork.
	Buffer is assumed to a size that's a minimum of 512 bytes or in a chunk size
	of 512 bytes. Forks must be opened using the FSRef class of functions using
	an FSRef, not an FSSpec or volume / directory / name. If the fork was
	opened with an older API, use CopyForkClassic(short, short, uint8_t*,
	uintptr_t) instead.

	\note This is the preferred API to copy files due its use of 64 bit file
		size values.

	\maconly

	\param iDestRefNum Opened file reference for the destination file
	\param iSourceRefNum Opened file reference for the source file
	\param pBuffer Pointer to the buffer to use for data transfer
	\param uBufferSize Size in bytes of the data transfer buffer

	\return Zero or Mac OSErr error code (Not Burgerlib)

	\sa CopyForkClassic(short, short, uint8_t*, uintptr_t)

***************************************/

int BURGER_API Burger::CopyForkCarbon(short iDestRefNum, short iSourceRefNum,
	uint8_t* pBuffer, uintptr_t uBufferSize)
{
	int iMacError;
	for (;;) {

		SInt64 uForkSize;

		// Get the size of the fork to copy
		iMacError = FSGetForkSize(iSourceRefNum, &uForkSize);
		if (iMacError) {
			break;
		}

		// Preallocate space for the fork on the destination file
		// Will fail if there is not enough space for the new fork
		iMacError = FSSetForkSize(iDestRefNum, fsFromStart, uForkSize);
		if (iMacError) {
			break;
		}

		// Set the marks for both source and destination
		iMacError = FSSetForkPosition(iSourceRefNum, fsFromStart, 0);
		if (iMacError) {
			break;
		}

		iMacError = FSSetForkPosition(iDestRefNum, fsFromStart, 0);
		if (iMacError) {
			break;
		}

		// Copy the data
		do {
			// Read it
			ByteCount uActualCount;
			int iSourceError = FSReadFork(iSourceRefNum, fsAtMark + noCacheMask,
				0, uBufferSize, pBuffer, &uActualCount);
			// Only EOF and no error are acceptable
			if (iSourceError && (iSourceError != eofErr)) {
				iMacError = iSourceError;
				break;
			}
			// Write it
			iMacError = FSWriteFork(iDestRefNum, fsAtMark + noCacheMask, 0,
				uActualCount, pBuffer, nullptr);

			// All done?
			if (iSourceError == eofErr) {
				break;
			}
		} while (!iMacError);

		break;
	}
	// Return the error code
	return iMacError;
}

/*! ************************************

	\brief Copy file manager metadata from one file to another.

	Given the volume, directory ID and filename for a source and destination
	file, read in the file manager metadata, such as the file creator type, file
	type, and finder location information. If ``bCopyLockBit`` is non-zero, the
	lock bit will also be copied.

	\maconly

	\param svRefNumDest Volume reference number of the destination file
	\param lDirIDDest Parent ID of the destination file
	\param pFilenameDest Pointer to a Pascal string for the destination file
	\param svRefNumSource Volume reference number of the source file
	\param lDirIDSource Parent ID of the source file
	\param pFilenameSource Pointer to a Pascal string for the source file
	\param bCopyLockBit Boolean to enable the copying of the lock status

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::CopyFileMgrAttributes(short svRefNumDest,
	long lDirIDDest, const uint8_t* pFilenameDest, short svRefNumSource,
	long lDirIDSource, const uint8_t* pFilenameSource,
	uint_t bCopyLockBit) BURGER_NOEXCEPT
{
	// Parameter buffers, they are shared by design
	union {
		CInfoPBRec m_CInfo;
		HParamBlockRec m_HParam;
	} pb;

	Str255 Temp;

	// Set up for the source file
	pb.m_CInfo.hFileInfo.ioVRefNum = svRefNumSource;
	pb.m_CInfo.hFileInfo.ioDirID = lDirIDSource;

	// File sharing driver needs a valid ioNamePtr because it hates me
	if (!pFilenameSource || !pFilenameSource[0]) {

		// A directory? Use a phony buffer and instruct GetCatInfo to use the
		// directory ID only
		Temp[0] = 0;
		pb.m_CInfo.hFileInfo.ioNamePtr = Temp;
		pb.m_CInfo.hFileInfo.ioFDirIndex = -1;
	} else {

		// File? Use the directory and filename
		pb.m_CInfo.hFileInfo.ioNamePtr =
			reinterpret_cast<StringPtr>(const_cast<uint8_t*>(pFilenameSource));
		pb.m_CInfo.hFileInfo.ioFDirIndex = 0;
	}

	// Read in the finder meta data
	int iMacError = PBGetCatInfoSync(&pb.m_CInfo);
	if (!iMacError) {

		// Save the directory state for later
		uint_t bObjectIsDirectory = static_cast<uint_t>(
			pb.m_CInfo.hFileInfo.ioFlAttrib & kioFlAttribDirMask);

		// Set up for the destination file
		pb.m_CInfo.hFileInfo.ioVRefNum = svRefNumDest;
		pb.m_CInfo.hFileInfo.ioDirID = lDirIDDest;
		if (pFilenameDest && !pFilenameDest[0]) {
			pb.m_CInfo.hFileInfo.ioNamePtr = nullptr;
		} else {
			pb.m_CInfo.hFileInfo.ioNamePtr = reinterpret_cast<StringPtr>(
				const_cast<uint8_t*>(pFilenameDest));
		}

		// Clear the "Has been initialized" bit so the OS can consider this a
		// fresh file and then save the metadata
		pb.m_CInfo.hFileInfo.ioFlFndrInfo.fdFlags = static_cast<uint16_t>(
			pb.m_CInfo.hFileInfo.ioFlFndrInfo.fdFlags & ~kHasBeenInited);
		iMacError = PBSetCatInfoSync(&pb.m_CInfo);

		// Should the lock be transferred? (Only if no previous error)
		if (!iMacError && bCopyLockBit &&
			(pb.m_CInfo.hFileInfo.ioFlAttrib & kioFlAttribLockedMask)) {

			// Lock the file (Or directory)
			pb.m_HParam.fileParam.ioFVersNum = 0;
			iMacError = PBHSetFLockSync(&pb.m_HParam);

			// If the destination is a directory, ignore errors
			if (iMacError && bObjectIsDirectory) {
				iMacError = noErr;
			}
		}
	}
	return iMacError;
}

/*! ************************************

	\brief Copy file manager metadata from one file to another.

	Given the FSSpecs for a source and destination file, read in the file
	manager metadata, such as the file creator type, file type, and finder
	location information. If ``bCopyLockBit`` is non-zero, the lock bit will
	also be copied.

	\maconly

	\param pDestSpec Pointer to the destination file's FSSpec
	\param pSourceSpec Pointer to the source file's FSSpec
	\param bCopyLockBit Boolean to enable the copying of the lock status

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::CopyFileMgrAttributes(const FSSpec* pDestSpec,
	const FSSpec* pSourceSpec, uint_t bCopyLockBit) BURGER_NOEXCEPT
{
	return CopyFileMgrAttributes(pDestSpec->vRefNum, pDestSpec->parID,
		pDestSpec->name, pSourceSpec->vRefNum, pSourceSpec->parID,
		pSourceSpec->name, bCopyLockBit);
}

/*! ************************************

	\brief Copy file manager metadata from one file to another.

	Given the FSRefs for a source and destination file, read in the file
	manager metadata, such as the file creator type, file type, and finder
	location information. If ``bCopyLockBit`` is non-zero, the lock bit will
	also be copied.

	\maconly

	\param pDestRef Pointer to the destination file's FSRef
	\param pSourceRef Pointer to the source file's FSRef
	\param bCopyLockBit Boolean to enable the copying of the lock status

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::CopyFileMgrAttributes(const FSRef* pDestRef,
	const FSRef* pSourceRef, uint_t bCopyLockBit) BURGER_NOEXCEPT
{
	FSCatalogInfo MyInfo;
	int iMacError = FSGetCatalogInfo(pSourceRef,
		kFSCatInfoSettableInfo | kFSCatInfoNodeFlags, &MyInfo, nullptr, nullptr,
		nullptr);

	if (!iMacError) {
		// Make sure that we're using the correct user and group
		MyInfo.permissions[0] = MyInfo.permissions[1] = 0;

		uint_t bObjectIsDirectory =
			static_cast<uint_t>(MyInfo.nodeFlags & kFSNodeIsDirectoryMask);

		// Clear the "Has been initialized" bit so the OS can consider this a
		// fresh file and then save the metadata
		(*((FileInfo*)(MyInfo.finderInfo))).finderFlags &=
			static_cast<uint16_t>(~kHasBeenInited);

		// Save the lock bit (Bit is cleared below)
		uint_t bSetLockBit =
			bCopyLockBit && (MyInfo.nodeFlags & kFSNodeLockedMask);

		// Clear the flag when setting
		MyInfo.nodeFlags &= static_cast<uint16_t>(~kFSNodeLockedMask);

		iMacError = FSSetCatalogInfo(pDestRef, kFSCatInfoSettableInfo, &MyInfo);

		// Should the lock be transferred? (Only if no previous error)
		if (!iMacError && bSetLockBit) {
			MyInfo.nodeFlags |= kFSNodeLockedMask;
			iMacError =
				FSSetCatalogInfo(pDestRef, kFSCatInfoNodeFlags, &MyInfo);

			// If the destination is a directory, ignore errors
			if (iMacError && bObjectIsDirectory) {
				iMacError = noErr;
			}
		}
	}
	return iMacError;
}

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === Volume handling functions ===
#endif

/***************************************

	\brief Call the 68000 version of PBXGetVolInfoSync()

	Locate the 68000 trap for PBXGetVolInfoSync() and dispatch to it so
	access to PBXGetVolInfoSync() is allowed.

	\maconly

	\param pXVolumeParam Pointer to a XVolumeParam record

	\return Zero if no error, or mac error code

***************************************/

#if !defined(BURGER_MACCARBON) && defined(BURGER_CFM) && !defined(DOXYGEN)
static pascal OSErr PBXGetVolInfoSyncGlue(XVolumeParam* pXVolumeParam)
{

	// 68000 trap address for FSDIspatch
	static UniversalProcPtr g_pDispatchTrapAddress = nullptr;

	// Initialized?
	if (!g_pDispatchTrapAddress) {
		// Grab it, only need to do once
		g_pDispatchTrapAddress = NGetTrapAddress(_FSDispatch, OSTrap);
	}

	// The 68000 code uses A0 for input and returns the result in D0
	// The function prototype is a match for this glue code
	enum {
		uppFSDispatchProcInfo = kRegisterBased |
			REGISTER_RESULT_LOCATION(kRegisterD0) |
			RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
			REGISTER_ROUTINE_PARAMETER(
				1, kRegisterD0, SIZE_CODE(sizeof(long)))   // Selector
			| REGISTER_ROUTINE_PARAMETER(
				  2, kRegisterD1, SIZE_CODE(sizeof(long))) // Trap
			| REGISTER_ROUTINE_PARAMETER(
				  3, kRegisterA0, SIZE_CODE(sizeof(XVolumeParam*)))
	};

	// Call the class 68000 trap code (From PPC or 68K) and hope for the best!
	return static_cast<OSErr>(CallOSTrapUniversalProc(g_pDispatchTrapAddress,
		uppFSDispatchProcInfo, kFSMXGetVolInfo, _FSDispatch, pXVolumeParam));
}
#endif

/*! ************************************

	\brief Call PBXGetVolInfoSync() on all targets

	On 68K CFM and PPC non-carbon targets, the symbol PBXGetVolInfoSync() was
	not properly exposed in InterfaceLib until MacOS 8.5. So, for those poor
	unforunate souls running on MacOS 7.1 - 8.1 have to use a peice of glue
	code that jumps to 68000 via the trap manager.

	For Carbon PPC and 68K classic apps, this function directly calls
	PBXGetVolInfoSync()

	\maconly

	\param pXVolumeParam Pointer to a XVolumeParam record

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::DoPBXGetVolInfoSync(
	XVolumeParam* pXVolumeParam) BURGER_NOEXCEPT
{
	// PPC Carbon and 68K classic work just fine.
	// PPC Classic and 68K CFM are screwed (InterfaceLib issue)
#if defined(BURGER_MACCARBON) || !defined(BURGER_CFM)

	// Just call the real thing!
	return PBXGetVolInfoSync(pXVolumeParam);
#else

	// Invoke this function to do the actual work
	typedef pascal OSErr (*PBXGetVolInfoProcPtr)(XVolumeParamPtr paramBlock);
	static PBXGetVolInfoProcPtr g_pPBXGetVolInfoSync = nullptr;

	// Is the pointer valid?
	if (!g_pPBXGetVolInfoSync) {

		// See if InterfaceLib is around.
		// Since this code can work with 7.5, the pointers must all be valid or
		// GetSharedLibrary() will crash. Only MyConnectionID matters
		Str255 ErrorMessage;
		Ptr pMainAddress;
		CFragConnectionID MyConnectionID;
		int iMacError = GetSharedLibrary("\pInterfaceLib", kCompiledCFragArch,
			kLoadCFrag, &MyConnectionID, &pMainAddress, ErrorMessage);
		if (!iMacError) {

			// Since InterfaceLib was found, see if the function exists
			iMacError = FindSymbol(MyConnectionID, "\pPBXGetVolInfoSync",
				(Ptr*)&g_pPBXGetVolInfoSync, nullptr);
		}

		// If there was any error, punt to a 68000 bridge
		if (iMacError) {
			g_pPBXGetVolInfoSync = PBXGetVolInfoSyncGlue;
		}
	}

	// Call either the real thing or some hacky glue
	return (*g_pPBXGetVolInfoSync)(pXVolumeParam);
#endif
}

/*! ************************************

	\brief Obtain information about an extended volume.

	Given a volume reference number, fill in a XVolumeParam record with
	information about the volume. The ioNamePtr entry is zeroed out so
	retrieving a volume name is not possible with this call.

	\maconly

	\param pOutput Pointer to an uninitialized XVolumeParam buffer
	\param svRefNum Volume reference number

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::GetVolumeInfo(
	XVolumeParam* pOutput, short svRefNum) BURGER_NOEXCEPT
{
	int iMacError;

	// Use API version 0
	pOutput->ioVRefNum = svRefNum;
	pOutput->ioXVersion = 0;

	// Use the volume number only
	pOutput->ioNamePtr = nullptr;
	pOutput->ioVolIndex = 0;

	// Carbon always has the API, but other targets, not so much. Preflight
	// it.
#if !defined(BURGER_MACCARBON)

	// Is PBXGetVolInfo() available?
	long lGestalt;

	if (Gestalt(gestaltFSAttr, &lGestalt) ||
		!(lGestalt & (1L << gestaltFSSupports2TBVols))) {

		// The volume is Olde Skuul. Use the older API
		iMacError = PBHGetVInfoSync((HParmBlkPtr)pOutput);
		if (!iMacError) {
			// Do a quick conversion for the number of bytes in the volume
			pOutput->ioVTotalBytes =
				static_cast<uint64_t>(pOutput->ioVNmAlBlks) *
				static_cast<uint64_t>(pOutput->ioVAlBlkSiz);
			pOutput->ioVFreeBytes = static_cast<uint64_t>(pOutput->ioVFrBlk) *
				static_cast<uint64_t>(pOutput->ioVAlBlkSiz);
		}
	} else
#endif
	{
		// The new API exists, use it, some magic may be required
		iMacError = DoPBXGetVolInfoSync(pOutput);
	}

	return iMacError;
}

/*! ************************************

	\brief Find the real volume reference number.

	Given a volume reference number, get its information and determine if it is
	an alias or a real number. This function will call PBHGetVInfoSync and may
	return a different number that was passed in. This number is the true device
	number.

	\maconly

	\param pOutput Pointer to receive the returned volume reference number
	\param svRefNum Volume reference number

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::DetermineVRefNum(
	short* pOutput, short svRefNum) BURGER_NOEXCEPT
{
	HParamBlockRec hpbr;

	// Get the information of the volume
	int iMacError = MacOS::get_volume_info(&hpbr, svRefNum);
	if (!iMacError) {
		// Get the real volume reference number
		*pOutput = hpbr.volumeParam.ioVRefNum;
	}
	return iMacError;
}

/*! ************************************

	\brief Check if a volume is locked.

	Given a volume reference number, get its information and determine if it is
	locked. If no error is returned, the volume is not locked. A wPrErr is
	returned if the lock is by hardware (Write protect tab) or vLckdErr if the
	volume was locked by a software flag.

	\maconly

	\param svRefNum Volume reference number

	\return Zero if no error, or mac error code which shows how it was locked.

***************************************/

int BURGER_API Burger::CheckVolLock(short svRefNum)
{
	HParamBlockRec hpbr;

	int iMacError = MacOS::get_volume_info(&hpbr, svRefNum);
	if (!iMacError) {
		if (hpbr.volumeParam.ioVAtrb & kHFSVolumeHardwareLockMask) {
			// Hardware volume lock
			iMacError = wPrErr;
		} else if (hpbr.volumeParam.ioVAtrb & kHFSVolumeSoftwareLockMask) {
			// Software volume lock
			iMacError = vLckdErr;
		}
	}
	return iMacError;
}

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === File catalog helper functions ===
#endif

/*! ************************************

	\brief Call PBGetCatInfoSync() quickly.

	Given information about a file/folder, quickly obtain information about this
	file object and return the record and the filename.

	\param pOutput Pointer to empty CInfoPBRec to be filled in.
	\param pOutputName Pointer to a 256 byte Pascal buffer for the filename.
	\param sVRefNum Volume reference number
	\param lDirID Current directory ID
	\param pName Pascal string of the file of interest, can be nullptr.

	\return Zero if no error, otherwise return MacOS error code.

	\sa DoGetCatInfo(CInfoPBRec*, uint8_t*, const FSSpec *)

***************************************/

int BURGER_API Burger::DoGetCatInfo(CInfoPBRec* pOutput,
	uint8_t pOutputName[256], short sVRefNum, long lDirID,
	const uint8_t* pName) BURGER_NOEXCEPT
{
	// Valid name?
	if (!pName || !pName[0]) {
		pOutputName[0] = 0;

		// Use ioDirID only
		pOutput->dirInfo.ioFDirIndex = -1;
	} else {
		// Copy the name into the output buffer
		MemoryCopy(pOutputName, pName, pName[0] + 1U);

		// Use ioNamePtr and ioDirID
		pOutput->dirInfo.ioFDirIndex = 0;
	}

	// Set the name buffer
	pOutput->dirInfo.ioNamePtr = pOutputName;

	// Pass in the directory and volume
	pOutput->dirInfo.ioVRefNum = sVRefNum;
	pOutput->dirInfo.ioDrDirID = lDirID;

	// Get the directory information and return the error
	return PBGetCatInfoSync(pOutput);
}

/*! ************************************

	\brief Call PBGetCatInfoSync() quickly.

	Given information about a file/folder, quickly obtain information about this
	file object and return the record and the filename.

	\param pOutput Pointer to empty CInfoPBRec to be filled in.
	\param pOutputName Pointer to a 256 byte Pascal buffer for the filename.
	\param pFSSpec Pointer to the FSSpec of the file to be queried.

	\return Zero if no error, otherwise return MacOS error code.

	\sa DoGetCatInfo(CInfoPBRec*, uint8_t*, short, long, const uint8_t*)

***************************************/

int BURGER_API Burger::DoGetCatInfo(CInfoPBRec* pOutput,
	uint8_t pOutputName[256], const FSSpec* pFSSpec) BURGER_NOEXCEPT
{
	return DoGetCatInfo(
		pOutput, pOutputName, pFSSpec->vRefNum, pFSSpec->parID, pFSSpec->name);
}

/*! ************************************

	\brief Call PBGetCatalogInfoSync() quickly.

	Given information about a file/folder, quickly obtain information about this
	file object and return the record and the filename.

	This function will return ``paramErr`` if PBGetCatalogInfoSync() is not
	supported on the version of MacOS

	\maconly

	\param pOutput Pointer to empty FSCatalogInfo to be filled in.
	\param pRefParam Pointer to an unitialized FSRefParam buffer.
	\param pFSRef Pointer to the FSRef of the file to be queried.
	\param uCatInfoBitmap 32 bit catalog information bitmap

	\return Zero if no error, otherwise return MacOS error code.

	\sa DoGetCatInfo(CInfoPBRec*, uint8_t*, const FSSpec*)

***************************************/

int BURGER_API Burger::DoGetCatInfo(FSCatalogInfo* pOutput,
	FSRefParam* pRefParam, const FSRef* pFSRef,
	uint32_t uCatInfoBitmap) BURGER_NOEXCEPT
{
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	InitFSRefParam(pRefParam, const_cast<FSRef*>(pFSRef), uCatInfoBitmap);
	pRefParam->catInfo = pOutput;
	return PBGetCatalogInfoSync(pRefParam);
#else
	BURGER_UNUSED(pOutput);
	BURGER_UNUSED(pRefParam);
	BURGER_UNUSED(pFSRef);
	BURGER_UNUSED(uCatInfoBitmap);
	return paramErr;
#endif
}

/*! ************************************

	\brief Call PBGetCatInfoSync() quickly.

	Given information about a file/folder, quickly obtain information about this
	file object and return the record. An internal buffer for the filename is
	removed from ioNamePtr before this function exits to prevent dangling
	pointers.

	\param pOutput Pointer to empty CInfoPBRec to be filled in.
	\param sVRefNum Volume reference number
	\param lDirID Current directory ID
	\param pName Pascal name of the file of interest, can be nullptr.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::DoGetCatInfo(CInfoPBRec* pOutput, short sVRefNum,
	long lDirID, const uint8_t* pName) BURGER_NOEXCEPT
{
	uint8_t TempString[256];

	// Perform the command with a temp buffer.
	int iMacError = DoGetCatInfo(pOutput, TempString, sVRefNum, lDirID, pName);

	// Get rid of the pointer, since it's local
	pOutput->dirInfo.ioNamePtr = nullptr;

	// Return the error, if any.
	return iMacError;
}

/*! ************************************

	\brief Call PBGetCatInfoSync() quickly.

	Given information about a file/folder, quickly obtain information about this
	file object and return the record. An internal buffer for the filename is
	removed from ioNamePtr before this function exits to prevent dangling
	pointers.

	\param pOutput Pointer to empty CInfoPBRec to be filled in.
	\param pFSSpec Pointer to FSSpec for the file to be queried.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::DoGetCatInfo(
	CInfoPBRec* pOutput, const FSSpec* pFSSpec) BURGER_NOEXCEPT
{
	uint8_t TempString[256];

	// Perform the command with a temp buffer.
	int iMacError = DoGetCatInfo(pOutput, TempString, pFSSpec);

	// Get rid of the pointer, since it's local
	pOutput->dirInfo.ioNamePtr = nullptr;

	// Return the error, if any.
	return iMacError;
}

/*! ************************************

	\brief Call PBGetCatalogInfoSync() quickly.

	Given information about a file/folder, quickly obtain information about this
	file object and return the record and the filename.

	This function will return ``paramErr`` if PBGetCatalogInfoSync() is not
	supported on the version of MacOS

	A FSRefParam record is used internally during the scope of this function and
	will be discarded on exit.

	\maconly

	\param pOutput Pointer to empty FSCatalogInfo to be filled in.
	\param pFSRef Pointer to the FSRef of the file to be queried.
	\param uCatInfoBitmap 32 bit catalog information bitmap

	\return Zero if no error, otherwise return MacOS error code.

	\sa DoGetCatInfo(CInfoPBRec*, uint8_t*, const FSSpec*)

***************************************/

int BURGER_API Burger::DoGetCatInfo(FSCatalogInfo* pOutput, const FSRef* pFSRef,
	uint32_t uCatInfoBitmap) BURGER_NOEXCEPT
{
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	FSRefParam Block;
	InitFSRefParam(&Block, const_cast<FSRef*>(pFSRef), uCatInfoBitmap);
	Block.catInfo = pOutput;
	return PBGetCatalogInfoSync(&Block);
#else
	BURGER_UNUSED(pOutput);
	BURGER_UNUSED(pFSRef);
	BURGER_UNUSED(uCatInfoBitmap);
	return paramErr;
#endif
}

/*! ************************************

	\brief Get Finder DInfo for a directory.

	Given information about a folder, obtain the finder information about this
	folder object and return the DInfo record. pName is converted into a pascal
	string before being passed to PBGetCatInfoSync().

	\param pFinderInfo Pointer to empty DInfo to be filled in.
	\param sVRefNum Volume reference number
	\param lDirID Current directory ID
	\param pName Name of the file of interest, can be nullptr.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetDirectoryInfo(DInfo* pFinderInfo, short sVRefNum,
	long lDirID, const uint8_t* pName) BURGER_NOEXCEPT
{
	CInfoPBRec InfoPBRec;

	// Call PBGetCatInfoSync()
	int iMacError = DoGetCatInfo(&InfoPBRec, sVRefNum, lDirID, pName);
	if (!iMacError) {

		// Check
		if (InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask) {

			// Only directories have finder information
			*pFinderInfo = InfoPBRec.dirInfo.ioDrUsrWds;

		} else {
			// Whoops! Abort
			iMacError = dirNFErr;
		}
	}

	// Return the error, if any
	return iMacError;
}

/*! ************************************

	\brief Get Finder DInfo for a directory using a FSSpec

	Given an FSSpec for a folder, obtain the finder information about this
	folder object and return the DInfo record.

	\param pFinderInfo Pointer to empty DInfo to be filled in.
	\param pFSSpec Pointer to a FSSpec that points to the folder of interest.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetDirectoryInfo(
	DInfo* pFinderInfo, const FSSpec* pFSSpec) BURGER_NOEXCEPT
{
	return GetDirectoryInfo(
		pFinderInfo, pFSSpec->vRefNum, pFSSpec->parID, pFSSpec->name);
}

/*! ************************************

	\brief Get Finder DInfo for a directory using a FSRef

	Given an FSRef for a folder, obtain the finder information about this
	folder object and return the DInfo record.

	\param pFinderInfo Pointer to empty DInfo to be filled in.
	\param pFSRef Pointer to a FSRef that points to the folder of interest.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetDirectoryInfo(
	DInfo* pFinderInfo, const FSRef* pFSRef) BURGER_NOEXCEPT
{
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	FSCatalogInfo MyInfo;
	int iMacError = DoGetCatInfo(
		&MyInfo, pFSRef, kFSCatInfoFinderInfo | kFSCatInfoNodeFlags);
	// Check
	if (MyInfo.nodeFlags & kFSNodeIsDirectoryMask) {

		// Only directories have finder information
		*pFinderInfo = *reinterpret_cast<DInfo*>(&MyInfo.finderInfo);

	} else {

		// Whoops! Abort
		iMacError = dirNFErr;
	}

	return iMacError;
#else
	BURGER_UNUSED(pFinderInfo);
	BURGER_UNUSED(pFSRef);
	return paramErr;
#endif
}

/*! ************************************

	\brief Get common information from a directory.

	Given information about a file, get the ID of the named file object, if it's
	a directory and if the access is that of a "Drop box" folder.

	\param sVRefNum Volume reference number
	\param lDirID Current directory ID
	\param pName Name of the file of interest, can be nullptr.
	\param pDirID Pointer to receive the DirID of the named folder
	\param pbDirectory Pointer to receive a boolean that is \ref TRUE if it's a
		directory
	\param pbDropBox Pointer to receive a boolean if the folder is a drop box.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetDestinationDirectoryInfo(short sVRefNum, long lDirID,
	const uint8_t* pName, long* pDirID, uint_t* pbDirectory,
	uint_t* pbDropBox) BURGER_NOEXCEPT
{
	CInfoPBRec InfoPBRec;

	// This has to be cleared since some file systems don't mark it properly
	InfoPBRec.dirInfo.ioACUser = 0;

	// Pull in the information
	int iError = DoGetCatInfo(&InfoPBRec, sVRefNum, lDirID, pName);

	// Next directory ID in the chain.
	if (pDirID) {
		*pDirID = InfoPBRec.dirInfo.ioDrDirID;
	}

	// Directory?
	if (pbDirectory) {
		*pbDirectory = (InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask) != 0;
	}

	// Check if this is a mac "Drop box" folder.
	if (pbDropBox) {
		*pbDropBox = MacOS::user_has_drop_box_access(InfoPBRec.dirInfo.ioACUser);
	}

	return iError;
}

/*! ************************************

	\brief Get common information from a directory.

	Given information about a file, get the ID of the named file object, if it's
	a directory and if the access is that of a "Drop box" folder.

	\param pFSSpec Pointer of an FSSpec of the file/directory to query.
	\param pDirID Pointer to receive the DirID of the named folder
	\param pbDirectory Pointer to receive a boolean that is \ref TRUE if it's a
		directory
	\param pbDropBox Pointer to receive a boolean if the folder is a drop box.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetDestinationDirectoryInfo(const FSSpec* pFSSpec,
	long* pDirID, uint_t* pbDirectory, uint_t* pbDropBox) BURGER_NOEXCEPT
{
	return GetDestinationDirectoryInfo(pFSSpec->vRefNum, pFSSpec->parID,
		pFSSpec->name, pDirID, pbDirectory, pbDropBox);
}

/*! ************************************

	\brief Get common information from a directory.

	Given information about a file, get the ID of the named file object, if it's
	a directory and if the access is that of a "Drop box" folder.

	\param pFSRef Pointer of an FSRef of the file/directory to query.
	\param pDirID Pointer to receive the DirID of the named folder
	\param pbDirectory Pointer to receive a boolean that is \ref TRUE if it's a
		directory
	\param pbDropBox Pointer to receive a boolean if the folder is a drop box.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetDestinationDirectoryInfo(const FSRef* pFSRef,
	long* pDirID, uint_t* pbDirectory, uint_t* pbDropBox) BURGER_NOEXCEPT
{
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	FSCatalogInfo MyInfo;
	int iMacError = FSGetCatalogInfo(pFSRef,
		kFSCatInfoNodeFlags | kFSCatInfoUserPrivs | kFSCatInfoNodeID, &MyInfo,
		nullptr, nullptr, nullptr);

	if (pDirID) {
		*pDirID = static_cast<long>(MyInfo.nodeID);
	}

	// Directory?
	if (pbDirectory) {
		*pbDirectory = (MyInfo.nodeFlags & kFSNodeIsDirectoryMask) != 0;
	}

	// Check if this is a mac "Drop box" folder.
	if (pbDropBox) {
		*pbDropBox = MacOS::user_has_drop_box_access(
			static_cast<int8_t>(MyInfo.userPrivileges));
	}
	return iMacError;
#else
	BURGER_UNUSED(pFSRef);
	BURGER_UNUSED(pDirID);
	BURGER_UNUSED(pbDirectory);
	BURGER_UNUSED(pbDropBox);
	return paramErr;
#endif
}

/*! ************************************

	\brief Get finder comment ID

	Given information about a file, get the comment ID of the named file object.

	\param pCommentID Pointer to receive the finder comment ID.
	\param sVRefNum Volume reference number
	\param lDirID Current directory ID
	\param pName Name of the file of interest, can be nullptr.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetCommentID(short* pCommentID, short sVRefNum,
	long lDirID, const uint8_t* pName) BURGER_NOEXCEPT
{
	CInfoPBRec InfoPBRec;
	int iError = DoGetCatInfo(&InfoPBRec, sVRefNum, lDirID, pName);

	// Return the comment resource ID
	*pCommentID = InfoPBRec.hFileInfo.ioFlXFndrInfo.fdComment;
	return iError;
}

/*! ************************************

	\brief Get finder comment ID

	Given information about a file, get the comment ID of the named file object.

	\param pCommentID Pointer to receive the finder comment ID.
	\param pFSSpec Pointer to the FSSpec of the file to query.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetCommentID(
	short* pCommentID, const FSSpec* pFSSpec) BURGER_NOEXCEPT
{
	CInfoPBRec InfoPBRec;
	int iError = DoGetCatInfo(&InfoPBRec, pFSSpec);

	// Return the comment resource ID
	*pCommentID = InfoPBRec.hFileInfo.ioFlXFndrInfo.fdComment;
	return iError;
}

/*! ************************************

	\brief Get finder comment ID

	Given information about a file, get the comment ID of the named file object.

	\param pCommentID Pointer to receive the finder comment ID.
	\param pFSRef Pointer to the FSRef of the file to query.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetCommentID(
	short* pCommentID, const FSRef* pFSRef) BURGER_NOEXCEPT
{
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	FSCatalogInfo MyInfo;
	int iError = DoGetCatInfo(&MyInfo, pFSRef, kFSCatInfoFinderXInfo);

	// Return the comment resource ID
	*pCommentID =
		reinterpret_cast<ExtendedFileInfo*>(MyInfo.extFinderInfo)->reserved2;
	return iError;
#else
	BURGER_UNUSED(pCommentID);
	BURGER_UNUSED(pFSRef);
	return paramErr;
#endif
}

/*! ************************************

	\brief Get directory ID of a named directory.

	Given information about a directory, get its directory ID. If the object is
	a file, return the parent directory ID.

	\param sVRefNum Volume reference number
	\param lDirID Current directory ID
	\param pName Pascal name of the file of interest, can be nullptr.
	\param pDirID Pointer to receive the directory's ID.
	\param pbDirectory Pointer to receive \ref TRUE if it's a directory.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetDirectoryID(short sVRefNum, long lDirID,
	const uint8_t* pName, long* pDirID, uint_t* pbDirectory) BURGER_NOEXCEPT
{
	CInfoPBRec InfoPBRec;

	int iMacError = DoGetCatInfo(&InfoPBRec, sVRefNum, lDirID, pName);
	if (!iMacError) {

		uint_t bDirectory =
			(InfoPBRec.hFileInfo.ioFlAttrib & kioFlAttribDirMask) != 0;

		// Return values if wanted.
		if (pbDirectory) {
			*pbDirectory = bDirectory;
		}

		if (pDirID) {
			// Which directory, the parent for a file, the actual ID if
			// directory
			long lDirectory;
			if (bDirectory) {
				lDirectory = InfoPBRec.dirInfo.ioDrDirID;
			} else {
				// Return the parent directory for a file.
				lDirectory = InfoPBRec.hFileInfo.ioFlParID;
			}
			*pDirID = lDirectory;
		}
	}
	return iMacError;
}

/*! ************************************

	\brief Get directory ID of a named directory.

	Given information about a directory, get its directory ID. If the object is
	a file, return the parent directory ID.

	\param pFSSpec Pointer to the FSSpec of the file or directory of interest.
	\param pDirID Pointer to receive the directory's ID.
	\param pbDirectory Pointer to receive \ref TRUE if it's a directory.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetDirectoryID(
	const FSSpec* pFSSpec, long* pDirID, uint_t* pbDirectory) BURGER_NOEXCEPT
{
	return GetDirectoryID(
		pFSSpec->vRefNum, pFSSpec->parID, pFSSpec->name, pDirID, pbDirectory);
}

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === Desktop Manager functions ===
#endif

/*! ************************************

	\brief Determine the name of the Finder Desktop Database

	On network shares, the finder stores metadata like file comments in a file
	called "Desktop" on the root folder. Due to localization, this file name
	might be in another language so loading by name is impossible. Iterate
	through the root directory of the volume for the first file with the creator
	code of 'ERIK' and the file type of 'FNDR' and return the name of that file.

	\maconly

	\param pDesktopName Pointer to a 256 byte buffer to receive the Pascal
		filename
	\param svRefNum Volume reference number of the volume to scan for the file.

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::GetDesktopFileName(
	uint8_t* pDesktopName, short svRefNum) BURGER_NOEXCEPT
{
	CInfoPBRec cinfo;

	// Set up the volume to iterate
	cinfo.hFileInfo.ioNamePtr = pDesktopName;
	cinfo.hFileInfo.ioVRefNum = svRefNum;

	// Start with file entry #1
	short iIndex = 1;
	int iMacError;
	for (;;) {

		// Scan the root volume
		cinfo.dirInfo.ioDrDirID = fsRtDirID;
		cinfo.dirInfo.ioFDirIndex = iIndex;
		iMacError = PBGetCatInfoSync(&cinfo);

		// Possible it ran out of entries
		if (iMacError) {
			break;
		}

		// Is this the file of interest?
		if ((cinfo.hFileInfo.ioFlFndrInfo.fdType == 'FNDR') &&
			(cinfo.hFileInfo.ioFlFndrInfo.fdCreator == 'ERIK')) {
			// Bingo!
			break;
		}
		++iIndex;
	}

	// Only returns zero if the file was found
	return iMacError;
}

/*! ************************************

	\brief Load in a comment from a Desktop resource file.

	On network shares, the finder stores metadata like file comments in a file
	called "Desktop" on the root folder. Given the location of a file of
	interest, look up its comment ID in the database looking for a match. If
	found, return a copy of the data within.

	\maconly

	\param pComment Pointer to a 256 byte buffer to receive the "C" string
		comment
	\param svRefNum Volume reference number of the file to check
	\param lDirID Directory ID of the file to check
	\param pFileName Pointer to a "C" string of the filename to check

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::GetCommentFromDesktopFile(char* pComment, short svRefNum,
	long lDirID, const uint8_t* pFileName) BURGER_NOEXCEPT
{
	// Init the output, just in case
	pComment[0] = 0;

	// Obtain the file's comment resource ID
	short sCommentID;
	int iMacError = GetCommentID(&sCommentID, svRefNum, lDirID, pFileName);
	if (!iMacError) {

		// The ID must not be zero
		if (!sCommentID) {
			// If there is no comment, just return an error
			iMacError = afpItemNotFound;

		} else {

			// Find the actual volume reference number
			short sRealVRefNum;
			iMacError = DetermineVRefNum(&sRealVRefNum, svRefNum);
			if (!iMacError) {

				// Using the real volume number, locate the "Desktop" file
				Str255 DesktopFileName;
				iMacError = GetDesktopFileName(DesktopFileName, sRealVRefNum);

				// If not found, give up.
				if (iMacError) {
					iMacError = afpItemNotFound;

				} else {

					// Open the "Desktop" resource file, however, make sure it
					// doesn't preload everything else when opened
					short sSavedResFile = CurResFile();

					// Open the resource file wrapped with the auto load
					// disabled.
					SetResLoad(false);
					short iRezRefNum = HOpenResFile(
						sRealVRefNum, fsRtDirID, DesktopFileName, fsRdPerm);
					SetResLoad(true);

					// Opened?
					if (iRezRefNum == -1) {

						// Nope!
						iMacError = afpItemNotFound;
					} else {

						// Using the comment ID, get the resource
						StringHandle hComment = reinterpret_cast<StringHandle>(
							Get1Resource('FCMT', sCommentID));
						if (!hComment) {
							// No handle means no data available
							iMacError = afpItemNotFound;

							// Verify the handle is not damaged
						} else if (GetHandleSize(reinterpret_cast<Handle>(
									   hComment)) <= 0) {
							iMacError = afpItemNotFound;
						} else {
							// Copy the data (Pascal string)
							PStringToCString(pComment, *hComment);
						}

						// Restore the resource file chain and close the Desktop
						// file
						UseResFile(sSavedResFile);
						CloseResFile(iRezRefNum);
					}
				}
			}
		}
	}
	return iMacError;
}

/*! ************************************

	\brief Open the desktop manager database.

	Given a volume reference number, open the finder database and return a
	reference to the open file.

	\maconly

	\param pVolumeName Pointer to a Pascal string of the volume name
	\param svRefNum Volume reference number of the file to check
	\param pRefNum Pointer to variable to receive the open file reference
	\param pbDatabaseCreated Pointer to receive boolean if the database was
		created, can be \ref nullptr

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::DesktopOpen(const uint8_t* pVolumeName, short svRefNum,
	short* pRefNum, uint_t* pbDatabaseCreated)
{
	// Does the volume support the Desktop Manager?
	GetVolParmsInfoBuffer MyGetVolParmsInfoBuffer;
	uint32_t uInfoBufferSize = sizeof(MyGetVolParmsInfoBuffer);
	int iMacError = MacOS::get_volume_parms(
		&MyGetVolParmsInfoBuffer, &uInfoBufferSize, svRefNum);

	// There shouldn't be an error, but you never know
	if (!iMacError) {

		// Does the volume support the Desktop Manager?
		if (!MacOS::has_desktop_manager(&MyGetVolParmsInfoBuffer)) {

			// No, so force an error for "Not Supported"
			iMacError = paramErr;

		} else {

			// Open or create the database
			DTPBRec dtpb;
			dtpb.ioNamePtr = (StringPtr)pVolumeName;
			dtpb.ioVRefNum = svRefNum;
			iMacError = PBDTOpenInform(&dtpb);

			// Check if the database was created which means that there is no
			// need to copy data since it's a clean slate.
			uint_t bDatabaseCreated = !(dtpb.ioTagInfo & 1L);

			// Wait, was there a "Not supported" error? Docs say to call
			// PDBTGetPath() in this case, go figure.
			if (iMacError == paramErr) {

				// Try again with PBDTGetPath()
				iMacError = PBDTGetPath(&dtpb);

				// Database creation is unknown with the above call, so assume
				// it's always existed
				bDatabaseCreated = FALSE;
			}
			// Only return if requested
			if (pbDatabaseCreated) {
				*pbDatabaseCreated = bDatabaseCreated;
			}

			// Always return (There's no point in calling without returning
			// this)
			*pRefNum = dtpb.ioDTRefNum;
		}
	}
	return iMacError;
}

/*! ************************************

	\brief Get a comment from the Desktop Manager

	Given a file's volume, directory and filename, check the desktop manager if
	the database is active and query it for any associated comment. If the
	desktop database is not supported, check for the "Desktop" file found on
	many network shares and query it for the comment.

	If there are no errors and there was a comment, the ``pOutput`` buffer will
	have a "C" string of the comment. It will be set to an empty string on error
	or if there was no comment found.

	\maconly

	\param pOutput Pointer to a 256 byte buffer to receive the comment "C"
		string
	\param svRefNum Volume reference number of the file to check
	\param lDirID Parent ID of the file to check
	\param pFilename Pointer to a Pascal string for the filename to check

	\return Zero if no error, or mac error code

	\sa DesktopSetComment(short, long, const uint8_t*, const char*)

***************************************/

int BURGER_API Burger::DesktopGetComment(char* pOutput, short svRefNum,
	long lDirID, const uint8_t* pFilename) BURGER_NOEXCEPT
{
	// Guarantee the comment is initialized on exit
	pOutput[0] = 0;

	// See if a Desktop Manager is present
	uint_t bDatabaseCreated;
	short sDesktopRefNum;
	int iMacError =
		DesktopOpen(pFilename, svRefNum, &sDesktopRefNum, &bDatabaseCreated);
	if (!iMacError) {
		// Woo hoo! Use this API

		// Only care if the database was preexisting. If it was created, assume
		// it can't have any data of interest
		if (!bDatabaseCreated) {
			DTPBRec pb;

			// Set up for the read
			pb.ioDTRefNum = sDesktopRefNum;
			pb.ioNamePtr =
				reinterpret_cast<StringPtr>(const_cast<uint8_t*>(pFilename));
			pb.ioDirID = lDirID;
			pb.ioDTBuffer = reinterpret_cast<Ptr>(pOutput);

			// Note: The docs claim comments are a max of 200 characters.

			// THEY LIE!!

			// Use a full 256 byte buffer, or you will regret it.
			// Also, HFS ignores ioDTReqCount and assumes 255, so just live with
			// it.

			pb.ioDTReqCount = 255;
			iMacError = PBDTGetCommentSync(&pb);
			if (!iMacError) {
				// Terminate the "C" string
				pOutput[pb.ioDTActCount] = 0;
			} else {
				// Ensure it's clear
				pOutput[0] = 0;
			}
		}
	} else {
		// Get the record from the desktop file
		iMacError =
			GetCommentFromDesktopFile(pOutput, svRefNum, lDirID, pFilename);
		if (iMacError) {
			// Standardize the error
			iMacError = afpItemNotFound;
		}
	}
	return iMacError;
}

/*! ************************************

	\brief Get a comment from the Desktop Manager

	Given a file's volume, directory and filename, check the desktop manager if
	the database is active and set the comment to the given file. If the
	desktop database is not supported, no transfer is performed.

	Comments are limited to a maximum of 200 characters. If the comment string
	is longer than 200 characters, it will be truncated.

	\maconly

	\param svRefNum Volume reference number of the file to set the comment
	\param lDirID Parent ID of the file
	\param pFilename Pointer to a Pascal string for the filename of the file
	\param pComment Pointer to a "C" string to set as a comment.

	\return Zero if no error, or mac error code

	\sa DesktopGetComment(char*, short, long, const uint8_t*)

***************************************/

int BURGER_API Burger::DesktopSetComment(short svRefNum, long lDirID,
	const uint8_t* pFilename, const char* pComment) BURGER_NOEXCEPT
{
	// Try to open the Desktop manager
	short sDesktopRefNum;
	int iMacError = DesktopOpen(pFilename, svRefNum, &sDesktopRefNum, nullptr);
	if (!iMacError) {

		// Try saving the data to the newly created file
		DTPBRec pb;
		pb.ioDTRefNum = sDesktopRefNum;
		pb.ioNamePtr =
			reinterpret_cast<StringPtr>(const_cast<uint8_t*>(pFilename));
		pb.ioDirID = lDirID;
		pb.ioDTBuffer = reinterpret_cast<Ptr>(const_cast<char*>(pComment));

		// Make sure the comment is a maximum of 200 characters, because some
		// file systems don't check and it's good to at least TRY to follow the
		// documentation of the Desktop Manager
		uintptr_t uStringLength = string_length(pComment);
		if (uStringLength > 200U) {
			uStringLength = 200U;
		}
		pb.ioDTReqCount = static_cast<long>(uStringLength);
		iMacError = PBDTSetCommentSync(&pb);
	}
	return iMacError;
}

/*! ************************************

	\brief Copy a comment from one file to another

	Given the volume, directory ID and filename for a source and destination
	file, read in the Desktop Manager comment associated with the source file
	and write a copy of the comment found onto the destination file.

	While this code can read comments from both the Desktop Manager and the
	``Desktop`` resource file, it is limited to only writing to systems with a
	Desktop Manager. If the destination file resides on a different volume as
	the source and the destination uses a "Desktop" resource file, this call
	will always fail. However, if the source and destination reside on the same
	volume that uses a ``Desktop`` file, this call is moot because the resource
	string already exists in the destination file's resource fork if the
	destination file is a copy of the source file.

	\maconly

	\param svRefNumDest Volume reference number of the destination file
	\param lDirIDDest Parent ID of the destination file
	\param pFilenameDest Pointer to a Pascal string for the destination file
	\param svRefNumSource Volume reference number of the source file
	\param lDirIDSource Parent ID of the source file
	\param pFilenameSource Pointer to a Pascal string for the source file

	\return Zero if no error, or mac error code

	\sa DesktopGetComment(char*, short, long, const uint8_t*) or
		DesktopSetComment(short, long, const uint8_t*, const char*)

***************************************/

int BURGER_API Burger::DesktopCopyComment(short svRefNumDest, long lDirIDDest,
	const uint8_t* pFilenameDest, short svRefNumSource, long lDirIDSource,
	const uint8_t* pFilenameSource) BURGER_NOEXCEPT
{
	// Buffer for the comment string
	char CommentBuffer[256];

	// Read in the comment from the source
	int iMacError = DesktopGetComment(
		CommentBuffer, svRefNumSource, lDirIDSource, pFilenameSource);

	// No error, and is there a comment?
	if (!iMacError && CommentBuffer[0]) {
		// Save the comment
		// Note: DesktopSetComment() only writes to Desktop Manager volumes
		iMacError = DesktopSetComment(
			svRefNumDest, lDirIDDest, pFilenameDest, CommentBuffer);
	}
	return iMacError;
}

/*! ************************************

	\brief Copy a comment from one file to another

	Given the source FSSpec and the destination FSSpec, read in the Desktop
	Manager comment associated with the source file and write a copy of the
	comment found onto the destination file.

	While this code can read comments from both the Desktop Manager and the
	``Desktop`` resource file, it is limited to only writing to systems with a
	Desktop Manager. If the destination file resides on a different volume as
	the source and the destination uses a "Desktop" resource file, this call
	will always fail. However, if the source and destination reside on the same
	volume that uses a ``Desktop`` file, this call is moot because the resource
	string already exists in the destination file's resource fork if the
	destination file is a copy of the source file.

	\maconly

	\param pFSSpecDest Pointer to the FSSpec of the destination file
	\param pFSSpecSource Pointer to the FSSpec of the source file

	\return Zero if no error, or mac error code

	\sa DesktopCopyComment(short, long, const uint8_t*, short, long,
		const uint8_t*)

***************************************/

int BURGER_API Burger::DesktopCopyComment(
	const FSSpec* pFSSpecDest, const FSSpec* pFSSpecSource) BURGER_NOEXCEPT
{
	return DesktopCopyComment(pFSSpecDest->vRefNum, pFSSpecDest->parID,
		pFSSpecDest->name, pFSSpecSource->vRefNum, pFSSpecSource->parID,
		pFSSpecSource->name);
}

/*! ************************************

	\brief Copy a comment from one file to another

	Given the source FSRef and the destination FSRef, read in the Desktop
	Manager comment associated with the source file and write a copy of the
	comment found onto the destination file.

	While this code can read comments from both the Desktop Manager and the
	``Desktop`` resource file, it is limited to only writing to systems with a
	Desktop Manager. If the destination file resides on a different volume as
	the source and the destination uses a "Desktop" resource file, this call
	will always fail. However, if the source and destination reside on the same
	volume that uses a ``Desktop`` file, this call is moot because the resource
	string already exists in the destination file's resource fork if the
	destination file is a copy of the source file.

	\maconly

	\param pFSRefDest Pointer to the FSSpec of the destination file
	\param pFSRefSource Pointer to the FSSpec of the source file

	\return Zero if no error, or mac error code

	\sa DesktopCopyComment(short, long, const uint8_t*, short, long,
		const uint8_t*)

***************************************/

int BURGER_API Burger::DesktopCopyComment(
	const FSRef* pFSRefDest, const FSRef* pFSRefSource) BURGER_NOEXCEPT
{
#if !defined(BURGER_CFM) || !defined(BURGER_68K)
	FSSpec SourceFSSpec;

	int iMacErr = FSGetCatalogInfo(
		pFSRefSource, kFSCatInfoNone, nullptr, nullptr, &SourceFSSpec, nullptr);
	if (!iMacErr) {
		FSSpec DestFSSpec;
		FSGetCatalogInfo(
			pFSRefDest, kFSCatInfoNone, nullptr, nullptr, &DestFSSpec, nullptr);
		if (!iMacErr) {
			iMacErr = DesktopCopyComment(&DestFSSpec, &SourceFSSpec);
		}
	}
	return iMacErr;
#else
	// Not supported on 68K CFM
	BURGER_UNUSED(pFSRefDest);
	BURGER_UNUSED(pFSRefSource);
	return paramErr;
#endif
}

/*! ************************************

	\brief Determine if a destination volume has free file space.

	Given a file, check how much space it currently is occupying on the drive.
	Then, check the destination volume if there is enough free space to receive
	a copy of the source file.

	\maconly

	\param svRefNumSource Volume reference number of the source file
	\param lDirIDSource Parent ID of the source file
	\param pFilenameSource Pointer to a Pascal string for the source file
	\param pVolumenameDest Pointer to a Pascal string of the dest volume
	\param svRefNumDest Volume reference number of the destination volume
	\param pbSpaceAvailable Pointer to a variable to receive the answer

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::PreflightFileCopySpace(short svRefNumSource,
	long lDirIDSource, const uint8_t* pFilenameSource,
	const uint8_t* pVolumenameDest, short svRefNumDest,
	uint_t* pbSpaceAvailable) BURGER_NOEXCEPT
{
	BURGER_UNUSED(pVolumenameDest);

	// Get the information on the destination volume for blocks used
	XVolumeParam MyXVolumeParam;
	int iMacError = GetVolumeInfo(&MyXVolumeParam, svRefNumDest);
	if (!iMacError) {

		// Note: The math below assumes a maximum drive size of 2T

		// Number of blocks needed for a single allocation block
		// A disk block is 512 bytes, so shift by 9
		uint32_t uDestBlocksPerAllocBlock =
			static_cast<uint32_t>(MyXVolumeParam.ioVAlBlkSiz >> 9U);

		// Determine the number of free allocation blocks from bytes (512 a
		// block)
		uint32_t uDestFreeBlocks =
			static_cast<uint32_t>(MyXVolumeParam.ioVFreeBytes >> 9U);

		// Get the size of the file (2 forks)
		HParamBlockRec pb;
		pb.fileParam.ioNamePtr =
			reinterpret_cast<StringPtr>(const_cast<uint8_t*>(pFilenameSource));
		pb.fileParam.ioVRefNum = svRefNumSource;
		pb.fileParam.ioFVersNum = 0;
		pb.fileParam.ioDirID = lDirIDSource;
		pb.fileParam.ioFDirIndex = 0;
		iMacError = PBHGetFInfoSync(&pb);
		if (!iMacError) {

			// Determine the number of allocation blocks this file will require
			// on the destination drive.

			// Get the data for data fork
			uint32_t uSourceDataBlocks =
				static_cast<uint32_t>(pb.fileParam.ioFlLgLen) >> 9U;
			// Round up if needed
			uSourceDataBlocks += (pb.fileParam.ioFlLgLen & 0x1FFU) != 0;

			// Convert to NEW data blocks
			if (uSourceDataBlocks % uDestBlocksPerAllocBlock) {
				uSourceDataBlocks =
					(uSourceDataBlocks / uDestBlocksPerAllocBlock) + 1;
			} else {
				uSourceDataBlocks /= uDestBlocksPerAllocBlock;
			}

			// Same as above, but for the resource fork
			uint32_t uSourceRezBlocks =
				static_cast<uint32_t>(pb.fileParam.ioFlRLgLen) >> 9U;
			// Round up if needed
			uSourceRezBlocks += (pb.fileParam.ioFlRLgLen & 0x1FFU) != 0;

			// Convert to NEW data blocks
			if (uSourceRezBlocks % uDestBlocksPerAllocBlock) {
				uSourceRezBlocks =
					(uSourceRezBlocks / uDestBlocksPerAllocBlock) + 1;
			} else {
				uSourceRezBlocks /= uDestBlocksPerAllocBlock;
			}

			// Now we have the proposed space needed on the destination volume.
			// Do we have the space?
			*pbSpaceAvailable =
				(((uSourceDataBlocks + uSourceRezBlocks) *
					 uDestBlocksPerAllocBlock) <= uDestFreeBlocks);
		}
	}
	return iMacError;
}

/*! ************************************

	\brief Open a file with OpenDeny modes.

	If the file system supports it, open a data fork using PBHOpenDenySync(),
	otherwise emulate the behavior using older APIs.

	\maconly

	\param svRefNum Volume the file resides on
	\param lDirID Directory ID the file resides on
	\param pFilename Pointer to a PASCAL string of the filename
	\param sDenyModes OpenDeny modes requested
	\param pRefNum Pointer to receive the open file reference

	\return Zero if no error, or mac error code

	\sa OpenAware(const FSSpec, short, const uint8_t*)

***************************************/

int BURGER_API Burger::OpenAware(short svRefNum, long lDirID,
	const uint8_t* pFilename, short sDenyModes, short* pRefNum) BURGER_NOEXCEPT
{
	// Check the volume attributes to see if the volume supports the OpenDeny
	// protocol.
	GetVolParmsInfoBuffer MyVolParmsInfo;
	uint32_t uInfoSize = sizeof(MyVolParmsInfo);
	int iMacError =
		MacOS::get_volume_parms(&MyVolParmsInfo, &uInfoSize, svRefNum);

	// Prepare for the file access
	HParamBlockRec pb;
	pb.ioParam.ioMisc = nullptr;
	pb.fileParam.ioFVersNum = 0;
	pb.fileParam.ioNamePtr =
		reinterpret_cast<StringPtr>(const_cast<uint8_t*>(pFilename));
	pb.fileParam.ioVRefNum = svRefNum;
	pb.fileParam.ioDirID = lDirID;

	// If it supports OpenDeny, use that API and exit
	if (!iMacError && MacOS::has_open_deny(&MyVolParmsInfo)) {
		pb.accessParam.ioDenyModes = sDenyModes;
		iMacError = PBHOpenDenySync(&pb);
		*pRefNum = pb.ioParam.ioRefNum;

		// paramErr means the volume didn't support GetVolParms()
	} else if (!iMacError || (iMacError == paramErr)) {

		// Try to mimic OpenDeny using the older File Manager

		// If the open command is trying to write to the file, first check if
		// the volume is locked. If it is, return the lock error and early out.
		iMacError = noErr;
		if (sDenyModes & fsWrPerm) {
			iMacError = CheckVolLock(svRefNum);
		}

		// Writable, or read access requested?
		if (!iMacError) {
			// Switch to a File Manager permission flag
			if ((sDenyModes == fsWrPerm) || (sDenyModes == fsRdWrPerm)) {
				pb.ioParam.ioPermssn = fsRdWrShPerm;
			} else {
				pb.ioParam.ioPermssn = static_cast<SInt8>(sDenyModes % 4);
			}

			// Open the data fork
			iMacError = PBHOpenDFSync(&pb);
			if (iMacError == paramErr) {
				// Not supported? Try PBHOpenSync() as a last resort
				iMacError = PBHOpenSync(&pb);
			}
			// Save the reference number
			*pRefNum = pb.ioParam.ioRefNum;
		}
	}
	return iMacError;
}

/*! ************************************

	\brief Open a file with OpenDeny modes.

	If the file system supports it, open a data fork using PBHOpenDenySync(),
	otherwise emulate the behavior using older APIs.

	\maconly

	\param pFSSpec FSSpec for the file to open
	\param sDenyModes OpenDeny modes requested
	\param pRefNum Pointer to receive the open file reference

	\return Zero if no error, or mac error code

	\sa OpenAware(short, long, const uint8_t*, short, const uint8_t*)

***************************************/

int BURGER_API Burger::OpenAware(
	const FSSpec* pFSSpec, short sDenyModes, short* pRefNum) BURGER_NOEXCEPT
{
	return OpenAware(
		pFSSpec->vRefNum, pFSSpec->parID, pFSSpec->name, sDenyModes, pRefNum);
}

/*! ************************************

	\brief Open a resource fork with OpenDeny modes.

	If the file system supports it, open a resource fork using
	PBHOpenRFDenySync(), otherwise emulate the behavior using older APIs.

	\maconly

	\param svRefNum Volume the file resides on
	\param lDirID Directory ID the file resides on
	\param pFilename Pointer to a PASCAL string of the filename
	\param sDenyModes OpenDeny modes requested
	\param pRefNum Pointer to receive the open file reference

	\return Zero if no error, or mac error code

	\sa OpenRFAware(const FSSpec, short, const uint8_t*)

***************************************/

int BURGER_API Burger::OpenRFAware(short svRefNum, long lDirID,
	const uint8_t* pFilename, short sDenyModes, short* pRefNum) BURGER_NOEXCEPT
{
	// Check the volume attributes to see if the volume supports the OpenDeny
	// protocol.
	GetVolParmsInfoBuffer MyVolParmsInfo;
	uint32_t uInfoSize = sizeof(MyVolParmsInfo);
	int iMacError =
		MacOS::get_volume_parms(&MyVolParmsInfo, &uInfoSize, svRefNum);

	// Prepare for the file access
	HParamBlockRec pb;
	pb.ioParam.ioMisc = nullptr;
	pb.fileParam.ioFVersNum = 0;
	pb.fileParam.ioNamePtr =
		reinterpret_cast<StringPtr>(const_cast<uint8_t*>(pFilename));
	pb.fileParam.ioVRefNum = svRefNum;
	pb.fileParam.ioDirID = lDirID;

	// If it supports OpenDeny, use that API and exit
	if (!iMacError && MacOS::has_open_deny(&MyVolParmsInfo)) {
		pb.accessParam.ioDenyModes = sDenyModes;
		iMacError = PBHOpenRFDenySync(&pb);
		*pRefNum = pb.ioParam.ioRefNum;

		// paramErr means the volume didn't support GetVolParms()
	} else if (!iMacError || (iMacError == paramErr)) {
		// Try to mimic OpenDeny using the older File Manager

		// If the open command is trying to write to the file, first check if
		// the volume is locked. If it is, return the lock error and early out.
		iMacError = noErr;
		if (sDenyModes & fsWrPerm) {
			iMacError = CheckVolLock(svRefNum);
		}

		// Writable, or read access requested?
		if (!iMacError) {
			// Switch to a File Manager permission flag
			if ((sDenyModes == fsWrPerm) || (sDenyModes == fsRdWrPerm)) {
				pb.ioParam.ioPermssn = fsRdWrShPerm;
			} else {
				pb.ioParam.ioPermssn = static_cast<SInt8>(sDenyModes % 4);
			}

			// Open the data fork
			iMacError = PBHOpenRFSync(&pb);
			*pRefNum = pb.ioParam.ioRefNum;
		}
	}
	return iMacError;
}

/*! ************************************

	\brief Open a resource fork with OpenDeny modes.

	If the file system supports it, open a resource fork using
	PBHOpenRFDenySync(), otherwise emulate the behavior using older APIs.

	\maconly

	\param pFSSpec FSSpec for the file to open
	\param sDenyModes OpenDeny modes requested
	\param pRefNum Pointer to receive the open file reference

	\return Zero if no error, or mac error code

	\sa OpenRFAware(short, long, const uint8_t*, short, const uint8_t*)

***************************************/

int BURGER_API Burger::OpenRFAware(
	const FSSpec* pFSSpec, short sDenyModes, short* pRefNum) BURGER_NOEXCEPT
{
	return OpenRFAware(
		pFSSpec->vRefNum, pFSSpec->parID, pFSSpec->name, sDenyModes, pRefNum);
}

/*! ************************************

	\brief Copy a file, with AppleShare awareness

	If the file system supports it, copy with PBHCopyFile(), otherwise, manually
	copy a file from one volume to another and also copy the finder metadata and
	comments.

	\maconly

	\param svRefNumSource Volume of the source file
	\param lDirIDSource Directory ID the source file
	\param pFilenameSource Pointer to a PASCAL string of the source file
	\param svRefNumDest Volume of the destination folder
	\param lDirIDDest Directory ID the destination folder
	\param pFilenameDest Pointer to a PASCAL string of the destination folder
	\param pCopyname Pointer to a PASCAL string of the new name for the
		destination file
	\param pBuffer Pointer to a buffer to use for data transfer
	\param uBufferSize Size in bytes of the data transfer buffer
	\param bPreflight Set to \ref TRUE if the destination volume should be
		checked for free space

	\return Zero if no error, or mac error code

	\sa FileCopy(const FSSpec *, const FSSpec*, const uint8_t*, void*,
		uintptr_t, uint_t)

***************************************/

int Burger::FileCopy(short svRefNumSource, long lDirIDSource,
	const uint8_t* pFilenameSource, short svRefNumDest, long lDirIDDest,
	const uint8_t* pFilenameDest, const uint8_t* pCopyname, void* pBuffer,
	uintptr_t uBufferSize, uint_t bPreflight) BURGER_NOEXCEPT
{
	// Start by seeing if the destination needs to be checked for free space
	int iMacError;
	if (bPreflight) {
		// Check if there's space
		uint_t bSpaceAvailable;
		iMacError = PreflightFileCopySpace(svRefNumSource, lDirIDSource,
			pFilenameSource, pFilenameDest, svRefNumDest, &bSpaceAvailable);
		if (iMacError) {
			return iMacError;
		}
		// Not enough space?
		if (!bSpaceAvailable) {
			return dskFulErr;
		}
	}

	// Check the destination directory if it's a dropbox, or not a directory
	uint_t bIsDirectory;
	uint_t bIsDropBox;
	iMacError = GetDestinationDirectoryInfo(svRefNumDest, lDirIDDest,
		pFilenameDest, &lDirIDDest, &bIsDirectory, &bIsDropBox);
	if (iMacError) {
		return iMacError;
	}
	if (!bIsDirectory) {
		// Destination is not a directory!
		return dirNFErr;
	}

	// Change the destination volume reference to the real thing
	// Aliases can confuse the code below
	iMacError = DetermineVRefNum(&svRefNumDest, svRefNumDest);
	if (iMacError) {
		return iMacError;
	}

	// Query the source volume and see if CopyFile exists
	GetVolParmsInfoBuffer MyVolParmsInfoBuffer;
	uint32_t uTemp = sizeof(MyVolParmsInfoBuffer);
	iMacError =
		MacOS::get_volume_parms(&MyVolParmsInfoBuffer, &uTemp, svRefNumSource);
	if (iMacError && (iMacError != paramErr)) {
		// If it's an unhandled error, abort
		return iMacError;
	}

	// The name of the new file, unknown at this moment
	Str63 NewFilename;

	// If the volume have PBHCopyFile, use that API
	if (!iMacError && MacOS::has_copy_file(&MyVolParmsInfoBuffer)) {

		// Is the source file on an AppleShare server?
		long srcServerAdr = MyVolParmsInfoBuffer.vMServerAdr;

		// Check if the destination is on the same server
		uTemp = sizeof(MyVolParmsInfoBuffer);
		iMacError = MacOS::get_volume_parms(
			&MyVolParmsInfoBuffer, &uTemp, svRefNumDest);
		if (iMacError && (iMacError != paramErr)) {
			return iMacError;
		}

		// No error, and the IP addresses match
		if (!iMacError && (srcServerAdr == MyVolParmsInfoBuffer.vMServerAdr)) {

			// Notes found around the interwebs.
			// Mac OS X Carbon File Manager in OS 10.2 has a bug where it may
			// share IP addresses for multiple AppleShare volumes. This can
			// cause PBHCopyFile() to fail if the source and destination
			// locations are different servers. When this happens, PBHCopyFile()
			// will return diffVolErr as an error, which is technically not an
			// error, so the code will fall out of this section and proceed with
			// a normal file copy operation instead.

			// Perform the file copy
			iMacError = DoCopyFile(svRefNumDest, lDirIDDest, pCopyname,
				svRefNumSource, lDirIDSource, pFilenameSource);
			if (!iMacError) {
				// AppleShare doesn't always copy the attributes properly, do it
				// manually. Also, don't bother with errors, failure is okay
				if (pCopyname) {
					CopyFileMgrAttributes(svRefNumDest, lDirIDDest, pCopyname,
						svRefNumSource, lDirIDSource, pFilenameSource, TRUE);
				} else {
					// Since the name wasn't supplied, use the source file's
					// name
					if (!GetFilenameFromPathname(
							NewFilename, pFilenameSource)) {
						CopyFileMgrAttributes(svRefNumDest, lDirIDDest,
							NewFilename, svRefNumSource, lDirIDSource,
							pFilenameSource, TRUE);
					}
				}
				// Return the DoCopyFile() error code
				return iMacError;
			}
			// This error is caused by the Mac OS X 10.2 bug, do a fall through
			if (iMacError != diffVolErr) {
				return iMacError;
			}
		}
	}

	// Looks like PBHCopyFile() doesn't exist, or was buggy, let's copy the old
	// fashioned way!

	// If a copy buffer wasn't allocated, let's allocate one anyways.
	static const uintptr_t kCopyBuffSize = 0x4000U;
	static const uintptr_t kSmallCopyBuffSize = 512U;

	// true if we had to allocate the copy buffer
	uint_t bLocalCopyBuffer = FALSE;
	if (!pBuffer) {
		// Allocate a local buffer
		uBufferSize = kCopyBuffSize;
		pBuffer = NewPtr(static_cast<long>(uBufferSize));
		if (!pBuffer) {
			uBufferSize = kSmallCopyBuffSize;
			pBuffer = NewPtr(static_cast<long>(uBufferSize));
			if (!pBuffer) {
				return memFullErr;
			}
		}
		bLocalCopyBuffer = TRUE;
	}

	// Open the source data fork
	short sRefNumSource = 0;
	iMacError = OpenAware(svRefNumSource, lDirIDSource, pFilenameSource,
		(fsRdPerm | fsWrDenyPerm), &sRefNumSource);
	if (iMacError) {
		goto Exit1;
	}

	// Use Exit2 from now on

	// Check for the destination file name. Was it supplied, or will it be
	// extracted from the source name?
	if (pCopyname) {
		MemoryCopy(NewFilename, pCopyname, pCopyname[0] + 1U);
	} else {
		short sTempVolume;
		iMacError = GetFileLocation(&sTempVolume,
			reinterpret_cast<long*>(&uTemp), NewFilename, sRefNumSource);
		if (iMacError) {
			goto Exit2;
		}
	}

	// If no error, create the destination file (No data)
	iMacError = CreateEmptyFile(svRefNumDest, lDirIDDest, NewFilename);
	if (iMacError) {
		goto Exit2;
	}
	// Use Exit3 from now on

	// Welcome to hell.

	// An AppleShare dropbox folder is a folder for which the user has the Make
	// Changes privilege (write access), but not See Files (read access) and See
	// Folders (search access). Copying a file into an AppleShare dropbox
	// presents some special problems.

	// Here are the rules we have to follow to copy a file into a dropbox:

	// File attributes can be changed only when both forks of a file are empty.

	// DeskTop Manager comments can be added to a file only when both forks of a
	// file are empty.

	// A fork can be opened for write access only when both forks of a file are
	// empty.

	// So, with those rules to live with, we'll do those operations now while
	// both forks are empty.

	if (bIsDropBox) {
		// Copy the attributes, but skip the lock bit (Drop box fun)
		iMacError = CopyFileMgrAttributes(svRefNumDest, lDirIDDest, NewFilename,
			svRefNumSource, lDirIDSource, pFilenameSource, FALSE);
		if (iMacError) {
			goto Exit3;
		}
	}

	// Copy the desktop comments for the file, don't bother with errors
	DesktopCopyComment(svRefNumDest, lDirIDDest, NewFilename, svRefNumSource,
		lDirIDSource, pFilenameSource);

	// Which forks need to be created
	uint_t bHasDataFork;
	uint_t bHasResourceFork;
	iMacError = CheckForForks(svRefNumSource, lDirIDSource, pFilenameSource,
		&bHasDataFork, &bHasResourceFork);
	if (iMacError) {
		goto Exit3;
	}

	// If a data fork is needed, copy it
	if (bHasDataFork) {
		short sDestDataForkRef;
		iMacError = OpenAware(svRefNumDest, lDirIDDest, NewFilename,
			(fsWrPerm + fsRdDenyPerm + fsWrDenyPerm), &sDestDataForkRef);
		if (!iMacError) {
			iMacError = CopyForkClassic(sDestDataForkRef, sRefNumSource,
				static_cast<uint8_t*>(pBuffer), uBufferSize);
			FSClose(sDestDataForkRef);
		}
	}
	// Close the input file, since it's using the data fork
	FSClose(sRefNumSource);
	sRefNumSource = 0;

	// Copy the resource fork, if needed
	if (!iMacError && bHasResourceFork) {
		iMacError = OpenRFAware(svRefNumSource, lDirIDSource, pFilenameSource,
			(fsRdPerm + fsWrDenyPerm), &sRefNumSource);
		if (!iMacError) {
			short sDestRezForkRef;
			iMacError = OpenRFAware(svRefNumDest, lDirIDDest, NewFilename,
				(fsWrPerm + fsRdDenyPerm + fsWrDenyPerm), &sDestRezForkRef);
			if (!iMacError) {
				iMacError = CopyForkClassic(sDestRezForkRef, sRefNumSource,
					static_cast<uint8_t*>(pBuffer), uBufferSize);
				FSClose(sDestRezForkRef);
			}
			FSClose(sRefNumSource);
			sRefNumSource = 0;
		}
	}

	// If there was no error, copy the attributes again, so that the
	// modification time is correct
	if (!iMacError) {
		// Ignore errors, AppleShare might generate it.
		// This will copy the lock bit, if needed
		CopyFileMgrAttributes(svRefNumDest, lDirIDDest, NewFilename,
			svRefNumSource, lDirIDSource, pFilenameSource, true);
	}

Exit3:
	// If there was an error, delete the output file
	if (iMacError) {
		HDelete(svRefNumDest, lDirIDDest, NewFilename);
	}

Exit2:
	// Close the source file if it's still open
	if (sRefNumSource) {
		FSClose(sRefNumSource);
	}

Exit1:
	// Release the buffer if it was allocated locally
	if (bLocalCopyBuffer) {
		DisposePtr(static_cast<Ptr>(pBuffer));
	}
	return iMacError;
}

/*! ************************************

	\brief Copy a file, with AppleShare awareness

	If the file system supports it, copy with PBHCopyFile(), otherwise, manually
	copy a file from one volume to another and also copy the finder metadata and
	comments.

	\maconly

	\param pFSSpecSource FSSpec of the source file
	\param pFSSpecDestFolder FSSpec of the destination folder
	\param pCopyname Pointer to a PASCAL string of the new name for the
		destination file
	\param pBuffer Pointer to a buffer to use for data transfer
	\param uBufferSize Size in bytes of the data transfer buffer
	\param bPreflight Set to \ref TRUE if the destination volume should be
		checked for free space

	\return Zero if no error, or mac error code

	\sa FileCopy(short, long, const uint8_t, short, long, const uint8_t*,
		const uint8_t*, void*, uintptr_t, uint_t)

***************************************/

int BURGER_API Burger::FileCopy(const FSSpec* pFSSpecSource,
	const FSSpec* pFSSpecDestFolder, const uint8_t* pCopyname, void* pBuffer,
	uintptr_t uBufferSize, uint_t bPreflight) BURGER_NOEXCEPT
{
	return FileCopy(pFSSpecSource->vRefNum, pFSSpecSource->parID,
		pFSSpecSource->name, pFSSpecDestFolder->vRefNum,
		pFSSpecDestFolder->parID, pFSSpecDestFolder->name, pCopyname, pBuffer,
		uBufferSize, bPreflight);
}

#endif
