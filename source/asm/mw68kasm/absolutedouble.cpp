/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 6.0 Pro C compiler

	double BURGER_API Burger::absolute(double dInput) BURGER_NOEXCEPT
	d0-d2, a0-a1 are volatile
	pResult in 4(a7)
	dInput = 8(a7)

***************************************/

#include "brfloatingpoint.h"

// The 68881 is inlined
#if !defined(BURGER_68881)

asm double BURGER_API Burger::absolute(double dInput) BURGER_NOEXCEPT
{
	// Get the pointer for the return value
	movea.l 4(a7), a0

	// The input is 8(a7)/12(a7)
	// Get the high word
	move.l	8(a7), d0

	// Force positive
	and.l	#0x7FFFFFFF, d0

	// Leave the lower 32 bits intact
	move.l	12(a7), d1

	// Store the double
	move.l	d0, (a0)
	move.l	d1, 4(a0)

#if defined(BURGER_CFM)
	rtd		#12
#else
	rts
#endif
}

#endif

