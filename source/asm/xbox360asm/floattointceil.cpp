/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	int32_t BURGER_API float_to_int_ceil(float fInput)
	r0, r3-12, fp0-fp13 are volatile
	Result in r3
	fInput = fp1

***************************************/

#include "brfixedpoint.h"
#include "brstructs.h"

__declspec(naked) int32_t BURGER_API Burger::float_to_int_ceil(
	float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
	__asm {
// Convert the input into an integer
	fctidz	fp2, fp1

// Load 1.0
	lau		r4, g_fOne

// Convert back to a double
	fcfid	fp2, fp2

// f0 = 1.0
	lfs		fp0, g_fOne(r4)

// Pointer to temp
	subi	r4, r1, 8

// Check if there was a fraction
	fsub	fp1, fp2, fp1

// Round up by 1.0
	fadd	fp0, fp2, fp0
	
// If there was a fraction, use the round down version
	fsel	fp1, fp1, fp2, fp0

// Convert to integer again
	fctiw	fp1, fp1

// Store at sp-16 (Temp)
	stfiwx	fp1, r0, r4

// Load the value (Sign extend)
	lwa		r3, -8(r1)
	blr
	}
	// clang-format on
}
