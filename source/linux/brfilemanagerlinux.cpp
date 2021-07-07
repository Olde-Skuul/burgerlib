/***************************************

	File Manager Class: Linux version

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_LINUX) || defined(DOXYGEN)
#include "brglobals.h"
#include "brmemoryfunctions.h"
#include "brosstringfunctions.h"

#include <dirent.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <unistd.h>

/***************************************

	\brief Return the name of a drive

	Given a drive number (0-?), return the name of the volume in the format of
	":Volume name:". The function will guarantee the existence of the colons.

	\note This function should be used with caution. Only mounted drives would
	return immediately and if the drive has ejectable media may take a while for
	it to respond to a volume name query.

	\param pOutput A Burger::Filename structure to contain the filename (Can be
		nullptr)

	\param uVolumeNum A valid drive number from 0-?? with ?? being the
		maximum number of drives in the system

	\return Zero if no error, non-zero if an error occurred

	\sa Burger::FileManager::GetVolumeNumber(const char *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::GetVolumeName(
	Filename* pOutput, uint_t uVolumeNum) BURGER_NOEXCEPT
{
	// Linux has no name for the boot volume, so create one.
	if (!uVolumeNum) {
		if (pOutput) {
			pOutput->Set(":boot_volume:");
		}
		return kErrorNone;
	}

	// Assume volume not permitted
	eError uResult = kErrorInvalidParameter;

	// Different versions of linux have the folder for mounted volumes different
	// places
	Globals::eLinuxFlavor uFlavor = Globals::GetLinuxFlavor();
	String VolumeRoot;

	// WSL uses /mnt
	if (uFlavor == Globals::kLinuxWSL) {
		VolumeRoot = "/mnt";

		// Cygwin uses /cygdrive
	} else if (uFlavor == Globals::kLinuxCygwin) {
		VolumeRoot = "/cygdrive";

		// MSYS2 uses / with the drives being "hidden"
	} else if (uFlavor == Globals::kLinuxMSYS2) {
		VolumeRoot = "/";

		// Ubuntu 20 uses /media/ followed by the logged in user's name
	} else if (uFlavor == Globals::kLinuxNative) {
		String Temp;
		if (!GetUserLoginName(&Temp)) {
			VolumeRoot = "/media/";
			VolumeRoot += Temp;
		}
	}

	// Was a parent directory found?
	if (VolumeRoot.length()) {
		// Change the error to a simple volume not found error
		char TempBuffer[PATH_MAX + 3];
		uResult = kErrorVolumeNotFound;

		// Open the volume directory
		DIR* fp = opendir(VolumeRoot.c_str());
		if (fp) {
			// Start with #1 (Boot volume is special cased)
			uint_t uEntry = 1;
			for (;;) {
				// Get the directory entry
				struct dirent *pDirEntry = readdir(fp);

				// No errors and an entry was returned?
				if (!pDirEntry) {
					break;
				}

				// Get the pointer to the volume data
				const char *pName = pDirEntry->d_name;

				// Special case for the root volume, it's a special link
				uint_t uType = pDirEntry->d_type;

				// Ignore . and ..
				if (pDirEntry->d_name[0] == '.') {
					const uintptr_t uD_Namlen = StringLength(pDirEntry->d_name);
					if ((uD_Namlen == 1) ||
						((uD_Namlen == 2) && (pDirEntry->d_name[1] == '.'))) {

						// Force the test ahead to fail
						uType = DT_BLK;
					}
				}

				// Normal volume (Enumate them)
				if (uType == DT_DIR) {

					// Found the entry?
					if (uVolumeNum == uEntry) {
						if (pOutput) {

							// Insert a starting and ending colon
							TempBuffer[0] = ':';
							uintptr_t uIndex = StringLength(pName);

							// Failsafe
							uIndex = Min(uIndex, static_cast<uintptr_t>(PATH_MAX));

							MemoryCopy(TempBuffer + 1, pName, uIndex);
							TempBuffer[uIndex + 1] = ':';
							TempBuffer[uIndex + 2] = 0;

							// Set the filename
							pOutput->Set(TempBuffer);
						}
						// Exit okay!
						uResult = kErrorNone;
						break;
					}
					++uEntry;
				}
			}

			// Close the directory
			closedir(fp);
		}
	}

	// Clear on error
	if ((uResult != kErrorNone) && pOutput) {
		// Kill the string since I have an error
		pOutput->Clear();
	}
	return uResult;
}
#endif
