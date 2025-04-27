/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com

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

// Force 0x80000000 to be 0x7FFFFFFF so in the end
// it is changed back to 0x80000000 (64 bit)
	clr.l	d2
	subq.l	#1, d0
	subx.l	d2, d1

// Do a bit count for both halves
	bfffo	d0{0:32},d0
	bfffo	d1{0:32},d1

// If the high byte is zero, do the lower 32 bits
	beq.s	low32

// Result is a power of 2 in the high 32 bits or zero

// Is it supposed to return 0?
	tst.l	d1
	beq.s	zero

// Convert 1-32 to 31-0 in that order
	moveq.l	#32,d2
	sub.l	d1,d2

// Prepare the result
	moveq	#0,d0

// Set the power of two bit
	bset	d2,d0

// Save the 64 bit result
	move.l	d0, (a0)
	clr.l	4(a0)

#if defined(BURGER_CFM)
	rtd		#12
#else
	rts
#endif

// Return zero
zero:
	clr.l	(a0)
	clr.l	4(a0)

#if defined(BURGER_CFM)
	rtd		#12
#else
	rts
#endif

// Input value resolves to low 32 bits.
// Answer is 0x1 to 0x100000000
low32:
// Is it supposed to return 0x100000000?
	tst.l	d0
	beq.s	lowmax

// Convert 1-32 to 31-0 in that order
	moveq.l	#32,d2
	sub.l	d0,d2

// Prepare the result
	moveq	#0,d1

// Set the power of two bit
	bset	d2,d1

// Save the result in the low 32 bits
	clr.l	(a0)
	move.l	d1, 4(a0)

#if defined(BURGER_CFM)
	rtd		#12
#else
	rts
#endif

// Save the result of 0x100000000
lowmax:
	move.l	#1,(a0)
	clr.l	4(a0)

#if defined(BURGER_CFM)
	rtd		#12
#else
	rts
#endif
}
// clang-format on
