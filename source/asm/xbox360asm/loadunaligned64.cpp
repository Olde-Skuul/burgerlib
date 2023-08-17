/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	uint64_t BURGER_API _load_unaligned(const uint64_t* pInput)
	r0, r3-12, fp0-fp13 are volatile
	Result in r3
	pInput = r3

***************************************/

#include "brendian.h"

__declspec(naked) uint64_t BURGER_API Burger::_load_unaligned(
	const uint64_t* /* pInput */) BURGER_NOEXCEPT
{
__asm {
// r7-r10 will get the upper 32 bits
	lbz		r10,0(r3)
	lbz		r9,1(r3)
	lbz		r8,2(r3)
	lbz		r7,3(r3)

// r6-r3 will get the lower 32 bits
	lbz		r6,4(r3)
	rlwimi	r7,r8,8,16,23
	lbz		r5,5(r3)
	rlwimi	r7,r9,16,8,15
	lbz		r4,6(r3)
	rlwimi	r7,r10,24,0,7
	lbz		r3,7(r3)

// r7 will have the upper 32 bits
	sldi	r7,r7,32

// Merge the lower 32 bits
	rlwimi	r3,r4,8,16,23
	rlwimi	r3,r5,16,8,15
	rlwimi	r3,r6,24,0,7

// Merge everything
	or		r3,r3,r7
	blr
	}
}
