/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	double BURGER_API get_ceiling(double dInput)
	r0, r3-12, fp0-fp13 are volatile
	Result in fp1
	dInput = fp1

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) double BURGER_API Burger::get_ceiling(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
	__asm {
// Convert the float to an integer, round to zero
		fctidz	fp2, fp1

// Prep for loading 1.0f
		lau		r3, g_fOne

// Get abs fInput
		fabs	fp6, fp1

// Convert back to float without fraction, possible final answer
		fcfid	fp2, fp2

// Prep for loading 4503599627370496.0
		lau		r4, g_dMinNoInteger

// Fetch 1.0f in fp3
		lfs		fp3, g_fOne(r3)

// Fetch 4503599627370496.0 in fp4
		lfd		fp4, g_dMinNoInteger(r4)

// Take the answer and add 1.0f
		fadd	fp3, fp3, fp2

// fp6 has abs(fInput) - 4503599627370496.0f
		fsub	fp6, fp6, fp4

// fp5 has answer - fInput, to test if it overflowed
		fsub	fp5, fp2, fp1

// answer = ((answer-fInput)>=0) ? answer : answer + 1.0f
		fsel	fp2, fp5, fp2, fp3

// answer = (fabs(fInput) - 8388608)>=0 ? fInput, answer
		fsel	fp1, fp6, fp1, fp2
		blr
	}
	// clang-format on
}
