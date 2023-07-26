#include "brfloatingpoint.h"

/***************************************

	\brief Change the floating point precision

	On PowerPC processors, there's a special floating point register to control
	the rounding. This function allows the modification of this register for FPU
	rounding.

	\note This function only exists on PowerPC compatible CPU targets.

	\param eInput New enumeration state
	\return Previous rounding state
	\sa ePowerPCRounding or GetPowerPCRounding(void)

***************************************/

#if defined(BURGER_XBOX360)

__declspec(naked) Burger::ePowerPCRounding BURGER_API
	Burger::SetPowerPCRounding(ePowerPCRounding /* uInput */)
{
	__asm {
// Get the floating point control register
	mffs fp0

// Store it in memory
	stfd fp0, -8(r1)

// Load into an integer register
	lwz r4, -4(r1)

// Get the return value, mask off the lowest 2 bits (AND & 3)
	extrwi r5, r4, 2, 30

// Move the lowest 2 bits into the existing value
	rlwimi r4, r3, 0, 30, 31

// Store it in memory
	stw r4, -4(r1)

// Reload the fp register
	lfd fp0, -8(r1)

// Set the flags
	mtfsf 255, fp0

// Return the previous value
	mr r3, r5

// Exit
	blr
	}
}

#endif