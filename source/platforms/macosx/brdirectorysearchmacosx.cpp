/***************************************

	MacOS version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
#include "macosx_version.h"

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
	uint32_t m_uLength;                // Length of this data structure
	attrreference_t m_Name;            // Offset for the filename
	fsobj_type_t m_uObjType;           // VREG for file, VREG for directory
	struct timespec m_CreationDate;    // Creation date
	struct timespec m_ModificatonDate; // Modification date
	uint32_t m_FinderInfo[8];          // Aux/File type are the first 8 bytes
	uint32_t m_Flags;                  // Hidden and locked flags
	off_t m_uFileSize;                 // Filesize
} __attribute__((packed));

struct BulkAttr {
	uint32_t m_uLength;                // Length of this data structure
	attribute_set_t m_Returned;        // Flags for which entries returned
	uint32_t m_uError;                 // Error code for this entry
	attrreference_t m_Name;            // Offset for the filename
	fsobj_type_t m_uObjType;           // VREG for file, VREG for directory
	struct timespec m_CreationDate;    // Creation date
	struct timespec m_ModificatonDate; // Modification date
	uint32_t m_FinderInfo[8];          // Aux/File type are the first 8 bytes
	uint32_t m_Flags;                  // Hidden and locked flags
	off_t m_uFileSize;                 // Filesize
} __attribute__((packed));
#endif

/***************************************

	\brief Parse directory using the old API

	Iterate over the directories using the old macOS API. This function will
	only work on macOS 10.0 to 10.9. 10.10 and later deprecates
	getdirentriesattr()

	\param fp Open directory reference
	\param pAttrlist Pointer to a prefilled attrlist
	\param pOutput Pointer to the DirectoryEntry_t array for output

	\return Zero on success, non-zero on an error or if the directory doesn't
		exist

***************************************/

static Burger::eError BURGER_API OldWay(int fp, attrlist* pAttrlist,
	Burger::SimpleArray<Burger::DirectoryEntry_t>* pOutput)
{
	// Buffer of directory entries
	uint8_t Buffer[(sizeof(FInfoAttrBuf) + 256U) * 8U];

	// Single entry for pushing onto the output
	Burger::DirectoryEntry_t Entry;

	// Assume no error
	Burger::eError uResult = Burger::kErrorNone;
	int iError;

	do {
		// Call getdirentriesattr() for macOS 10.0 through 10.9
		// Load only a single directory entry
		WordAttr uCount = static_cast<WordAttr>(
			sizeof(Buffer) / (sizeof(FInfoAttrBuf) + 256U));
		WordAttr uJunkBase = 0;
		WordAttr uNewState = 0;

		// Get the directory entrys
		iError = getdirentriesattr(fp, pAttrlist, Buffer, sizeof(Buffer),
			&uCount, &uJunkBase, &uNewState, 0);

		// Fail on error
		if (iError < 0) {
			uResult = Burger::kErrorReadFailure;
			break;
		}

		// Get the number of entries obtained
		WordAttr uEntryCount = uCount;
		if (!uEntryCount) {
			// End of directory?
			break;
		}

		// Since entries were loaded, parse them
		const uint8_t* pBuffer = Buffer;
		do {
			const FInfoAttrBuf* pDirEntry =
				reinterpret_cast<const FInfoAttrBuf*>(pBuffer);

			// Make a copy of the string and store it
			const char* pName = Burger::StringDuplicate(
				reinterpret_cast<const char*>(&pDirEntry->m_Name) +
				pDirEntry->m_Name.attr_dataoffset);

			// Failed to allocate memory for the filename?
			if (!pName) {
				uResult = Burger::kErrorOutOfMemory;
				break;
			}
			Entry.m_pName = pName;

			// Is this a directory?
			if (pDirEntry->m_uObjType == VDIR) {
				Entry.m_bDir = TRUE;
				Entry.m_uFileSize = 0;
			} else {
				Entry.m_bDir = FALSE;
				// Get the file size
				Entry.m_uFileSize =
					static_cast<uint64_t>(pDirEntry->m_uFileSize);
			}

			// Get the file dates
			Entry.m_CreationDate.Load(&pDirEntry->m_CreationDate);
			Entry.m_ModificatonDate.Load(&pDirEntry->m_ModificatonDate);

			// 0x4000 = kIsInvisible finder flag (Big Endian!!!)
			Entry.m_bHidden = ((pName[0] == '.') ||
				((reinterpret_cast<const uint8_t*>(pDirEntry->m_FinderInfo)[8] &
					 0x40U) != 0));

			// Is the file locked?
			Entry.m_bLocked = (pDirEntry->m_Flags & UF_IMMUTABLE) != 0;

			Entry.m_bSystem = FALSE;

			// Get the mac specific file type and creator type
			Entry.m_uFileType = pDirEntry->m_FinderInfo[0];
			Entry.m_uCreatorType = pDirEntry->m_FinderInfo[1];

			// Accept the entry
			pOutput->append(&Entry, 1);

			// Skip to the next entry
			pBuffer = reinterpret_cast<const uint8_t*>(pDirEntry) +
				pDirEntry->m_uLength;
		} while (--uEntryCount);

		// Was there an error?
		if (uResult) {
			break;
		}

		// If iError is 1, then what is in the buffer is all that remains.
		// if iError is 0, then there is still more data pending which needs
		// another pass
	} while (!iError);
	return uResult;
}

