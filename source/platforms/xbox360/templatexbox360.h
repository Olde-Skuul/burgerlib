/***************************************

	What follows are commands for the directories to find the included headers

***************************************/

#ifndef __TEMPLATEXBOX360_H__
#define __TEMPLATEXBOX360_H__
#endif

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
