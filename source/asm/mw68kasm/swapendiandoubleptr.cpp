/***************************************

	68000 assembly for implementing the function
	Burger::_swapendiandouble(const double*) for Metrowerks Codewarrior

	Build with the C compiler

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

***************************************/

#include "brendian.h"

asm double BURGER_API Burger::_swapendiandouble(
	const double* pInput BURGER_68K_A0)
{
	// The input pointer is a0
	move.l  (a0), d0
	move.l	4(a0), d1

	// Swap endian of the low 16 bits
	ror.w	#8, d0
	ror.w	#8, d1

	// Swap halves
	swap	d0
	swap	d1
	
	// Swap endian of the upper 16 bits
	ror.w	#8, d0
	ror.w	#8, d1

	// Get the pointer for the return value
	movea.l 4(a7), a0

	// Save the result
	move.l	d1, (a0)
	move.l	d0, 4(a0)

#if defined(BURGER_CFM)
	rtd		#4
#else
	rts
#endif
}
