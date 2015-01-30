/***************************************

	Create and work with Windows style GUID structures

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brguid.h"

#if defined(BURGER_IOS)
#include <CoreFoundation/CFUUID.h>

/***************************************

	Create a unique UUID

***************************************/

void BURGER_API Burger::GUIDInit(GUID *pOutput)
{
	CFUUIDRef rUUID = CFUUIDCreate(kCFAllocatorDefault);
	reinterpret_cast<CFUUIDBytes *>(pOutput)[0] = CFUUIDGetUUIDBytes(rUUID);
	CFRelease(rUUID);
}

#endif
