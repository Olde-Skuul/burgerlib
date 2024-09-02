/***************************************

	What follows are commands for the directories to find the included headers

***************************************/

#ifndef __TEMPLATEWINDOWS_H__
#define __TEMPLATEWINDOWS_H__
#endif

/* DIR: "platforms:windows" */
/* IGNORE: "brtypes.h" */
/* IGNORE: "brguid.h" */
/* IGNORE: "brerror.h" */
/* IGNORE: "brfilename.h" */
/* IGNORE: "brstring.h" */

/* BEGIN */
/***************************************

	Burgerlib functions for Windows

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

	Documentation can be found at https://burgerlib.readthedocs.io/en/latest/index.htm

	Comments, suggestions, bug fixes, email me at becky@burgerbecky.com

***************************************/

#ifndef __BURGERWINDOWS__
#define __BURGERWINDOWS__

#ifndef __BURGER__
#error Include burger.h instead
#endif

// clang-format off
#include "win_types.h"
#include "win_directxdata.h"
#include "win_globals.h"
#include "win_version.h"
#include "win_platformshims.h"
#include "win_dinput.h"
#include "win_xinput.h"
#include "win_ddraw.h"
#include "win_d3d9.h"
#include "win_d3d11.h"
#include "win_dsound.h"
#include "win_dplay.h"
#include "win_rpcrt4.h"
#include "win_hid.h"
#include "win_winmm.h"
#include "win_shlwapi.h"
#include "win_setupapi.h"
#include "win_kernel32.h"
#include "win_user32.h"
#include "win_shell32.h"
#include "win_synchapi.h"
#include "win_loadlibrary.h"
#include "win_launcher.h"
#include "win_winutils.h"
#include "win_registry.h"
#include "win_memorymanager.h"
#include "win_error.h"
// clang-format on

#endif
/* END */
