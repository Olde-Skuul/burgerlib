/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 6.0 Pro C compiler

	void BURGER_API Burger::_store_unaligned(uint32_t* pOutput BURGER_68K_A0,
		uint32_t uInput BURGER_68K_D0) BURGER_NOEXCEPT
	d0-d2, a0-a1 are volatile
	pOutput = a0
	uInput = d0

***************************************/

#include "brendian.h"

asm void BURGER_API Burger::_store_unaligned(
	uint32_t* pOutput BURGER_68K_A0, uint32_t uInput BURGER_68K_D0) BURGER_NOEXCEPT
{
	// Store the high byte
	move.b	d0, 3(a0)
	lsr.l	#8,d0

	// Next byte
	move.b	d0, 2(a0)
	lsr.l	#8,d0

	move.b	d0, 1(a0)
	lsr.l	#8,d0

	// Store the final byte
	move.b	d0, (a0)
	rts
}
