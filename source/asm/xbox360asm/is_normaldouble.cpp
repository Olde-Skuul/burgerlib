/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	uint_t BURGER_API is_normal(double dInput)
	r0, r3-12, fp0-fp13 are volatile
	Result in r3
	dInput = fp1

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) uint_t BURGER_API Burger::is_normal(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
	__asm {

	// Move the float from fp1 to r3 as is
	stfd	fp1, -8(r1)
	lwz		r3, -8(r1)

	// Remove 0x7FE0000 for denormals and NaN/Inf
	subis	r3, r3, 0x0010

	// Kick off the sign
	andis.	r3, r3, 0x7FFF

	// Check for the range of 0x00010000-0x7FE00000
	subis	r3, r3, 0x7FE0

	// If high bit is set, it's in the above range
	extrwi	r3, r3, 1, 0
	blr
	}
	// clang-format on
}