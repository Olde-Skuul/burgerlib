/***************************************

	File Manager Class: MacOS X version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
#include "brfile.h"
#include "brmemoryfunctions.h"
#include "brstring.h"

#include <crt_externs.h>
#include <dirent.h>
#include <fcntl.h>
#include <mach-o/dyld.h>
#include <stdlib.h>
#include <string.h>
#include <sys/attr.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/vnode.h>
#include <unistd.h>

#include <AvailabilityMacros.h>

#if defined(BURGER_METROWERKS)
#include <CarbonCore/Files.h>
#include <CarbonCore/Folders.h>
#else
#include <Carbon/Carbon.h>
#endif

#include <Foundation/NSAutoreleasePool.h>
#include <Foundation/NSFileManager.h>
#include <Foundation/NSString.h>
#undef Free

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
	char TempBuffer[PATH_MAX + 3];
	eError uResult = kErrorVolumeNotFound;

	// Open the volume directory
	DIR* fp = opendir("/Volumes");
	if (fp) {

		// Volume not found, nor the boot volume
		uint_t bFoundIt = FALSE;
		uint_t bFoundRoot = FALSE;

		// Start with #1 (Boot volume is special cased)
		uint_t uEntry = 1;
		for (;;) {
			// Get the directory entry
			struct dirent* pDirEntry = readdir(fp);

			// No errors and an entry was returned?
			if (!pDirEntry) {
				break;
			}

			// Get the pointer to the volume data
			const char* pName = pDirEntry->d_name;

			// Special case for the root volume, it's a special link
			uint_t uType = pDirEntry->d_type;
			if (!bFoundRoot && (uType == DT_LNK)) {

				// Read in the link to see if it's pointing to the home folder
				// of '/'
				String Linkname("/Volumes/", pName);
				ssize_t uLinkDataSize =
					readlink(Linkname.c_str(), TempBuffer, PATH_MAX);

				// Only care if it resolves to '/', all others, ignore,
				// including errors
				if (uLinkDataSize == 1 && TempBuffer[0] == '/') {

					// This is the boot volume
					bFoundRoot = TRUE;
					// Is the user looking for the boot volume?
					if (!uVolumeNum) {
						bFoundIt = TRUE;
					}
				} else {
					// Pretend it's a normal mounted volume
					uType = DT_DIR;
				}

				// Ignore . and ..
			} else if (pDirEntry->d_name[0] == '.') {
				if ((pDirEntry->d_namlen == 1) ||
					((pDirEntry->d_namlen == 2) &&
						(pDirEntry->d_name[1] == '.'))) {

					// Force the test ahead to fail
					uType = DT_BLK;
				}
			}

			// Normal volume (Enumate them)
			if (uType == DT_DIR) {
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

		// Close the directory
		closedir(fp);
	}

	// Clear on error
	if ((uResult != kErrorNone) && pOutput) {
		// Kill the string since I have an error
		pOutput->clear();
	}
	return uResult;
}

#endif
