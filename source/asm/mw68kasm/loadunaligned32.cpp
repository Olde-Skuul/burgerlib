/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 6.0 Pro C compiler

	uint32_t BURGER_API Burger::_load_unaligned(
		const uint32_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT
	d0-d2, a0-a1 are volatile
	Result in d0
	pInput = a0

***************************************/

#include "brendian.h"

asm uint32_t BURGER_API Burger::_load_unaligned(
	const uint32_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT
{
	// Get the highest byte first
	move.b	(a0),d0

	// Shift it up and fetch byte 2
	lsl.l	#8,d0
	move.b	1(a0),d0

	// Shift it up and fetch byte 3
	lsl.l	#8,d0
	move.b	2(a0),d0

	// Shift it up and fetch byte 4
	lsl.l	#8,d0
	move.b	3(a0),d0
	rts
}