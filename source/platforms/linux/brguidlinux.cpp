/***************************************

	Linux version of GUID

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brguid.h"

#if defined(BURGER_LINUX)

#include <uuid/uuid.h>

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
	unsigned char* pWork = reinterpret_cast<unsigned char*>(pOutput);
	uuid_generate(pWork);
}

#endif
