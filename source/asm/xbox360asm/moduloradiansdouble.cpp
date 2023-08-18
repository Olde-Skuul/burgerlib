/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	double BURGER_API Burger::modulo_radians(double dInput)
	r0, r3-12, fp0-fp13 are volatile
	Result in fp1
	dInput = fp1

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) double BURGER_API Burger::modulo_radians(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
	__asm {

// Fetch the pointers
	lau		r3, g_dReciprocalPi2
	lau		r4, g_dHalf
	lau		r5, g_dNegPi2

// Load in 1/ 2Pi
	lfd		fp3, g_dReciprocalPi2(r3)

// Load in 0.5f
	lfd		fp4, g_dHalf(r4)

// Load in Pi2
	lfd		fp5, g_dNegPi2(r5)

// (fInput*g_fReciprocalPi2)
	fmul	fp3, fp1, fp3

// (fInput*g_fReciprocalPi2)+g_fHalf
	fadd	fp3, fp3, fp4

// Convert to integer
	fctidz	fp2, fp3

// Convert back to float
	fcfid	fp2, fp2

// Compare the two and get rid of the pre-rounded
	fcmpu	cr0, fp2, fp3
	ble		FModule1

// Load 1.0f
	fadd	fp0, fp4, fp4

// Subtract 1.0f
	fsub	fp2, fp2, fp0
FModule1:
// Don't use fnmsubs, it doesn't handle the sign bit properly
// (fVar*-g_fPi2) + fInput
	fmadd	fp1, fp2, fp5, fp1
	blr
	}
	// clang-format on
}
