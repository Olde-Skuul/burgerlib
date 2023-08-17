/***************************************

	68000 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 6.0 Pro C compiler

	uint64_t BURGER_API Burger::_swapendian64(uint64_t uInput) BURGER_NOEXCEPT
	d0-d2, a0-a1 are volatile
	pResult in 4(a7)
	uInput = 8(a7)

***************************************/

#include "brendian.h"

#if defined(BURGER_METROWERKS)

// clang-format off
asm uint64_t _swapendian64(uint64_t uInput)
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
// clang-format on

#elif defined(BURGER_APPLE_SC)

// The SC assembler doesn't allow for "inline assembly", so
// we fake it by declaring the byte stream and then call it with
// a C stub.

// clang-format off
#pragma parameter _swapendian64_asm(__A0, __A1)
extern "C" void _swapendian64_asm(uint64_t* pDest, const uint64_t* pInput) = {
	0x2029, 0x0000,	// move.l 0(a1),d0
	0x2229, 0x0004,	// move.l 4(a1),d1
	0xE058,			// ror.w #8,d0
	0xE059,			// ror.w #8,d1
	0x4840,			// swap d0
	0x4841,			// swap d1
	0xE058,			// ror.w #8,d0
	0xE059,			// ror.w #8,d1
	0x2141, 0x0000,	// move.l d1,0(a0)
	0x2140, 0x0004	// move.l d0,4(a0)
};
// clang-format on

uint64_t _swapendian64(const uint64_t& rInput)
{
	// Call the assembly fragment
	uint64_t temp;
	_swapendian64_asm(&temp, &rInput);
	return temp;
}

#endif
