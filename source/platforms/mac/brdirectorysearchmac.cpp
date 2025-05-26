/***************************************

	MacOS version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brfile.h"
#include "brmemoryfunctions.h"
#include "brstring16.h"
#include "brutf8.h"

#include <Files.h>

/***************************************

	\brief Open a directory for scanning

	\param pDirName Pointer to the "C" string filename in BurgerLib format.

	\return Zero on success, non-zero on an error or if the directory doesn't
		exist

	\sa close(void) or get_next_entry(void)

***************************************/

Burger::eError Burger::DirectorySearch::open(Filename* pDirName) BURGER_NOEXCEPT
{
	DirectoryEntry_t Entry;

	// Make sure the class is in a default state
	close();

	eError uResult;

#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Try MacOS 9 or higher
	FSRef* pFSRef = pDirName->get_FSRef();
	if (pFSRef) {

		// Init the buffer
		m_Entries.reserve(Burger::DirectorySearch::kDefaultReserve);

		// Got the FSRef, so lets try this
		OpaqueFSIterator* pIterator;

		OSErr iError = FSOpenIterator(pFSRef, kFSIterateFlat, &pIterator);
		if (iError) {
			uResult = platform_convert_to_error(iError);

		} else {
			ItemCount Count;
			Boolean bChanged;
			FSCatalogInfo MyRec;
			HFSUniStr255 uName;

			uResult = kErrorNone;

			while (!FSGetCatalogInfoBulk(pIterator, 1, &Count, &bChanged,
				kFSCatInfoCreateDate | kFSCatInfoContentMod |
					kFSCatInfoFinderInfo | kFSCatInfoDataSizes |
					kFSCatInfoNodeFlags,
				&MyRec, nullptr, nullptr, &uName)) {

				// Is this a directory?
				if (MyRec.nodeFlags & kFSNodeIsDirectoryMask) {
					Entry.m_bDir = TRUE;
					Entry.m_uFileSize = 0;
					Entry.m_uFileType = 0;
					Entry.m_uCreatorType = 0;
				} else {
					Entry.m_bDir = FALSE;
					Entry.m_uFileSize =
						static_cast<uint64_t>(MyRec.dataLogicalSize);
					Entry.m_uFileType =
						((FileInfo*)(MyRec.finderInfo))->fileType;
					Entry.m_uCreatorType =
						((FileInfo*)(MyRec.finderInfo))->fileCreator;
				}
				Entry.m_bLocked =
					(MyRec.nodeFlags &
						(kFSNodeIsDirectoryMask | kFSNodeLockedMask)) != 0;

#if 0
				if (MyRec.dirInfo.ioACUser & 0x03) {	// Can't see folder/file
					Entry.m_bHidden = TRUE;
				} else
#else
				{
					Entry.m_bHidden = FALSE;
				}
#endif
				Entry.m_bSystem = FALSE;

				Entry.m_CreationDate.Load(&MyRec.createDate);
				Entry.m_ModificatonDate.Load(&MyRec.contentModDate);

				String TempName;
				// Convert the filename to UTF8
				uResult = TempName.assign(uName.unicode, uName.length);
				if (uResult) {
					break;
				}

				// Make a copy of the string and store it
				const char* pName = string_duplicate(TempName.c_str());
				if (!pName) {
					uResult = kErrorOutOfMemory;
					break;
				}
				Entry.m_pName = pName;

				// Add to the list
				m_Entries.append(&Entry, 1);
			}
		}
		FSCloseIterator(pIterator);
	} else
#endif
	{
		// Create a CInfoPBRec for iterating. No actual work is done at this
		// moment.
		FSSpec MySpec;
		pDirName->get_FSSpec(&MySpec);

		// Get the directory's real id
		long lNewDirID;
		uint_t bDirectory;
		int iMacError = GetDirectoryID(&MySpec, &lNewDirID, &bDirectory);
		// Convert the error code
		uResult = platform_convert_to_error(iMacError);
		if (!uResult) {
			// Can't iterate a file
			if (!bDirectory) {
				uResult = kErrorNotADirectory;
			} else {
				// Prep for directory traversal
				Str255 Name;
				CInfoPBRec InfoPBRec;

				memory_clear(&InfoPBRec, sizeof(InfoPBRec));
				InfoPBRec.hFileInfo.ioNamePtr = Name;
				InfoPBRec.hFileInfo.ioVRefNum = MySpec.vRefNum;
				short iIndex = 1;
				uResult = kErrorNone;

				for (;;) {
					InfoPBRec.dirInfo.ioDrDirID = lNewDirID;
					InfoPBRec.dirInfo.ioFDirIndex = iIndex;
					iMacError = PBGetCatInfoSync(&InfoPBRec);
					if (iMacError) {
						uResult = kErrorNone;
						break;
					}

					// Is this a directory?
					if (InfoPBRec.hFileInfo.ioFlAttrib & kioFlAttribDirMask) {
						Entry.m_bDir = TRUE;
						Entry.m_uFileSize = 0;
						Entry.m_uFileType = 0;
						Entry.m_uCreatorType = 0;
					} else {
						Entry.m_bDir = FALSE;
						Entry.m_uFileSize = static_cast<uint64_t>(
							InfoPBRec.hFileInfo.ioFlLgLen);
						Entry.m_uFileType =
							InfoPBRec.hFileInfo.ioFlFndrInfo.fdType;
						Entry.m_uCreatorType =
							InfoPBRec.hFileInfo.ioFlFndrInfo.fdCreator;
					}

					Entry.m_bLocked = (InfoPBRec.hFileInfo.ioFlAttrib &
										  kioFlAttribLockedMask) != 0;
					Entry.m_bSystem = FALSE;
					Entry.m_bHidden =
						(InfoPBRec.hFileInfo.ioFlFndrInfo.fdFlags &
							kIsInvisible) != 0;
					Entry.m_CreationDate.LoadFileSeconds(
						InfoPBRec.hFileInfo.ioFlCrDat);
					Entry.m_ModificatonDate.LoadFileSeconds(
						InfoPBRec.hFileInfo.ioFlMdDat);

					// Copy the filename
					String TempString;
					uResult = TempString.assign_mac_roman_us(
						reinterpret_cast<const char*>(&Name[1]), Name[0]);

					// Make a copy of the string and store it
					const char* pName = string_duplicate(TempString.c_str());
					if (!pName) {
						uResult = kErrorOutOfMemory;
						break;
					}
					Entry.m_pName = pName;

					// Add to the list
					m_Entries.append(&Entry, 1);

					++iIndex;
				}
			}
		}
	}
	return uResult;
}

#endif
