/***************************************

	What follows are commands for the directories to find the included headers

***************************************/

#ifndef __TEMPLATEMSDOS_H__
#define __TEMPLATEMSDOS_H__
#endif

/* DIR: "platforms:msdos" */
/* IGNORE: "brtypes.h" */

/* BEGIN */
/***************************************

	Burgerlib functions for MSDOS

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

	Documentation can be found at http://www.burgerbecky.com/burgerlib/docs

	Comments, suggestions, bug fixes, email me at becky@burgerbecky.com

***************************************/

#ifndef __BURGERMSDOS__
#define __BURGERMSDOS__

#ifndef __BURGER__
#error Include burger.h instead
#endif

// clang-format off
#include "msdos_structs.h"
#include "msdos_memory.h"
#include "msdos_interrupts.h"
// clang-format on

#endif
/* END */
