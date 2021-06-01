/***************************************

    Xbox 360 version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"

#if defined(BURGER_XBOX360)
#define NOD3D
#define NONET
#include <xtl.h>

/***************************************

	Open a directory for scanning
	Return an error if the directory doesn't exist

***************************************/

uint_t Burger::DirectorySearch::Open(Filename *pDirName) BURGER_NOEXCEPT
{
	// Leave room for 5 extra samples
	String CopyOfName(pDirName->GetNative(),5);
	char *pPath = CopyOfName.GetPtr();
	uintptr_t uLength = StringLength(pPath);
	// Make sure the directory ends with a slash
	if (uLength && pPath[uLength-1]!='\\') {
		pPath[uLength] = '\\';
		++uLength;
	}
	// Append *.* to it.
	pPath[uLength] = '*';
	pPath[uLength+1] = 0;
	// Open the directory
	HANDLE hDir = FindFirstFileA(pPath,reinterpret_cast<WIN32_FIND_DATAA *>(m_MyFindW));
	uint_t uResult = TRUE;		// Assume I'm in error
	if (hDir != INVALID_HANDLE_VALUE) {
		m_hDirHandle = hDir;
		m_bDir = 123;
		uResult = FALSE;			// It's all good!
	}
	return uResult;
}

/***************************************

	Get the next directory entry
	Return FALSE if the entry is valid, TRUE if
	an error occurs.

***************************************/

uint_t Burger::DirectorySearch::GetNextEntry(void) BURGER_NOEXCEPT
{
	// Am I scanning a directory?
	HANDLE hDir = m_hDirHandle;
	if (hDir==INVALID_HANDLE_VALUE) {
		return TRUE;
	}

	// Here's the fun part. Since opening causes a data read,
	// assume the record is valid. Process it now.
	// Then read in the NEXT one. If it's over, shut
	// down and abort on the next pass.

	uint_t uFlags;
	for (;;) {
		// This is only 123 on the first pass. All others, it's TRUE or FALSE
		if (m_bDir!=123) {
			if (!FindNextFileA(hDir,(WIN32_FIND_DATAA *)m_MyFindW)) {
				// Release the directory
				FindClose(hDir);
				m_hDirHandle = INVALID_HANDLE_VALUE;
				return TRUE;		// I'm so gone
			}
		}
		// Convert to UTF8
		StringCopy(m_Name,sizeof(m_Name),((WIN32_FIND_DATAA *)m_MyFindW)->cFileName);
		uFlags = ((WIN32_FIND_DATAA *)m_MyFindW)->dwFileAttributes;
		m_bDir = FALSE;
		m_bHidden = FALSE;
		m_bSystem = FALSE;
		m_bLocked = FALSE;
		if (uFlags & FILE_ATTRIBUTE_DIRECTORY) {
			m_bDir = TRUE;
			if (!StringCompare(".",m_Name) || !StringCompare("..",m_Name)) {
				continue;
			}
		}
		break;
	}
	if (uFlags & FILE_ATTRIBUTE_HIDDEN) {
		m_bHidden = TRUE;
	}
	if (uFlags & FILE_ATTRIBUTE_SYSTEM) {
		m_bSystem = TRUE;
	}
	if (uFlags & FILE_ATTRIBUTE_READONLY) {
		m_bLocked = TRUE;
	}
	m_CreationDate.Load(&((WIN32_FIND_DATAA *)m_MyFindW)->ftCreationTime);
	m_ModificatonDate.Load(&((WIN32_FIND_DATAA *)m_MyFindW)->ftLastWriteTime);
	// Peg at the maximum
	if (((WIN32_FIND_DATAA *)m_MyFindW)->nFileSizeHigh) {
		m_uFileSize = 0xFFFFFFFFUL;
	} else {
		m_uFileSize = ((WIN32_FIND_DATAA *)m_MyFindW)->nFileSizeLow;
	}
	return FALSE;
}

/***************************************

	Close a directory that's being scanned

***************************************/

void Burger::DirectorySearch::Close(void) BURGER_NOEXCEPT
{
	if (m_hDirHandle!=INVALID_HANDLE_VALUE) {
		FindClose(m_hDirHandle);
		m_hDirHandle = INVALID_HANDLE_VALUE;
	}
}

#endif
