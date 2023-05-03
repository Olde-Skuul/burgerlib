/***************************************

	What follows are commands for the directories to find the included headers

***************************************/

#ifndef __TEMPLATEUNIX_H__
#define __TEMPLATEUNIX_H__
#endif

/* DIR: "platforms:unix" */
/* IGNORE: "brtypes.h" */
/* IGNORE: "brerror.h" */

/* BEGIN */
/***************************************

	Burgerlib functions for Posix platforms

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

	Documentation can be found at http://www.burgerbecky.com/burgerlib/docs

	Comments, suggestions, bug fixes, email me at becky@burgerbecky.com

***************************************/

#ifndef __BURGERUNIX__
#define __BURGERUNIX__

#ifndef __BURGER__
#error Include burger.h instead
#endif

// clang-format off
#include "unix_types.h"
#include "unix_sleep.h"
#include "unix_shims.h"
#include "unix_dbus.h"
#include "unix_dbusinstance.h"
// clang-format on

#endif
/* END */
