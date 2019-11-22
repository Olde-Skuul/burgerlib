/***************************************

	MacOS version

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
#include "brfile.h"
#include "brglobals.h"
#include "brmemoryfunctions.h"
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/attr.h>
#include <sys/stat.h>
#include <sys/vnode.h>
#include <unistd.h>

// Type to handle data type for the call to getdirentriesattr()

#if !defined(DOXYGEN)

#ifndef ATTR_CMN_RETURNED_ATTRS
#define ATTR_CMN_RETURNED_ATTRS 0x80000000
#endif

#ifndef ATTR_CMN_ERROR
#define ATTR_CMN_ERROR 0x20000000
#endif

#ifndef FSOPT_PACK_INVAL_ATTRS
#define FSOPT_PACK_INVAL_ATTRS 0x00000008
#endif

#if defined(BURGER_64BITCPU)
typedef unsigned int WordAttr;
#else
typedef unsigned long WordAttr;
#endif

// Structure declaration of data coming from getdirentriesattr()
struct FInfoAttrBuf {
	Word32 m_uLength;				   // Length of this data structure
	attrreference_t m_Name;			   // Offset for the filename
	fsobj_type_t m_uObjType;		   // VREG for file, VREG for directory
	struct timespec m_CreationDate;	// Creation date
	struct timespec m_ModificatonDate; // Modification date
	Word32 m_FinderInfo[8];			   // Aux/File type are the first 8 bytes
	Word32 m_Flags;					   // Hidden and locked flags
	off_t m_uFileSize;				   // Filesize
} __attribute__((packed));

struct BulkAttr {
	Word32 m_uLength;				   // Length of this data structure
	attribute_set_t m_Returned;		   // Flags for which entries returned
	Word32 m_uError;				   // Error code for this entry
	attrreference_t m_Name;			   // Offset for the filename
	fsobj_type_t m_uObjType;		   // VREG for file, VREG for directory
	struct timespec m_CreationDate;	// Creation date
	struct timespec m_ModificatonDate; // Modification date
	Word32 m_FinderInfo[8];			   // Aux/File type are the first 8 bytes
	Word32 m_Flags;					   // Hidden and locked flags
	off_t m_uFileSize;				   // Filesize
} __attribute__((packed));
#endif

/***************************************

	Using a Burgerlib path, open a directory to scan it
	for filenames

***************************************/

