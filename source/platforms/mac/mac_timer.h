/***************************************

	MacOS specific timer code

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __MAC_TIMER_H__
#define __MAC_TIMER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger { namespace MacOS {
#if defined(BURGER_PPC) || defined(DOXYGEN)
extern uint64_t PollRTC601(void) BURGER_NOEXCEPT;
extern uint64_t PollTBR603(void) BURGER_NOEXCEPT;
#endif

}}

/* END */

#endif
