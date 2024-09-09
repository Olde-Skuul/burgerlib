/***************************************

	What follows are commands for the directories to find the included headers

***************************************/

#ifndef __TEMPLATEUNIX_H__
#define __TEMPLATEUNIX_H__
#endif

/* DIR: "platforms:unix" */
/* IGNORE: "brtypes.h" */
/* IGNORE: "brerror.h" */
/* IGNORE: "brcodelibrary.h" */

/* BEGIN */
/*! ************************************

	\file burgerunix.h
	\brief Burgerlib functions for Posix platforms

	This folder has classes, functions and defines that are shared
	among BSD/Unix/Linux platforms that have common Posix APIs.

	\sa BURGER_LINUX, BURGER_MACOSX, BURGER_STADIA, and BURGER_ANDROID

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
