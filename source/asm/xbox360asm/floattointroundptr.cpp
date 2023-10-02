/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	void BURGER_API float_to_int_round(int32_t* pOutput, float fInput)
	r0, r3-12, fp0-fp13 are volatile
	pOutput in r3
	fInput = fp1

***************************************/

#include "brfixedpoint.h"
#include "brstructs.h"

__declspec(naked) void BURGER_API Burger::float_to_int_round(
	int32_t* /* pOutput */, float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
	__asm {
// Get the absolute value
	fabs	fp4, fp1

// Load 0.5
	lau		r4, g_fHalf

// Convert the input into an integer
	fctidz	fp2, fp4

// f0 = 0.5
	lfs		fp0, g_fHalf(r4)

// Convert back to a double
	fcfid	fp2, fp2

// Pointer to temp
	subi	r4, r1, 8

// f5 = 1.0
	fadd	fp5, fp0, fp0
	
// f6 = 0.0
	fsub	fp6, fp0, fp0

// Get the fraction
	fsub	fp4, fp4, fp2
	
// Test for rounding
	fsub	fp0, fp4, fp0
	fsel	fp0, fp0, fp5, fp6
	fadd	fp2, fp2, fp0

// Restore the sign
	fneg	fp3, fp2
	fsel	fp1, fp1, fp2, fp3

// Convert to integer again
	fctiw	fp1, fp1

// Store the result
	stfiwx	fp1, r0, r3
	blr
	}
	// clang-format on
}
