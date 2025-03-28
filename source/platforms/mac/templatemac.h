/***************************************

	What follows are commands for the directories to find the included headers

***************************************/

#ifndef __TEMPLATEMAC_H__
#define __TEMPLATEMAC_H__
#endif

/* DIR: "platforms:mac" */
/* IGNORE: "brtypes.h" */
/* IGNORE: "brerror.h" */
/* IGNORE: "brstructs.h" */

/* BEGIN */
/*! ************************************

	\file burgermac.h
	\brief Burgerlib functions for MacOS 7-9 and Carbon

	This file is only included with MacOS 7-9, Carbon, and MacOS for 68K
	platforms. This is not included on MacOSX

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
// clang-format on

#endif
/* END */
