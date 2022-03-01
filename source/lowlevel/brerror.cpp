/***************************************

	Error codes.

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brerror.h"

/*! ************************************

	\enum Burger::eError
	\brief Global error codes

	When a Burgerlib function fails, it will return one of these error codes. If
	the code returned by the operating system can't be mapped to these codes, it
	will be returned unmodified instead.

***************************************/
