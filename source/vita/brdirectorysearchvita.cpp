/***************************************

    Playstation Vita version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

	Using a Burgerlib path, open a directory to scan it
	for filenames
	
***************************************/

uint_t Burger::DirectorySearch::Open(Burger::Filename *pDirName) BURGER_NOEXCEPT
{	
	// Make sure there's nothing pending
	Close();
	uint_t uResult = kErrorFileNotFound;
	// Open the directory for reading
	int fp = sceIoDopen(pDirName->GetNative());
	if (fp>=SCE_OK) {
		m_fp = fp;
		uResult = kErrorNone;
	}
	return uResult;
}

/***************************************

	Return a directory entry
	
***************************************/

uint_t Burger::DirectorySearch::GetNextEntry(void) BURGER_NOEXCEPT
{
	// Assume no more entries
	uint_t uResult = kErrorInvalidParameter;
	int fp = m_fp;
	if (fp>=SCE_OK) {
		SceIoDirent Entry;
		MemoryClear(&Entry,sizeof(Entry));
		// <0 = Error, 0 = Nothing read, >0 read an entry
		if (sceIoDread(fp,&Entry)>SCE_OK) {

			// First, grab the filename
			StringCopy(m_Name,sizeof(m_Name),Entry.d_name);

			// Is this a directory?
			if (SCE_STM_ISDIR(Entry.d_stat.st_mode)) {
				m_bDir = TRUE;
				m_uFileSize = 0;
			} else {
				m_bDir = FALSE;
			// Get the file size (clamp on 32 bit systems)
				if (Entry.d_stat.st_size>=0xFFFFFFFFUL) {
					m_uFileSize = 0xFFFFFFFFUL;
				} else {
					m_uFileSize = static_cast<uintptr_t>(Entry.d_stat.st_size);
				}
			}
			// Get the file dates
			m_CreationDate.Load(&Entry.d_stat.st_ctime);
			m_ModificatonDate.Load(&Entry.d_stat.st_mtime);

			// Not valid on the Vita
			m_bSystem = FALSE;

			// Linux style invisible flag
			m_bHidden = (m_Name[0]=='.');

			// Is the file locked?
			m_bLocked = !(Entry.d_stat.st_mode&SCE_FWRITE);
	
			// It's parsed!
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	Release an opened directory
	
***************************************/

void Burger::DirectorySearch::Close(void) BURGER_NOEXCEPT
{
	int fp = m_fp;
	if (fp>=SCE_OK) {
		sceIoDclose(fp);
		m_fp = -1;
	}
}

#endif
