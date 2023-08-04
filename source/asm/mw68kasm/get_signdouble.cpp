/***************************************

	68000 assembly for implementing the function
	Burger::get_sign(double) for Metrowerks Codewarrior

	Build with the C compiler

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

***************************************/

#include "brfloatingpoint.h"

asm double BURGER_API Burger::get_sign(double dInput) BURGER_NOEXCEPT
{
	// Get the pointer for the return value
	movea.l 4(a7), a0

	// The input is 8(a7)/12(a7)
	// Get the high word
	move.l	8(a7), d1

	// 1.0f shifted one bit up
	move.l #0x7FE00000, d0

	// Take the floating point number and move the sign to carry
	add.l	d1, d1

	// d0 has -1.0f or 1.0f by shifting down with x
	roxr.l	#1, d0

	// If the input is zero, return zero
	or.l	d1, 12(a7)
	bne.s	L0
	clr.l	d0

L0:
	// Store the double (The low word is always zero)
	move.l	d0,(a0)
	clr.l	4(a0)
#if defined(BURGER_CFM)
	rtd		#12
#else
	rts
#endif
}
