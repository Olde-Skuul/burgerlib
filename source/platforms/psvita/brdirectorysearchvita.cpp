/***************************************

	Playstation Vita version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"

#if defined(BURGER_VITA) || defined(DOXYGEN)
#include "brfile.h"
#include "brmemoryfunctions.h"

#include <kernel.h>

/***************************************

	\brief Open a directory for scanning

	\param pDirName Pointer to the \ref Filename object

	\return Zero on success, non-zero on an error or if the directory doesn't
		exist

	\sa close(void) or get_next_entry(void)

***************************************/

Burger::eError Burger::DirectorySearch::open(
	Burger::Filename* pDirName) BURGER_NOEXCEPT
{
	// Make sure there's nothing pending
	close();

	eError uResult = kErrorFileNotFound;
	// Open the directory for reading
	int fp = sceIoDopen(pDirName->get_native());
	if (fp >= SCE_OK) {

		// Init the buffer
		m_Entries.reserve(Burger::DirectorySearch::kDefaultReserve);

		for (;;) {
			SceIoDirent DirEntry;
			memory_clear(&DirEntry, sizeof(DirEntry));

			// <0 = Error, 0 = Nothing read, >0 read an entry
			if (sceIoDread(fp, &DirEntry) <= SCE_OK) {
				uResult = kErrorNone;
				break;
			}

			DirectoryEntry_t Entry;

			// Make a copy of the string and store it
			const char* pName = StringDuplicate(DirEntry.d_name);
			if (!pName) {
				uResult = kErrorOutOfMemory;
				break;
			}
			Entry.m_pName = pName;

			// Is this a directory?
			if (SCE_STM_ISDIR(DirEntry.d_stat.st_mode)) {
				Entry.m_bDir = TRUE;
				Entry.m_uFileSize = 0;
			} else {
				Entry.m_bDir = FALSE;
				Entry.m_uFileSize =
					static_cast<uint64_t>(DirEntry.d_stat.st_size);
			}
			// Get the file dates
			Entry.m_CreationDate.Load(&DirEntry.d_stat.st_ctime);
			Entry.m_ModificatonDate.Load(&DirEntry.d_stat.st_mtime);

			// Not valid on the Vita
			Entry.m_bSystem = FALSE;

			// Linux style invisible flag
			Entry.m_bHidden = (DirEntry.d_name[0] == '.');

			// Is the file locked?
			Entry.m_bLocked = !(DirEntry.d_stat.st_mode & SCE_FWRITE);

			// Add to the list
			m_Entries.append(&Entry, 1);
		}

		sceIoDclose(fp);
	}
	return uResult;
}

#endif
