/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 6.0 Pro C compiler

	float BURGER_API Burger::get_sign(float fInput) BURGER_NOEXCEPT
	d0-d2, a0-a1 are volatile
	CFM
	Result in d0
	fInput = 4(a7)

	Classic
	pResult in 4(a7)
	fInput = 8(a7)

***************************************/

#include "brfloatingpoint.h"

asm float BURGER_API Burger::get_sign(float fInput) BURGER_NOEXCEPT
{
// CFM returns the value in d0
#if defined(BURGER_CFM)
	// The input is 4(a7)
	move.l	4(a7), d1

	// 1.0f shifted one bit up
	move.l	#0x7F000000, d0

	// Take the floating point number and move the sign to carry
	add.l	d1, d1

	// d0 has -1.0f or 1.0f by shifting down with x
	roxr.l	#1, d0

	// If the input is zero, return zero
	tst.l	d1
	bne.s	L0
	clr.l	d0

L0:	rtd		#4

#else
	// Get the pointer for the return value
	movea.l 4(a7), a0

	// The input is 8(a7)
	move.l	8(a7), d1

	// 1.0f shifted one bit up
	move.l	#0x7F000000, d0

	// Take the floating point number and move the sign to carry
	add.l	d1, d1

	// d0 has -1.0f or 1.0f by shifting down with x
	roxr.l	#1, d0

	// If the input is zero, return zero
	tst.l	d1
	bne.s	L0
	clr.l	d0

	// Save the result
L0:	move.l	d0, (a0)

	rts
#endif
}
