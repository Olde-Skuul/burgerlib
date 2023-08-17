/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	uint_t BURGER_API is_NaN(float fInput)
	r0, r3-12, fp0-fp13 are volatile
	Result in r3
	fInput = fp1

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) uint_t BURGER_API Burger::is_NaN(
	float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
	__asm {

// Perform a NaN test
	fcmpu	cr0, fp1, fp1

// Move cr0 into r0
	mfcr	r0

// (Flags>>28)&1 Grab the "Unordered" flag
	extrwi	r3, r0, 1, 3
	blr
	}
	// clang-format on
}
