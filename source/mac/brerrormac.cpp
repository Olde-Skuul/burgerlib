/***************************************

	Error codes.
	MacOS Version

	Copyright (c) 2021-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brerror.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include <MacErrors.h>
#include <MacTypes.h>

/*! ************************************

	\brief Convert mac error code to a Burgerlib error code.

	Take a MacOS Error code and convert it to a Burgerlib equivalent error code.

	\param iMacError MacOS OSErr or OSStatus error code

	\return Burgerlib error code, or kErrorGeneric if unknown

***************************************/

Burger::eError BURGER_API Burger::MacConvertError(int iMacError) BURGER_NOEXCEPT
{
	eError uResult;
	switch (iMacError) {
	case noErr:
		uResult = kErrorNone;
		break;

	case ioErr:
		uResult = kErrorIO;
		break;
	case nsvErr:
		uResult = kErrorVolumeNotFound;
		break;
	case mFulErr:
		uResult = kErrorOutOfMemory;
		break;
	case tmfoErr:
		uResult = kErrorNoMoreFileDescriptors;
		break;
	case fnfErr:
		uResult = kErrorFileNotFound;
		break;
	case wPrErr:
		uResult = kErrorWriteProtected;
		break;
	case fLckdErr:
		uResult = kErrorIsLocked;
		break;
	case notAFileErr:
		uResult = kErrorNotAFile;
		break;

	// This also can mean "Not implemented" on this OS
	case paramErr:
		uResult = kErrorInvalidParameter;
		break;
	case unimpErr:
		uResult = kErrorNotSupportedOnThisPlatform;
		break;

	default:
		uResult = kErrorGeneric;
		break;
	}
	return uResult;
}

#endif
