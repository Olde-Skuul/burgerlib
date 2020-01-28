/***************************************

	Global variable manager, MSDOS version

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brglobals.h"
#if defined(BURGER_MSDOS) || defined(DOXYGEN)
#include "brstring16.h"
#include "brstring.h"
#include "brstringfunctions.h"
#include <stdlib.h>

/***************************************

	Read an environment variable as UTF8

***************************************/

const char *BURGER_API Burger::GetEnvironmentString(const char *pKey) BURGER_NOEXCEPT
{
	const char *pValue = getenv(pKey);
	if (pValue) {
		pValue = StringDuplicate(pValue);
	}
	return pValue;
}

/***************************************

	Set an environment variable with a UTF8 string

***************************************/

Burger::eError BURGER_API Burger::SetEnvironmentString(const char *pKey,const char *pInput) BURGER_NOEXCEPT
{
    // Pass to the operating system
    return static_cast<eError>(setenv(pKey, pInput, TRUE));
}

#endif
