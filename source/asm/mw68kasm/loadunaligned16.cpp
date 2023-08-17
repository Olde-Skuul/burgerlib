/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 6.0 Pro C compiler

	uint16_t BURGER_API Burger::_load_unaligned(
		const uint16_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT
	d0-d2, a0-a1 are volatile
	Result in d0
	pInput = a0

***************************************/

#include "brendian.h"

asm uint16_t BURGER_API Burger::_load_unaligned(
	const uint16_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT
{
	// A0 = pInput
	// Get the highest byte first
	moveq	#0, d0
	move.b	(a0),d0

	// Shift it up
	lsl.w	#8,d0

	// Fetch the final byte
	move.b	1(a0),d0
	rts
}
