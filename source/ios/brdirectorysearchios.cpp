/***************************************

    iOS version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"

#if defined(BURGER_IOS) || defined(DOXYGEN)
#include "brfile.h"
#include "brstringfunctions.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/attr.h>
//#include <sys/vnode.h>

//
// vnode is missing, so include the values directly
//
// #include <sys/vnode.h>
enum vtype { VNON, VREG, VDIR, VBLK, VCHR, VLNK, VSOCK, VFIFO, VBAD, VSTR, VCPLX };

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
	int fp = open(pDirName->GetNative(),O_RDONLY,0);
	if (fp!=-1) {
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
	if (fp!=-1) {
		
		// Structure declaration of data coming from getdirentriesattr()
		struct FInfoAttrBuf {
			uint32_t length;				// Length of this data structure
			attrreference_t name;		// Offset for the filename
			fsobj_type_t objType;		// VREG for file, VREG for directory
			struct timespec m_CreationDate;		// Creation date
			struct timespec m_ModificatonDate;	// Modification date
			char finderInfo[32];		// Aux/File type are the first 8 bytes
			u_long m_Flags;				// Hidden and locked flags
			off_t m_uFileSize;			// Filesize
			char m_Name[256];
		};

		// Attributes requested
		attrlist AttributesList;
		// Buffer to hold the attributes and the filename
		FInfoAttrBuf Entry;

		// Initialize the attributes list
		MemoryClear(&AttributesList,sizeof(AttributesList));
		// "sizeof" for the structure
		AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;
		// Let's get the name, type of file, creation time, modification time, finder information and hidden/locked flags

		// Note: If these flags are changed, the FInfoAttrBuf MUST be
		// adjusted to reflect the request or weird stuff will happen
		AttributesList.commonattr = ATTR_CMN_NAME | ATTR_CMN_OBJTYPE | ATTR_CMN_CRTIME | ATTR_CMN_MODTIME | ATTR_CMN_FNDRINFO | ATTR_CMN_FLAGS;
		AttributesList.fileattr = ATTR_FILE_DATALENGTH;
		
		// For some dumb reason, SDK 10.5 insists this is declared unsigned int on 64 bit CPUs
#if defined(BURGER_64BITCPU)
		unsigned int uCount = 1;
		unsigned int uJunkBase;
		unsigned int uNewState;
#else
		unsigned long uCount = 1;			// Load only a single directory entry
		unsigned long uJunkBase;
		unsigned long uNewState;
#endif

		// Get the directory entry
		int eError = getdirentriesattr(fp,&AttributesList,&Entry,sizeof(Entry),&uCount,&uJunkBase,&uNewState,NULL);

		// No errors and an entry was returned?
		// Note: eError is 0 if more data is pending, 1 if this is the last entry.
		// uCount is zero when no entry is loaded

		if (eError>=0 && uCount) {
			// First, grab the filename
			StringCopy(m_Name,sizeof(m_Name),reinterpret_cast<char *>(&Entry.name)+Entry.name.attr_dataoffset);

			// Is this a directory?
			if (Entry.objType==VDIR) {
				m_bDir = TRUE;
				m_uFileSize = 0;
			} else {
				m_bDir = FALSE;
			// Get the file size (clamp on 32 bit systems)
#if defined(BURGER_64BITCPU)
				m_uFileSize = Entry.m_uFileSize;
#else
				if (Entry.m_uFileSize>=0xFFFFFFFFUL) {
					m_uFileSize = 0xFFFFFFFFUL;
				} else {
					m_uFileSize = static_cast<uintptr_t>(Entry.m_uFileSize);
				}
#endif
			}
			// Get the file dates
			m_CreationDate.Load(&Entry.m_CreationDate);
			m_ModificatonDate.Load(&Entry.m_ModificatonDate);

			// Not valid on the Mac
			m_bSystem = FALSE;

			// 0x4000 = kIsInvisible finder flag (Big Endian!!!)
			m_bHidden = ((m_Name[0]=='.') || ((reinterpret_cast<const uint8_t *>(Entry.finderInfo)[8]&0x40U)!=0));

			// Is the file locked?
			m_bLocked = (Entry.m_Flags&UF_IMMUTABLE)!=0;
	
			// Get the mac specific file type and creator type
			m_uFileType = reinterpret_cast<uint32_t *>(Entry.finderInfo)[0];
			m_uAuxType = reinterpret_cast<uint32_t *>(Entry.finderInfo)[1];


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
	if (fp!=-1) {
		close(fp);
		m_fp = -1;
	}
}

#endif
