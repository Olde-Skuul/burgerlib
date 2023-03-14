/***************************************

	Error codes.
	MacOS Version

	Copyright (c) 2021-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brerror.h"

#if defined(BURGER_MAC) && !defined(DOXYGEN)
#include <MacErrors.h>
#include <MacTypes.h>

// Struct to convert macos errors to Burgerlib
struct OSStatus2Error_t {
	// int32_t is what OSStatus maps to
	OSStatus m_iMacError;
	Burger::eError m_Error;
};

static const OSStatus2Error_t g_MacErrorLookup[] = {
	// No error at all, make it first since it's the most likely to match
	{noErr, Burger::kErrorNone}, {ioErr, Burger::kErrorIO},
	{nsvErr, Burger::kErrorVolumeNotFound},
	{mFulErr, Burger::kErrorOutOfMemory},
	{tmfoErr, Burger::kErrorNoMoreFileDescriptors},
	{fnfErr, Burger::kErrorFileNotFound},
	{wPrErr, Burger::kErrorWriteProtected}, {fLckdErr, Burger::kErrorIsLocked},
	{notAFileErr, Burger::kErrorNotAFile},

	// This also can mean "Not implemented" on this OS
	{paramErr, Burger::kErrorInvalidParameter},
	{unimpErr, Burger::kErrorNotSupportedOnThisPlatform}};

/***************************************

	\brief Convert platform error code to a Burgerlib error code.

	Take a native platform error code and convert it to a Burgerlib equivalent
	error code.

	\param iNativeError MacOS OSErr or OSStatus, Windows HRESULT error code

	\return Burgerlib error code, or kErrorGeneric if unknown

***************************************/

Burger::eError BURGER_API Burger::platform_convert_to_error(
	int iNativeError) BURGER_NOEXCEPT
{
	// Set the default error if not found
	eError uResult = kErrorGeneric;

	// Scan the table for a match
	const OSStatus2Error_t* pWork = g_MacErrorLookup;
	uintptr_t uCount = BURGER_ARRAYSIZE(g_MacErrorLookup);
	do {
		if (pWork->m_iMacError == iNativeError) {
			uResult = pWork->m_Error;
			break;
		}
		++pWork;
	} while (--uCount);

	// Return the converted error code
	return uResult;
}

#endif
