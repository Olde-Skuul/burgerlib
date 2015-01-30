/***************************************

	Create and work with Windows style GUID structures

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brguid.h"

#if defined(BURGER_WINDOWS)
#include "brglobals.h"

/***************************************

	Create a unique UUID

***************************************/

void BURGER_API Burger::GUIDInit(GUID *pOutput)
{
	Globals::UuidCreateSequential(pOutput);
}

#endif
