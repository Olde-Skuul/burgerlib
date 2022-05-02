/***************************************

	Directory search Class

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDIRECTORYSEARCH_H__
#define __BRDIRECTORYSEARCH_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
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
	BURGER_DISABLE_COPY(DirectorySearch);

public:
	DirectorySearch() BURGER_NOEXCEPT;
	~DirectorySearch();
	eError Open(Filename* pDirName) BURGER_NOEXCEPT;
	eError Open(const char* pDirName) BURGER_NOEXCEPT;
	eError GetNextEntry(void) BURGER_NOEXCEPT;
	eError GetNextEntryExtension(const char* pExt) BURGER_NOEXCEPT;
	void Close(void) BURGER_NOEXCEPT;

	/** Size of the file in bytes (64 bits on 64 bit CPU platforms) */
	uintptr_t m_uFileSize;
	/** File creation time */
	TimeDate_t m_CreationDate;
	/** File last modification time */
	TimeDate_t m_ModificatonDate;
	/** True if this is a directory */
	uint8_t m_bDir;
	/** True if this is a system file */
	uint8_t m_bSystem;
	/** True if this file is hidden */
	uint8_t m_bHidden;
	/** True if this file is read only */
	uint8_t m_bLocked;
	/** UTF8 Filename */
	char m_Name[256];
#if (defined(BURGER_MACOS) || defined(BURGER_IOS)) || defined(DOXYGEN)
	/** File's type (MacOS and iOS Only) */
	uint32_t m_uFileType;
	/** File's creator code (MacOS and iOS Only) */
	uint32_t m_uAuxType;
#endif

private:
#if defined(BURGER_MAC) || defined(DOXYGEN)
	/** Directory iterator for MacOS 9 or higher (MacOS Carbon Only) */
	OpaqueFSIterator* m_pIterator;
	/** Directory iterator for MacOS 7 to 8 (MacOS Carbon Only) */
	void* m_pLocal;
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	/** Open directory file \macosxonly */
	int m_fp;
	/** Number of entries in the cache */
	uint_t m_uEntryCount;
	/** \ref TRUE if directory parsing is complete */
	uint_t m_bDone;
	/** Pointer to the next entry in the buffer */
	const void* m_pEntry;
	// Note: The void * above forces 8 byte alignment on m_Buffer
	/** Directory entries */
	uint8_t m_Buffer[2048];
#endif

#if (defined(BURGER_IOS) || defined(BURGER_VITA) || defined(BURGER_LINUX)) || \
	defined(DOXYGEN)
	/** Open directory file (MacOSX/iOS/Vita Only) */
	int m_fp;
#endif

#if defined(BURGER_MSDOS) || defined(DOXYGEN)
	/** Handle is valid (MSDOS Only) */
	uint_t m_bHandleOk;
	/** Handle to the open directory (MSDOS Only) */
	short m_sFileHandle;
	/** Dos FindT structure (MSDOS Only) */
	char m_MyFindT[44];
#endif

#if defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(DOXYGEN)
	/** Windows file handle (Windows or Xbox 360 Only) */
	void* m_hDirHandle;
	/** Windows WIN32_FIND_DATAW structure (Windows or Xbox 360 Only) */
	char m_MyFindW[592];
#endif
};
}
/* END */

#endif
