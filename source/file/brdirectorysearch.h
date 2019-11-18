/***************************************

	Directory search Class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDIRECTORYSEARCH_H__
#define __BRDIRECTORYSEARCH_H__

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

/* BEGIN */
namespace Burger {
class DirectorySearch {
public:
	DirectorySearch();
	~DirectorySearch();
	Word Open(Filename *pDirName);
	Word Open(const char *pDirName);
	Word GetNextEntry(void);
	Word GetNextEntryExtension(const char *pExt);
	void Close(void);

	WordPtr m_uFileSize;	///< Size of the file in bytes (64 bits on 64 bit CPU platforms)
	TimeDate_t m_CreationDate;		///< File creation time
	TimeDate_t m_ModificatonDate;	///< File last modification time
	Word8 m_bDir;			///< True if this is a directory
	Word8 m_bSystem;		///< True if this is a system file
	Word8 m_bHidden;		///< True if this file is hidden
	Word8 m_bLocked;		///< True if this file is read only
	char m_Name[256];		///< UTF8 Filename
#if (defined(BURGER_MACOS) || defined(BURGER_IOS)) || defined(DOXYGEN)
	Word32 m_uFileType;		///< File's type (MacOS and iOS Only)
	Word32 m_uAuxType;		///< File's creator code (MacOS and iOS Only)
#endif

private:

#if defined(BURGER_MAC) || defined(DOXYGEN)
	OpaqueFSIterator* m_pIterator;	///< Directory iterator (MacOS Carbon Only)
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	int m_fp;				///< Open directory file \macosxonly
	Word m_uEntryCount;		///< Number of entries in the cache
	Word m_bDone;			///< \ref TRUE if directory parsing is complete
	const void *m_pEntry;	///< Pointer to the next entry in the buffer
	// Note: The void * above forces 8 byte alignment on m_Buffer
	Word8 m_Buffer[2048];	///< Directory entries
#endif

#if (defined(BURGER_IOS) || defined(BURGER_VITA) || defined(BURGER_LINUX)) || defined(DOXYGEN)
	int m_fp;				///< Open directory file (MacOSX/iOS/Vita Only)
#endif

#if defined(BURGER_MSDOS) || defined(DOXYGEN)
	Word m_bHandleOk;		///< Handle is valid (MSDOS Only)
	short m_sFileHandle;	///< Handle to the open directory (MSDOS Only)
	char m_MyFindT[44];		///< Dos FindT structure (MSDOS Only)
#endif

#if defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(DOXYGEN)
	void *m_hDirHandle;		///< Windows file handle (Windows or Xbox 360 Only)
	char m_MyFindW[592];	///< Windows WIN32_FIND_DATAW structure (Windows or Xbox 360 Only)
#endif
};
}
/* END */

#endif

