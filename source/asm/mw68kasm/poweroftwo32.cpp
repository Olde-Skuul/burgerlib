/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 6.0 Pro C compiler

	uint32_t BURGER_API Burger::power_of_two(uint32_t uInput) BURGER_NOEXCEPT
	d0-d2, a0-a1 are volatile
	uInput = d0
	Result = d0

***************************************/

#include "brpoweroftwo.h"

// clang-format off
asm uint32_t BURGER_API Burger::power_of_two(uint32_t uInput BURGER_68K_D0)
{
// Check if already a power of 2 by reducing by 1
	subq.l	#1,d0

// Which is the highest bit? 0 -> 32, 1 -> 32, 0x80000000 -> 0
	bfffo	d0{0:32},d1

// Prepare the result
	moveq	#0,d0

// Is it supposed to return 0?
	tst.l	d1
	beq.s	exit

// Convert 1-32 to 31-0 in that order
	moveq.l	#32,d2
	sub.l	d1,d2

// Set the power of two bit
	bset	d2,d0
exit:
	rts
}
// clang-format on
