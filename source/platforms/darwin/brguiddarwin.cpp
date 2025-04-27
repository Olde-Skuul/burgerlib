/***************************************

	Create and work with Windows style GUID structures

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brguid.h"

#if defined(BURGER_DARWIN)
#include <CoreFoundation/CFUUID.h>

/***************************************

	\brief Create a new GUID

	Using a MAC address, current time and deep voodoo, generate a unique 128 bit
	number for labeling a data object.

	http://en.wikipedia.org/wiki/Globally_unique_identifier

	http://tools.ietf.org/html/rfc4122.html

	\param pOutput Pointer to an uninitialized GUID buffer
	\sa GUID_from_string() or GUID_to_string()

***************************************/

void BURGER_API Burger::GUID_init(GUID* pOutput) BURGER_NOEXCEPT
{
	CFUUIDRef rUUID = CFUUIDCreate(kCFAllocatorDefault);
	reinterpret_cast<CFUUIDBytes*>(pOutput)[0] = CFUUIDGetUUIDBytes(rUUID);
	CFRelease(rUUID);
}

#endif
