/***************************************

	What follows are commands for the directories to find the included headers

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __TEMPLATEXBOX360_H__
#define __TEMPLATEXBOX360_H__

/* DIR: "platforms:xbox360" */
/* IGNORE: "brtypes.h" */
/* IGNORE: "brerror.h" */

/* BEGIN */
/*! ************************************

	\file burgerxbox360.h
	\brief Burgerlib functions for Microsoft Xbox 360

	This file is only included for the Xbox 360. It is not included for
	classic Xbox, Windows or Xbox ONE.

	\sa \ref burgerwindows.h

***************************************/

#ifndef __BURGERXBOX360__
#define __BURGERXBOX360__

#ifndef __BURGER__
#error Include burger.h instead
#endif

// clang-format off
#include "x360_thread.h"
// clang-format on

#endif
/* END */

#endif
