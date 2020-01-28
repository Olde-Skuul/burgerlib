/***************************************

	Operating system string functions
	MacOS version

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brosstringfunctions.h"

#if defined(BURGER_MAC)
#include "brcodelibrary.h"
#include "brglobals.h"
#include <Folders.h>
#include <Resources.h>
#include <TextUtils.h>


typedef const struct __SCDynamicStore* SCDynamicStoreRef;

/***************************************

	\brief Get the name of the current user

	When someone has logged onto a computer, that person had to give a user
name. This routine will retrieve that user name. If for some reason a user name
	can't be found or the operating system doesn't support user log ons,
	the name "User" will be returned.

	\param pOutput Pointer to a \ref String to receive the name in UTF-8
encoding \return Zero on no error, or non zero on failure. \note On platforms
where networking or user level access isn't available, it will return \ref
kErrorNotSupportedOnThisPlatform as an error code.

	\a GetMachineName(String *)

***************************************/

Burger::eError BURGER_API Burger::GetLoggedInUserName(String* pOutput) BURGER_NOEXCEPT
{
	eError uResult = kErrorItemNotFound;

	// Carbon
#if TARGET_API_MAC_CARBON
	if (Globals::GetMacOSVersion() < 0x1000U)
#endif

	{
		// Get my resource reference
		short sOldResFile = CurResFile();

		// Use the Mac system reference
		UseResFile(0);

		// Get the user name string from the system resource
		StringHandle ppString = GetString(-16096);

		// Restore the resource reference
		UseResFile(sOldResFile);

		// Did I get a name?
		if (ppString && ppString[0][0]) {
			pOutput->Set(static_cast<const char*>(
							 static_cast<const void*>(&ppString[0][1])),
				ppString[0][0]);
			uResult = kErrorNone;
		}
	}

#if TARGET_API_MAC_CARBON
	else {
		CodeFramework LibRef;
		if (!LibRef.Init("SystemConfiguration.framework")) {

			CFStringRef (*pNSFullUserName)(void);
			pNSFullUserName =
				(CFStringRef(*)(void))LibRef.GetFunction("NSFullUserName");
			if (pNSFullUserName) {

				// Return the user name
				CFStringRef pStringRef = pNSFullUserName();
				if (pStringRef) {

					Globals::StringCopy(pOutput, pStringRef);

					// Dispose of the string ref
					CFRelease(pStringRef);
					uResult = kErrorNone;
				}
			}
		}
	}
#endif

	if (uResult) {
		// The name wasn't present, use the default
		pOutput->Set("User");
	}

	return uResult;
}

/***************************************

	\brief Get the name the user has called the computer

	Some computer owners have the option to give their computer a whimsical
	name. This routine will retrieve that name. If for some reason a name can't
	be found or the operating system doesn't support naming, the name of
"Computer" will be returned.

	\param pOutput Pointer to a \ref String to receive the name in UTF-8
encoding \return Zero on no error, or non zero on failure. \note On platforms
where networking or user level access isn't available, it will return \ref
kErrorNotSupportedOnThisPlatform as an error code.

	\note On MacOS 9, the machine name is found in the OS string
		number -16413 from the system resource file.

	\a GetLoggedInUserName(String *)

***************************************/

Burger::eError BURGER_API Burger::GetMachineName(String* pOutput) BURGER_NOEXCEPT
{
	eError uResult = kErrorItemNotFound;

	// Carbon
#if TARGET_API_MAC_CARBON
	if (Globals::GetMacOSVersion() < 0x1000U)
#endif

	{
		// Get my resource reference
		short sOldResFile = CurResFile();

		// Use the Mac system reference
		UseResFile(0);

		// Get the machine name string from the system resource
		StringHandle ppString = GetString(-16413);

		// Restore the resource reference
		UseResFile(sOldResFile);

		// Did I get a name?
		if (ppString && ppString[0][0]) {
			pOutput->Set(static_cast<const char*>(
							 static_cast<const void*>(&ppString[0][1])),
				ppString[0][0]);
			uResult = kErrorNone;
		}
	}

#if TARGET_API_MAC_CARBON
	else {
		CodeFramework LibRef;
		if (!LibRef.Init("SystemConfiguration.framework")) {

			CFStringRef (*pSCDynamicStoreCopyComputerName)(
				SCDynamicStoreRef store, CFStringEncoding * nameEncoding);

			pSCDynamicStoreCopyComputerName =
				(CFStringRef(*)(SCDynamicStoreRef, CFStringEncoding*))
					LibRef.GetFunction("SCDynamicStoreCopyComputerName");
			if (pSCDynamicStoreCopyComputerName) {

				// Return the computer name
				CFStringRef pStringRef = pSCDynamicStoreCopyComputerName(0, 0);
				if (pStringRef) {
					Globals::StringCopy(pOutput, pStringRef);

					// Dispose of the string ref
					CFRelease(pStringRef);
					uResult = kErrorNone;
				}
			}
		}
	}
#endif

	if (uResult) {
		// The name wasn't present, use the default
		pOutput->Set("Computer");
	}

	return uResult;
}

#endif
