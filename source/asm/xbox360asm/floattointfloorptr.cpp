/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	void BURGER_API float_to_int_floor(int32_t* pOutput, float fInput)
	r0, r3-12, fp0-fp13 are volatile
	pOutput in r3
	fInput = fp1

***************************************/

#include "brfixedpoint.h"
#include "brstructs.h"

__declspec(naked) void BURGER_API Burger::float_to_int_floor(
	int32_t* /* pOutput */, float /* fInput */) BURGER_NOEXCEPT
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

// Check if there was a fraction
	fsub	fp1, fp1, fp2

// Round down by 1.0
	fsub	fp0, fp2, fp0
	
// If there was a fraction, use the round down version
	fsel	fp1, fp1, fp2, fp0

// Convert to integer again
	fctiw	fp1, fp1

// Store the result
	stfiwx	fp1, r0, r3
	blr
	}
	// clang-format on
}
