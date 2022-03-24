/***************************************

	Operating system string functions
	MacOSX version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brosstringfunctions.h"

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
#include "brglobals.h"

#include <Foundation/NSPathUtilities.h>
#include <IOKit/IOKitLib.h>
#include <SystemConfiguration/SCDynamicStoreCopySpecific.h>
#include <limits.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

/***************************************

	\brief Get the name the user has called the computer.

	Some computer owners have the option to give their computer a whimsical
	name. This routine will retrieve that name. If for some reason a name can't
	be found or the operating system doesn't support naming, the name of
	"Computer" will be returned.

	\param pOutput Pointer to a \ref String to receive the name in UTF-8
		encoding
	\return Zero on no error, or non zero on failure.

	\note On platforms where networking or user level access isn't available, it
		will return \ref kErrorNotSupportedOnThisPlatform as an error code.

	\note On MacOS 9, the machine name is found in the OS string number -16413
		from the system resource file.

	\sa GetUserLoginName(String *) or NetworkManager::GetHostName()

***************************************/

Burger::eError BURGER_API Burger::GetMachineName(
	String* pOutput) BURGER_NOEXCEPT
{
	eError uResult = kErrorItemNotFound;

	// Return the computer name
	CFStringRef pStringRef = SCDynamicStoreCopyComputerName(nullptr, nullptr);

	if (pStringRef) {
		Globals::StringCopy(pOutput, pStringRef);

		// Dispose of the string ref
		CFRelease(pStringRef);

		uResult = kErrorNone;
	}
	if (uResult != kErrorNone) {
		// The name wasn't present, use the default
		pOutput->Set("Computer");
	}
	return uResult;
}

/*! ************************************

	Return the name of the mac model.

	Query the Registry entries that are found in a Macintosh and
	return the model string that denotes the specific model of Macintosh
	that is currently running this code.

	\param pOutput Pointer to a \ref String to receive the name in UTF-8
		encoding

	\return Zero on no error, or non zero on failure.

 ***************************************/

Burger::eError BURGER_API Burger::GetMacModelIdentifier(
	String* pOutput) BURGER_NOEXCEPT
{
	// Assume failure
	eError uResult = kErrorItemNotFound;

	// Get the master port
	mach_port_t pMachPort;
	kern_return_t uKernelReturn = IOMasterPort(MACH_PORT_NULL, &pMachPort);
	if (uKernelReturn != kIOReturnSuccess) {
		// If the failure was because of lack of privilege, note that.
		if (uKernelReturn == KERN_NO_ACCESS) {
			uResult = kErrorAccessDenied;
		}
	} else {

		// Got the master port, let's get the registry dictionary
		CFDictionaryRef pServiceMatch =
			IOServiceMatching("IOPlatformExpertDevice");
		if (pServiceMatch) {
			// IOServiceGetMatchingServices consumes the pServiceMatch
			io_iterator_t pIOIterator;
			uKernelReturn = IOServiceGetMatchingServices(
				pMachPort, pServiceMatch, &pIOIterator);
			if (uKernelReturn == kIOReturnSuccess) {

				// Loop until a device has the name of the Mac
				io_service_t pIOService;
				while ((pIOService = IOIteratorNext(pIOIterator))) {

					// Look up the entry
					CFDataRef pDataRef = static_cast<CFDataRef>(
						IORegistryEntryCreateCFProperty(pIOService,
							CFSTR("model"), kCFAllocatorDefault, kNilOptions));
					if (pDataRef) {
						// Found it! However, it's raw data, not a string
						uintptr_t uLength =
							static_cast<uintptr_t>(CFDataGetLength(pDataRef));
						const char* pData = reinterpret_cast<const char*>(
							CFDataGetBytePtr(pDataRef));
						uResult = pOutput->Set(pData, uLength);

						// Release the data and exit
						CFRelease(pDataRef);
						break;
					}
				}
				// Release the objects
				IOObjectRelease(pIOService);
				IOObjectRelease(pIOIterator);
			}
		}
	}
	return uResult;
}

#endif
