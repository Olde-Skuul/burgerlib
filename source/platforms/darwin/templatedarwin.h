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
/*! ************************************

	\file burgerdarwin.h
	\brief Burgerlib functions for Darwin platforms

	This header is only included on Darwin based platforms such as
	iOS, TVOS, iPadOS, and MacOSX.

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
