/***************************************

	Memory functions for MSDos

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __MSDOS_MEMORY_H__
#define __MSDOS_MEMORY_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger { namespace MSDos {
extern void* BURGER_API real_to_protected(uint32_t pReal) BURGER_NOEXCEPT;
extern uint32_t BURGER_API alloc_real(uint32_t uSize) BURGER_NOEXCEPT;
extern void BURGER_API free_real(uint32_t pReal) BURGER_NOEXCEPT;
extern uint32_t BURGER_API get_temp_real_buffer(void) BURGER_NOEXCEPT;
extern void* BURGER_API get_temp_protected_buffer(void) BURGER_NOEXCEPT;
}}
/* END */

#endif
