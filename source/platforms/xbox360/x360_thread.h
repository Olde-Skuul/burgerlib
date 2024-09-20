/***************************************

	XBox 360 thread helpers

	Copyright (c) 2010-2024 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __X360_THREAD_H__
#define __X360_THREAD_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger { namespace Xbox360 {
extern void set_thread_name(
	const char* pName, uint32_t uThreadID) BURGER_NOEXCEPT;
}}
/* END */

#endif
