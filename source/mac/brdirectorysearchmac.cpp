/***************************************

	MacOS version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brfile.h"
#include "brstring16.h"
#include "brutf8.h"
#include <Files.h>

#if !defined(DOXYGEN)
struct Local_t {
	CInfoPBRec m_InfoPBRec;
	uint8_t m_Name[256];
	long m_parID;
	short m_iIndex;
};

#endif

/***************************************

	Open a directory for scanning
	Return an error if the directory doesn't exist

***************************************/

Burger::eError Burger::DirectorySearch::Open(Filename* pDirName) BURGER_NOEXCEPT
{
	// Make sure the class is in a default state
	Close();

#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Try MacOS 9 or higher
	FSRef* pFSRef = pDirName->GetFSRef();
	if (pFSRef) {
		// Got the FSRef, so lets try this
		return MacConvertError(
			FSOpenIterator(pFSRef, kFSIterateFlat, &m_pIterator));
	}
#endif

	// Create a CInfoPBRec for iterating. No actual work is done at this moment.
	Local_t* pLocal = static_cast<Local_t*>(AllocClear(sizeof(Local_t)));
	if (!pLocal) {
		return kErrorOutOfMemory;
	}
	m_pLocal = pLocal;

	FSSpec MySpec;
	pDirName->GetFSSpec(&MySpec);

	// Get the directory's real id
	long lNewDirID;
	uint_t bDirectory;
	int iMacError = GetDirectoryID(
		&MySpec, &lNewDirID, &bDirectory);
	// Convert the error code
	eError uResult = MacConvertError(iMacError);
	if (!uResult) {
		// Can't iterate a file
		if (!bDirectory) {
			uResult = kErrorNotADirectory;
		} else {
			// Prep for directory traversal
			pLocal->m_InfoPBRec.hFileInfo.ioNamePtr = pLocal->m_Name;
			pLocal->m_InfoPBRec.hFileInfo.ioVRefNum = MySpec.vRefNum;
			pLocal->m_parID = lNewDirID;
			pLocal->m_iIndex = 1;
			uResult = kErrorNone;
		}
	}

	// Clean up on error
	if (uResult) {
		Close();
	}
	return uResult;
}

/***************************************

	Get the next directory entry
	Return FALSE if the entry is valid, TRUE if
	an error occurs.

***************************************/

Burger::eError Burger::DirectorySearch::GetNextEntry(void) BURGER_NOEXCEPT
{
	int iMacError;

	m_bDir = FALSE;
	m_bLocked = FALSE;
	m_bSystem = FALSE;
	m_bHidden = FALSE;

#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	if (m_pIterator) {
		ItemCount Count;
		Boolean bChanged;
		FSCatalogInfo MyRec;
		HFSUniStr255 uName;

		iMacError = FSGetCatalogInfoBulk(m_pIterator, 1, &Count, &bChanged,
			kFSCatInfoCreateDate | kFSCatInfoContentMod | kFSCatInfoFinderInfo |
				kFSCatInfoDataSizes | kFSCatInfoNodeFlags,
			&MyRec, nullptr, nullptr, &uName);
		if (!iMacError) {

			// Is this a directory?
			if (MyRec.nodeFlags & kFSNodeIsDirectoryMask) {
				m_bDir = TRUE;
				m_uFileSize = 0;
				m_uFileType = 0;
				m_uAuxType = 0;
			} else {
				// m_bDir = FALSE;
				m_uFileSize = static_cast<uintptr_t>(MyRec.dataLogicalSize);
				m_uFileType = ((FileInfo*)(MyRec.finderInfo))->fileType;
				m_uAuxType = ((FileInfo*)(MyRec.finderInfo))->fileCreator;
			}
			if (MyRec.nodeFlags & kFSNodeIsDirectoryMask & kFSNodeLockedMask) {
				m_bLocked = TRUE;
			}
			// if (MyRec.dirInfo.ioACUser & 0x03) {	// Can't see folder/file
			//	m_bHidden = TRUE;
			//}
			m_CreationDate.Load(&MyRec.createDate);
			m_ModificatonDate.Load(&MyRec.contentModDate);

			// Copy the filename
			UTF8::FromUTF16(
				m_Name, sizeof(m_Name), uName.unicode, uName.length);
			return kErrorNone;
		}
	}
#endif
	// Iterate the directory the old way?
	Local_t* pLocal = static_cast<Local_t*>(m_pLocal);
	if (pLocal) {
		pLocal->m_InfoPBRec.dirInfo.ioDrDirID = pLocal->m_parID;
		pLocal->m_InfoPBRec.dirInfo.ioFDirIndex = pLocal->m_iIndex;
		iMacError = PBGetCatInfoSync(&pLocal->m_InfoPBRec);
		if (!iMacError) {

			// Is this a directory?
			if (pLocal->m_InfoPBRec.hFileInfo.ioFlAttrib & kioFlAttribDirMask) {
				m_bDir = TRUE;
				m_uFileSize = 0;
				m_uFileType = 0;
				m_uAuxType = 0;
			} else {
				m_uFileSize = static_cast<uintptr_t>(
					pLocal->m_InfoPBRec.hFileInfo.ioFlLgLen);
				m_uFileType = pLocal->m_InfoPBRec.hFileInfo.ioFlFndrInfo.fdType;
				m_uAuxType =
					pLocal->m_InfoPBRec.hFileInfo.ioFlFndrInfo.fdCreator;
			}
			m_CreationDate.LoadFileSeconds(
				pLocal->m_InfoPBRec.hFileInfo.ioFlCrDat);
			m_ModificatonDate.LoadFileSeconds(
				pLocal->m_InfoPBRec.hFileInfo.ioFlMdDat);

			// Copy the filename
			UTF8::FromMacRomanUS(m_Name, sizeof(m_Name),
				reinterpret_cast<const char*>(&pLocal->m_Name[1]),
				pLocal->m_Name[0]);
			++pLocal->m_iIndex;
			return kErrorNone;
		}
	}
	// All done!
	return kErrorNotEnumerating;
}

/***************************************

	Close a directory that's being scanned

***************************************/

void Burger::DirectorySearch::Close(void) BURGER_NOEXCEPT
{
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	if (m_pIterator) {
		FSCloseIterator(m_pIterator);
		m_pIterator = nullptr;
	}
#endif
	// Release the temp memory
	Free(m_pLocal);
	m_pLocal = nullptr;
}

#endif
