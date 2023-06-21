/***************************************

	What follows are commands for the directories to find the included headers

***************************************/

#ifndef __TEMPLATEMAC_H__
#define __TEMPLATEMAC_H__
#endif

/* DIR: "platforms:mac" */
/* IGNORE: "brtypes.h" */
/* IGNORE: "brerror.h" */

/* BEGIN */
/***************************************

	Burgerlib functions for MacOS 7-9 and Carbon

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

	Documentation can be found at http://www.burgerbecky.com/burgerlib/docs

	Comments, suggestions, bug fixes, email me at becky@burgerbecky.com

***************************************/

#ifndef __BURGERMAC__
#define __BURGERMAC__

#ifndef __BURGER__
#error Include burger.h instead
#endif

// clang-format off
#include "mac_types.h"
#include "mac_carbon_accessors.h"
#include "mac_volparms.h"
#include "mac_timer.h"
#include "mac_appleevents.h"
#include "mac_kernel.h"
#include "mac_quickdraw.h"
#include "mac_version.h"
#include "mac_sprockets.h"
#include "mac_folders.h"
#include "mac_gamma.h"
// clang-format on

#endif
/* END */
