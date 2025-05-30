/***************************************

	MacOSX specific version checking

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __MACOSX_VERSION_H__
#define __MACOSX_VERSION_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger { namespace MacOSX {
extern uint32_t BURGER_API get_os_version(void) BURGER_NOEXCEPT;
}}

/* END */

#endif
