/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 6.0 Pro C compiler

	uint64_t BURGER_API Burger::_load_unaligned(
		const uint64_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT
	d0-d2, a0-a1 are volatile
	pResult = 4(a7)
	pInput = a0

***************************************/

#include "brendian.h"

asm uint64_t BURGER_API Burger::_load_unaligned(
	const uint64_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT
{
	// Get the destination pointer
	movea.l	4(a7), a1

	// Copy the data
	move.b	(a0), (a1)
	move.b	1(a0), 1(a1)
	move.b	2(a0), 2(a1)
	move.b	3(a0), 3(a1)
	move.b	4(a0), 4(a1)
	move.b	5(a0), 5(a1)
	move.b	6(a0), 6(a1)
	move.b	7(a0), 7(a1)

#if defined(BURGER_CFM)
	rtd		#4
#else
	rts
#endif
}