/***************************************

	Win95 version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"

#if defined(BURGER_WINDOWS)
#include "brstring16.h"

#include "win_windows.h"

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
	String16 UnicodeName(pDirName->get_native(), 5);

	uint16_t* pPath = UnicodeName.c_str();
	uintptr_t uLength = string_length(pPath);

	// Make sure the directory ends with a slash
	if (uLength && pPath[uLength - 1] != '\\') {
		pPath[uLength] = '\\';
		++uLength;
	}

	// Append *.* to it.
	pPath[uLength] = '*';
	pPath[uLength + 1] = '.';
	pPath[uLength + 2] = '*';
	pPath[uLength + 3] = 0;

	// Open the directory
	WIN32_FIND_DATAW LocalData;
	HANDLE hDir =
		FindFirstFileW(reinterpret_cast<const WCHAR*>(pPath), &LocalData);

	// Assume I'm in error
	eError uResult = kErrorPathNotFound;
	if (hDir != INVALID_HANDLE_VALUE) {

		String TempFilename;

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
				(!string_compare(reinterpret_cast<const uint16_t*>(L"."),
					 reinterpret_cast<const uint16_t*>(LocalData.cFileName)) ||
					!string_compare(reinterpret_cast<const uint16_t*>(L".."),
						reinterpret_cast<const uint16_t*>(
							LocalData.cFileName)))) {
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

			// Convert to UTF8
			uResult = TempFilename.assign(
				reinterpret_cast<const uint16_t*>(LocalData.cFileName));
			if (uResult) {
				break;
			}

			// Make a copy of the string and store it
			const char* pName = string_duplicate(TempFilename.c_str());
			if (!pName) {
				uResult = kErrorOutOfMemory;
				break;
			}
			Entry.m_pName = pName;

			// Add to the list
			m_Entries.append(&Entry, 1);

			// If FindNextFileW() failes, it's not an error.
			uResult = kErrorNone;
		} while (FindNextFileW(hDir, &LocalData));

		// Close the directory
		FindClose(hDir);
	}

	return uResult;
}

#endif
