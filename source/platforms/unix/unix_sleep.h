/***************************************

	Sleep helper functions for Unix

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __UNIX_SLEEP_H__
#define __UNIX_SLEEP_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __UNIX_TYPES_H__
#include "unix_types.h"
#endif

/* BEGIN */
namespace Burger { namespace Unix {

extern eError BURGER_API nanosleep_delay(
	timespec* pInput, uint_t bAlertable = FALSE) BURGER_NOEXCEPT;

}}
/* END */

#endif
