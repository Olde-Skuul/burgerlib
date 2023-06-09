/***************************************

	MSDos interrupt functions

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __MSDOS_INTERRUPTS_H__
#define __MSDOS_INTERRUPTS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if defined(BURGER_MSDOS) || defined(DOXYGEN)
namespace Burger { namespace MSDos {

extern void far* BURGER_API get_interrupt_protected(
	uint32_t uInterrupt) BURGER_NOEXCEPT;
extern void BURGER_API set_interrupt_protected(
	uint32_t uInterrupt, void far* pCode) BURGER_NOEXCEPT;

extern uint32_t BURGER_API get_interrupt_real(
	uint32_t uInterrupt) BURGER_NOEXCEPT;
extern void BURGER_API set_interrupt_real(
	uint32_t uInterrupt, uint32_t pCode) BURGER_NOEXCEPT;

extern void BURGER_API set_interrupt_both(
	uint32_t uInterrupt, void far* pCode) BURGER_NOEXCEPT;

extern void* BURGER_API map_physical_address(
	void* pInput, uint32_t uLength) BURGER_NOEXCEPT;
}}

#endif
/* END */

#endif
