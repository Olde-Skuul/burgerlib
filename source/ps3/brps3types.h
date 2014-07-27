/***************************************

	Typedefs specific to the Playstation 3

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRPS3TYPES_H__
#define __BRPS3TYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if defined(BURGER_PS3) && !defined(DOXYGEN)
namespace Burger {
typedef Word32 _sys_sleep_queue_t;
struct sys_lwmutex_lock_info_t {
	volatile Word32 owner;
	volatile Word32 waiter;
};
union sys_lwmutex_variable_t {
	sys_lwmutex_lock_info_t info;
	volatile Word64 all_info;
};
struct sys_lwmutex_t {
	sys_lwmutex_variable_t lock_var;
	Word32 attribute;
	Word32 recursive_count;
	_sys_sleep_queue_t sleep_queue;
	Word32 pad;
};
}
#endif
/* END */

#endif
