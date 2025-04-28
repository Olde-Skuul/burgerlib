/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 6.0 Pro C compiler

	uint64_t BURGER_API Burger::bit_reverse(uint64_t uInput, uint_t uBitLength) BURGER_NOEXCEPT
	d0-d2, a0-a1 are volatile
	pResult in 4(a7)
	uInput = 8(a7)
	uBitLength = 16(a7)

***************************************/

#include "brpoweroftwo.h"

// clang-format off
asm uint64_t BURGER_API Burger::bit_reverse(
	uint64_t uInput, uint_t uBitLength) BURGER_NOEXCEPT
{
// Get the pointer for the return value
	movea.l 4(a7), a0

// The input is 8(a7)
	move.l	8(a7), d1
	move.l	12(a7), d0

// Save registers
	move.l	d3, 8(a7)
	move.l	d4, 12(a7)

// The count is 16(a7)
	move.l	16(a7), d2

// Initialize the result
	clr.l	d3
	clr.l	d4

// Shift out a bit
BitReverse64:
	lsr.l	#1, d1
	roxr.l	#1, d0

// Shift into the result
	addx.l	d3, d3
	addx.l	d4, d4

// Count down the iterator
	subq	#1, d2
	bne.s	BitReverse64

// Save the result
	move.l	d4, (a0)
	move.l	d3, 4(a0)

// Restore registers
	move.l	8(a7), d3
	move.l	12(a7), d4

#if defined(BURGER_CFM)
	rtd		#16
#else
	rts
#endif
}
// clang-format on
