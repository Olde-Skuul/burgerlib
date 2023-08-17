/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	uint_t BURGER_API is_infinite(double dInput)
	r0, r3-12, fp0-fp13 are volatile
	Result in r3
	dInput = fp1

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) uint_t BURGER_API Burger::is_infinite(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
	__asm {

// Get the address of g_dInf
	lau		r3, g_dInf

// Strip the sign
	fabs	fp1, fp1

// Fetch dInf
	lfd		fp0, g_dInf(r3)

// Compare for equality
	fcmpu	cr0, fp1, fp0

// Move cr0 into r0
	mfcr	r0

// (Flags>>29)&1 Grab the "Equal" flag
	extrwi	r3, r0, 1, 2
	blr
	}
	// clang-format on
}
