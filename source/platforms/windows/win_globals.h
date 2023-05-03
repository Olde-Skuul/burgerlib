/***************************************

	Memory manager functions specific to Windows

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_GLOBALS_H__
#define __WIN_GLOBALS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __WIN_TYPES_H__
#include "win_types.h"
#endif

/* BEGIN */
namespace Burger { namespace Win32 {

extern HINSTANCE__* BURGER_API get_instance(void) BURGER_NOEXCEPT;
extern void BURGER_API set_instance(HINSTANCE__* pInput) BURGER_NOEXCEPT;

}}
/* END */

#endif
