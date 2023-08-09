/***************************************

	\brief Return \ref TRUE if the value is a NaN

	Test for QNan and SNan and return \ref TRUE if so.

	\param fInput A 32 bit floating point number.
	\return \ref TRUE if Nan, \ref FALSE if not.
	\sa is_NaN(double), IsInf(float), IsFinite(float), IsNormal(float) and
		SignBit(float)

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) uint_t BURGER_API Burger::is_NaN(
	float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
	__asm {

// Perform a NaN test
	fcmpu	cr0, fp1, fp1

// Move cr0 into r0
	mfcr	r0

// (Flags>>28)&1 Grab the "Unordered" flag
	extrwi	r3, r0, 1, 3

// Exit
	blr
	}
	// clang-format on
}
