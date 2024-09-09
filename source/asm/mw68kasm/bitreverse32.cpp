/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 6.0 Pro C compiler

	uint32_t BURGER_API Burger::bit_reverse(uint32_t uInput, uint_t uBitLength) BURGER_NOEXCEPT
	d0-d2, a0-a1 are volatile
	pResult in d0
	uInput = 4(a7)
	uBitLength = 8(a7)

***************************************/

#include "brpoweroftwo.h"

// clang-format off
asm uint32_t BURGER_API Burger::bit_reverse(
	uint32_t uInput, uint_t uBitLength) BURGER_NOEXCEPT
{
// The input is 4(a7)
	move.l	4(a7), d1
	move.l	8(a7), d2

// Initialize the result
	clr.l	d0
	
// Shift out a bit
BitReverse32:
	lsr.l	#1, d1

// Shift into the result
	addx.l	d0, d0

// Count down the iterator
	subq	#1, d2
	bne.s	BitReverse32

#if defined(BURGER_CFM)
	rtd		#8
#else
	rts
#endif
}
// clang-format on
