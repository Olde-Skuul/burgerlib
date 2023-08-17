/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	float BURGER_API Burger::ModuloRadians(float fInput)
	r0, r3-12, fp0-fp13 are volatile
	Result in fp1
	fInput = fp1

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) float BURGER_API Burger::ModuloRadians(
	float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
	__asm {

// Fetch the pointers
	lau		r3, g_fReciprocalPi2
	lau		r4, g_fHalf
	lau		r5, g_fNegPi2

// Load in 1/ 2Pi
	lfs		fp3, g_fReciprocalPi2(r3)

// Load in 0.5f
	lfs		fp4, g_fHalf(r4)

// Load in Pi2
	lfs		fp5, g_fNegPi2(r5)

// (fInput*g_fReciprocalPi2)+g_fHalf
	fmadds	fp3, fp1, fp3, fp4

// Convert to integer
	fctidz	fp2, fp3

// Convert back to float
	fcfid	fp2, fp2

// Compare the two and get rid of the pre-rounded
	fcmpu	cr0, fp2, fp3
	ble		FModule1

// Load 1.0f
	fadds	fp0, fp4, fp4
// Subtract 1.0f
	fsubs	fp2, fp2, fp0

FModule1:
// Don't use fnmsubs, it doesn't handle the sign bit properly
// (fVar*-g_fPi2) + fInput
	fmadds	fp1, fp2, fp5, fp1
	blr
	}
	// clang-format on
}
