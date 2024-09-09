/***************************************

	What follows are commands for the directories to find the included headers

***************************************/

#ifndef __TEMPLATEMACOSX_H__
#define __TEMPLATEMACOSX_H__
#endif

/* DIR: "platforms:macosx" */
/* IGNORE: "brtypes.h" */
/* IGNORE: "brerror.h" */

/* BEGIN */
/*! ************************************

	\file burgermacosx.h
	\brief Burgerlib functions for MacOSX or higher

	This file is included for MacOSX desktop platforms. This is not
	included for iOS, see \ref burgerdarwin.h for shared Darwin classes
	and defines.

***************************************/

#ifndef __BURGERMACOSX__
#define __BURGERMACOSX__

#ifndef __BURGER__
#error Include burger.h instead
#endif

// clang-format off
#include "macosx_types.h"
#include "macosx_version.h"
// clang-format on

#endif
/* END */
