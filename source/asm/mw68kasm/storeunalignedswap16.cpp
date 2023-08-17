/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 6.0 Pro C compiler

	void BURGER_API Burger::_store_unaligned_swap(uint16_t* pOutput BURGER_68K_A0,
		uint16_t uInput BURGER_68K_D0) BURGER_NOEXCEPT
	d0-d2, a0-a1 are volatile
	pOutput = a0
	uInput = d0

***************************************/

#include "brendian.h"

asm void BURGER_API Burger::_store_unaligned_swap(
	uint16_t* pOutput BURGER_68K_A0, uint16_t uInput BURGER_68K_D0) BURGER_NOEXCEPT
{
	// Store the low byte
	move.b	d0, (a0)

	// Shift it down
	lsr.w	#8,d0

	// Store the final byte
	move.b	d0, 1(a0)
	rts
}
