/***************************************

	What follows are commands for the directories to find the included headers

***************************************/

#ifndef __TEMPLATEDARWIN_H__
#define __TEMPLATEDARWIN_H__
#endif

/* DIR: "platforms:darwin" */
/* IGNORE: "brtypes.h" */
/* IGNORE: "brerror.h" */

/* BEGIN */
/***************************************

	Burgerlib functions for Darwin platforms

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

	Documentation can be found at https://burgerlib.readthedocs.io/en/latest/index.htm

	Comments, suggestions, bug fixes, email me at becky@burgerbecky.com

***************************************/

#ifndef __BURGERDARWIN__
#define __BURGERDARWIN__

#ifndef __BURGER__
#error Include burger.h instead
#endif

// clang-format off
#include "darwin_types.h"
#include "darwin_autorelease.h"
// clang-format on

#endif
/* END */
