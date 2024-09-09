/***************************************

	MacOS specific convenience functions

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMACTYPES_H__
#define __BRMACTYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */

#if defined(BURGER_MAC) || defined(DOXYGEN)

#if !defined(DOXYGEN)

struct FSRef;
struct UTCDateTime;
struct CCrsr;
struct GrafPort;
struct FSSpec;
struct DInfo;
struct FileInfo;
struct FSRefParam;
struct XVolumeParam;
struct FSCatalogInfo;
struct OTAddress;
struct OpaqueOTClientContextPtr;
union CInfoPBRec;
union HParamBlockRec;

#if TARGET_API_MAC_CARBON
struct __CFBundle;
struct __CFString;
#endif

#endif

namespace Burger {

extern void BURGER_API InitFileInfo(FileInfo* pFileInfo) BURGER_NOEXCEPT;
extern void BURGER_API InitFSRefParam(FSRefParam* pFSRefParam, FSRef* pFSRef,
	uint32_t uCatInfoBitmap) BURGER_NOEXCEPT;
extern int BURGER_API GetFilenameFromPathname(
	uint8_t* pOutput, const uint8_t* pInput) BURGER_NOEXCEPT;

extern int BURGER_API GetFileLocation(short* pVRefNum, long* pDirID,
	uint8_t* pOutputFilename, short iFileRefNum) BURGER_NOEXCEPT;
extern int BURGER_API GetFileLocation(
	FSSpec* pOutput, short iFileRefNum) BURGER_NOEXCEPT;
extern int BURGER_API GetFileLocation(
	FSRef* pOutput, short iFileRefNum) BURGER_NOEXCEPT;
extern int BURGER_API CreateEmptyFile(
	short svRefNum, long lDirID, const uint8_t* pFilename) BURGER_NOEXCEPT;
extern int BURGER_API CreateEmptyFile(const FSSpec* pFSSpec) BURGER_NOEXCEPT;
extern int BURGER_API CreateEmptyFile(
	FSRef* pOutput, const FSRef* pFSSpec, const char* pName) BURGER_NOEXCEPT;
extern int BURGER_API CheckForForks(short svRefNum, long lDirID,
	const uint8_t* pName, uint_t* pbDataFork,
	uint_t* pbResourceFork) BURGER_NOEXCEPT;
extern int BURGER_API CheckForForks(const FSSpec* pFSSpec, uint_t* pbDataFork,
	uint_t* pbResourceFork) BURGER_NOEXCEPT;
extern int BURGER_API CheckForForks(const FSRef* pFSRef, uint_t* pbDataFork,
	uint_t* pbResourceFork) BURGER_NOEXCEPT;
extern int BURGER_API DoCopyFile(short svRefNumDest, long lDirIDDest,
	const uint8_t* pFilenameDest, short svRefNumSource, long lDirIDSource,
	const uint8_t* pFilenameSource) BURGER_NOEXCEPT;
extern int BURGER_API DoCopyFile(
	const FSSpec* pFSDest, const FSSpec* pFSSource) BURGER_NOEXCEPT;
extern int BURGER_API CopyForkClassic(short iDestRefNum, short iSourceRefNum,
	uint8_t* pBuffer, uintptr_t uBufferSize) BURGER_NOEXCEPT;
extern int BURGER_API CopyForkCarbon(short iDestRefNum, short iSourceRefNum,
	uint8_t* pBuffer, uintptr_t uBufferSize) BURGER_NOEXCEPT;
extern int BURGER_API CopyFileMgrAttributes(short svRefNumDest, long lDirIDDest,
	const uint8_t* pFilenameDest, short svRefNumSource, long lDirIDSource,
	const uint8_t* pFilenameSource, uint_t bCopyLockBit = TRUE) BURGER_NOEXCEPT;
extern int BURGER_API CopyFileMgrAttributes(const FSSpec* pDestSpec,
	const FSSpec* pSourceSpec, uint_t bCopyLockBit = TRUE) BURGER_NOEXCEPT;
extern int BURGER_API CopyFileMgrAttributes(const FSRef* pDestRef,
	const FSRef* pSourceRef, uint_t bCopyLockBit = TRUE) BURGER_NOEXCEPT;

extern int BURGER_API DoPBXGetVolInfoSync(
	XVolumeParam* pXVolumeParam) BURGER_NOEXCEPT;
extern int BURGER_API GetVolumeInfo(
	XVolumeParam* pOutput, short svRefNum) BURGER_NOEXCEPT;
extern int BURGER_API DetermineVRefNum(
	short* pOutput, short svRefNum) BURGER_NOEXCEPT;
extern int BURGER_API CheckVolLock(short svRefNum) BURGER_NOEXCEPT;

extern int BURGER_API DoGetCatInfo(CInfoPBRec* pOutput,
	uint8_t pOutputName[256], short sVRefNum, long lDirID,
	const uint8_t* pName) BURGER_NOEXCEPT;
extern int BURGER_API DoGetCatInfo(CInfoPBRec* pOutput,
	uint8_t pOutputName[256], const FSSpec* pFSSpec) BURGER_NOEXCEPT;
extern int BURGER_API DoGetCatInfo(FSCatalogInfo* pOutput,
	FSRefParam* pRefParam, const FSRef* pFSRef,
	uint32_t uCatInfoBitmap) BURGER_NOEXCEPT;
extern int BURGER_API DoGetCatInfo(CInfoPBRec* pOutput, short sVRefNum,
	long lDirID, const uint8_t* pName) BURGER_NOEXCEPT;
extern int BURGER_API DoGetCatInfo(
	CInfoPBRec* pOutput, const FSSpec* pFSSpec) BURGER_NOEXCEPT;
extern int BURGER_API DoGetCatInfo(FSCatalogInfo* pOutput, const FSRef* pFSRef,
	uint32_t uCatInfoBitmap) BURGER_NOEXCEPT;
extern int BURGER_API GetDirectoryInfo(DInfo* pFinderInfo, short sVRefNum,
	long lDirID, const uint8_t* pName) BURGER_NOEXCEPT;
extern int BURGER_API GetDirectoryInfo(
	DInfo* pFinderInfo, const FSSpec* pFSSpec) BURGER_NOEXCEPT;
extern int BURGER_API GetDirectoryInfo(
	DInfo* pFinderInfo, const FSRef* pFSRef) BURGER_NOEXCEPT;
extern int BURGER_API GetDestinationDirectoryInfo(short sVRefNum, long lDirID,
	const uint8_t* pName, long* pDirID, uint_t* pbDirectory,
	uint_t* pbDropBox) BURGER_NOEXCEPT;
extern int BURGER_API GetDestinationDirectoryInfo(const FSSpec* pFSSpec,
	long* pDirID, uint_t* pbDirectory, uint_t* pbDropBox) BURGER_NOEXCEPT;
extern int BURGER_API GetDestinationDirectoryInfo(const FSRef* pFSRef,
	long* pDirID, uint_t* pbDirectory, uint_t* pbDropBox) BURGER_NOEXCEPT;
extern int BURGER_API GetCommentID(short* pCommentID, short sVRefNum,
	long lDirID, const uint8_t* pName) BURGER_NOEXCEPT;
extern int BURGER_API GetCommentID(
	short* pCommentID, const FSSpec* pFSSpec) BURGER_NOEXCEPT;
extern int BURGER_API GetCommentID(
	short* pCommentID, const FSRef* pFSRef) BURGER_NOEXCEPT;
extern int BURGER_API GetDirectoryID(short sVRefNum, long lDirID,
	const uint8_t* pName, long* pDirID, uint_t* pbDirectory) BURGER_NOEXCEPT;
extern int BURGER_API GetDirectoryID(
	const FSSpec* pFSSpec, long* pDirID, uint_t* pbDirectory) BURGER_NOEXCEPT;

extern int BURGER_API GetDesktopFileName(
	uint8_t* pDesktopName, short svRefNum) BURGER_NOEXCEPT;
extern int BURGER_API GetCommentFromDesktopFile(char* pComment, short svRefNum,
	long lDirID, const uint8_t* pFileName) BURGER_NOEXCEPT;
extern int BURGER_API DesktopOpen(const uint8_t* pVolumeName, short vRefNum,
	short* pRefNum, uint_t* pbDatabaseCreated) BURGER_NOEXCEPT;
extern int BURGER_API DesktopGetComment(char* pOutput, short svRefNum,
	long lDirID, const uint8_t* pFilename) BURGER_NOEXCEPT;
extern int BURGER_API DesktopSetComment(short svRefNum, long lDirID,
	const uint8_t* pFilename, const char* pComment) BURGER_NOEXCEPT;
extern int BURGER_API DesktopCopyComment(short svRefNumDest, long lDirIDDest,
	const uint8_t* pFilenameDest, short svRefNumSource, long lDirIDSource,
	const uint8_t* pFilenameSource) BURGER_NOEXCEPT;
extern int BURGER_API DesktopCopyComment(
	const FSSpec* pFSSpecDest, const FSSpec* pFSSpecSource) BURGER_NOEXCEPT;
extern int BURGER_API DesktopCopyComment(
	const FSRef* pFSRefDest, const FSRef* pFSRefSource) BURGER_NOEXCEPT;

extern int BURGER_API PreflightFileCopySpace(short svRefNumSource,
	long lDirIDSource, const uint8_t* pFilenameSource,
	const uint8_t* pVolumenameDest, short svRefNumDest,
	uint_t* pbSpaceAvailable) BURGER_NOEXCEPT;
extern int BURGER_API OpenAware(short svRefNum, long lDirID,
	const uint8_t* pFilename, short sDenyModes, short* pRefNum) BURGER_NOEXCEPT;
extern int BURGER_API OpenAware(
	const FSSpec* pFSSpec, short sDenyModes, short* pRefNum) BURGER_NOEXCEPT;
extern int BURGER_API OpenRFAware(short svRefNum, long lDirID,
	const uint8_t* pFilename, short sDenyModes, short* pRefNum) BURGER_NOEXCEPT;
extern int BURGER_API OpenRFAware(
	const FSSpec* pFSSpec, short sDenyModes, short* pRefNum) BURGER_NOEXCEPT;
extern int FileCopy(short svRefNumSource, long lDirIDSource,
	const uint8_t* pFilenameSource, short svRefNumDest, long lDirIDDest,
	const uint8_t* pFilenameDest, const uint8_t* pCopyname, void* pBuffer,
	uintptr_t uBufferSize, uint_t bPreflight) BURGER_NOEXCEPT;
extern int BURGER_API FileCopy(const FSSpec* pFSSpecSource,
	const FSSpec* pFSSpecDestFolder, const uint8_t* pCopyname, void* pBuffer,
	uintptr_t uBufferSize, uint_t bPreflight) BURGER_NOEXCEPT;
}

#endif

/* END */

#endif
