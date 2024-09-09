/***************************************

	What follows are commands for the directories to find the included headers

***************************************/

#ifndef __TEMPLATEMSDOS_H__
#define __TEMPLATEMSDOS_H__
#endif

/* DIR: "platforms:msdos" */
/* IGNORE: "brtypes.h" */

/* BEGIN */
/*! ************************************

	\file burgermsdos.h
	\brief Burgerlib functions for MSDOS

	This file is included on MSDOS platforms.
	It is not included for Windows.

	\sa \ref burgerwindows.h

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
