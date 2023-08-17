/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	double BURGER_API get_floor(double dInput)
	r0, r3-12, fp0-fp13 are volatile
	Result in fp1
	dInput = fp1

***************************************/

#include "brfloatingpoint.h"
#include <math.h>

double BURGER_API Burger::get_floor(double dInput) BURGER_NOEXCEPT
{
	// Convert to the input to an integer (Can fail on large numbers)
	double dVar = __fcfid(__fctidz(dInput));
	// Floor the value to - infinity
	dVar = __fsel(dInput - dVar, dVar, dVar - 1.0); // Return floored value

	// If the input number was invalid, return the original number
	return __fsel(fabs(dInput) - 4503599627370496.0, dInput, dVar);
}
