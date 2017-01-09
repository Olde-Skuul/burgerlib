/***************************************

	Playstation Vita version
	
	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "brdirectorysearch.h"

#if defined(BURGER_VITA) || defined(DOXYGEN)
#include "brfile.h"
#include "brstringfunctions.h"
#include <kernel.h>

/***************************************

	Using a Burgerlib path, open a directory to scan it
	for filenames
	
***************************************/

Word Burger::DirectorySearch::Open(Burger::Filename *pDirName)
{	
	// Make sure there's nothing pending
	Close();
	Word uResult = File::FILENOTFOUND;
	// Open the directory for reading
	int fp = sceIoDopen(pDirName->GetNative());
	if (fp>=SCE_OK) {
		m_fp = fp;
		uResult = File::OKAY;
	}
	return uResult;
}

/***************************************

	Return a directory entry
	
***************************************/

Word Burger::DirectorySearch::GetNextEntry(void)
{
	// Assume no more entries
	Word uResult = File::OUTOFRANGE;
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
					m_uFileSize = static_cast<WordPtr>(Entry.d_stat.st_size);
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
			uResult = File::OKAY;
		}
	}
	return uResult;
}

/***************************************

	Release an opened directory
	
***************************************/

void Burger::DirectorySearch::Close(void)
{
	int fp = m_fp;
	if (fp>=SCE_OK) {
		sceIoDclose(fp);
		m_fp = -1;
	}
}

#endif