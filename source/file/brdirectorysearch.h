/***************************************

	Directory search Class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRSIMPLEARRAY_H__
#include "brsimplearray.h"
#endif

#if defined(BURGER_MAC) && !defined(__BRMACTYPES_H__)
#include "brmactypes.h"
#endif

/* BEGIN */
namespace Burger {

struct DirectoryEntry_t {
	/** Pointer to the UTF8 filename */
	const char* m_pName;
	/** Size of the file in bytes */
	uint64_t m_uFileSize;
	/** File creation time */
	TimeDate_t m_CreationDate;
	/** File last modification time */
	TimeDate_t m_ModificatonDate;

#if (defined(BURGER_MACOS) || defined(BURGER_DARWIN)) || defined(DOXYGEN)
	/** File's creator code (MacOS and Darwin Only) */
	uint32_t m_uCreatorType;
	/** File's type (MacOS and Darwin Only) */
	uint32_t m_uFileType;
#endif

	/** True if this is a directory */
	uint8_t m_bDir;
	/** True if this is a system file */
	uint8_t m_bSystem;
	/** True if this file is hidden */
	uint8_t m_bHidden;
	/** True if this file is read only */
	uint8_t m_bLocked;
};

class DirectorySearch {
	BURGER_DISABLE_COPY(DirectorySearch);

private:
#if (defined(BURGER_UNIX) && !defined(BURGER_DARWIN)) || defined(DOXYGEN)
	/** Linux/Android only, folder path for calling stat() */
	String m_NativePath;
#endif

public:
	/** Initial array size for DirectoryEntry_t records in m_Entries */
	static const uintptr_t kDefaultReserve = 32U;

protected:
	/** Directory cache of stored entries */
	SimpleArray<DirectoryEntry_t> m_Entries;
	/** Index to the m_Entries array for \ref get_next_entry() */
	uintptr_t m_uIndex;

	eError direntry_copy(DirectoryEntry_t* pOutput,
		const DirectoryEntry_t* pInput) BURGER_NOEXCEPT;

public:
	DirectorySearch() BURGER_NOEXCEPT;
	~DirectorySearch();
	eError open(Filename* pDirName) BURGER_NOEXCEPT;
	eError open(const char* pDirName) BURGER_NOEXCEPT;
	const char* get_next_entry(void) BURGER_NOEXCEPT;
	eError get_next_entry(DirectoryEntry_t* pOutput) BURGER_NOEXCEPT;
	eError get_next_entry(
		DirectoryEntry_t* pOutput, const char* pExt) BURGER_NOEXCEPT;
	void close(void) BURGER_NOEXCEPT;
};
}
/* END */

#endif