/***************************************

	\brief Parse directory using the new API

	On macOS 10.10 or later, getattrlistbulk() is the function to use. Since the
	executable cannot directly link to the function, since it would break
	compatiblity with 10.5, use a function that will dynamically location and
	call the function.

	\note This function doesn't exist on PowerPC builds, since there is no
		PowerPC version of macOS 10.10

	\param fp Open directory reference
	\param pAttrlist Pointer to a prefilled attrlist
	\param pOutput Pointer to the DirectoryEntry_t array for output

	\return Zero on success, non-zero on an error or if the directory doesn't
		exist

***************************************/

#if !defined(BURGER_PPC)
static Burger::eError BURGER_API NewWay(int fp, attrlist* pAttrlist,
	Burger::SimpleArray<Burger::DirectoryEntry_t>* pOutput)
{
	// Buffer of directory entries
	uint8_t Buffer[2048];

	// Single entry for pushing onto the output
	Burger::DirectoryEntry_t Entry;

	// MacOSX 10.10 version of the code using getattrlistbulk()

	pAttrlist->commonattr |= ATTR_CMN_RETURNED_ATTRS | ATTR_CMN_ERROR;

	// Assume no error
	Burger::eError uResult = Burger::kErrorNone;
	do {

		// Load the entries
		int iError = Burger::MacOSXOld::getattrlistbulk(
			fp, pAttrlist, Buffer, sizeof(Buffer), FSOPT_PACK_INVAL_ATTRS);

		// Error? Abort.
		if (iError < 0) {
			uResult = Burger::kErrorReadFailure;
			break;
		}

		// If there are no more entries, exit gracefully
		if (!iError) {
			break;
		}

		// Unsigned and assume non-zero
		uint_t uEntryCount = static_cast<uint_t>(iError);
		const uint8_t* pBuffer = Buffer;
		do {
			// Handle 10.10
			const BulkAttr* pDirEntry =
				reinterpret_cast<const BulkAttr*>(pBuffer);

			// Starting point is after the length
			const char* pWork = reinterpret_cast<const char*>(pBuffer) +
				sizeof(uint32_t) + sizeof(attribute_set_t);

			if (pDirEntry->m_Returned.commonattr & ATTR_CMN_ERROR) {
				pWork += sizeof(uint32_t);
			}

			// First, grab the filename

			const char* pName;
			Entry.m_bSystem = FALSE;
			if (pDirEntry->m_Returned.commonattr & ATTR_CMN_NAME) {
				pName = pWork +
					reinterpret_cast<const attrreference_t*>(pWork)
						->attr_dataoffset;
				pWork += sizeof(attrreference_t);
				Entry.m_bHidden = (pName[0] == '.');
			} else {
				pName = "";
				Entry.m_bHidden = FALSE;
			}

			// Copy the string
			pName = Burger::StringDuplicate(pName);
			if (!pName) {
				uResult = Burger::kErrorOutOfMemory;
				break;
			}
			Entry.m_pName = pName;
			
			Entry.m_bDir = FALSE;
			if (pDirEntry->m_Returned.commonattr & ATTR_CMN_OBJTYPE) {
				fsobj_type_t uObj =
					reinterpret_cast<const fsobj_type_t*>(pWork)[0];
				if (uObj == VDIR) {
					Entry.m_bDir = TRUE;
				}
				pWork += sizeof(fsobj_type_t);
			}

			if (pDirEntry->m_Returned.commonattr & ATTR_CMN_CRTIME) {
				Entry.m_CreationDate.Load(
					reinterpret_cast<const struct timespec*>(pWork));
				pWork += sizeof(struct timespec);
			} else {
				Entry.m_CreationDate.Clear();
			}

			if (pDirEntry->m_Returned.commonattr & ATTR_CMN_MODTIME) {
				Entry.m_ModificatonDate.Load(
					reinterpret_cast<const struct timespec*>(pWork));
				pWork += sizeof(struct timespec);
			} else {
				Entry.m_ModificatonDate.Clear();
			}

			if (pDirEntry->m_Returned.commonattr & ATTR_CMN_FNDRINFO) {
				// Get the mac specific file type and creator type
				Entry.m_uFileType = reinterpret_cast<const uint32_t*>(pWork)[0];
				Entry.m_uCreatorType =
					reinterpret_cast<const uint32_t*>(pWork)[1];

				// 0x4000 = kIsInvisible finder flag (Big Endian!!!)
				if (pWork[8] & 0x40U) {
					Entry.m_bHidden = TRUE;
				}
				pWork += 32;
			} else {
				Entry.m_uCreatorType = 0;
				Entry.m_uFileType = 0;
			}

			if (pDirEntry->m_Returned.commonattr & ATTR_CMN_FLAGS) {
				// Is the file locked?
				Entry.m_bLocked = (reinterpret_cast<const uint32_t*>(pWork)[0] &
									  UF_IMMUTABLE) != 0;
				pWork += sizeof(uint32_t);
			} else {
				Entry.m_bLocked = FALSE;
			}

			Entry.m_uFileSize = 0;
			if (pDirEntry->m_Returned.fileattr & ATTR_FILE_DATALENGTH) {
				if (!Entry.m_bDir) {
					// Get the file size (clamp on 32 bit systems)
					Entry.m_uFileSize =
						reinterpret_cast<const off_t*>(pWork)[0];
				}
				// pWork += sizeof(off_t);
			}

			// Accept the entry
			pOutput->append(&Entry, 1);

			// Next entry
			pBuffer = reinterpret_cast<const uint8_t*>(pDirEntry) +
				pDirEntry->m_uLength;
		} while (--uEntryCount);
	} while (!uResult);

	return uResult;
}
#endif

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

	eError uResult = kErrorFileNotFound;
	// Open the directory for reading
	int fp = ::open(pDirName->get_native(), O_RDONLY, 0);
	if (fp != -1) {

		// Attributes requested
		attrlist AttributesList;
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
		// MacOSX Version 10.10 or higher uses a new API
		// Only available for Intel or ARM
		const uint32_t uOSVersion = MacOSX::get_os_version();
#endif

		// Since PowerPC only goes to macOS 10.5, no need to implement the 10.10
		// code
#if !defined(BURGER_PPC)
		if (uOSVersion < 0x10A0) {
#endif

			// Read the directory using an API for 10.0 to 10.9
			uResult = OldWay(fp, &AttributesList, &m_Entries);

#if !defined(BURGER_PPC)
		} else {
			// MacOSX 10.10 version of the code using getattrlistbulk()
			// Why? Because Apple said so.
			uResult = NewWay(fp, &AttributesList, &m_Entries);
		}
#endif

		// Close the directory
		::close(fp);
	}
	return uResult;
}

#endif
