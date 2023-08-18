/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	double BURGER_API get_round(double dInput)
	r0, r3-12, fp0-fp13 are volatile
	Result in fp1
	dInput = fp1

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) double BURGER_API Burger::get_round(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
	__asm {
// fAbs = absolute(fInput)
	fabs	fp5, fp1

// Prep for loading 0.5f
	lau		r3, g_fHalf

// Convert the absolute to an integer, round to zero
	fctidz	fp2, fp5

// Prep for loading 4503599627370496.0f
	lau		r4, g_dMinNoInteger

// fAnswer = absolute(fInput) without fraction
	fcfid	fp2, fp2

// Fetch 0.5f in fp3
	lfs		fp3, g_fHalf(r3)

// Fetch 8388608.0f in fp4
	lfd		fp4, g_dMinNoInteger(r4)

// Create 1.0f in fp6
	fadds	fp6, fp3, fp3

// Create 0.0f in fp0
	fsubs	fp0, fp3, fp3

// fTemp = fAnswer - fAbs
	fsub	fp8, fp5, fp2

// fTemp = fTemp - 0.5f
	fsub	fp8, fp8, fp3

// fTemp = (fTemp>=0) ? 1 : 0
	fsel	fp8, fp8, fp6, fp0

// fAnswer += fTemp
	fadd	fp2, fp2, fp8

// if (fInput<0) neg(fAnswer)
	fneg	fp8, fp2
	fsel	fp2, fp1, fp2, fp8

// fTemp = fAbs - 4503599627370496.0f
	fsub	fp5, fp5, fp4

// fp1 = (fAbs - 8388608)>=0 ? fInput, fAnswer
	fsel	fp1, fp5, fp1, fp2
	blr
	}
	// clang-format on
}
