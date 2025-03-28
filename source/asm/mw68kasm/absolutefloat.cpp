/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 6.0 Pro C compiler

	float BURGER_API Burger::absolute(float fInput) BURGER_NOEXCEPT
	d0-d2, a0-a1 are volatile
	CFM
	Result in d0
	fInput = 4(a7)

	Classic
	pResult in 4(a7)
	fInput = 8(a7)

***************************************/

#include "brfloatingpoint.h"

// The 68881 is inlined
#if !defined(BURGER_68881)

asm float BURGER_API Burger::absolute(float fInput) BURGER_NOEXCEPT
{
// CFM returns the value in d0
#if defined(BURGER_CFM)
	// The input is 4(a7)
	move.l	4(a7), d0

	// Make positive
	and.l	#0x7FFFFFFF, d0
	rtd		#4

#else
	// Get the pointer for the return value
	movea.l 4(a7), a0

	// The input is 8(a7)
	move.l	8(a7), d0

	// Make positive
	and.l	#0x7FFFFFFF, d0

	// Save the result
	move.l	d0, (a0)
	rts
#endif
}

#endif
