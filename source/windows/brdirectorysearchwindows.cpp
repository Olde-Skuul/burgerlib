/***************************************

	Win95 version
	
	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"

#if defined(BURGER_WINDOWS)
#include "brutf8.h"
#include "brstring16.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <wchar.h>
#if !defined(__MWERKS__)
#include <io.h>
#include <stdlib.h>
#else
#include <stdio.h>
// For some wacky reason, this function is in the Codewarrior libraries, yet
// it isn't exposed in a single header. I know, I searched.
extern "C" FILE * _MSL_CDECL _wfopen(const wchar_t * _MSL_RESTRICT name, const wchar_t * _MSL_RESTRICT mode);
#endif
#if defined(BURGER_WATCOM)
#define std
#endif

/***************************************

	Open a directory for scanning
	Return an error if the directory doesn't exist

***************************************/

Word Burger::DirectorySearch::Open(Filename *pDirName)
{
	// Leave room for 5 extra samples
	String16 UnicodeName(pDirName->GetNative(),5);
	Word16 *pPath = UnicodeName.GetPtr();
	WordPtr uLength = StringLength(pPath);
	// Make sure the directory ends with a slash
	if (uLength && pPath[uLength-1]!='\\') {
		pPath[uLength] = '\\';
		++uLength;
	}
	// Append *.* to it.
	pPath[uLength] = '*';
	pPath[uLength+1] = '.';
	pPath[uLength+2] = '*';
	pPath[uLength+3] = 0;
	// Open the directory
	HANDLE hDir = FindFirstFileW(reinterpret_cast<const WCHAR *>(pPath),reinterpret_cast<WIN32_FIND_DATAW *>(m_MyFindW));
	Word uResult = TRUE;		// Assume I'm in error
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

Word Burger::DirectorySearch::GetNextEntry(void)
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

	Word uFlags;
	for (;;) {
		// This is only 123 on the first pass. All others, it's TRUE or FALSE
		if (m_bDir!=123) {
			if (!FindNextFileW(hDir,(WIN32_FIND_DATAW *)m_MyFindW)) {
				// Release the directory
				FindClose(hDir);
				m_hDirHandle = INVALID_HANDLE_VALUE;
				return TRUE;		// I'm so gone
			}
		}
		// Convert to UTF8
		UTF8::FromUTF16(m_Name,sizeof(m_Name),reinterpret_cast<const Word16 *>(((WIN32_FIND_DATAW *)m_MyFindW)->cFileName));
		uFlags = ((WIN32_FIND_DATAW *)m_MyFindW)->dwFileAttributes;
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
	m_CreationDate.Load(&((WIN32_FIND_DATAW *)m_MyFindW)->ftCreationTime);
	m_ModificatonDate.Load(&((WIN32_FIND_DATAW *)m_MyFindW)->ftLastWriteTime);
#if !defined(BURGER_64BITCPU)
	// Peg at the maximum
	if (((WIN32_FIND_DATAW *)m_MyFindW)->nFileSizeHigh) {
		m_uFileSize = 0xFFFFFFFFUL;
	} else {
		m_uFileSize = ((WIN32_FIND_DATAW *)m_MyFindW)->nFileSizeLow;
	}
#else
	// 64 bit machines can handle files bigger than 4 gig.
	WordPtr uLength = (static_cast<WordPtr>(((WIN32_FIND_DATAW *)m_MyFindW)->nFileSizeHigh)<<32ULL)+(((WIN32_FIND_DATAW *)m_MyFindW)->nFileSizeLow);
	m_uFileSize = uLength;
#endif
	return FALSE;
}

/***************************************

	Close a directory that's being scanned

***************************************/

void Burger::DirectorySearch::Close(void)
{
	if (m_hDirHandle!=INVALID_HANDLE_VALUE) {
		FindClose(m_hDirHandle);
		m_hDirHandle = INVALID_HANDLE_VALUE;
	}
}

#endif
