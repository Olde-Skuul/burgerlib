/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 6.0 Pro C compiler

	double BURGER_API Burger::_swapendiandouble(double dInput) BURGER_NOEXCEPT
	d0-d2, a0-a1 are volatile
	pResult in 4(a7)
	dInput = 8(a7)

***************************************/

#include "brendian.h"

asm double BURGER_API Burger::_swapendiandouble(double /* dInput */) BURGER_NOEXCEPT
{
// Get the pointer for the return value
	movea.l 4(a7), a0

	// The input is 8(a7)
	move.l	8(a7), d0
	move.l	12(a7), d1

	// Swap endian of the low 16 bits
	ror.w	#8, d0
	ror.w	#8, d1

	// Swap halves
	swap	d0
	swap	d1

	// Swap endian of the upper 16 bits
	ror.w	#8, d0
	ror.w	#8, d1

	// Save the result
	move.l	d1, (a0)
	move.l	d0, 4(a0)

#if defined(BURGER_CFM)
	rtd		#12
#else
	rts
#endif
}
