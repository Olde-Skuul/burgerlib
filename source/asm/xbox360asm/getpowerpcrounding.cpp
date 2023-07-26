#include "brfloatingpoint.h"

/***************************************

    \brief Get the floating point rounding setting

    On PowerPC processors, there's a special floating point register to control
    the rounding behavior. This function returns the current setting.

    \note This function only exists on PowerPC compatible CPU targets.

    \return Current precision state
    \sa ePowerPCRounding or SetPowerPCRounding(ePowerPCRounding)

***************************************/

#if defined(BURGER_XBOX360)

__declspec(naked) Burger::ePowerPCRounding BURGER_API Burger::GetPowerPCRounding(void)
{
__asm {

// Get the floating point control register
	mffs fp0 

// Store it in memory
	stfd fp0, -8(r1)

// Load into an integer register
	lwz r3, -4(r1)

// Mask off the lowest 2 bits (AND & 3)
	clrlwi	r3, r3, 30

// Exit
	blr
	}
}

#endif