/***************************************

	MacOS specific directory functions

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __MAC_FOLDERS_H__
#define __MAC_FOLDERS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger { namespace MacOS {
extern uint_t BURGER_API is_user_owner(int8_t ioACUser) BURGER_NOEXCEPT;
extern uint_t BURGER_API user_has_full_access(int8_t ioACUser) BURGER_NOEXCEPT;
extern uint_t BURGER_API user_has_drop_box_access(
	int8_t ioACUser) BURGER_NOEXCEPT;
extern uint_t BURGER_API user_has_change_access(
	int8_t ioACUser) BURGER_NOEXCEPT;
extern uint_t BURGER_API user_has_no_access(int8_t ioACUser) BURGER_NOEXCEPT;
}}

/* END */

#endif
