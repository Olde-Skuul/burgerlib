/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 6.0 Pro C compiler

	float BURGER_API Burger::_swapendianfloat(const float* pInput) BURGER_NOEXCEPT
	d0-d2, a0-a1 are volatile
	CFM
	Result in d0
	pInput = a0

	Classic
	pResult in 4(a7)
	pInput = a0

***************************************/

#include "brendian.h"

asm float BURGER_API Burger::_swapendianfloat(
	const float* pInput BURGER_68K_A0) BURGER_NOEXCEPT
{
// CFM returns the value in d0
#if defined(BURGER_CFM)
	move.l	(a0), d0

	// Swap endian of the low 16 bits
	ror.w	#8, d0

	// Swap halves
	swap	d0

	// Swap endian of the upper 16 bits
	ror.w	#8, d0

	rts
#else
	// Get the value
	move.l	(a0), d0

	// Swap endian of the low 16 bits
	ror.w	#8, d0

	// Swap halves
	swap	d0

	// Swap endian of the upper 16 bits
	ror.w	#8, d0

	// Get the pointer for the return value
	movea.l 4(a7), a0

	// Save the result
	move.l	d0, (a0)

	rts
#endif
}
