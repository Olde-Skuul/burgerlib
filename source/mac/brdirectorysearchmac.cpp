/***************************************

	MacOS version

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "brdirectorysearch.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brfile.h"
#include "brstring16.h"
#include "brutf8.h"
#include <Files.h>

/***************************************

	Open a directory for scanning
	Return an error if the directory doesn't exist

***************************************/

Word Burger::DirectorySearch::Open(Filename *pDirName)
{
	// Convert the filename to unicode
	String16 MyName(pDirName->GetNative());
	FSRef MyRef;
	// Create a UFT16 FSRef
	OSErr eError = FSMakeFSRefUnicode(pDirName->GetFSRef(),MyName.GetLength(),MyName.GetPtr(),kUnicode16BitFormat,&MyRef);
	Word uResult = File::FILENOTFOUND;
	if (!eError) {
		eError = FSOpenIterator(&MyRef,kFSIterateFlat,&m_pIterator);
		if (!eError) {
			uResult = File::OKAY;
		}
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
	ItemCount Count;
	Boolean bChanged;
	FSCatalogInfo MyRec;
	HFSUniStr255 uName;
	OSErr eErr = FSGetCatalogInfoBulk(m_pIterator,1,&Count,&bChanged,
			kFSCatInfoCreateDate|kFSCatInfoContentMod|kFSCatInfoFinderInfo|kFSCatInfoDataSizes|kFSCatInfoNodeFlags,
			&MyRec,NULL,NULL,&uName);
	if (!eErr) {
		m_bDir = FALSE;
		m_bLocked = FALSE;
		m_bSystem = FALSE;
		m_bHidden = FALSE;
		if (MyRec.nodeFlags&kFSNodeIsDirectoryMask) {	/* Is this a directory */
			m_bDir = TRUE;
			m_uFileSize = 0;
			m_uFileType = 0;
			m_uAuxType = 0;
		} else {
			m_bDir = FALSE;
			m_uFileSize = static_cast<WordPtr>(MyRec.dataLogicalSize);
			m_uFileType = ((FileInfo *)(MyRec.finderInfo))->fileType;
			m_uAuxType = ((FileInfo *)(MyRec.finderInfo))->fileCreator;
		}
		if (MyRec.nodeFlags&kFSNodeIsDirectoryMask&kFSNodeLockedMask) {
			m_bLocked = TRUE;
		}
//		if (MyRec.dirInfo.ioACUser & 0x03) {	/* Can't see folder/file */
//			m_bHidden = TRUE;
//		}
		m_CreationDate.Load(&MyRec.createDate);
		m_ModificatonDate.Load(&MyRec.contentModDate);
		Burger::UTF8::FromUTF16(m_Name,sizeof(m_Name),uName.unicode,uName.length*2U);	/* Copy the filename */
		return FALSE;
	}
	return TRUE;			// All done!
}

/***************************************

	Close a directory that's being scanned

***************************************/

void Burger::DirectorySearch::Close(void)
{
	if (m_pIterator) {
		FSCloseIterator(m_pIterator);
		m_pIterator = NULL;
	}
}

#endif