Word Burger::DirectorySearch::Open(Filename* pDirName)
{
	// Make sure there's nothing pending
	Close();
	Word uResult = File::FILENOTFOUND;
	// Open the directory for reading
	int fp = open(pDirName->GetNative(), O_RDONLY, 0);
	if (fp != -1) {
		m_fp = fp;
		uResult = File::OKAY;
	}
	m_bDone = FALSE;
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
	if (fp != -1 && !m_bDone) {

#if !defined(BURGER_PPC)
		// MacOSX Version 10.10 or higher uses a new API
		Word uOSVersion = Globals::GetMacOSVersion();
#endif

		int eError = 0;
		if (!m_uEntryCount) {

			// Attributes requested
			attrlist AttributesList;

			// Initialize the attributes list
			MemoryClear(&AttributesList, sizeof(AttributesList));

			// "sizeof" for the structure
			AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

			// Let's get the name, type of file, creation time, modification
			// time, finder information and hidden/locked flags

			// Note: If these flags are changed, the FInfoAttrBuf MUST be
			// adjusted to reflect the request or weird stuff will happen
			AttributesList.commonattr = ATTR_CMN_NAME | ATTR_CMN_OBJTYPE |
				ATTR_CMN_CRTIME | ATTR_CMN_MODTIME | ATTR_CMN_FNDRINFO |
				ATTR_CMN_FLAGS;
			AttributesList.fileattr = ATTR_FILE_DATALENGTH;

#if !defined(BURGER_PPC)
			if (uOSVersion < 0x10A0) {
#endif

				// Call getdirentriesattr() for macOS 10.0 through 10.9
				WordAttr uCount = static_cast<WordAttr>(sizeof(m_Buffer) /
					(sizeof(FInfoAttrBuf) +
						sizeof(m_Name))); // Load only a single directory entry
				WordAttr uJunkBase;
				WordAttr uNewState;

				// Get the directory entry
				eError = getdirentriesattr(fp, &AttributesList, m_Buffer,
					sizeof(m_Buffer), &uCount, &uJunkBase, &uNewState, 0);
				if (eError < 0) {
					eError = errno;
				} else {
					m_uEntryCount = uCount;
					eError = 0;
				}

#if !defined(BURGER_PPC)
			} else {
				// MacOSX 10.10 version of the code using getattrlistbulk()

				AttributesList.commonattr |=
					ATTR_CMN_RETURNED_ATTRS | ATTR_CMN_ERROR;
				eError = MacOSX::getattrlistbulk(fp, &AttributesList, m_Buffer,
					sizeof(m_Buffer), FSOPT_PACK_INVAL_ATTRS);
				if (eError < 0) {
					eError = errno;
				} else {
					m_uEntryCount = eError;
					eError = 0;
				}
			}
#endif
			m_pEntry = m_Buffer;
		}

		// No errors and an entry is in the cache?
		// Note: eError is 0 if more data is pending, 1 if this is the last
		// entry. uCount is zero when no entry is loaded

		if (!eError && m_uEntryCount) {

			// Not valid on the Mac
			m_bSystem = FALSE;

#if !defined(BURGER_PPC)
			if (uOSVersion < 0x10A0) {
#endif

				const FInfoAttrBuf* pEntry =
					static_cast<const FInfoAttrBuf*>(m_pEntry);
				// First, grab the filename
				StringCopy(m_Name, sizeof(m_Name),
					reinterpret_cast<const char*>(&pEntry->m_Name) +
						pEntry->m_Name.attr_dataoffset);

				// Is this a directory?
				if (pEntry->m_uObjType == VDIR) {
					m_bDir = TRUE;
					m_uFileSize = 0;
				} else {
					m_bDir = FALSE;
					// Get the file size (clamp on 32 bit systems)
#if defined(BURGER_64BITCPU)
					m_uFileSize = pEntry->m_uFileSize;
#else
				if (pEntry->m_uFileSize >= 0xFFFFFFFFUL) {
					m_uFileSize = 0xFFFFFFFFUL;
				} else {
					m_uFileSize = static_cast<WordPtr>(pEntry->m_uFileSize);
				}
#endif
				}
				// Get the file dates
				m_CreationDate.Load(&pEntry->m_CreationDate);
				m_ModificatonDate.Load(&pEntry->m_ModificatonDate);

				// 0x4000 = kIsInvisible finder flag (Big Endian!!!)
				m_bHidden = ((m_Name[0] == '.') ||
					((reinterpret_cast<const Word8*>(pEntry->m_FinderInfo)[8] &
						 0x40U) != 0));

				// Is the file locked?
				m_bLocked = (pEntry->m_Flags & UF_IMMUTABLE) != 0;

				// Get the mac specific file type and creator type
				m_uFileType = pEntry->m_FinderInfo[0];
				m_uAuxType = pEntry->m_FinderInfo[1];
				--m_uEntryCount;
				m_pEntry =
					reinterpret_cast<const Word8*>(pEntry) + pEntry->m_uLength;
#if !defined(BURGER_PPC)
			} else {
				// Handle 10.10
				const BulkAttr* pEntry = static_cast<const BulkAttr*>(m_pEntry);
				// Starting point is after the length
				const char* pWork = static_cast<const char*>(m_pEntry) +
					sizeof(Word32) + sizeof(attribute_set_t);

				if (pEntry->m_Returned.commonattr & ATTR_CMN_ERROR) {
					pWork += sizeof(Word32);
				}

				// First, grab the filename
				m_Name[0] = 0;
				m_bHidden = FALSE;
				if (pEntry->m_Returned.commonattr & ATTR_CMN_NAME) {
					StringCopy(m_Name, sizeof(m_Name),
						pWork +
							reinterpret_cast<const attrreference_t*>(pWork)
								->attr_dataoffset);
					pWork += sizeof(attrreference_t);
					m_bHidden = (m_Name[0] == '.');
				}

				m_bDir = FALSE;
				if (pEntry->m_Returned.commonattr & ATTR_CMN_OBJTYPE) {
					fsobj_type_t uObj =
						reinterpret_cast<const fsobj_type_t*>(pWork)[0];
					if (uObj == VDIR) {
						m_bDir = TRUE;
					}
					pWork += sizeof(fsobj_type_t);
				}

				if (pEntry->m_Returned.commonattr & ATTR_CMN_CRTIME) {
					m_CreationDate.Load(
						reinterpret_cast<const struct timespec*>(pWork));
					pWork += sizeof(struct timespec);
				} else {
					m_CreationDate.Clear();
				}

				if (pEntry->m_Returned.commonattr & ATTR_CMN_MODTIME) {
					m_ModificatonDate.Load(
						reinterpret_cast<const struct timespec*>(pWork));
					pWork += sizeof(struct timespec);
				} else {
					m_ModificatonDate.Clear();
				}

				if (pEntry->m_Returned.commonattr & ATTR_CMN_FNDRINFO) {
					// Get the mac specific file type and creator type
					m_uFileType = reinterpret_cast<const Word32*>(pWork)[0];
					m_uAuxType = reinterpret_cast<const Word32*>(pWork)[1];

					// 0x4000 = kIsInvisible finder flag (Big Endian!!!)
					if (pWork[8] & 0x40U) {
						m_bHidden = TRUE;
					}
					pWork += 32;
				} else {
					m_uAuxType = 0;
					m_uFileType = 0;
				}

				if (pEntry->m_Returned.commonattr & ATTR_CMN_FLAGS) {
					// Is the file locked?
					m_bLocked = (reinterpret_cast<const Word32*>(pWork)[0] &
									UF_IMMUTABLE) != 0;
					pWork += sizeof(Word32);
				} else {
					m_bLocked = FALSE;
				}

				m_uFileSize = 0;
				if (pEntry->m_Returned.fileattr & ATTR_FILE_DATALENGTH) {
					if (!m_bDir) {
						// Get the file size (clamp on 32 bit systems)
#if defined(BURGER_64BITCPU)
						m_uFileSize = reinterpret_cast<const off_t*>(pWork)[0];
#else
						if (reinterpret_cast<const off_t*>(pWork)[0] >=
							0xFFFFFFFFUL) {
							m_uFileSize = 0xFFFFFFFFUL;
						} else {
							m_uFileSize = static_cast<Word32>(
								reinterpret_cast<const off_t*>(pWork)[0]);
						}
#endif
					}
					// pWork += sizeof(off_t);
				}

				// Next entry
				--m_uEntryCount;
				m_pEntry =
					reinterpret_cast<const Word8*>(pEntry) + pEntry->m_uLength;
			}
#endif
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
	if (fp != -1) {
		close(fp);
		m_fp = -1;
	}
	m_bDone = TRUE;
	m_uEntryCount = 0;
}

#endif
