/***************************************

	File Manager Class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILEMANAGER_H__
#define __BRFILEMANAGER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFILENAME_H__
#include "brfilename.h"
#endif

#ifndef __BRTIMEDATE_H__
#include "brtimedate.h"
#endif

#if defined(BURGER_MAC) && !defined(__BRMACTYPES_H__)
#include "brmactypes.h"
#endif

#ifndef _INC_STDIO
#include <stdio.h>
#endif

/* BEGIN */
namespace Burger {
struct FileManager {
public:
	enum {
		PREFIXCURRENT=8,		///< 8: Current working directory at application launch
		PREFIXAPPLICATION=9,	///< 9: Directory where the application executable resides
		PREFIXBOOT=32,			///< "*:" Boot volume prefix number
		PREFIXPREFS=33,			///< "@:" User preferences folder prefix number
		PREFIXSYSTEM=34,		///< "$:" System volume folder prefix number
		PREFIXMAX=35,			///< Maximum number of pathname prefixes
		PREFIXINVALID=999		///< Illegal prefix number
	};
static void BURGER_API Init(void);
static void BURGER_API Shutdown(void);
#if defined(BURGER_MSDOS)
static Word BURGER_API AreLongFilenamesAllowed(void);
#else
static BURGER_INLINE Word AreLongFilenamesAllowed(void) { return TRUE; }
#endif
static Word BURGER_API GetVolumeName(Filename *pOutput,Word uVolumeNum);
static Word BURGER_API GetVolumeNumber(const char *pInput);
static void BURGER_API DefaultPrefixes(void);
static Word BURGER_API GetPrefix(Filename *pOutput,Word uPrefixNum);
static Word BURGER_API SetPrefix(Word uPrefixNum,const char *pPrefixName);
static Word BURGER_API SetPrefix(Word uPrefixNum,Filename *pPrefixName);
static void BURGER_API PopPrefix(Word uPrefixNum);
static Word BURGER_API GetModificationTime(const char *pFileName,TimeDate_t *pOutput);
static Word BURGER_API GetModificationTime(Filename *pFileName,TimeDate_t *pOutput);
static Word BURGER_API GetCreationTime(const char *pFileName,TimeDate_t *pOutput);
static Word BURGER_API GetCreationTime(Filename *pFileName,TimeDate_t *pOutput);
static Word BURGER_API DoesFileExist(const char *pFileName);
static Word BURGER_API DoesFileExist(Filename *pFileName);
static Word32 BURGER_API GetAuxType(const char *pFileName);
static Word32 BURGER_API GetAuxType(Filename *pFileName);
static Word32 BURGER_API GetFileType(const char *pFileName);
static Word32 BURGER_API GetFileType(Filename *pFileName);
static Word BURGER_API GetFileAndAuxType(const char *pFileName,Word32 *pFileType,Word32 *pAuxType);
static Word BURGER_API GetFileAndAuxType(Filename *pFileName,Word32 *pFileType,Word32 *pAuxType);
static Word BURGER_API SetAuxType(const char *pFileName,Word32 uAuxType);
static Word BURGER_API SetAuxType(Filename *pFileName,Word32 uAuxType);
static Word BURGER_API SetFileType(const char *pFileName,Word32 uFileType);
static Word BURGER_API SetFileType(Filename *pFileName,Word32 uFileType);
static Word BURGER_API SetFileAndAuxType(const char *pFileName,Word32 uFileType,Word32 uAuxType);
static Word BURGER_API SetFileAndAuxType(Filename *pFileName,Word32 uFileType,Word32 uAuxType);
static Word BURGER_API CreateDirectoryPath(const char *pFileName);
static Word BURGER_API CreateDirectoryPath(Filename *pFileName);
static Word BURGER_API CreateDirectoryPathDirName(const char *pFileName);
static Word BURGER_API CreateDirectoryPathDirName(Filename *pFileName);
static Word BURGER_API DeleteFile(const char *pFileName);
static Word BURGER_API DeleteFile(Filename *pFileName);
static Word BURGER_API RenameFile(const char *pNewName,const char *pOldName);
static Word BURGER_API RenameFile(Filename *pNewName,Filename *pOldName);
static Word BURGER_API ChangeOSDirectory(const char *pDirName);
static Word BURGER_API ChangeOSDirectory(Filename *pDirName);
static FILE *BURGER_API OpenFile(const char *pFileName,const char *pType);
static FILE *BURGER_API OpenFile(Filename *pFileName,const char *pType);
static Word BURGER_API CopyFile(const char *pDestName,const char *pSourceName);
static Word BURGER_API CopyFile(Filename *pDestName,Filename *pSourceName);
static Word BURGER_API SaveFile(const char *pFileName,const void *pData,WordPtr uLength);
static Word BURGER_API SaveFile(Filename *pFileName,const void *pData,WordPtr uLength);
static Word BURGER_API SaveTextFile(const char *pFileName,const void *pData,WordPtr uLength);
static Word BURGER_API SaveTextFile(Filename *pFileName,const void *pData,WordPtr uLength);
static void * BURGER_API LoadFile(const char *pFileName,WordPtr *pLength);
static void * BURGER_API LoadFile(Filename *pFileName,WordPtr *pLength);
#if (defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
static BURGER_INLINE const char *GetBootName(void) { return g_FileManager.m_pBootName; }
static BURGER_INLINE Word GetBootNameSize(void) { return g_FileManager.m_uBootNameSize; }
#endif
private:
	void ReleasePrefixes(void);
	const char *m_pPrefix[PREFIXMAX];	///< Array of prefix strings
#if defined(BURGER_MSDOS) || defined(DOXYGEN)
	Word8 m_bAllowed;					///< True if MSDOS has long filename support (MSDOS Only)
#endif
#if (defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	Word m_uBootNameSize;	///< Length of the boot volume name (MACOSX or iOS only)
	char *m_pBootName;		///< Boot volume name in the format ":FooBar" (Zero terminated) (MACOSX or iOS only)
#endif
	static FileManager g_FileManager;	///< Global instance of the file manager
};

class FileManagerSimple {
public:
	FileManagerSimple() { Burger::FileManager::Init(); }
	~FileManagerSimple() { Burger::FileManager::Shutdown(); }
};
}
/* END */

#endif

