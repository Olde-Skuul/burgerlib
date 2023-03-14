/***************************************

	File Manager Class: iOS version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_IOS) || defined(DOXYGEN)
#include "brerror.h"
#include "brfile.h"
#include "brmemoryfunctions.h"
#include "brstring.h"
#include "brstringfunctions.h"
#include <CoreFoundation/CFString.h>
#include <Foundation/NSAutoreleasePool.h>
#include <Foundation/NSFileManager.h>
#include <Foundation/NSString.h>
#include <fcntl.h>
#include <mach-o/dyld.h>
#include <stdlib.h>
#include <sys/attr.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <unistd.h>
#undef Free

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

//
// crt_externs.h is missing, so include the values directly
//

extern "C" char*** _NSGetArgv();
extern "C" int* _NSGetArgc();
#endif

/***************************************

	\brief Return the name of a drive

	Given a drive number (0-?), return the name of the volume in the format of
	":Volume name:". The function will guarantee the existence of the colons.

	\note This function should be used with caution. Only mounted drives would
	return immediately and if the drive has ejectable media may take a while for
	it to respond to a volume name query.

	\param pOutput A \ref Filename structure to contain the filename (Can be
		\ref nullptr )

	\param uVolumeNum A valid drive number from 0-?? with ?? being the
		maximum number of drives in the system

	\return Zero if no error, non-zero if an error occurred

	\sa get_volume_number(const char *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::get_volume_name(
	Filename* pOutput, uint_t uVolumeNum) BURGER_NOEXCEPT
{
	eError uResult = kErrorVolumeNotFound;

	// If running in the emulator, it's really running on top of the MacOSX
	// file system, so perform the MacOSX handler to obtain volume names

	int fp = open("/Volumes", O_RDONLY, 0);
	if (fp != -1) {
		int eError;

		// Volume not found, nor the boot volume
		uint_t bFoundIt = FALSE;
		uint_t bFoundRoot = FALSE;

		// Start with #1 (Boot volume is special cased)
		uint_t uEntry = 1;
		do {
			// Structure declaration of data coming from getdirentriesattr()
			struct FInfoAttrBuf {
				u_int32_t length;     // Length of this data structure
				attrreference_t name; // Offset for the filename
				fsobj_type_t objType; // VREG for file, VREG for directory
				char m_Name[256 * 4];
			};

			// Attributes requested
			attrlist AttributesList;
			// Buffer to hold the attributes and the filename
			FInfoAttrBuf Entry;

			// Initialize the attributes list
			MemoryClear(&AttributesList, sizeof(AttributesList));
			// "sizeof" for the structure
			AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;
			// Let's get the name, type of file, creation time, modification
			// time, finder information and hidden/locked flags

			// Note: If these flags are changed, the FInfoAttrBuf MUST be
			// adjusted to reflect the request or weird stuff will happen
			AttributesList.commonattr = ATTR_CMN_NAME | ATTR_CMN_OBJTYPE;

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
			eError = getdirentriesattr(fp, &AttributesList, &Entry,
				sizeof(Entry), &uCount, &uJunkBase, &uNewState, 0);

			// No errors and an entry was returned?
			// Note: eError is 0 if more data is pending, 1 if this is the last
			// entry. uCount is zero when no entry is loaded

			if (eError >= 0 && uCount) {

				// Get the pointer to the volume name
				char* pName = (reinterpret_cast<char*>(&Entry.name) +
					Entry.name.attr_dataoffset);

				// Special case for the root volume, it's a special link
				if (!bFoundRoot && Entry.objType == VLNK) {

					// Read in the link to see if it's pointing to the home
					// folder

					char LinkBuffer[128];
					String Linkname("/Volumes/", pName);
					ssize_t uLinkDataSize = readlink(
						Linkname.c_str(), LinkBuffer, sizeof(LinkBuffer));
					if (uLinkDataSize == 1 && LinkBuffer[0] == '/') {

						// This is the boot volume
						bFoundRoot = TRUE;
						// Is the user looking for the boot volume?
						if (!uVolumeNum) {
							bFoundIt = TRUE;
						}
					} else {
						// Pretend it's a normal mounted volume
						Entry.objType = VDIR;
					}
				}
				// Normal volume (Enumate them)
				if (Entry.objType == VDIR) {
					if (uVolumeNum == uEntry) {
						bFoundIt = TRUE;
					}
					++uEntry;
				}
				// Matched a volume!

				if (bFoundIt) {
					if (pOutput) {
						// Insert a starting and ending colon
						String TempString(":");
						TempString.append(pName);
						TempString.append(':');
						// Set the filename
						pOutput->assign(TempString.c_str());
					}
					// Exit okay!
					uResult = kErrorNone;
					break;
				}
			}
		} while (eError == 0);
		// Close the directory
		close(fp);

		// Assume this is running on a mobile device.
		// Fake a single volume named :iOSDevice:

	} else if (!uVolumeNum) {
		pOutput->assign(":iOSDevice:");
		uResult = kErrorNone;
	}

	// Clear on error
	if ((uResult != kErrorNone) && pOutput) {
		// Kill the string since I have an error
		pOutput->clear();
	}
	return uResult;
}

#endif
