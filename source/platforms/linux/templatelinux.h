/***************************************

	What follows are commands for the directories to find the included headers

***************************************/

#ifndef __TEMPLATELINUX_H__
#define __TEMPLATELINUX_H__
#endif

/* DIR: "platforms:linux" */
/* IGNORE: "brtypes.h" */
/* IGNORE: "brerror.h" */

/* BEGIN */
/*! ************************************

	\file burgerlinux.h
	\brief Burgerlib functions for Linux platforms

	This header is only included on Linux platforms.

	\note This is not included on Linux like platforms, such as 
	Darwin, Android, or Unix. Look at \ref burgerunix.h for shared
	functions and classes.

***************************************/

#ifndef __BURGERLINUX__
#define __BURGERLINUX__

#ifndef __BURGER__
#error Include burger.h instead
#endif

// clang-format off
#include "linux_sandbox.h"
// clang-format on

#endif
/* END */
