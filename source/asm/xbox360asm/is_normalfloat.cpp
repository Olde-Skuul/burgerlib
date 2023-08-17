/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	uint_t BURGER_API is_normal(float fInput)
	r0, r3-12, fp0-fp13 are volatile
	Result in r3
	fInput = fp1

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) uint_t BURGER_API Burger::is_normal(
	float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
	__asm {

	// Move the float from fp1 to r3 as is
	stfs	fp1, -8(r1)
	lwz		r3, -8(r1)

	// Remove 0x7F800000 for denormals and NaN/Inf
	subis	r3, r3, 0x0080

	// Kick off the sign
	andis.	r3, r3, 0x7FFF

	// Check for the range of 0x00800000-0x7F800000
	subis	r3, r3, 0x7F00

	// If high bit is set, it's in the above range
	extrwi	r3, r3, 1, 0
	blr
	}
	// clang-format on
}
