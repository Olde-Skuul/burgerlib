/***************************************

	MacOSX specific version checking

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "macosx_version.h"

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
#include "brstring.h"

#include <sys/sysctl.h>

/*! ************************************

	\brief Return the version of MacOS.

	Ask MacOS what version it is and return that value.

	\return Version in the format of 0x0102 -> 1.2

***************************************/

uint32_t BURGER_API Burger::MacOSX::get_os_version(void) BURGER_NOEXCEPT
{
	// Discovered version of MacOS (Cached)
	static uint32_t s_uMacOSXVersion = UINT32_MAX;

	uint32_t uVersion = s_uMacOSXVersion;
	if (s_uMacOSXVersion == UINT32_MAX) {

		int selector[2];

		// Create a machine information block

		selector[0] = CTL_KERN;
		selector[1] = KERN_OSRELEASE;
		size_t uLength = 0;

		// Get the size of the string
		int iError = sysctl(selector, 2, nullptr, &uLength, nullptr, 0);
		uVersion = 0;

		if (!iError) {

			// Space for the string
			String Buffer;
			Buffer.resize(uLength);

			// Make sure the buffer was allocated
			if (Buffer.length() == uLength) {

				// Get the string
				iError =
					sysctl(selector, 2, Buffer.c_str(), &uLength, nullptr, 0);

				// String is in the format of 9.8.0 where
				// 10 is assumed, 9 is the minor version (+4) and
				// 8 is the patch version. 9.8.0 translates
				// to 1058 (10.5.8)

				const char* pBuffer = Buffer.c_str();
				const char* pEnd;
				uint32_t uMajor = AsciiToInteger(pBuffer, &pEnd);
				if (pEnd != pBuffer) {
					pBuffer = pEnd + 1;
					uint32_t uMinor = AsciiToInteger(pBuffer, &pEnd);
					if (pEnd != pBuffer) {
						// Of course, there's MacOS 11 and higher
						if (uMajor >= 20) {
							uVersion = ((uMajor - 9) << 12U) + (uMinor<< 4U);
						} else {
							uVersion = 0x1000 + ((uMajor - 4) << 4) + uMinor;
						}
					}
				}
			}
		}
		s_uMacOSXVersion = uVersion;
	}
	return uVersion;
}

#endif
