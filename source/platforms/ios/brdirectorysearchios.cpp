/***************************************

	iOS version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"

#if defined(BURGER_IOS) || defined(DOXYGEN)
#include "brfile.h"
#include "brmemoryfunctions.h"
#include "brstringfunctions.h"
#include <fcntl.h>
#include <sys/attr.h>
#include <sys/stat.h>
#include <unistd.h>
// #include <sys/vnode.h>

#if !defined(DOXYGEN)
//
// vnode is missing, so include the values directly
//
// #include <sys/vnode.h>
enum vtype {
	VNON,
	VREG,
	VDIR,
	VBLK,
	VCHR,
	VLNK,
	VSOCK,
	VFIFO,
	VBAD,
	VSTR,
	VCPLX
};

// Structure declaration of data coming from getdirentriesattr()
struct FInfoAttrBuf {
	uint32_t length;                   // Length of this data structure
	attrreference_t name;              // Offset for the filename
	fsobj_type_t objType;              // VREG for file, VREG for directory
	struct timespec m_CreationDate;    // Creation date
	struct timespec m_ModificatonDate; // Modification date
	char finderInfo[32];               // Aux/File type are the first 8 bytes
	u_long m_Flags;                    // Hidden and locked flags
	off_t m_uFileSize;                 // Filesize
	char m_Name[256];
};

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

	DirectoryEntry_t Entry;

	// Open the directory for reading
	int fp = ::open(pDirName->get_native(), O_RDONLY, 0);
	if (fp != -1) {

		// Attributes requested
		attrlist AttributesList;

		// Initialize the attributes list
		memory_clear(&AttributesList, sizeof(AttributesList));

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

		int iError;
		do {

			// Buffer to hold the attributes and the filename
			FInfoAttrBuf DirEntry;

			// For some dumb reason, SDK 10.5 insists this is declared unsigned
			// int on 64 bit CPUs
#if defined(BURGER_64BITCPU)
			unsigned int uCount = 1;
			unsigned int uJunkBase;
			unsigned int uNewState;
#else
			unsigned long uCount = 1; // Load only a single directory entry
			unsigned long uJunkBase;
			unsigned long uNewState;
#endif

			// Get the directory entry
			iError = getdirentriesattr(fp, &AttributesList, &DirEntry,
				sizeof(DirEntry), &uCount, &uJunkBase, &uNewState, NULL);

			// No errors and an entry was returned?
			// Note: iError is 0 if more data is pending, 1 if this is the last
			// entry. uCount is zero when no entry is loaded

			if (iError < 0) {
				uResult = kErrorReadFailure;
				break;
			}

			if (!uCount) {
				break;
			}

			// First, grab the filename
			char* pName =
				StringDuplicate(reinterpret_cast<char*>(&DirEntry.name) +
					DirEntry.name.attr_dataoffset);

			// Failed to allocate memory for the filename?
			if (!pName) {
				uResult = kErrorOutOfMemory;
				break;
			}
			Entry.m_pName = pName;

			// Is this a directory?
			if (DirEntry.objType == VDIR) {
				Entry.m_bDir = TRUE;
				Entry.m_uFileSize = 0;
			} else {
				Entry.m_bDir = FALSE;
				// Get the file size (clamp on 32 bit systems)
				Entry.m_uFileSize = static_cast<uint64_t>(DirEntry.m_uFileSize);
			}
			// Get the file dates
			Entry.m_CreationDate.Load(&DirEntry.m_CreationDate);
			Entry.m_ModificatonDate.Load(&DirEntry.m_ModificatonDate);

			// Not valid on the Mac
			Entry.m_bSystem = FALSE;

			// 0x4000 = kIsInvisible finder flag (Big Endian!!!)
			Entry.m_bHidden = ((pName[0] == '.') ||
				((reinterpret_cast<const uint8_t*>(DirEntry.finderInfo)[8] &
					 0x40U) != 0));

			// Is the file locked?
			Entry.m_bLocked = (DirEntry.m_Flags & UF_IMMUTABLE) != 0;

			// Get the mac specific file type and creator type
			Entry.m_uFileType =
				reinterpret_cast<uint32_t*>(DirEntry.finderInfo)[0];
			Entry.m_uCreatorType =
				reinterpret_cast<uint32_t*>(DirEntry.finderInfo)[1];

			m_Entries.append(&Entry, 1);

			// If iError is 1, then what is in the buffer is all that remains.
			// if iError is 0, then there is still more data pending which needs
			// another pass
		} while (!iError);
		::close(fp);
	}
	return uResult;
}

#endif
