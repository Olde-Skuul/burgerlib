/***************************************

	Shims functions that may or may not be available on Unix

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __UNIX_SHIMS_H__
#define __UNIX_SHIMS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger { namespace Unix {

#if defined(BURGER_DARWIN) && !defined(DOXYGEN)
extern int pthread_setname_np(const char* name);
#else
extern int pthread_setname_np(uintptr_t target_thread, const char* name);
#endif
}}
/* END */

#endif
