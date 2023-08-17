/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	float BURGER_API get_floor(float fInput)
	r0, r3-12, fp0-fp13 are volatile
	Result in fp1
	fInput = fp1

***************************************/

#include "brfloatingpoint.h"
#include <math.h>

float BURGER_API Burger::get_floor(float fInput) BURGER_NOEXCEPT
{
	// Convert to the input to an integer (Can fail on large numbers)
	double dVar = __fcfid(__fctiwz(fInput));
	// Floor the value to - infinity
	dVar = __fsel(fInput - dVar, dVar, dVar - 1.0f); // Return floored value

	// If the input number was invalid, return the original number
	return __fself(
		fabsf(fInput) - 8388608.0f, fInput, static_cast<float>(dVar));
}
