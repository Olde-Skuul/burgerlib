/***************************************

	Linux/Stadia/Android version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"

#if (defined(BURGER_UNIX) && !defined(BURGER_DARWIN)) || defined(DOXYGEN)
#include "brfilename.h"
#include "brmemoryfunctions.h"
#include "brstringfunctions.h"

#include <dirent.h>
#include <sys/stat.h>

/***************************************

	\brief Open a directory for scanning

	\param pDirName Pointer to the "C" string filename in BurgerLib format.

	\return Zero on success, non-zero on an error or if the directory doesn't
		exist

	\sa close(void) or get_next_entry(void)

***************************************/

Burger::eError Burger::DirectorySearch::open(Filename* pDirName) BURGER_NOEXCEPT
{
	// Make sure there's nothing pending
	close();

	// Open the directory for reading
	eError uResult = m_NativePath.assign(pDirName->get_native());
	DIR* fp = ::opendir(m_NativePath.c_str());
	if (!fp) {
		uResult = kErrorPathNotFound;
	} else {

		// Init the buffer
		m_Entries.reserve(Burger::DirectorySearch::kDefaultReserve);

		// Ensure the path ends with a slash
		if (!m_NativePath.ends_with('/')) {
			m_NativePath.push_back('/');
		}

		struct dirent* pDirEntry;
		while ((pDirEntry = readdir(fp)) != nullptr) {

			// If it's a directory and it's named "." or "..", skip them
			if ((pDirEntry->d_type == DT_DIR) &&
				(!string_compare(".", pDirEntry->d_name) ||
					!string_compare("..", pDirEntry->d_name))) {
				uResult = kErrorNone;
				continue;
			}
			DirectoryEntry_t Entry;
			memory_clear(&Entry, sizeof(Entry));

			Entry.m_bDir = pDirEntry->d_type == DT_DIR;
			// Make a copy of the string and store it
			const char* pName = StringDuplicate(pDirEntry->d_name);
			if (!pName) {
				uResult = kErrorOutOfMemory;
				break;
			}
			Entry.m_pName = pName;

			// Add to the list
			m_Entries.append(&Entry, 1);
		}
		closedir(fp);
		uResult = kErrorNone;
	}
	return uResult;
}

/***************************************

	\brief Internal function for copying \ref DirectoryEntry_t

	On some platforms, reading a directory only returns a filename list. For
	performance, only DirectoryEntry_t records are fully parsed on records when
	they are being used and records skipped will be ignored.

	All other platforms, the entry is simply copied.

	\return Zero on success, non-zero on an error.

	\sa get_next_entry(DirectoryEntry_t*)

***************************************/

Burger::eError Burger::DirectorySearch::direntry_copy(
	DirectoryEntry_t* pOutput, const DirectoryEntry_t* pInput) BURGER_NOEXCEPT
{
	// Init the rest of the records
	memory_clear(pOutput, sizeof(DirectoryEntry_t));
	pOutput->m_pName = pInput->m_pName;
	pOutput->m_bDir = pInput->m_bDir;

	String TempName;
	eError uResult = TempName.assign(m_NativePath);
	if (!uResult) {
		TempName.append(pInput->m_pName);
		struct stat64 MyStat;
		int iError = stat64(TempName.c_str(), &MyStat);
		if (iError != -1) {
			pOutput->m_uFileSize = MyStat.st_size;

			// Linux/Android doesn't handle creation date
			pOutput->m_CreationDate.Load(
				reinterpret_cast<const timespec*>(&MyStat.st_mtime));
			pOutput->m_ModificatonDate = pOutput->m_CreationDate;
			pOutput->m_bHidden = pInput->m_pName[0] == '.';
			pOutput->m_bLocked = !(MyStat.st_mode & S_IWUSR);
			pOutput->m_bSystem = !MyStat.st_uid;
		}
	}
	return uResult;
}

#endif
