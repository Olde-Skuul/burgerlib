/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	uint_t BURGER_API get_sign_bit(double dInput)
	r0, r3-12, fp0-fp13 are volatile
	Result in r3
	dInput = fp1

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) uint_t BURGER_API Burger::get_sign_bit(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
	__asm {
	// Store it in memory
	stfd	fp1, -8(r1)
	lwz		r3, -8(r1)

	// (Flags>>31)&1 Get the sign bit
	extrwi	r3, r3, 1, 0
	blr
	}
	// clang-format on
}
