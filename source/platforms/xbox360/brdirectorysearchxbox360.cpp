/***************************************

	Xbox 360 version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

	\brief Open a directory for scanning

	\param pDirName Pointer to the "C" string filename in BurgerLib format.

	\return Zero on success, non-zero on an error or if the directory doesn't
		exist

	\sa close(void) or get_next_entry(void)

***************************************/

Burger::eError Burger::DirectorySearch::open(Filename* pDirName) BURGER_NOEXCEPT
{
	// Make sure the cache is cleared
	close();

	// Convert to UTF16
	String UnicodeName(pDirName->get_native());

	// Make sure the directory ends with a slash
	if (!UnicodeName.empty() && !UnicodeName.ends_with('\\')) {
		UnicodeName.push_back('\\');
	}

	// Append *.* to it.
	UnicodeName.append("*.*");

	// Open the directory
	WIN32_FIND_DATAA LocalData;
	HANDLE hDir = FindFirstFileA(UnicodeName.c_str(), &LocalData);

	// Assume I'm in error
	eError uResult = kErrorFileNotFound;
	if (hDir != INVALID_HANDLE_VALUE) {

		// Init the buffer
		m_Entries.reserve(kDefaultReserve);

		// Iterate over the directory, caching all the data found
		do {

			// LocalData has an entry. Add it to the cache
			DirectoryEntry_t Entry;

			// Get the file attributes
			const DWORD uFlags = LocalData.dwFileAttributes;
			Entry.m_bDir = (uFlags & FILE_ATTRIBUTE_DIRECTORY) != 0;

			// If it's a directory and it's named "." or "..", skip them
			if (Entry.m_bDir &&
				(!StringCompare(".", LocalData.cFileName) ||
					!StringCompare("..", LocalData.cFileName))) {
				uResult = kErrorNone;
				continue;
			}

			// File size
			Entry.m_uFileSize =
				(static_cast<uint64_t>(LocalData.nFileSizeHigh) << 32ULL) +
				LocalData.nFileSizeLow;

			// Time/dates
			Entry.m_CreationDate.Load(&LocalData.ftCreationTime);
			Entry.m_ModificatonDate.Load(&LocalData.ftLastWriteTime);

			// Get the rest of the file attributes
			Entry.m_bHidden = (uFlags & FILE_ATTRIBUTE_HIDDEN) != 0;
			Entry.m_bSystem = (uFlags & FILE_ATTRIBUTE_SYSTEM) != 0;
			Entry.m_bLocked = (uFlags & FILE_ATTRIBUTE_READONLY) != 0;

			// Make a copy of the string and store it
			const char* pName = StringDuplicate(LocalData.cFileName);
			if (!pName) {
				uResult = kErrorOutOfMemory;
				break;
			}
			Entry.m_pName = pName;

			// Add to the list
			m_Entries.append(&Entry, 1);

			// If FindNextFileW() failes, it's not an error.
			uResult = kErrorNone;
		} while (FindNextFileA(hDir, &LocalData));

		// Close the directory
		FindClose(hDir);
	}

	return uResult;
}

#endif
