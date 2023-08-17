/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	double BURGER_API get_sign(double dInput)
	r0, r3-12, fp0-fp13 are volatile
	Result in fp1
	dInput = fp1

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) double BURGER_API Burger::get_sign(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
	__asm {

	// Test for NaN first, to avoid stalls later
	fcmpu	cr1, fp1, fp1

	// Get 0.5
	lau		r3, g_fHalf

	// -fInput
	fneg	fp2, fp1
	lfs		fp3, g_fHalf(r3)

	// -0.5
	fneg	fp4, fp3

	// Test twice, to also find 0.0f
	fsel	fp0, fp1, fp3, fp4
	fsel	fp4, fp2, fp4, fp3

	// Yields 0.0f, 1.0f, or -1.0f
	fadd	fp1, fp4, fp0

	// If not NaN, return now
	beqlr	cr1

	// NaN detected, you are boned
	// Store the -NaN
	stfd	fp2, -8(sp)

	// Assume result is 1.0f (0.5*2)
	fadd	fp1, fp3, fp3

	// Test for negative with integer registers
	lwz		r3, -8(sp)
	andis.	r3, r3, 0x8000

	// Note: Testing on -NaN so not equal is positive
	bnelr

	// Return negative
	fneg	fp1, fp1
	blr
	}
	// clang-format on
}
