/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 6.0 Pro C compiler

	uint64_t BURGER_API Burger::power_of_two(uint64_t uInput) BURGER_NOEXCEPT
	d0-d2, a0-a1 are volatile
	pResult in 4(a7)
	uInput = 8(a7)

***************************************/

#include "brpoweroftwo.h"

// clang-format off
asm uint64_t BURGER_API Burger::power_of_two(uint64_t uInput)
{
// Get the pointer for the return value
	movea.l 4(a7), a0

// The input is 8(a7)
	move.l	8(a7), d1
	move.l	12(a7), d0

// Save registers
	move.l	d3,	8(a7)

// Force 0x40000000 to be 0x3FFFFFFF so in the end
// it is changed back to 0x40000000 (64 bit)
	clr.l	d2
	subq.l	#1, d0
	subx.l	d2, d1

// Implement the C code, but use faster code instead
// uInput |= uInput>>1
	move.l	d0, d2
	move.l	d1, d3
	lsr.l	#1, d1
	roxr.l	#1, d0
	or.l	d2, d0
	or.l	d3, d1

// uInput |= uInput>>2
	move.l	d0, d2
	lsr.l	#2,	d2
	or.l	d2, d0

	move.l	d1, d2
	moveq.l	#30, d3
	lsl.l	d3, d2
	move.l	d1, d3
	lsr.l	#2, d3
	or.l	d2, d0
	or.l	d3, d1

// uInput |= uInput>>4
	move.l	d0, d2
	lsr.l	#4,	d2
	or.l	d2, d0

	move.l	d1, d2
	moveq.l	#28, d3
	lsl.l	d3, d2
	move.l	d1, d3
	lsr.l	#4, d3
	or.l	d2, d0
	or.l	d3, d1

// uInput |= uInput>>8
	move.l	d0, d2
	lsr.l	#8,	d2
	or.l	d2, d0

	move.l	d1, d2
	moveq.l	#24, d3
	lsl.l	d3, d2
	move.l	d1, d3
	lsr.l	#8, d3
	or.l	d2, d0
	or.l	d3, d1

// uInput |= uInput>>16
	move.l	d0, d2
	swap	d2
	or.w	d2, d0

	move.l	d1, d2
	swap	d2
	or.w	d2,	d1
	clr.w	d2
	or.l	d2, d0
	
// uInput |= uInput>>32
	or.l	d1, d0

// Increment to get the final power of 2
	clr.l	d2
	addq.l	#1, d0
	addx.l	d2, d1

// Save the result
	move.l	d1, (a0)
	move.l	d0, 4(a0)
	
// Restore registers
	move.l	8(a7), d3

#if defined(BURGER_CFM)
	rtd		#12
#else
	rts
#endif
}
// clang-format on
